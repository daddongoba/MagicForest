# NPC 第三人称观察相机

2026-10-07：改为近距离右肩跟随，用于观察 NPC 寻路和下蹲。

- 场景对象：`NPC_Road_TestCamera`。
- `FollowDistance`：280 cm；`ShoulderOffset`：65 cm；`ShoulderHeight`：相对角色中心 55 cm；`FollowPitch`：-8°；`FollowFOV`：70°。
- 相机平滑跟随角色位置和朝向，弹簧臂碰到真实环境障碍时缩短，NPC 自身胶囊和装饰植物不阻挡镜头。
- **F6**：玩家视角 / NPC 跟随。
- **F7**：切换三个 NPC。
- **F8**：当前 NPC 下蹲预览；强制下蹲通道内仍保持下蹲。
- **F9**：显示或隐藏真实导航路径。
- Play 默认玩家视角，按 F6 接管观察视角；返回玩家视角或退出 Play 恢复玩家输入。

在 Details 面板可调整以上相机参数。当前是自动跟随 NPC 朝向的观察相机。
