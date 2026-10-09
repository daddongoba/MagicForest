# 天与每日蘑菇系统

## 当前实现

`UDailyMushroomSubsystem` 提供全局“天”状态，默认从第 1 天开始，保存到 `WorldDayState`。正式流程在一次干锅合成结果确认后调用 `ConfirmDryPotSynthesis`，确认成功后递增一天并保存；`AdvanceDay` 保留给测试或调试，`SetCurrentDay` 可用于读档或关卡切换时恢复指定日期。

每日蘑菇定义由 19 个现有的基础蘑菇类型组成（5 个普通类型、4 个 Cap 类型、10 个 Small 类型）。系统根据“天数 + 蘑菇类型索引”生成稳定种子，每天重新计算：

- `bAvailable`：当天该类型是否可采摘；
- `CardId`：当天这株蘑菇对应的小牌行标识；
- `Element`：由蘑菇类型固定决定的火、风或水，不随天随机漂移；
- `PipValue`：本地随机生成的 1 到 9 点，不调用 LLM；
- `Value`：把 `Element + PipValue` 映射到 `FAlchemyVector` 的单轴数值；
- `Weight`、`Tags`：为后续稀有度、掉落权重和条件扩展预留的表字段；
- `Day`：该定义所属的天。

同一天内同一类型的结果稳定，换天后可采摘集合和点数会变化，元素保持不变。随机只负责生成当天表行的可用状态、点数和权重，AI 不参与数值决定。数值使用现有 `FAlchemyVector`，可以直接传给炼金合成或评估逻辑。

## 次日元素修正表（条件占位）

`FDailyElementModifierRow` 为前一天合成结果影响后一天蘑菇获取预留结构。它记录来源天、目标天、角色、受影响元素、阻断/减少/增强效果，以及条件和效果占位文本。当前提供三条模板：低分阻断主导元素、低分减少主导元素、高分奖励主导元素；具体评分阈值和数值暂不写死，也不会自动应用到蘑菇生成。

评分系统完成后，结算流程可以将解析后的修正通过 `AddElementModifier` 保存，再用 `GetElementModifiersForDay` 在目标日读取。这样不会把评分条件、蘑菇固定元素和每日随机数值混在同一层。

## 蓝图接入

现有玩家采摘逻辑位于 `Content/Witch_House/Demo/FirstPerson/Blueprints/BP_FirstPersonCharacter.uasset`，当前只按 Mesh 名称计数。需要在完整工程的 `HarvestMushroom(MeshName)` 中加入：

1. 通过 `Get Game Instance Subsystem → DailyMushroomSubsystem` 取得当天状态；
2. 调用 `IsMushroomAvailable(MeshName)`，不可采摘时显示当天提示并保留蘑菇；
3. 调用 `GetMushroomValue(MeshName)`，把返回的 `FAlchemyVector` 与背包条目一起保存；
4. 采摘成功后再隐藏命中的蘑菇、增加数量并刷新 HUD；
5. 干锅合成结果确认成功后调用 `ConfirmDryPotSynthesis`，然后刷新场景中的可采摘状态；不要在普通对话或 LLM 回调里换天。

本增量仓库没有可运行的 UE 编辑器会话，未直接改写二进制蓝图，也未把上述节点伪装成已接入。当前 C++ 接口已经可供蓝图调用；蓝图接线和 PIE 采摘验证仍待在完整工程中完成。

## 原型对照

桌面原型包含 22 张大牌、4 张宫廷牌、9 个委托及三轴目标数值，但没有独立的每日蘑菇表。原型的 `generateHandCards` 会从委托必需牌复制牌，再随机补两张 1 到 8 的小牌。当前系统把蘑菇表行直接映射成火、风、水小牌，点数改为 1 到 9；委托对玩家和 NPC 的叙事使用元素药性，不把牌作为世界实体。

委托暂按原型的“三人各三条”绑定到世界日：第 1 天启用三人的第 1 条委托，第 2 天启用第 2 条，第 3 天启用第 3 条。现有 `CustomerOrderIndex` 就是每个 NPC 的日槽位；森林 NPC 对话会把当前世界日换算成对应的委托序号并传给 AI 提示词。

## 未验证项

- 未在 UE 5.8 编辑器内编译插件；
- 未把 `BP_FirstPersonCharacter` 的采摘节点接到新子系统；
- 未运行 PIE 验证换天、可采摘集合、蘑菇值和背包/坩埚流程。
