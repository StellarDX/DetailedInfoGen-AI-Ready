# InfoGen-WorkBuddy

> 本插件由腾讯Hy4生成

加入了 AI 的 SpaceEngine 详细信息生成器，适配 0.99x，可向下兼容 0.980。
New SpaceEngine Detailed Information Generator with AI.

本目录是一个 **WorkBuddy 专家包（Agent 型）**，让 AI 通过 InfoGen 的 MCP 服务读取已导入的行星系统数据，生成忠于数据的系统详情，以及文明、矿产、宜居度、历史沿革等扩展设定。

## 目录结构

```
WorkBuddy/
├── .codebuddy-plugin/
│   └── plugin.json          # 专家元数据（名称、版本、分类、展示信息）
├── agents/
│   └── infogen-workbuddy.md # 专家定义：激活条件、连接策略、工具用法、行为约束
├── references/
│   └── Global.txt           # 全局提示词副本（MCP instructions 未送达时回退使用）
└── README.md                # 本文件
```

没有 Logo，因此不提供头像；软件本身也没有图标资源。

## 专家信息

| 项目 | 内容 |
| --- | --- |
| 专家名（花名） | InfoGen-WorkBuddy |
| 类型 | Agent 型（单个 AI 专家） |
| 版本 | 26H2（与 InfoGen MCP 服务版本一致） |
| 行业分类 | 03-GameSpatial 游戏空间 |
| 描述（中） | 加入了AI的SpaceEngine详细信息生成器，适配0.99x，可向下兼容0.980。 |
| 描述（英） | New SpaceEngine Detailed Information Generator with AI |

## 依赖：InfoGen MCP 服务

专家通过 InfoGen 暴露的 MCP 服务获取数据，使用前需先启动服务。

- 服务支持 stdio、http 等多种传输模式。
- 当前已使用 http 模式启动，地址为 `http://localhost:8000/mcp`。
- 启动示例：

```bash
python InfoGen.py service mcp -n<namespace> -Dtransport=http
```

- 若需要让客户端读取全局提示词，可在启动时追加 `--mcp-global-prompt-instruction`（源文件为 `./InfoGen_Data/StreamingAssets/Prompts/Global.txt`）。是否读取 MCP `instructions` 取决于客户端实现；未收到时专家会自动回退使用包内 `references/Global.txt` 的副本。

### MCP 工具清单

| 工具 | 作用 |
| --- | --- |
| `QuerySystem` | 查询指定系统的信息（JSON） |
| `QueryAllObjectsInSystem` | 查询指定系统内某一类型（质心/恒星/行星/矮行星/卫星/小行星/彗星）的全部物体（TSV） |
| `QueryObject` | 查询指定物体的详细信息，可附加系统名与物体类型（JSON） |

三个工具的完整参数、返回值、单位与枚举说明见 `agents/infogen-workbuddy.md`。

### 服务未启动时

专家会自动尝试重连，最多重试 3 次；若仍失败，会从窗口顶部弹出消息静默提醒并简要说明，不会崩溃、不会影响你继续使用其他功能。此时不会生成任何数据内容，启动服务后重试即可。

## 启用方式

**本专家仅在用户明确声明启用本 Agent 时生效**，其他情况一律不生效。例如：

> 启用 InfoGen-WorkBuddy

> 用 InfoGen-WorkBuddy 这个专家帮我查系统

未启用时，专家不会连接 MCP、不会查询数据、不会读取设定文档，也不会生成任何 InfoGen 相关内容，仅提示需显式启用。

## 导入方式（手动导入）

本专家包为**手动导入**，不附带安装脚本，不推送到 npm 或任何安装源。请按 WorkBuddy 客户端的「导入本地专家」流程，手动选择本目录（即包含 `.codebuddy-plugin/` 的这一层）完成导入。

导入后请确认专家目录层级：`.codebuddy-plugin/plugin.json` 必须位于所选目录的根下，不要多选或漏选一层父目录。

## 试试这样问我

- 为某个行星系统生成一份详细信息概览
- 列出某系统内的全部行星并对比物理参数
- 基于某行星的数据设计文明与矿产设定

## 数据约束速览

- 天体固有属性（轨道、物理、大气、水圈、生物圈、系统结构等）必须与检索到的数据一致，不改写数值、不补齐缺失字段。
- 事实冲突优先级：数据源 > 用户提供的设定文档 > 模型先验知识。
- 数据缺失会显式说明"该数据缺失"及其影响，不静默跳过。
- 涉及文化、历史、政体、命名等主观设定时，以用户的设定文档为第一参考，沿用其专有名词与译名。
- 扩展设定需科学自洽，单位与精度沿用数据源，并与数据源信息可区分（数据源 / 推论 / 虚构）。
- 同名或多匹配时会列出候选并请确认，不擅自选择；目标不存在会如实说明未找到。

## 许可证

随 InfoGen 项目一并分发，遵循项目根目录的 GPL-3.0 许可证。
