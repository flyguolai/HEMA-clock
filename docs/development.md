# 自开发指南

## 开发环境搭建

### 所需软件

| 软件 | 版本 | 用途 |
|------|------|------|
| Keil MDK | 5.36 | 编译固件 |
| SmartSnippets Toolbox | v5.0.10 | 烧录、调试、OTP 管理 |
| J-Link 驱动 | V7.88j | SWD 调试器驱动 |
| DA14585 SDK | 6.0.18+ | 官方开发包 |

### SDK 获取

1. 访问 Renesas 官网: https://www.renesas.com/en/products/da14585
2. 下载 `DA1458x_SDK_6.0.18.1182.1`（或更新版本）
3. SDK 包含:
   - HAL 驱动库
   - BLE 协议栈
   - 示例工程（blink、peripheral 等）
   - 二次 Bootloader 代码

### SDK 目录结构

```
SDK_6.0.18.1182.1/
├── projects/
│   └── target_apps/
│       ├── ble_examples/
│       │   └── blink/          # LED 闪烁示例
│       │   └── peripheral/     # BLE 外设示例
│       └── hrp_example/
├── sdk/
│   ├── driver/                 # HAL 驱动
│   ├── ble/                    # BLE 协议栈
│   └── platform/               # 平台相关
└── utilities/
    └── jlink/                  # J-Link 配置文件
```

## 开发流程

### 第一步：点灯验证环境

1. 打开 SDK 中的 `blink` 示例工程
2. 修改 LED 引脚定义（盒马价签 LED 在 P2_5）:

```c
// 原始定义
#define LED_PORT  GPIO_PORT_1
#define LED_PIN   GPIO_PIN_0

// 修改为盒马价签
#define LED_PORT  GPIO_PORT_2
#define LED_PIN   GPIO_PIN_5
```

3. Keil 中选择 Target: DA14585
4. Rebuild 编译
5. 通过 J-Link 烧录/调试
6. 按 F5 运行，确认 LED 闪烁

### 第二步：驱动墨水屏

1. 确认 SPI 接口引脚（见 [引脚定义](../references/pinout.md)）
2. 移植 SSD1680 驱动代码
3. 实现基本显示功能

### 第三步：实现 RTC 时钟

1. 配置 DA14585 内部 RTC
2. 实现时间计时和显示
3. 低功耗管理（定时唤醒刷新屏幕）

### 第四步：BLE 时间同步（可选）

1. 配置 BLE Peripheral
2. 实现时间同步协议
3. 开发手机端小程序或使用现有工具

## 关键代码示例

### GPIO 配置

```c
#include "hw_gpio.h"

// 配置 P2_5 为输出（LED）
GPIO_ConfigurePin(LED_PORT, LED_PIN, OUTPUT, PID_GPIO, false);

// 控制 LED
GPIO_SetActive(LED_PORT, LED_PIN);    // 点亮
GPIO_SetInactive(LED_PORT, LED_PIN);  // 熄灭
```

### SPI 配置（墨水屏）

以下引脚定义来自 [epd_pinout.md](../references/epd_pinout.md) 实板测量数据，与 firmware/ssd1680.c 保持一致。

```c
// SPI 数据/时钟
#define EPD_PORT_MOSI  GPIO_PORT_2
#define EPD_PIN_MOSI   GPIO_PIN_2      // P2_2

#define EPD_PORT_SCK   GPIO_PORT_2
#define EPD_PIN_SCK    GPIO_PIN_0      // P2_0

// 控制线
#define EPD_PORT_CS    GPIO_PORT_2
#define EPD_PIN_CS     GPIO_PIN_1      // P2_1

#define EPD_PORT_DC    GPIO_PORT_0
#define EPD_PIN_DC     GPIO_PIN_1      // P0_1

#define EPD_PORT_RST   GPIO_PORT_0
#define EPD_PIN_RST    GPIO_PIN_7      // P0_7

#define EPD_PORT_BUSY  GPIO_PORT_1
#define EPD_PIN_BUSY   GPIO_PIN_1      // P1_1

#define EPD_PORT_POWER GPIO_PORT_2
#define EPD_PIN_POWER  GPIO_PIN_3      // P2_3 (boost 升压电路开关)
```

## 调试技巧

- 使用 UART (P0_4 TX, P0_5 RX) 输出日志
- 用 TTL 转 USB 模块查看串口输出
- SmartSnippets Toolbox 提供 OTP 查看和内存读取功能

## 参考资源

- [与非网 - 玩转蓝牙墨水屏电子标签（一）点灯](https://www.eefocus.com/article/1612900.html)
- [CSDN - DA14585 调试记录](https://blog.csdn.net/m0_46324060/article/details/120562093)
- [Renesas DA14585 产品页](https://www.renesas.com/en/products/da14585)
