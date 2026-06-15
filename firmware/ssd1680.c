/**
 * SSD1680 墨水屏驱动实现
 *
 * 引脚定义来自 references/epd_pinout.md 实板测量数据
 *   MOSI = P2_2    SCK  = P2_0    CS   = P2_1
 *   DC   = P0_1    RST  = P0_7    BUSY = P1_1
 *   POWER = P2_3 (boost 升压电路开关)
 */

#include "ssd1680.h"
#include "hw_gpio.h"

/* ============ 引脚定义 (DA14585 GPIO) ============ */

/* SPI 数据/时钟 */
#define EPD_PORT_MOSI   GPIO_PORT_2
#define EPD_PIN_MOSI    GPIO_PIN_2      /* P2_2 */

#define EPD_PORT_SCK    GPIO_PORT_2
#define EPD_PIN_SCK     GPIO_PIN_0      /* P2_0 */

/* 控制线 */
#define EPD_PORT_CS     GPIO_PORT_2
#define EPD_PIN_CS      GPIO_PIN_1      /* P2_1 */

#define EPD_PORT_DC     GPIO_PORT_0
#define EPD_PIN_DC      GPIO_PIN_1      /* P0_1 */

#define EPD_PORT_RST    GPIO_PORT_0
#define EPD_PIN_RST     GPIO_PIN_7      /* P0_7 */

#define EPD_PORT_BUSY   GPIO_PORT_1
#define EPD_PIN_BUSY    GPIO_PIN_1      /* P1_1 */

#define EPD_PORT_POWER  GPIO_PORT_2
#define EPD_PIN_POWER   GPIO_PIN_3      /* P2_3 */

/* ============ GPIO 辅助宏 ============ */

#define CS_LOW()    GPIO_SetInactive(EPD_PORT_CS, EPD_PIN_CS)
#define CS_HIGH()   GPIO_SetActive(EPD_PORT_CS, EPD_PIN_CS)
#define DC_CMD()    GPIO_SetInactive(EPD_PORT_DC, EPD_PIN_DC)
#define DC_DATA()   GPIO_SetActive(EPD_PORT_DC, EPD_PIN_DC)
#define RST_LOW()   GPIO_SetInactive(EPD_PORT_RST, EPD_PIN_RST)
#define RST_HIGH()  GPIO_SetActive(EPD_PORT_RST, EPD_PIN_RST)
#define POWER_ON()  GPIO_SetActive(EPD_PORT_POWER, EPD_PIN_POWER)
#define POWER_OFF() GPIO_SetInactive(EPD_PORT_POWER, EPD_PIN_POWER)

/* ============ 延时函数 ============ */

/*
 * 平台延时（毫秒级）
 *
 * 当前实现为循环计数延时，精度依赖 CPU 主频：
 *   - DA14585 默认使用内部 RC 16MHz，约 16000 cycles/ms
 *   - 若 SDK 配置了外部 XTAL 或分频，需调整循环值
 *
 * TODO: 实际集成时替换为 SDK 硬件定时器（timer0），以获得精确延时、
 *       更好的低功耗表现。SDK 提供 timer0_delay_ms() 或等价函数。
 */
static void platform_delay_ms(uint32_t ms)
{
    /* 每毫秒约 16000 个 NOP 周期 @ 16MHz，含循环开销 */
    for (uint32_t i = 0; i < ms; i++) {
        for (volatile uint32_t j = 0; j < 16000; j++) {
            /* GCC: 使用 __asm__ __volatile__("nop") 替代 */
            __asm("nop");
        }
    }
}

/* ============ 软件 SPI 发送 ============ */

static void hw_spi_send_byte(uint8_t data)
{
    for (int i = 7; i >= 0; i--) {
        if (data & (1 << i)) {
            GPIO_SetActive(EPD_PORT_MOSI, EPD_PIN_MOSI);
        } else {
            GPIO_SetInactive(EPD_PORT_MOSI, EPD_PIN_MOSI);
        }
        GPIO_SetActive(EPD_PORT_SCK, EPD_PIN_SCK);
        GPIO_SetInactive(EPD_PORT_SCK, EPD_PIN_SCK);
    }
}

/* ============ EPD 读写原语 ============ */

static bool epd_is_busy(void)
{
    /*
     * BUSY 低电平有效（LOW = 忙，HIGH = 空闲）
     * 来源: references/epd_pinout.md 实测——"平时 3.3V，EPD 处理时拉到 0V"
     */
    return !GPIO_GetPinStatus(EPD_PORT_BUSY, EPD_PIN_BUSY);
}

static void epd_wait_busy(void)
{
    /* BUSY 低电平表示屏幕忙，等待拉高 */
    while (epd_is_busy()) {
        /* 空循环，超时由看门狗兜底 */
    }
}

static void epd_write_cmd(uint8_t cmd)
{
    DC_CMD();
    CS_LOW();
    hw_spi_send_byte(cmd);
    CS_HIGH();
}

static void epd_write_data(uint8_t data)
{
    DC_DATA();
    CS_LOW();
    hw_spi_send_byte(data);
    CS_HIGH();
}

static void epd_write_data_bulk(const uint8_t *data, uint32_t len)
{
    DC_DATA();
    CS_LOW();
    for (uint32_t i = 0; i < len; i++) {
        hw_spi_send_byte(data[i]);
    }
    CS_HIGH();
}

/* ============ 硬件复位 ============ */

static void epd_hw_reset(void)
{
    RST_HIGH();
    platform_delay_ms(10);
    RST_LOW();
    platform_delay_ms(10);
    RST_HIGH();
    platform_delay_ms(10);
}

/* ============ SPI 引脚初始化 ============ */

static void epd_spi_init(void)
{
    /* SPI 数据/时钟引脚 → GPIO 输出 */
    GPIO_ConfigurePin(EPD_PORT_MOSI, EPD_PIN_MOSI, OUTPUT, PID_GPIO, false);
    GPIO_ConfigurePin(EPD_PORT_SCK,  EPD_PIN_SCK,  OUTPUT, PID_GPIO, false);

    /* 控制引脚 → GPIO 输出 */
    GPIO_ConfigurePin(EPD_PORT_CS,    EPD_PIN_CS,    OUTPUT, PID_GPIO, true);   /* CS 默认高 */
    GPIO_ConfigurePin(EPD_PORT_DC,    EPD_PIN_DC,    OUTPUT, PID_GPIO, false);
    GPIO_ConfigurePin(EPD_PORT_RST,   EPD_PIN_RST,   OUTPUT, PID_GPIO, true);   /* RST 默认高 */
    GPIO_ConfigurePin(EPD_PORT_POWER, EPD_PIN_POWER, OUTPUT, PID_GPIO, false);  /* 电源默认关 */

    /* BUSY → 输入 */
    GPIO_ConfigurePin(EPD_PORT_BUSY, EPD_PIN_BUSY, INPUT, PID_GPIO, false);
}

/* ============ SSD1680 控制器初始化 ============ */

static void epd_controller_init(void)
{
    epd_wait_busy();
    epd_write_cmd(0x12);  /* SW Reset */
    platform_delay_ms(10);
    epd_wait_busy();

    epd_write_cmd(0x01);  /* Driver Output Control */
    epd_write_data(0x79); /* [0] = (122-1) = 0x79, 低 8 位 */
    epd_write_data(0x00); /* [1] = 高位 */
    epd_write_data(0x00); /* [2] = GD=0, SM=0, TB=0 */

    epd_write_cmd(0x11);  /* Data Entry Mode */
    epd_write_data(0x01); /* X 增, Y 增 */

    epd_write_cmd(0x44);  /* Set RAM X Start/End */
    epd_write_data(0x00);
    epd_write_data(0x1F); /* 0x1F = 31: 32 字节覆盖 250 像素 */

    epd_write_cmd(0x45);  /* Set RAM Y Start/End */
    epd_write_data(0xF9); /* 0xF9 = 249 = (122-1) 低 8 位 */
    epd_write_data(0x00);
    epd_write_data(0x00);
    epd_write_data(0x00);

    epd_write_cmd(0x3C);  /* Border Waveform */
    epd_write_data(0x01);

    epd_write_cmd(0x18);  /* Temperature Sensor */
    epd_write_data(0x80); /* 使用内置温度传感器 */

    epd_write_cmd(0x22);  /* Load Temperature & Waveform */
    epd_write_data(0xB1);
    epd_write_cmd(0x20);
    epd_wait_busy();

    epd_write_cmd(0x4E);  /* RAM X Counter */
    epd_write_data(0x00);

    epd_write_cmd(0x4F);  /* RAM Y Counter */
    epd_write_data(0xF9);
    epd_write_data(0x00);
}

/* ============ 对外接口 ============ */

void epd_init(void)
{
    epd_spi_init();
    POWER_ON();
    platform_delay_ms(100);
    epd_hw_reset();
    epd_controller_init();
}

void epd_display(const uint8_t *image)
{
    /* 设置光标到左上角 */
    epd_write_cmd(0x4E);
    epd_write_data(0x00);
    epd_write_cmd(0x4F);
    epd_write_data(0xF9);
    epd_write_data(0x00);

    /* 写入帧缓冲 */
    epd_write_cmd(0x24);  /* Write Black/White RAM */
    epd_write_data_bulk(image, EPD_BUF_SIZE);

    /* 触发全刷 */
    epd_write_cmd(0x22);
    epd_write_data(0xF7);
    epd_write_cmd(0x20);
    epd_wait_busy();
}

void epd_clear(void)
{
    /* 光标归位 */
    epd_write_cmd(0x4E);
    epd_write_data(0x00);
    epd_write_cmd(0x4F);
    epd_write_data(0xF9);
    epd_write_data(0x00);

    /* 全白：逐行发送，每行 32 字节 */
    epd_write_cmd(0x24);
    for (uint32_t y = 0; y < EPD_HEIGHT; y++) {
        for (uint32_t x = 0; x < EPD_WIDTH_BYTES; x++) {
            epd_write_data(0xFF);
        }
    }

    /* 刷新 */
    epd_write_cmd(0x22);
    epd_write_data(0xF7);
    epd_write_cmd(0x20);
    epd_wait_busy();
}

void epd_sleep(void)
{
    epd_write_cmd(0x10);  /* Deep Sleep Mode */
    epd_write_data(0x01); /* Mode 1: 需硬件复位唤醒 */
    POWER_OFF();
}
