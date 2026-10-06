# 更新记录

本文件记录增量仓库每次提交上传的内容。最新更新放在最前面；每次上传前必须把本条记录与对应文件一起提交。验证状态应区分“本次验证”“历史记录”与“未验证”。

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
- **上传状态**：已上传至私有仓库 [daddongoba/demo3-ue58-updates](https://github.com/daddongoba/demo3-ue58-updates)，分支 `main`；首批提交 [a834c21](https://github.com/daddongoba/demo3-ue58-updates/commit/a834c21c10ae4cf98b7708d4eca73961361d09b2)，上传结果已核对。
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
