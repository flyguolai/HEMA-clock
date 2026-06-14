/**
 * SSD1680 墨水屏驱动 —— 盒马 2.13" 价签 (E029A01, 250x122)
 *
 * 引脚定义来自 references/epd_pinout.md 实板测量数据
 */

#ifndef SSD1680_H
#define SSD1680_H

#include <stdint.h>
#include <stdbool.h>

/* 屏幕参数 */
#define EPD_WIDTH       250
#define EPD_HEIGHT      122
#define EPD_BUF_SIZE    ((EPD_WIDTH / 8) * EPD_HEIGHT)  /* 3813 bytes */

/**
 * 初始化墨水屏硬件（GPIO + SPI + 控制器）
 * 会执行完整的硬件复位和初始化序列
 */
void epd_init(void);

/**
 * 全刷：发送整个帧缓冲并执行屏幕刷新
 * @param image 黑白位图，1 字节 = 8 像素，1=黑 0=白，按行扫描
 */
void epd_display(const uint8_t *image);

/**
 * 清屏：全白显示
 */
void epd_clear(void);

/**
 * 进入深度睡眠，降低功耗
 * 唤醒需要重新调用 epd_init()
 */
void epd_sleep(void);

#endif /* SSD1680_H */
