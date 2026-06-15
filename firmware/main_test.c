/**
 * 墨水屏测试程序 —— 盒马价签 HEMA ESL
 *
 * 功能：初始化 SSD1680 墨水屏，显示黑白相间的条纹测试图案
 * 用途：验证 SPI 通信、屏幕驱动、引脚接线是否正确
 *
 * 用法：
 *   1. 在 Keil 中新建 DA14585 工程，将本文件和 ssd1680.c/ssd1680.h 加入
 *   2. 添加 SDK 路径到 Include Paths
 *   3. 编译 → J-Link 烧录 → 观察屏幕
 *
 * 工具链兼容性：
 *   - Keil MDK (ARMCC):   __asm("wfi") / __asm("nop")
 *   - ARM GCC:            需替换为 __asm__ __volatile__("wfi") 等
 *   本文件当前按 Keil 语法编写。
 *
 * 前置条件：
 *   DA14585 SDK 中 system_init() / SystemInit() 必须在 epd_init() 前调用，
 *   以配置系统时钟、使能 GPIO 模块电源。startup 文件通常会自动调用 SystemInit()，
 *   但需确认其实现开启了所需外设时钟。
 */

#include "ssd1680.h"

/*
 * 帧缓冲：250x122 像素，每行 32 字节（250 像素向上对齐），共 3904 字节
 *
 * 注意：DA14585 在二次启动模式下固件运行于 RAM（0x07FC0000），
 * 该 buffer 会占用 ~4KB 的 .bss / .data 段。链接脚本需保证 RW/ZI
 * 区域足够容纳此 buffer 及其它运行时数据。
 */
static uint8_t frame_buffer[EPD_BUF_SIZE];

/**
 * 生成测试图案：黑白竖条纹，每 8 像素交替
 *
 * 像素编码规则（SSD1680）：
 *   bit=1 → 黑色    bit=0 → 白色
 *   每字节高位在左，低位在右
 */
static void generate_test_pattern(void)
{
    for (int y = 0; y < EPD_HEIGHT; y++) {
        for (int x_byte = 0; x_byte < EPD_WIDTH_BYTES; x_byte++) {
            if ((x_byte % 2) == 0) {
                frame_buffer[y * EPD_WIDTH_BYTES + x_byte] = 0x00; /* 黑 */
            } else {
                frame_buffer[y * EPD_WIDTH_BYTES + x_byte] = 0xFF; /* 白 */
            }
        }
    }
}

/**
 * 生成测试图案 2：黑白横条纹
 */
static void generate_test_pattern_h(void)
{
    for (int y = 0; y < EPD_HEIGHT; y++) {
        uint8_t val = ((y / 8) % 2 == 0) ? 0x00 : 0xFF;
        for (int x_byte = 0; x_byte < EPD_WIDTH_BYTES; x_byte++) {
            frame_buffer[y * EPD_WIDTH_BYTES + x_byte] = val;
        }
    }
}

/**
 * 生成测试图案 3：棋盘格
 */
static void generate_test_pattern_checker(void)
{
    for (int y = 0; y < EPD_HEIGHT; y++) {
        for (int x_byte = 0; x_byte < EPD_WIDTH_BYTES; x_byte++) {
            int block_y = (y / 16) % 2;
            int block_x = (x_byte / 2) % 2;  /* 每 16 像素一块 */
            if (block_y == block_x) {
                frame_buffer[y * EPD_WIDTH_BYTES + x_byte] = 0x00;
            } else {
                frame_buffer[y * EPD_WIDTH_BYTES + x_byte] = 0xFF;
            }
        }
    }
}

int main(void)
{
    /*
     * 若 startup 文件未自动调用 SystemInit()，需在 epd_init() 前手动调用：
     *   system_init();
     * 否则 GPIO 模块时钟未使能，引脚操作无效。
     */

    /* 初始化墨水屏（GPIO + SPI + SSD1680 控制器） */
    epd_init();

    /* 清屏为全白 */
    epd_clear();

    /* 显示测试图案（三选一，取消注释即可） */

    /* 方案 A：竖条纹 */
    generate_test_pattern();
    epd_display(frame_buffer);

    /* 方案 B：横条纹（取消注释测试） */
    // generate_test_pattern_h();
    // epd_display(frame_buffer);

    /* 方案 C：棋盘格（取消注释测试） */
    // generate_test_pattern_checker();
    // epd_display(frame_buffer);

    /* 测试完成，进入低功耗 */
    epd_sleep();

    /* 主循环（实际产品中可在此做 BLE / 定时唤醒等） */
    while (1) {
        /* GCC 用户: __asm__ __volatile__("wfi") */
        __asm("wfi");  /* 等待中断，低功耗 */
    }

    return 0;
}
