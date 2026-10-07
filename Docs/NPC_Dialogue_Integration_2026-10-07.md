# 森林 NPC 对话与记忆接入

批次：2026-10-07-03。基线：MagicForest main 67d6097，demo3_UE58 1.5，UE 5.8.3 Windows。

## 已实现

- 森林女巫、Natta、Fawnia 注册为 forest_witch、natta、fawnia，保留原漫游、碰撞与动画。
- 靠近且无遮挡时 E 交谈，NPC 暂停并面向玩家。输入框、滚动记录、Enter/发送按钮、Esc/结束按钮可用。结束后恢复玩家移动、视角和 NPC 漫游。
- 输入时使用 UI 输入模式，避免文字 E/F/R/G 触发玩法。距离过远、观察视角切换或 NPC 消失时退出。NPC 处理狭窄通道时可能暂不能交谈。
- 各 NPC 独立保存近期消息（最多 16 条）及长期记忆。结束时异步总结，严格校验 NPC、会话编号和字段后落盘。
- 网络错误恢复输入；关闭后延迟回复不会恢复旧会话。总结失败保留待总结记录，重新交谈时重试，也可调用 RetryPendingSummaries。待总结记录最多 32 条消息，超过后保留最近部分。
- 2026-10-07-05 更正接入：森林女巫（forest_witch）继承 novelist，Natta（natta）继承 blacksmith，Fawnia（fawnia）继承 operator 的完整插件人设，包括语气、日常状态、背景、三层提示和升级关键词；保留场景名字和独立记忆 ID。发送有效玩家消息前执行关键词升级，优先匹配第三级，允许从一级直升三级，只升不降并落盘；本次回复使用升级后的层级。
- 原卡牌、委托、合成评估接口保留。本批接入范围是 NPC 对话，不表示所有委托玩法已连接场景。

## 应用与体验

1. 备份完整工程，需要前置 NPC 批次 2026-10-07-01 和原森林资源、角色及动画。
2. 合并 Plugins/DialogueMemoryTable 和本批 Source/demo3_UE58；启用 DialogueMemoryTable 插件。仓库 uproject 原已启用，本机完整工程本次已安装并启用。
3. 关闭编辑器，构建 demo3_UE58Editor Win64 Development，再打开工程。
4. 获取 Git LFS 的 Content/Biomes/PNB_Enchanted_Forest/Map/Demo_Day.umap。本次只为现有 NPC_EncounterInteraction 开启 Offline Test Mode，方便体验。已有个人关卡编辑时，可仅在现有管理器勾选该属性并保存，避免整体替换关卡。
5. 开始游戏，靠近 NPC 按 E，输入后 Enter，Esc 退出。

## 离线与在线

场景默认离线联调，窗口明确显示“模拟回复，不调用 AI”。用于验证交互和存档，不提供真实智能聊天。

在线默认地址 https://api.deepseek.com/v1，模型 deepseek-chat。在本机设置用户环境变量 FOREST_DIALOGUE_API_KEY，完全退出并重新打开 UE，然后取消 NPC_EncounterInteraction 的 Offline Test Mode。

可选环境变量 FOREST_DIALOGUE_BASE_URL、FOREST_DIALOGUE_MODEL 支持兼容 Chat Completions 的服务，基础 URL 不含 /chat/completions。无 Key 时拒绝发送并显示提示；Key 只在运行内存中，不写入地图、SaveGame 或仓库。

在线发送该角色提示、紧凑记忆和近期消息；结束时另发总结请求，超时 35 秒。对话内容会发给所配置服务。

## 存档与接口

默认槽 ForestNPCConversations（消息和待总结队列）、DialogueMemoryLedger（四张记忆表），位于本机 Saved/SaveGames，不上传。

ForestNPCDialogueSubsystem 提供 BeginConversation、SendPlayerMessage、EndConversation、GetCompactMemoryJson、RetryPendingSummaries、GetPendingSummaryCount。ConfigurePersistence 仅在无活动会话和未完成请求时允许切换槽。UI 与 NPC 暂停由 ForestNPCInteraction 管理，不能仅调用后端接口来替代玩家交互。

DialogueMemoryTableSubsystem 支持只读快照、CSV 导出。森林人设目前在 ForestNPCDialogueSubsystem::RegisterForestPersonas 中配置。

## 原批次 2026-10-07-03 实测（不代表后续修改已实测）

使用独立槽 ForestNPCConversations_IntegrationTest / DialogueMemoryLedger_IntegrationTest，未将测试内容写入玩家默认记忆。

| 检查 | 结果 |
|---|---|
| UE 5.8.3 Windows Editor 模块与插件编译 | 成功 |
| 三 NPC 离线对话、独立摘要、暂停与恢复移动/视角 | PIE 通过 |
| 空白、超过 500 字输入 | 拒绝 |
| 本机模拟 HTTP 聊天与 JSON 总结 | 通过 |
| 网络超时、非法响应 JSON | 恢复输入，可重试 |
| 请求途中结束，延迟回复返回 | 控制恢复，旧回复丢弃 |
| 总结返回非法补丁 | 保存待总结；结束 PIE、再次进入后重试成功 |
| 再次进入 PIE、记忆隔离和回忆 | 通过 |
| 无 API Key | 拒绝发送、显示提示、可以结束 |
| 重复/过期会话、身份不匹配、额外字段 | 幂等或拒绝 |
| 熟悉度先 3 后 1 | 保持 3 |
| Slate 输入并 Enter、Esc、结束按钮、近距离 E | 实测通过 |
| 插件 validate_package.py | 通过 |

本机模拟 HTTP 网关已停止。真实 DeepSeek 因未提供 Key 尚未验证；未做打包、其他平台、多人或长期负载测试。已添加总结提示词 UFS 打包依赖，但尚不构成打包验证。测试存档、脚本和截图保留本机 Saved，未发布。

## 回退

源码、插件、Build.cs、地图须配套恢复。完整工程源码配置备份在本机 Saved/RouteDesign/BackupBeforeDialogueIntegration。清理测试槽不影响默认玩家记忆；清空默认槽前先备份。

## 2026-10-07-05 核对

完整人设映射与桌面原型升级关键词已静态核对；插件包检查及源码差异检查通过。本次未运行 UE 编译、PIE 或真实 AI，请在完整工程中合并源码后编译验证。离线模式只验证升级与存档，不生成对应人设的智能回复。
