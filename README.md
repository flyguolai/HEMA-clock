# 盒马价签改时钟 (Hema ESL Clock)

将盒马超市的电子价签（ESL）改造成墨水屏时钟/日历。

## 硬件概况

- **主控**: Renesas DA14585 (BLE SoC)
- **屏幕**: 2.13" 墨水屏 (E029A01, SSD1680 驱动)
- **通信**: BLE 5.0
- **供电**: 纽扣电池 / 锌空气电池

## 两条路径

### 路径 A：刷现成固件（推荐入门）

社区已有成熟固件，只需 J-Link + 焊接飞线即可完成。

- 详见 [烧录教程](docs/flashing.md)
- 详见 [固件选项](docs/firmware-options.md)

### 路径 B：自己开发固件

基于 Renesas DA14585 SDK 从零开发。

- 详见 [开发指南](docs/development.md)

## 快速开始

1. 拆解价签，确认屏幕型号和 PCB 版本
2. 准备 J-Link 调试器
3. 焊接 SWD 接口（SWDIO、SWCLK、GND、3.3V）
4. 选择固件并烧录
5. 组装，完成

## 参考资源

- [硬件资料](docs/hardware.md)
- [引脚定义](references/pinout.md)
- [参考链接](references/links.md)
- [社区资源](references/community.md)
