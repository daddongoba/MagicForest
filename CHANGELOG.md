# 更新记录

本文件记录增量仓库每次提交上传的内容。最新更新放在最前面；每次上传前必须把本条记录与对应文件一起提交。验证状态应区分“本次验证”“历史记录”与“未验证”。

## 2026-10-07-01：NPC 道路漫游、相遇交互与步行修正

- **目的**：让三个 NPC 持续在道路上活动，玩家能够遇见或寻找并交谈；不可达路线自动换路。取代七目的地固定巡逻。
- **基线 / 前置更新**：demo3_UE58 1.5 完整工程，UE 5.8.3 Windows；增量基线 main `5e81998`，沿用 `2026-10-06-01` NPC 和 `2026-10-06-05` 既有更新。
- **上传状态**：待上传；提交准备阶段，远程核对完成后补记。
- **新增文件**：`Source/demo3_UE58/ForestRoadAIController.{h,cpp}`、`ForestCrouchNavigation.{h,cpp}`、`ForestNPCInteraction.{h,cpp}`、`ForestRoadTestCamera.{h,cpp}`、`ForestNPCSetupLibrary.{h,cpp}`；`Content/NPC/Navigation/SM_Log_NPCCollision.uasset`、`SM_Portal_NPCCollision.uasset`；`.gitattributes`；`Docs/NPC更新应用说明_2026-10-07.md`、`Docs/NPC_FreeRoaming_Interaction.md`、`Docs/NPC_Camera_Test.md`。
- **关卡新增入库**：`Content/Biomes/PNB_Enchanted_Forest/Map/Demo_Day.umap` 是完整工程已有但本次修改的关卡，约 157 MB，使用 Git LFS。包含用户编辑的道路、三 NPC、交互管理器、跟随相机和小物件碰撞修改。
- **修改文件**：`Content/NPC/AI/BP_NPC_WanderTest.uasset`、`BP_NPC_Natta_Wander.uasset`、`BP_NPC_Fawnia_Wander.uasset`；`Source/demo3_UE58/demo3_UE58.Build.cs`；`Config/DefaultEngine.ini`；`README.md`；本更新记录。
- **完整工程侧删除操作**：从 Demo_Day 关卡删除 NPC01–NPC07 七个箱子，已体现在关卡文件中；未删除独立资产。旧 Arrival 辅助点保留但不再控制移动。
- **最终行为**：速度 93.75 cm/s，模型朝向 Yaw=-90，Natta/Fawnia 模型高度 -88 cm；恢复原走路动画并停用实验性下蹲骨骼叠加；小物件对 NPC 放行，大障碍保留碰撞与导航；E 基础交谈，E/Esc 结束；F6/F7/F8/F9 提供 NPC 观察工具。
- **依赖与应用**：需要匹配完整工程原有模型、动画、场景与玩家资产；先获取 Git LFS 关卡，合并源码和导航/碰撞配置、编译模块，再应用资产和关卡。ECC_GameTraceChannel1 分配给 ForestNPC，已有通道需审查。完整步骤与文件表见 [本批应用说明](Docs/NPC更新应用说明_2026-10-07.md)。既有插件和 uproject 保留。
- **验证来源**：本开发会话原生模块构建成功，漫游阶段 PIE 确认三 NPC 连续完成路段、基础交互暂停和恢复移动输入。最后的碰撞、朝向、慢步行、删除箱子等在编辑器读取确认并保存；本次上传整理不额外运行游戏测试。
- **未验证项 / 限制**：最后修正未做完整 PIE 回归或打包；真实下蹲动画、足部 IK、剧情对话尚未接入，E 与原采摘交互的冲突未检查。
- **发布范围**：只包含上述增量，未包含缓存、Saved、构建产物、调试截图、完整资源包或本机令牌。上传配置保留 SecurityToken 为空。
- **回退**：完整工程恢复应用前备份，地图、NPC 蓝图、源码与配置须配套回退。

## 2026-10-06-05：坩埚投料合成（蓝图）与 DialogueMemoryTable 插件入库

- **功能更新范围**：2026-10-06 晚间，玩家走近坩埚后的投料—合成链路；以及一组非本次制作的第三方插件源码入库。
- **适用基线 / 引擎版本**：`demo3_UE58` 1.5 版完整工程，UE 5.8（历史记录为 5.8.3，macOS / Apple 芯片）。
- **前置更新**：本仓库批次 `2026-10-06-01`（首批 NPC 游走与采摘功能）；流程约定批次 `2026-10-06-04`。
- **上传状态**：已上传至 [daddongoba/MagicForest](https://github.com/daddongoba/MagicForest)，分支 `main`；批次提交 [020c429](https://github.com/daddongoba/MagicForest/commit/020c429bb70351af0689847494b7931bedabcd39)，远程分支已核对。
- **上传核对（2026-10-07）**：接受仓库协作邀请后，通过 SSH 完成推送；`git ls-remote origin refs/heads/main` 已返回 `020c429bb70351af0689847494b7931bedabcd39`。
  **离线交付与防误推（2026-10-07 00:15 补充）**：
  ① 已导出 `MagicForest_2026-10-06-05.bundle`（167 KB，含本批唯一提交，依赖父提交 `a31dbd5`），
  可在任何具备推送权限的环境中用 `git pull <bundle路径> main` 导入后补推，无需依赖本机凭据；
  ② 桌面的 `推送到GitHub.command` 会轮询 SSH 认证状态，**公钥登记后自动完成推送**，无需再执行任何命令；
  ③ 完整工程侧的 `origin` 已把 `pushurl` 设为禁用占位值 —— 两套 Git 历史无合并基，
  此举用于防止误用完整工程历史覆盖本仓库（保留 `fetch` 以便按 `AGENTS.md` 检查更新）。
  ④ **仓库归属已核实（2026-10-07 00:55）**：本仓库归 `dadongoba <285905484@qq.com>` 所有，
  经确认**属于协作方，本机操作者并非该账号持有人，且没有 GitHub 账号**。⇒ 本批的推送权在仓库所有者一侧。
  **因此本批以 Git bundle 形式交付**：`MagicForest_2026-10-06-05.bundle` 可由仓库所有者用
  `git pull <bundle 路径> main` + `git push origin main` 两条命令并入（已实测：从远端干净克隆后导入成功，
  `main` 前进到本批提交，仓库共 36 个文件）。
  ⑤ 复查结论：本机仍无任何 GitHub 凭据 —— 无 `gh` CLI、无 `credential.helper`、无 `.netrc`／
  `.git-credentials`、钥匙串无 `github.com` 条目、Xcode 源码控制账号列表为空（`IDESourceControlHostAccounts_10` 为 `()`）、
  未安装 GitHub Desktop。⇒ **剩余唯一缺环是账号侧授权，本机无法自行取得。**
- **目的与行为变化**：让玩家在保留原有 E 键采摘的基础上，能走近柜台坩埚打开面板，把手上的东西投入锅中、取回、并合成；合成判定单独抽成一层函数，便于将来换成外部 AI 或接插件内的炼药子系统。

### 文件范围

| 类型 | 工程相对路径 | 说明 |
|---|---|---|
| 新增 | `Content/Gameplay/Harvest/WBP_CauldronPanel.uasset` | 坩埚合成面板；含 `Txt_Bag`、`Txt_Hint` 与函数 `Refresh(InText)` |
| 修改 | `Content/Witch_House/Demo/FirstPerson/Blueprints/BP_FirstPersonCharacter.uasset` | 新增 `CauldronContents`、`CauldronList`、`CauldronOpen`、`CauldronPanel`、`CraftResult`、`LastPicked`、`LastPutIn`；新增函数 `ToggleCauldronPanel`、`UpdateCauldronUI`、`PutInCauldron(InKey)`、`TakeOutCauldron(InKey)`、`PutHeldInCauldron`、`TakeLastFromCauldron`、`ResolveCraft()`、`DoCraft()`；`TraceAndHarvest` 改为按命中对象分派（`Mushroom`→采摘，`Cauldron`→开关面板） |
| 新增 | `Plugins/DialogueMemoryTable/`（16 个文件） | ⚠️ **非本次制作**：他人于 2026-10-06 21:38–21:41 装入完整工程，`.uplugin` 标注 `CreatedBy: Codex`。含 `DialogueMemoryTableSubsystem`（AI 对话记忆，存 `USaveGame` 槽 `DialogueMemoryLedger`）与 `AlchemyGameplaySubsystem`（火/风/水元素、卡牌类型、`FAlchemyVector`、委托 Commissions、Personas）两个子系统。已做凭据扫描，未发现硬编码密钥或令牌 |
| 修改 | `demo3_UE58.uproject` | 启用 `DialogueMemoryTable` 插件（该改动同样不是本次制作） |
| 新增 | `Docs/DialogueMemoryTable-接入说明.md` | 插件自带的接入说明；其中也自述未启动编辑器验证 |
| 修改 | `Docs/更新汇总_2026-10-05至10-06.md` | 补记“阶段 16”一节，与完整工程记录对齐 |

### 依赖与应用

- **按键**：`E` 交互分派（采摘 / 开面板）· `F` 投料 · `R` 取回 · `G` 合成。完整工程需在角色蓝图中保留这些键盘事件绑定。
- **未包含的依赖**：锅体模型位于 `Witch_House` 关卡（未纳入本仓库）；合成 UI 依赖本面板资产，缺 `WBP_CauldronPanel` 时角色蓝图将出现空引用。
- **应用步骤**：关闭编辑器并备份后，按相对路径复制上表两个蓝图；再复制 `Plugins/DialogueMemoryTable/` 并合并 `demo3_UE58.uproject` 的插件启用项，**然后重新生成项目文件并编译 C++**。
- **插件与本次合成的关系**：本批次只是把现成文件入库与同步，未修改插件源码；是否采用 `AlchemyGameplaySubsystem` 替代本次的占位 `ResolveCraft()` 尚未决定。

### 验证与已知问题

- **本次验证**：对新增与修改文件做凭据扫描，未发现密钥、令牌或个人路径泄漏；`git diff --check` 无空白错误；文件清单与上述表格逐项核对。
- **历史记录**：本批蓝图的 `compile_blueprint` 返回无错误、`save_assets` 返回成功，来自完整工程晚间操作记录。
- **未验证项**：**未运行 UE，未做 PIE 实测**——面板排版、按键触发、投料扣减与产物入包均未经过运行验证。**插件未在 Unreal Build Tool 中编译过**，下次打开编辑器会触发编译，存在失败可能，建议优先确认。
- **已知问题**：① 合成产物目前是占位字符串 `"未知的混合物"`，`ResolveCraft()` 待接入正式规则或 AI；② MCP 侧无数组索引取值节点（`Utilities|Array|Get` 不存在），因此面板暂时无法按编号点选第 N 个物品，当前用 `LastPicked` / `LastPutIn` 记录最近操作的物品绕开；③ 完整工程记录中的女巫小屋 3.5 米高差与室内外导航未连通问题本批未处理。
- **回退方式**：删除 `Plugins/DialogueMemoryTable/`、还原 `demo3_UE58.uproject` 可去掉插件；蓝图可用 Git 恢复上一版本，但先前批次已包含的 `BP_FirstPersonCharacter.uasset` 也会一并回到旧版。

## 2026-10-06-04：明确 AI 开发与 Git 更新流程

- **目的**：让参与项目的 AI 先同步并检查 Git 进度，再制作功能，最后整理增量、维护记录并提交推送。
- **文件范围**：更新 `AGENTS.md`，补充必需工作顺序、安全同步方式、验证和交付要求；更新 `README.md`，增加可直接提供给 AI 的任务提示。
- **上传状态**：已上传至公开仓库 [daddongoba/MagicForest](https://github.com/daddongoba/MagicForest)，分支 `main`；实现提交 [94e6a9c](https://github.com/daddongoba/MagicForest/commit/94e6a9c51e18b42d36af0ed0495722471a3e1663)，远程分支已核对。
- **验证结果**：检查仓库当前约定与 README 后编写；`git diff --check` 通过；本次只改流程文档，不涉及 UE 资产或功能验证。
- **未验证项**：尚未在其他 AI 编码工具中实测其对 `AGENTS.md` 的读取情况。工具若不自动读取该文件，须在任务提示中附上 README 提供的指令并确保文件处于其上下文中。
- **回退方式**：可通过 Git 恢复这两份文档的上一版本。

## 2026-10-06-03：仓库改名并公开

- **目的**：将 GitHub 仓库名改为 `MagicForest` 并设为公开，让其他人可以查看迭代文件与记录。
- **文件范围**：本次修改 `README.md` 与 `CHANGELOG.md` 的仓库名称、链接和可见性说明；UE 功能资产没有变化。
- **上传状态**：GitHub 仓库设置已变更为公开，名称已核实为 `daddongoba/MagicForest`；文档提交待上传。
- **验证与应用**：仓库 API 返回 `private=false` 且名称匹配。使用新仓库地址克隆；应用功能文件仍需匹配的 UE 5.8 完整工程。
- **回退方式**：可将仓库可见性恢复为私有并恢复旧名；公开期间仓库内容可能已被访问或克隆。

## 2026-10-06-02：补记首批上传结果

- **适用基线 / 前置更新**：本仓库批次 `2026-10-06-01`；引擎与功能资产未变。
- **目的与文件范围**：仅修改 `CHANGELOG.md`，补记首批文件上传成功及验证结果。
- **依赖与应用**：无新增依赖，无需向完整 UE 工程覆盖任何文件。
- **验证结果**：首批 17 个文件通过 GitHub API 上传，每个文件的 Git blob SHA 与本地提交一致，远程 `main` 已核对为首批提交 `a834c21c10ae4cf98b7708d4eca73961361d09b2`。本条随后续文档提交同步至同一远程仓库。
- **未验证项 / 已知问题**：未运行 UE；功能限制沿用首批记录。
- **回退方式**：只影响文档，可通过 Git 恢复上一版 `CHANGELOG.md`。

## 2026-10-06-01：首批增量文件整理

- **功能更新范围**：2026-10-05 至 2026-10-06。
- **适用基线**：`demo3_UE58` 1.5 完整工程，UE 5.8（历史记录为 5.8.3）。这是首批增量文件，无本仓库前置批次。
- **上传状态**：已上传至 [daddongoba/MagicForest](https://github.com/daddongoba/MagicForest)，分支 `main`；首批提交 [a834c21](https://github.com/daddongoba/MagicForest/commit/a834c21c10ae4cf98b7708d4eca73961361d09b2)，上传结果已核对。仓库目前公开。
- **目的**：保存 NPC 游走蓝图、玩家采摘蘑菇与背包 HUD 的迭代文件，并明确增量仓库的用途及后续记录要求。

### 文件范围

以下功能文件分类依据 [原始清单](_清单.md)，路径已经与当前目录核对；未对蓝图内部逻辑重新验证。

| 类型 | 工程相对路径 | 说明 |
|---|---|---|
| 新增 | `Content/Gameplay/Harvest/WBP_MushroomHUD.uasset` | 采摘计数、背包与交互提示 HUD |
| 新增 | `Content/NPC/AI/BP_NPC_Natta_Wander.uasset` | Natta 游走蓝图 |
| 新增 | `Content/NPC/AI/BP_NPC_Fawnia_Wander.uasset` | Fawnia 游走蓝图 |
| 修改 | `Content/Witch_House/Demo/FirstPerson/Blueprints/BP_FirstPersonCharacter.uasset` | 玩家射线采摘、蘑菇隐藏及关闭碰撞、按种类入背包、HUD 刷新；E 键绑定采摘 |
| 修改 | `Content/NPC/AI/BP_NPC_WanderTest.uasset` | 原记录判定为编辑器重新保存、无语义变化 |
| 修改 | `Config/DefaultEngine.ini` | 启动关卡设为 `Demo_Day`，默认 GameMode 设为第一人称 GameMode |
| 参考，未修改 | `Source/demo3_UE58.Target.cs` | 工程构建目标 |
| 参考，未修改 | `Source/demo3_UE58Editor.Target.cs` | 编辑器构建目标 |
| 参考，未修改 | `Source/demo3_UE58/demo3_UE58.Build.cs` | 模块构建规则 |
| 参考，未修改 | `Source/demo3_UE58/demo3_UE58.h` | 模块头文件 |
| 参考，未修改 | `Source/demo3_UE58/demo3_UE58.cpp` | 模块实现 |

本批仓库维护文件：新增 `README.md`、`CHANGELOG.md`、`AGENTS.md` 与 `.gitignore`；附带既有 `_清单.md` 和 `Docs/更新汇总_2026-10-05至10-06.md`。上传准备时清空 `Config/DefaultEngine.ini` 中本机 Android File Server 的 `SecurityToken`，目标工程需在本地配置自己的令牌。本批没有声明需从完整工程删除或重命名的文件。

### 依赖与应用

- 完整工程需提供原素材、第一人称 GameMode、NPC 相关依赖及 `Demo_Day` 等关卡；本仓库未提供完整工程下载地址。
- `Content/Biomes/PNB_Enchanted_Forest/Map/Demo_Day.umap` 未纳入本批。NPC 摆放、路点及导航状态需由完整工程提供或自行配置。
- 关闭编辑器并备份后，按原路径复制本批蓝图；审查并合并 `DefaultEngine.ini` 中启动关卡与默认 GameMode 的设置。未修改的 `Source/` 文件无需作为功能更新覆盖。
- 详细应用流程见 [仓库说明](README.md#如何应用更新)，背景见 [历史更新汇总](Docs/更新汇总_2026-10-05至10-06.md)。

### 验证与已知问题

- **本次检查**：核对当前文件清单与文档路径；本次仅整理仓库文档，没有改动功能资产，也没有运行 UE、编译或打包。
- **历史记录**：macOS 编译通过，三个 NPC 游走和采摘至 HUD 刷新的逻辑链路已做 PIE 验证。这些结果来自完整工程历史汇总。
- **仍需验证**：人工在 PIE 中按 E 采摘；应用到目标完整工程后的资产引用、蓝图编译、运行及打包。
- **已知问题**：历史记录指出女巫小屋室内外导航未连通、门口约 3.5 米高差；本批未修复。NPC 游走路点距离小屋较远，复制蓝图不会自动改变路点或摆放。

---

## 后续更新模板（复制到文件顶部并填写）

```markdown
## YYYY-MM-DD-NN：更新标题

- 功能更新范围：
- 适用基线 / 引擎版本：
- 前置更新：无 / 批次编号
- 上传状态：待上传 / 已上传（已确认的提交或标签，如有）
- 目的与行为变化：

### 文件范围

| 类型（新增/修改/删除/重命名/参考） | 工程相对路径 | 说明 |
|---|---|---|
| | | |

### 依赖与应用

- 未包含的依赖：
- 复制、配置合并、删除、迁移或关卡操作步骤：
- 详细说明链接（如有）：

### 验证与已知问题

- 本次验证方式与结果：
- 历史验证依据（如引用）：
- 未验证项：
- 已知问题与回退方式：
```
