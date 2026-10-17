# Teloa Firmware

**让 Teloa 的小松鼠助手住进硬件。** 面向 ESP32-S3 的 Teloa 自有固件：同一 Wi-Fi 下直连你自己的 Teloa 社区版宿主，以语音提出任务、跟进结果并播报。屏幕沿用绿色毛绒小松鼠，支持触屏查看和处理已授权的待确认操作。

[English](README.en.md) · [构建](docs/build.md) · [USB 配对与协议](docs/pairing.md) · [恢复与安装边界](docs/recovery.md) · [Teloa 社区版](https://github.com/teloa-ai/teloa)

![Teloa 小松鼠](main/teloa/assets/standing.png)

默认构建使用 Teloa 直连协议，跳过小智官方激活、官方后台和启动升级。模型、ASR 与 TTS 由本人 Teloa 宿主提供；固件不内置 API Key，也不附带免费语音服务。上游通用驱动和部分兼容代码仍保留来源署名。

## 可以做什么

- 触屏开始、结束语音输入，向原助手提交工作并查询结果；接收 Opus 语音和字幕，等待实际播放完成。
- 小松鼠随待机、倾听、思考、播报和完成状态变化，使用内嵌素材和中文字体。
- 查看审批详情；宿主另行授权后，可在详情末页长按 1.2 秒批准本次操作，或拒绝。审批绑定当前卡片与连接，不能用旧卡片批准新申请。
- 本人先在 Web 工作台建立配对意图，再通过 USB 和设备物理确认写入实例专属配置。可在宿主撤销设备，关闭抽屉不会提交审批。

## 当前开发状态

这是 **`0.1.0-dev-b2.1` 开发源码**。候选配置为星喵 Bubble / ESP-VoCat、ESP32-S3、32 MiB Flash、360 × 360 屏幕；**实际 PCB、触控、麦克风、扬声器和整机恢复尚待实机验收**。产品名称相同不代表引脚与分区相同，不能据此直接安装。

已完成软件层的语音往返、配对、审批、有限协议解析与候选编译验证。软件结果不能替代实体设备验收；本仓没有发布可直接刷写的通用二进制，也没有自动刷机入口。应用级 OTA 下载与完整升级流程尚未交付。

直连需要兼容 `teloa-device/v1`、USB 配对 IPC 和审批桥接的 Teloa 社区版开发宿主。相关能力正在社区版开发源中联调；**当前公开 npm/Docker 安装不能据此认定已经支持**。本仓先独立开放固件与协议，宿主正式发行另行提供。无兼容宿主也可编译、运行工具测试和查看离线屏幕预览。

## 从源码开始

准备固定的 ESP-IDF **v6.1** 和锁定组件后，在仓库根目录运行：

```sh
idf.py build
```

新目录默认选择 `teloa-bubble` 和 Teloa 直连模式。旧的非直连 `sdkconfig` 会被拒绝，详细步骤见[构建指南](docs/build.md)。构建本身不连接或写入设备。

离线预览无需 ESP-IDF、凭据或设备，输出文件须尚未存在：

```sh
python3 tools/preview.py --output "screen-preview.html"
```

用浏览器打开生成文件即可体验状态和审批卡片。这是软件交互演示，不会处理真实审批。

## 来源与许可

基于 [78/xiaozhi-esp32](https://github.com/78/xiaozhi-esp32) 固定提交 `d7a1c05308fb6e116ffb44b9e2c6e67b3366302a`，感谢上游作者与贡献者。导入范围为通用实现、ESP-VoCat 候选驱动及简中、繁中、英文资源，原始摘要保留在 [upstream.lock.json](upstream.lock.json)。

固件代码和工具采用 [MIT](LICENSE)；从 Teloa 社区版复用的小松鼠素材保留 Apache-2.0，字体保留 SIL OFL 1.1，gifdec 保留其公共领域声明，详见 [THIRD_PARTY_NOTICES.md](THIRD_PARTY_NOTICES.md)。软件许可不授予 Teloa 商标权。

公开仓为社区版 `firmware/esp32` 的独立源码导出，使用全新发布历史；`source-manifest.json` 记录逐文件摘要与来源版本。开发修改集中维护于社区版源目录，再通过允许清单导出，避免维护两个分叉。设备备份、运行数据、私钥与个人配置不属于本仓。
