# InfoGen-WorkBuddy

>本插件由腾讯HY4编写

| 项目 | 值 |
| --- | --- |
| 插件名 | `InfoGen-WorkBuddy`（标识 `infogen-workbuddy`） |
| 专家名 | SpaceEngine详细信息生成器 |
| 版本 | `26H2` |
| 形态 | WorkBuddy / CodeBuddy 专家包（Agent 型，单专家） |
| 导入方式 | **手动导入**（见下文） |
| 目标软件 | InfoGen（SpaceEngine 0.99x 详细信息生成器，可向下兼容 0.980） |

本插件把 InfoGen 暴露的三个查询工具、MCP 服务与全局提示词打包成一个开箱即用的 WorkBuddy 专家：显式启用后，专家通过 InfoGen MCP 服务读取已导入的行星系统数据，并在此基础上生成可溯源的扩展设定信息（矿产、文明、生态圈、宜居度、历史沿革等）。

---

## 一、功能

- 通过 MCP 接入 InfoGen 的三个查询工具：
  - `QuerySystem`：查询指定系统的信息
  - `QueryAllObjectsInSystem`：查询指定系统内某一类型的全部物体
  - `QueryObject`：查询指定物体的完整详细信息
- 支持读取 InfoGen 通过 MCP `instructions` 字段下发的全局提示词；客户端未实现该字段时自动回退到包内 `references/Global.txt` 副本。
- 生成内容遵循全局提示词的事实优先、来源可区分、科学自洽与输出规范要求。
- 仅在用户显式声明启用本专家时生效；其他情况完全不介入，不影响客户端任何原有功能。
- InfoGen MCP 服务未启动时自动重连，连续失败 3 次后从窗口顶部给出一条静默提醒并正常结束本次请求，不崩端、不影响用户正常使用。
- MCP 服务地址支持在端内修改（无需编辑插件文件），默认 `http://localhost:8000/mcp`。

---

## 二、前置条件

1. 已通过 `create system --store` 把 SpaceEngine 导出的 `.sc` 文件导入数据库，并记住导入时使用的命名空间。
2. 已以 HTTP 模式启动 InfoGen 的 MCP 服务：

   ```bash
   python InfoGen.py service mcp -n<namespace> -Dtransport=http --mcp-global-prompt-instruction
   ```

   - `-n<namespace>` 必须与导入数据时使用的命名空间一致，否则查询无结果。
   - 加上 `--mcp-global-prompt-instruction`，服务才会把 `Global.txt` 放进 MCP 的 `instructions` 字段；不加时客户端只拿得到工具、拿不到全局提示词（此时专家会回退读取包内 `references/Global.txt` 副本）。
   - 需要换端口时用 `-Dport=<端口>`，并同步在插件配置中修改 MCP 地址（见「第五节：配置」）。
   - 服务也支持 stdio 等其它模式；本插件**默认以 HTTP 模式接入**，因为当前服务已用该模式启动。

---

## 三、导入方式（手动导入）

本插件**只随仓库提供，不设置任何安装脚本，也不推送到 NPM 等任何安装源**，需要手动导入：

1. 打开 WorkBuddy / CodeBuddy 的插件管理，选择「从本地目录导入 / 添加本地插件」。
2. 选择**本目录本身**作为插件根目录，即包含 `.codebuddy-plugin/` 与 `.mcp.json` 的那一层：

   ```
   <InfoGen 安装前缀>/InfoGen_Data/Plug-ins/WorkBuddy/
   ├── .codebuddy-plugin/
   │   └── plugin.json
   ├── .mcp.json
   ├── agents/
   ├── references/
   ├── scripts/
   └── README.md
   ```

3. 确认导入时选中的是 `WorkBuddy` 这一层，**不要多选或漏选一层父目录**；`.codebuddy-plugin/plugin.json` 必须位于所选目录的根下。
4. 导入后启用插件。源码树或 `cmake --install` 安装前缀下的该目录均可作为导入路径，填绝对路径即可。

> `scripts/` 下的 Python 文件是插件运行时的连通性探测脚本，**不是安装脚本**，无需执行、无需安装依赖（仅使用 Python 标准库）。

---

## 四、启用方式

**只有显式声明启用本专家时才会生效**，例如：

- 「启用 InfoGen-WorkBuddy，帮我查询 XXX 系统的概览」
- 「用这个专家帮我查 XXX」
- 在专家中心选中「SpaceEngine详细信息生成器」后首条消息点名本专家并提出任务

未显式启用时，专家不连接 MCP、不查询数据、不生成内容、不注入全局提示词，也不主动介绍自己，对客户端完全透明。话题看起来与天文 / SpaceEngine / 设定创作相关也不会自动激活。

启用后在本会话内保持激活，直到明确停用或会话结束。可用「试试这样问我」中的推荐提示词快速开始。

---

## 五、配置：修改 MCP 地址

MCP 地址由插件配置项 `mcp_url` 提供，**可直接在端内修改，无需编辑任何插件文件**：

- 在「插件管理 → InfoGen-WorkBuddy → 配置 / 高级设置」中修改 `mcp_url`；
- 默认值：`http://localhost:8000/mcp`；
- 用 `-Dport=<端口>` 更换服务端口、以 IPv6（`-6`）启动、或把服务部署到其它主机时，把该值改成对应地址即可，例如 `http://127.0.0.1:9000/mcp`。

该值同时用于 `.mcp.json` 中的 MCP 服务连接与连通性探测，两处自动保持一致。MCP 服务地址以 HTTP 模式为例；若改用其它传输模式，请按客户端的 MCP 配置规则另行调整。

---

## 六、全局提示词

InfoGen 的全局提示词位于：

```
<InfoGen 安装前缀>/InfoGen_Data/StreamingAssets/Prompts/Global.txt
```

它支持通过 MCP 的 `instructions` 字段下发给客户端（启动时加 `--mcp-global-prompt-instruction`，可用 `-C<文件>` 指定其它提示词文件）。MCP 规范并未强制要求客户端实现 `instructions`，因此本插件在包内保留了 `references/Global.txt` 逐字副本：客户端未收到 `instructions` 时，专家会回退读取该副本作为等效全局约束。

若服务端与客户端均未启用该字段，专家仍会遵循基本的输出与注意事项要求，并在结果中简短提示「全局提示词未加载」。

---

## 七、MCP 服务不可达时的行为

| 环节 | 行为 |
| --- | --- |
| 首次查询发现服务不可达 | 自动重试，最多 3 次（含首次），参数不变 |
| 3 次均失败 | 判定「InfoGen MCP 服务未启动或不可达」 |
| 提醒方式 | 从窗口顶部给出**一条**简短静默提醒，不弹窗打断、不输出堆栈与报错详情、不重复刷屏 |
| 影响范围 | 不崩端、不中断会话、不阻塞后续消息、不影响客户端其它功能 |
| 数据 | 不编造、不用相似天体替代、不凭先验知识补全天体固有属性 |
| 服务恢复 | 下一次查询自动恢复正常，无需重新启用专家或重载插件 |

推荐启动命令（供排查参考）：

```bash
python InfoGen.py service mcp -n<namespace> -Dtransport=http --mcp-global-prompt-instruction
```

---

## 八、目录结构

```
WorkBuddy/
├── README.md                          # 本文件（中文）
├── .codebuddy-plugin/
│   └── plugin.json                    # 专家包清单：展示字段、userConfig、PreToolUse 钩子
├── .mcp.json                          # MCP 服务声明（HTTP，地址取 ${user_config.mcp_url}）
├── agents/
│   └── infogen-workbuddy.md           # 专家定义：激活闸门、连接与故障处理、工作流程、输出规范
├── references/
│   ├── Global.txt                     # 全局提示词逐字副本（instructions 回退用）
│   └── tools.md                       # 三个查询工具的参数、枚举值与返回格式参考
└── scripts/
    └── mcp_probe.py                   # 工具调用前的连通性探测（PreToolUse 钩子）
```

> 本软件没有 Logo，因此插件未设计、也未引用任何图标文件。

---

## 九、常见问题

| 现象 | 处理 |
| --- | --- |
| 导入后找不到专家 | 确认导入的是**本目录本身**（`.codebuddy-plugin/` 位于所选目录根下），且已启用插件；旧会话可能需要新建会话后才生效 |
| 查询无结果 | 确认 MCP 服务启动时 `-n<namespace>` 与 `create system --store` 导入数据时的命名空间一致，且数据库配置仍有效 |
| 客户端读不到全局提示词 | 服务启动加 `--mcp-global-prompt-instruction`（默认不发送 `instructions`），或确认 `-C` 指向的文件存在且为 UTF-8；客户端不支持 `instructions` 时会自动回退到包内副本 |
| 提示服务未启动 | 按「第二节」启动 MCP 服务；换过端口时同步修改插件配置中的 `mcp_url` |
| 换了端口后仍连不上 | 检查插件配置 `mcp_url` 与服务端 `-Dport` 是否一致，地址需包含 `/mcp` 路径 |
| 服务在别的机器 / IPv6 | 在插件配置中把 `mcp_url` 改为对应地址（IPv6 需带方括号，如 `http://[::1]:8000/mcp`） |

---

## 十、版本与许可

- 版本：`26H2`
- 本插件随 InfoGen 仓库提供，遵循与 InfoGen 相同的 [GPL-3.0](../../../../LICENSE) 许可协议。
- 插件为**手动导入**，不提供安装脚本，不推送到 NPM 等任何安装源。
