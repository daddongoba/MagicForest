# DialogueMemoryTable 接入说明

## 当前状态

DialogueMemoryTable 已作为项目插件安装到：

~~~
Plugins/DialogueMemoryTable
~~~

项目文件 demo3_UE58.uproject 已启用：

~~~
{
  "Name": "DialogueMemoryTable",
  "Enabled": true
}
~~~

本次只做本地文件接入，没有启动 Unreal Editor，也没有改动 Content/ 下已有资产。

## 功能范围

这不是只针对一个试用 NPC 的存档插件。当前插件已经扩展为 HTML 玩法的 UE 运行时骨架：

- 三个 NPC：blacksmith、operator、novelist。
- 每个 NPC 三层熟悉度，含层级 Prompt、触发词、秘密前史和不可回退规则。
- DeepSeek-compatible 对话请求层。
- 22 张大牌、4 张宫廷牌、9 个委托和元素向量结算。
- 世界后果 Prompt 构造。
- 紧凑记忆补丁验证与 SaveGame 落表。

HTML 的 3D 轨迹 Canvas 按需求不迁移。

插件接收对话结束后的紧凑 JSON 增量补丁，并写入四组有上限的运行时存档表：

1. NPC 状态：关系摘要、态度、信任、话题标签、会话号。
2. 人物事实：稳定键、主谓宾事实、可见性、可信度、权威来源。
3. 世界状态：只接受 AI 的 npc_report 或 inference 建议；game_event 必须由游戏逻辑写入。
4. 未结事项：承诺、问题、秘密、冲突和目标。

默认存档槽：

~~~
DialogueMemoryLedger
~~~

## 蓝图接线顺序

在对话系统结束一次会话时：

1. 获取 DialogueMemoryTableSubsystem。
2. 用游戏真实数据调用 Set Authoritative NPC Progress，写入 familiarity 和 known_secret_flags。
3. 调用 Apply Memory Patch JSON，传入：
   - AI 返回的原始 JSON；
   - 预期 NPC ID；
   - 当前会话号；
   - 当前世界日；
   - bSaveImmediately=true。
4. 检查返回结构中的 bSuccess、ErrorCode、ErrorMessage。
5. 需要调试表格时调用 Export Ledger To CSV。

插件不会让 AI 直接修改熟悉度、秘密解锁、任务状态或游戏事件权威值。

## 三层 NPC 接入

AlchemyGameplaySubsystem 内置三个完整人设：

| NPC ID | 第 1 层 | 第 2 层 | 第 3 层 |
|---|---|---|---|
| blacksmith | 森林铁匠 | 以前是拆东西的 | 安置办与老师的炉子 |
| operator | 森林跑腿 | 以前是接官线的 | 总局监听与无人的电话 |
| novelist | 代笔人 | 以前是写书的 | 被禁的三本书与雨 |

对话流程使用 EvaluatePersonaTrigger 检查当前玩家句子。触发后熟悉度只能向上
推进，随后用 BuildPersonaSystemPrompt 组装当前层级的人设 Prompt，再由
DialogueAIServiceSubsystem::RequestNpcDialogue 请求模型。

## 资源位置

- JSON Schema：Plugins/DialogueMemoryTable/Resources/DialogueMemoryPatch.schema.json
- 前置提示词：Plugins/DialogueMemoryTable/Resources/DialogueMemorySummarizerPrompt.md
- 示例补丁：Plugins/DialogueMemoryTable/Resources/ExampleMemoryPatch.json
- 公开蓝图接口：Plugins/DialogueMemoryTable/Source/DialogueMemoryTable/Public/DialogueMemoryTableSubsystem.h

## 本地静态检查

在插件目录执行：

~~~
python3 Tools/validate_package.py
~~~

此检查验证插件描述文件、示例 JSON、字段契约和 C++ 括号结构；目标工程仍需在 Unreal Build Tool 中编译确认。
