# NPC 道路漫游与交互

2026-10-07：按最新需求取代 NPC01–07 固定目的地巡逻。

## 当前玩法

- 三个 NPC 在道路附近选择可达的局部路段，完成一段后立即选择下一段。
- 道路样条保留用户编辑，作为道路偏好；实际移动使用导航和 CharacterMovement，地形高度由碰撞处理。
- 路径不完整则换路；移动受阻则短暂后退，并避开失败位置约 45 秒，再继续漫游。
- 已确认无法通过的北侧原木连接停用。低矮通道支持下蹲，物理受阻时退出该连接并换路。
- 玩家在约 2.5 米内、有视线时按 E 交谈。NPC 停下并面对玩家；E 或 Esc 结束后继续漫游。
- 当前交互提供基础问候，后续剧情可接控制器的 OnInteractionChanged 事件。

## 摄像头

Play 默认玩家视角。F6 切换 NPC 右肩观察，F7 换 NPC，F8 下蹲预览，F9 导航路径。

## 模型与碰撞修正

Natta、Fawnia 原本 Mesh 相对 Z=0，模型被抬到胶囊中心，产生约 88 厘米浮空。
蓝图默认与场景实例均改为 Z=-88；重开编辑器后检查持久化。
后续按用户反馈撤掉自制下蹲骨骼后处理，恢复三个角色原来的走路动画。
物理胶囊下蹲仍保留，但不再用骨骼旋转拼出下蹲动作；完整的下蹲视觉需要角色适配的真实下蹲动画。
胶囊仍负责物理接地，坡面脚部没有完整的独立足部 IK，陡坡和动画摆腿时可能有局部脚部高度误差。

## 验证与备份

原生模块编译成功；PIE 中三个 NPC 连续完成多个路段。
交谈开始返回成功，NPC 停止漫游、玩家移动被暂时锁定；结束交谈后恢复移动输入。
修正前蓝图备份：Saved/RouteDesign/BackupBeforeMeshGroundAlignment。
漫游改造前地图与控制器备份：Saved/RouteDesign/BackupBeforeFreeRoaming。

旧 NPC_Destination_Itinerary.md、NPC_Navigation_Crouch.md 中固定七目标的说明属于历史方案，以本文件为准。

## 小物件放行、大障碍绕行

新增 ForestNPC 对象通道。NPC 胶囊使用 ForestNPCPawn，保留地形、大障碍碰撞，角色之间互相重叠。
NPC 骨骼网格不参与物理碰撞。
已有装饰物 ForestDecoration 对 NPC 忽略；新增 ForestSmallProp 只对 NPC 忽略，玩家仍可碰撞。
草和藤蔓放行；枝条、树根、蘑菇、晶体、晶洞、灯笼按场景实际缩放后尺寸筛选，高度不超过 100 cm、最长尺寸不超过 180 cm 时放行。
这些小物件同时从导航障碍中移除；树干、岩壁、大原木、建筑等保留原设置。
场景 Actor 添加 NPC_SmallDecoration 标签可以显式放行；NPC_LargeObstacle 标签优先保留障碍。
编辑器辅助函数 ConfigureNPCCollision 可再次应用规则，不会重画用户道路。

## 模型朝向

Natta、Fawnia 创建时模型相对 Yaw 为 0，而角色移动前方为 +X。
模型朝向偏移统一为 Yaw=-90；保留原动画，角色按移动方向转身，关闭控制器强制朝向，转身速度统一 540 度/秒。
蓝图默认和场景实例均保存；修改前蓝图备份在 BackupBeforeFacingFix。

## 正常步行速度

三个 NPC 的 MaxWalkSpeed 从 200 改为 93.75 cm/s（约 0.94 米/秒）。
该数值匹配三个原始 Idle/Walk/Run 混合空间的 Walk 采样点，避免正常巡游混入跑步动画。
蓝图默认和场景实例已保存；修改前蓝图备份在 BackupBeforeWalkSpeed。
