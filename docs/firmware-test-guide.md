# 固件测试指南

本文档说明如何编译、烧录和验证 `firmware/` 目录下的测试固件。

## 文件说明

```
firmware/
├── ssd1680.h       -- 驱动接口（epd_init / epd_display / epd_clear / epd_sleep）
├── ssd1680.c       -- 驱动实现（软件 SPI + SSD1680 初始化序列）
└── main_test.c     -- 测试主程序（生成条纹/棋盘格测试图案）
```

## 环境准备

| 软件 | 版本 | 用途 |
|------|------|------|
| Keil MDK | 5.36 | 编译固件 |
| J-Link 驱动 | V7.88j | SWD 烧录驱动 |
| SmartSnippets Toolbox | v5.0.10 | 可选，用于烧录和调试 |
| DA14585 SDK | 6.0.18+ | 官方 SDK（HAL 驱动、启动文件等） |

J-Link 克隆版需替换破解 DLL，详见 [J-Link OB 使用手册](jlink-ob-guide.md)。

## 硬件准备

- 盒马价签（拆机，确认屏幕为 2.13" E029A01）
- J-Link OB（淘宝搜 "J-Link OB DA14585"，选带破解补丁的）
- 烙铁 + 飞线
- 杜邦线

## 焊接 SWD 接口

在价签 PCB 上找到调试焊盘，焊接飞线引出：

```
价签焊盘   →   J-Link 引脚
SWDIO      →   SWDIO
SWCLK      →   SWCLK
GND        →   GND
3.3V       →   VCC
```

焊接注意事项：
- 焊盘较小，建议使用细头烙铁和助焊剂
- 飞线不宜过长，10cm 左右为宜
- 焊好后用万用表蜂鸣档检查是否虚焊

## 搭建 Keil 工程

1. 打开 Keil MDK 5.36，新建工程，芯片选择 `DA14585-00`
2. 从 SDK 的 `blink` 示例工程中复制以下文件到工程目录：
   - `startup_DA14585.s`（启动文件）
   - `system_DA14585.c` / `system_DA14585.h`（系统初始化）
   - SDK 中 `sdk/platform/` 下的相关源文件
3. 将 `firmware/` 目录下的 `ssd1680.c`、`ssd1680.h`、`main_test.c` 添加到工程
4. 配置 Include Paths，添加以下路径（根据实际 SDK 安装位置调整）：
   ```
   sdk/platform/include
   sdk/platform/driver/gpio/include
   sdk/platform/arch/soc/api
   ```
5. Rebuild 编译

## 烧录运行

### 方式一：Keil 直接烧录

1. 连接 J-Link OB 与价签
2. Keil → Options for Target → DEBUG 标签 → 选择 J-LINK
3. Settings 中确认识别到 DA14585 芯片
4. 点击 Download 烧录
5. 按 F5 运行

### 方式二：J-Flash 烧录

1. 打开 JFlash.exe，创建新工程，选择芯片 `DA14585-00`
2. 加载编译生成的 `.bin` 或 `.hex` 文件
3. 连接 → 烧录

### 方式三：SmartSnippets Toolbox

详见 [烧录教程](flashing.md)。

## 预期结果

屏幕应显示**黑白相间的竖条纹**，全屏覆盖：

```
████    ████    ████    ████
████    ████    ████    ████
████    ████    ████    ████
```

每 8 像素交替一次，共约 31 条竖纹。

## 切换测试图案

在 `main_test.c` 的 `main()` 函数中，注释/取消注释即可切换图案：

```c
/* 方案 A：竖条纹（默认） */
generate_test_pattern();
epd_display(frame_buffer);

/* 方案 B：横条纹 */
// generate_test_pattern_h();
// epd_display(frame_buffer);

/* 方案 C：棋盘格 */
// generate_test_pattern_checker();
// epd_display(frame_buffer);
```

重新编译烧录即可看到新图案。

## 常见问题

| 现象 | 可能原因 | 解决方法 |
|------|---------|---------|
| J-Link 连不上 | SWD 焊接不良 | 用万用表检查焊点，补焊虚焊处 |
| J-Link 连不上 | 驱动版本不对 | 确认安装 V7.88j，DLL 已替换 |
| 烧录成功但屏幕无显示 | SPI 引脚虚焊 | 检查 P2_0/P2_1/P2_2 焊接 |
| 烧录成功但屏幕无显示 | 电源未接通 | 检查 P2_3（boost 使能）和 3.3V 供电 |
| 屏幕闪一下就白了 | 初始化序列异常 | 检查 RST(P0_7) 和 BUSY(P1_1) 连接 |
| 显示内容乱码 | 引脚定义与板子不匹配 | 用万用表/示波器测量确认引脚，对照 [引脚定义](../references/epd_pinout.md) 修改驱动中的宏定义 |
| 连接失败（红色日志） | 芯片未复位 | RST 飞线碰 VCC 线 1-3 秒触发复位，再重试 |

## 参考资料

- [烧录教程](flashing.md)
- [J-Link OB 使用手册](jlink-ob-guide.md)
- [硬件资料](hardware.md)
- [引脚定义](../references/epd_pinout.md)
