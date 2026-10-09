# 天与每日蘑菇系统

## 当前实现

`UDailyMushroomSubsystem` 提供全局“天”状态，默认从第 1 天开始，保存到 `WorldDayState`。调用 `AdvanceDay` 会递增一天并保存；`SetCurrentDay` 可用于读档、测试或关卡切换时恢复指定日期。

每日蘑菇定义由 19 个现有的基础蘑菇类型组成（5 个普通类型、4 个 Cap 类型、10 个 Small 类型）。系统根据“天数 + 蘑菇类型索引”生成稳定种子，每天重新计算：

- `bAvailable`：当天该类型是否可采摘；
- `Value`：绑定的火、风、水三轴数值，范围为 -3 到 5；
- `Day`：该定义所属的天。

同一天内同一类型的结果稳定，换天后可采摘集合和数值会变化。数值使用现有 `FAlchemyVector`，可以直接传给炼金合成或评估逻辑。

## 蓝图接入

现有玩家采摘逻辑位于 `Content/Witch_House/Demo/FirstPerson/Blueprints/BP_FirstPersonCharacter.uasset`，当前只按 Mesh 名称计数。需要在完整工程的 `HarvestMushroom(MeshName)` 中加入：

1. 通过 `Get Game Instance Subsystem → DailyMushroomSubsystem` 取得当天状态；
2. 调用 `IsMushroomAvailable(MeshName)`，不可采摘时显示当天提示并保留蘑菇；
3. 调用 `GetMushroomValue(MeshName)`，把返回的 `FAlchemyVector` 与背包条目一起保存；
4. 采摘成功后再隐藏命中的蘑菇、增加数量并刷新 HUD；
5. 每晚或完成日结时调用 `AdvanceDay`，然后刷新场景中的可采摘状态。

本增量仓库没有可运行的 UE 编辑器会话，未直接改写二进制蓝图，也未把上述节点伪装成已接入。当前 C++ 接口已经可供蓝图调用；蓝图接线和 PIE 采摘验证仍待在完整工程中完成。

## 原型对照

桌面原型包含 22 张大牌、4 张宫廷牌、9 个委托及三轴目标数值，但没有独立的每日蘑菇表。原型的 `generateHandCards` 会从委托必需牌复制牌，再随机补两张 1 到 8 的小牌。每日蘑菇系统因此先复用三轴数值结构，之后可将每日蘑菇值作为小牌来源，替换这段随机补牌逻辑。

## 未验证项

- 未在 UE 5.8 编辑器内编译插件；
- 未把 `BP_FirstPersonCharacter` 的采摘节点接到新子系统；
- 未运行 PIE 验证换天、可采摘集合、蘑菇值和背包/坩埚流程。
