# J-Link OB 使用手册

## 什么是 J-Link OB

J-Link OB 是 Segger 官方授权的低成本 SWD 调试器，常用于 ARM Cortex-M 芯片的烧录和调试。淘宝上 10-50 元区间的 "J-Link OB" 多为克隆版，功能等同但需特殊处理。

## 软件版本选择

### V5 vs V7

| | V5 | V7 |
|---|---|---|
| DA14585 支持 | ❌ 不支持 | ✅ 支持 |
| 发布时间 | 2014 年前 | 2016 年后 |
| 与克隆版兼容 | — | V7.88j 最佳 |

DA14585 芯片在 J-Link V6 之后才加入设备支持列表，V5 版本根本无法识别该芯片。

### 为什么是 V7.88j

- **上限**：V7.88j 是最后一个对克隆版兼容良好的版本。更高版本（V7.90+）Segger 加强了盗版检测，克隆 J-Link 可能被拒绝工作
- **下限**：必须 ≥ V6 才能识别 DA14585
- **结论**：V7.88j 是社区反复验证后的最佳选择

### 获取方式

1. 墨韵时光网站下载区提供 `JLink_Windows_V788j_x86_64.exe`
2. 配套的 J-Flash 替换文件和破解 DLL 一般由淘宝商家提供，也可在社区找到

## 克隆版 J-Link OB 的特殊处理

淘宝 10 多元的 J-Link OB 均为克隆版，**必须完成以下步骤才能正常使用**：

### 1. 安装指定版本驱动

安装 `JLink_Windows_V788j_x86_64.exe`，不要使用 Segger 官网的最新版本。

### 2. 替换 DLL 破解文件

商家提供的补丁包中通常包含以下文件，覆盖到 J-Link 安装目录（默认 `C:\Program Files (x86)\SEGGER\JLink_V788j\`）：

- `JLinkARM.dll`
- `JLink.exe`（部分需要）

### 3. SmartSnippets Toolbox 也要替换

如果使用 SmartSnippets Toolbox，同样需要将破解 DLL 覆盖到其安装目录下的 J-Link 相关文件夹，否则 Toolbox 调用 J-Link 时会因盗版检测而失败。

### 4. 硬件检查：GND 虚焊

克隆版 J-Link OB 最常见的问题就是 **GND 排针虚焊**。连接前建议：

- 用万用表蜂鸣档测量 GND 排针与 PCB 焊盘的导通性
- 如有虚焊，补焊即可
- 连接失败时优先排查这个

## 连接步骤

1. 焊接价签 SWD 焊盘（SWDIO / SWCLK / GND / 3.3V），飞线引出
2. 杜邦线连接 J-Link 对应引脚（参考 `references/pinout.md` 中的 J-Link 连接表）
3. 安装 J-Link 驱动 V7.88j + 替换 DLL
4. 打开 J-Flash 或 SmartSnippets Toolbox
5. 选择芯片 `DA14585-00`
6. 点击连接

## 连接失败的排查

| 现象 | 可能原因 | 解决 |
|------|---------|------|
| 完全连不上 | GND 虚焊 | 补焊 J-Link 的 GND 排针 |
| 偶尔能连上 | 杜邦线接触不良 | 更换杜邦线，确保插紧 |
| 红色错误日志 | 芯片未复位 | 用 RST 线碰 VCC 线 1-3 秒再重试 |
| J-Link 被识别为克隆 | DLL 未替换或版本不对 | 检查 DLL 是否成功覆盖到安装目录 |
| 设备列表找不到 DA14585 | J-Link 软件版本太低 | 确认版本 ≥ V6，推荐 V7.88j |

### RST 复位技巧

RST 线不接 J-Link 的 RESET 引脚。连接前，手持 RST 飞线碰一下 VCC 线（短接 1-3 秒触发硬件复位），然后立即在软件中点 "Connect"。这个技巧可解决大部分"芯片不响应"的问题。

## 验证 OTP 与启动模式

烧录前建议确认 OTP 状态，避免误写入 OTP 导致无法恢复。

### DA14585 启动模式

| 启动方式 | OTP 内容 | 固件位置 | 可重复烧录 |
|---------|---------|---------|-----------|
| 二次启动（正常） | Bootloader Header | 外部 SPI Flash | ✅ 随意刷 |
| 一次启动 XIP（异常） | 完整应用固件 | OTP 自身 | ❌ 锁死 |

### 使用 SmartSnippets Toolbox 验证

1. 连接芯片后，在 Toolbox 中选择 **OTP Programmer** 或 **OTP Header** 选项卡
2. 读取 OTP Header 内容：
   - 显示 "Boot from SPI Flash" 或类似标记 → 二次启动，正常
   - 显示 "Boot from OTP" / "XIP" → 固件直接烧在 OTP 里，无法更改
   - OTP 完全空白 → 芯片从未被编程，只能通过 SWD 加载运行

### 读取 SPI Flash 内容

在 SmartSnippets Toolbox 中选择 **SPI Flash Programmer**，读取 Flash 内容：
- 有数据 → Flash 中有固件
- 全 0xFF → Flash 为空，需要烧录
- 读取失败 → Flash 芯片可能损坏或焊接不良

### 行为验证法

刷一个新固件到 SPI Flash，上电看屏幕：
- 屏幕内容变了 → OTP 正常，二次启动模式
- 屏幕无变化，还是原画面 → 固件很可能跑在 OTP 里，SPI Flash 被忽略

## 盒马价签常见状态

全新拆机的盒马价签出厂时：
- OTP 已烧录 Bootloader（二次启动模式）
- SPI Flash 存有原厂价签固件
- 上电后屏幕显示价签内容

刷社区固件时，只需替换 SPI Flash 中的内容，OTP 不会被触碰，可以放心操作。

## 注意事项

- **绝不手动擦写 OTP**：OTP 是一次性可编程存储器，写入后无法擦除。误写 OTP 会导致芯片变砖
- **V7.88j 是保守之选**：如果手上的克隆 J-Link 恰好支持更高版本，也可以用，但 V7.88j 是社区验证最稳妥的版本
- **J-Link 软件和 SmartSnippets 的 DLL 要同时替换**：两者共用 J-Link 驱动，缺一不可
- **购买时选择带破解补丁的商家**：节省自己找补丁的时间
