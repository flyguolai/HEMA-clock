# CLAUDE.md

本文件为 Claude Code (claude.ai/code) 在本仓库中工作时提供指导。

## 项目概述

纯文档仓库，记录将盒马超市电子价签 (ESL) 改造为墨水屏时钟/日历的方案。无可执行源代码，无构建系统，无测试，无 CI/CD。

目标硬件：Renesas DA14585 BLE SoC + SSD1680 驱动的 2.13" 墨水屏 (E029A01)，纽扣/锌空气电池供电。

## 约束条件

- **语言**：所有面向用户的内容必须使用简体中文。编辑时保持此规则。不要用英文写文档，除非不得不使用英文表达一些专用名词。
- **无可构建/测试的代码**：所有编辑均为 Markdown 文件。
- **硬件特异性**：内容针对 DA14585 + SSD1680 组合，不要泛化硬件细节。
- **外部链接**：许多链接指向 myetoys.com、eefocus.com 和 Renesas，添加新链接前请验证可用性。
- **OTP 安全**：绝不能建议擦除或写入 OTP（一次性可编程存储器）。OTP 写入不可逆——误操作会导致芯片变砖。盒马价签采用二次启动模式，OTP 中已烧录 Bootloader，不可触碰。

## 仓库结构

- `docs/` — 教程与指南：烧录、固件选项、硬件资料、自开发指南、J-Link OB 排错
- `references/` — 引脚映射（24 针 EPD 连接器）、社区资源、外部链接汇总
- `README.md` — 项目概述，包含两条开发路径（刷现成固件 vs 自开发固件）

## 编辑规范

- 固件条目：遵循 `docs/firmware-options.md` 中的现有表格格式
- 引脚定义：`references/epd_pinout.md`
- 社区/外部资源：`references/links.md` 或 `references/community.md`
- 开发流程与代码示例：`docs/development.md`

## 关键硬件参考

- SWD 接口需焊接：SWDIO、SWCLK、GND、3.3V
- 克隆版 J-Link OB 必须使用 V7.88j 版驱动（详见 `docs/jlink-ob-guide.md`）
- EPD SPI 引脚（实板测量值，见 `references/epd_pinout.md`）：MOSI=P2_2, SCK=P2_0, CS=P2_1, DC=P0_1, RST=P0_7, BUSY=P1_1
- EPD 电源使能（boost 升压电路）：P2_3
- 板载 LED：P2_5
- UART 调试串口：TX=P0_5, RX=P0_4
- **引脚冲突警告**：`docs/development.md` 中的代码示例引脚定义与实测数据不一致——始终以 `references/epd_pinout.md`（万用表/示波器实测）为准

## 启动模式

DA14585 支持两种启动模式，盒马价签使用**二次启动**：
- **二次启动（正常）**：OTP 中烧录 Bootloader Header，固件运行于外部 SPI Flash——可随意重复烧录
- **一次启动 / XIP（异常）**：固件直接烧入 OTP——不可逆，芯片锁定为该固件
