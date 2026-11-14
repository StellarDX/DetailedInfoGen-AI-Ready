# InfoGen MCP 工具参考（只读）

InfoGen MCP 服务（服务名 `infogen`，地址由插件配置项 `mcp_url` 提供，默认 `http://localhost:8000/mcp`）仅暴露以下三个**只读查询工具**。工具在客户端中的实际名称带服务前缀：`mcp__infogen__QuerySystem` 等。

三个工具均从 InfoGen 数据库读取「已通过 `create system --store` 导入」的行星系统数据，全部为 SELECT 查询，**不提供任何写入、修改、删除能力**。调用前需保证 MCP 服务已以正确命名空间启动（`-n<namespace>`）。

---

## 1. QuerySystem — 查询指定系统的信息

根据系统名称查询系统数据，以 JSON 字符串返回。若不存在则返回「没找到任何资源」。

| 参数 | 类型 | 必填 | 说明 |
| --- | --- | --- | --- |
| `SystemName` | str | 是 | 要查询的系统名称或 ID |

返回：命中时返回系统信息（JSON 格式字符串）；未命中时返回文本「没找到任何资源」。

系统级字段包括：`system_id`、`main_id`、`source_file`、`source_hash`、`code_page`、`esi_estimator`、`absolute_orbit`、`common_plane_threshold`、`namespace`、`n_stars`、`n_planets`、`n_dwarf_planets`、`n_satellites`、`n_minor_planets`、`n_comets`、`spectral_types`、`create_date`、`modified_date`。

---

## 2. QueryAllObjectsInSystem — 查询系统内某一类型的全部物体

根据系统名称查询该系统下包含的全部对象，以 TSV（制表符分隔）字符串返回。若没有匹配数据则返回「没找到任何资源」。

| 参数 | 类型 | 必填 | 说明 |
| --- | --- | --- | --- |
| `SystemName` | str | 是 | 要查询的系统名称或 ID |
| `ObjectType` | str \| None | 否 | 物体类型过滤；不指定时不限制类型 |

`ObjectType` 可选值：

| 值 | 含义 |
| --- | --- |
| `Barycenter` | 质心 |
| `Star` | 恒星 |
| `Planet` | 行星 |
| `DwarfPlanet` | 矮行星 |
| `Moon` | 卫星 |
| `DwarfMoon` | 小卫星 |
| `Asteroid` | 小行星 |
| `Comet` | 彗星 |

返回：该系统内所有对象的数据表，TSV 格式（列间以 `\t` 分隔，不含行索引）；无数据时返回「没找到任何资源」。

行星类型（`/class`）字段解释：

| 值 | 含义 |
| --- | --- |
| `Ferria` | 铁质行星 |
| `Terra` | 岩质行星（类地行星） |
| `Aquaria` | 水/冰质行星 |
| `Carbonia` | 碳质行星 |
| `Neptune` / `IceGiant` | 冰巨星 |
| `Jupiter` / `GasGiant` | 气态行星；出现在恒星（`otype` 为 `Star`）上时表示该物体为自由漂浮气态巨行星（次褐矮星） |
| `Chthonia` / `HeliumGiant` | 氦行星 |
| `Asteroid` | 小型物体（无实际意义） |

---

## 3. QueryObject — 查询指定物体的完整信息

按物体名检索该物体的轨道、物理、大气、水圈、生物圈、环、子系统等模块数据，返回格式化后的 JSON 字符串（数据中只包含该物体「拥有」的属性）。

| 参数 | 类型 | 必填 | 说明 |
| --- | --- | --- | --- |
| `ObjectName` | str | 是 | 物体名称、别名或 ID |
| `SystemName` | str \| None | 否 | 限定检索范围；不指定时搜索所有系统 |
| `ObjectType` | str \| None | 否 | 物体类型过滤；取值同上表 |

返回：

- 命中：该物体信息的 JSON 字符串（`ensure_ascii=False`，`indent=4`）；
- 匹配到多个：返回「匹配到多个物体」及候选列表，**此时必须询问用户具体指代哪一个，不得自行选择**；
- 未命中：返回「没有那个系统或物体」。

### 单位规范（国际单位制）

长度米（面积平方米、体积立方米）、质量千克（密度千克每立方米）、时间秒（`age` 为年，轨道 `Epoch` 为 JD）、重力米每平方秒、速度米每秒、光度瓦特、温度开氏度、压强帕斯卡、成分体积分数、角度度；所有成分的 `fraction` 为体积分数的百分比。

### 常用字段解释

**环系统类型（`/rings/ring_type`）**

| 值 | 含义 |
| --- | --- |
| `CircumplanetaryDisk` | 由小行星组成的星周盘 |
| `SaturnELike` | 类似土星 E 环的渐变环 |
| `JupiterTransparent` | 类似木星环的暗环 |
| `NeptuneNarrow` | 类似海王星环的窄环 |
| `UranusSparse` | 类似天王星环的稀疏环 |
| `Colorful` | 颜色鲜艳的彩色环 |

**生命字段**

| 字段 | 值 |
| --- | --- |
| 生命类型 `/biosphere/class` | `Organic` 有机生命 / `Exotic` 奇异生命 |
| 生物形态 `/biosphere/type` | `Unicellular` 单细胞 / `Multicellular` 多细胞 |
| 生物群系 `/biosphere/biome` | `Marine` 海洋 / `Terrestrial` 陆地 / `Aerial`·`Floaters` 空中 / `Subglacial` 冰下 |

**行星类型（`/class`）**：同上表。

---

## 调用决策顺序

1. 想了解系统整体结构与规模 → `QuerySystem`。
2. 想枚举系统内某类物体（如「列出所有行星」）→ `QueryAllObjectsInSystem`。
3. 想看某个具体物体的完整参数 → `QueryObject`。
4. 需要「先枚举再细看」→ 先 `QueryAllObjectsInSystem` 拿名单，再逐个 `QueryObject`。
5. 目标有歧义（同名天体、同名不同系统、未指明系统或类型）→ 先列出候选请用户确认，不自行选择。
6. 服务不可达 → 按 Agent 定义的「3 次重试 + 静默提醒」流程降级，不编造数据。
