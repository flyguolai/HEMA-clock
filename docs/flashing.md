# 烧录教程

## 所需工具

| 工具 | 说明 | 参考价格 |
|------|------|---------|
| J-Link OB | SWD 调试器（淘宝搜 "J-Link OB DA14585"） | 20-50 元 |
| 烙铁 + 飞线 | 焊接 SWD 接口 | — |
| 杜邦线 | 连接 J-Link 和价签 | 几块钱 |

### J-Link 软件版本

**重要**: J-Link 软件需要使用 **V7.88j** 版本（过高版本可能不兼容）。

克隆版 J-Link OB 的详细配置与排错，参见 [J-Link OB 使用手册](jlink-ob-guide.md)。

## 烧录步骤

### 1. 焊接 SWD 接口

在价签 PCB 上找到以下焊盘并焊接飞线：

```
价签焊盘     →    J-Link 引脚
─────────────────────────────
SWDIO        →    SWDIO
SWCLK        →    SWCLK
GND          →    GND
3.3V (VBAT)  →    VCC
RST          →    RESET (可选)
```

### 2. 安装 J-Link 软件

1. 下载 `JLink_Windows_V788j_x86_64.exe`
2. 安装 J-Link 驱动
3. 下载 J-Flash 替换文件，覆盖到 J-Link 安装目录

### 3. 使用 J-Flash 烧录

1. 打开 `JFlash.exe`
2. 创建新工程 → 选择芯片 `DA14585-00`
3. 菜单 → 文件 → 打开数据文件（选择要烧录的 .bin/.hex 文件）
4. 点击连接 → 烧录

### 4. 使用 SmartSnippets Toolbox（推荐）

更详细的开发和烧录工具：

1. 安装 `SmartSnippets Toolbox v5.0.10`
2. 安装 Keil MDK 5.36
3. 替换商家提供的 DLL/EXE 文件到对应安装目录
4. 连接 J-Link OB + 价签板子
5. 在 SmartSnippets 中选择 JTAG ID 123456 和芯片 DA14585-00
6. 点击 Connect 连接芯片

### 常见问题

- **连接失败**: 用 RST 线碰 VCC 线 1-3 秒进行复位，然后重新连接
- **红色日志**: 检查杜邦线是否接触不良，更换杜邦线
- **GND 虚焊**: 部分 J-Link OB 存在 GND 虚焊问题，需要补焊

## 使用 Keil 调试

1. 打开 Keil 工程
2. 魔法棒 → DEBUG 标签 → USE 选择 JLINK
3. Settings 中确认识别到芯片
4. 点击 DEBUG 按钮
5. 在汇编页面按 F5 运行

## 参考资源

- [墨韵时光 - DA14585 烧录说明](https://www.myetoys.com/Flashing/26.html)
- [与非网 - 玩转蓝牙墨水屏电子标签](https://www.eefocus.com/article/1612900.html)
- [CSDN - DA14585 调试记录](https://blog.csdn.net/m0_46324060/article/details/120562093)
