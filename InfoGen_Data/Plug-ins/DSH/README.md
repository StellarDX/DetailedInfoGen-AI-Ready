# InfoGen-DSH

注：本插件本身也是大肥鱼写的

`InfoGen-DSH` 是 [InfoGen（SpaceEngine 详细信息生成器）](../../..) 的 DSH Agent 插件。把 InfoGen 的 MCP 服务接入 DeepSeek Harness，让一个**只读**的 Agent 能够查询行星系统数据、读取用户设定文档，并按照 InfoGen 的全局提示词生成扩展设定。

- **关于插件版本**：`package.json` 中写作 `26.0.0-26H2`，因为 npm 版本号必须符合语义化版本规范
- **适配软件**：SpaceEngine 0.99x，向下兼容 0.980
- **导入方式**：手动导入

## 功能

1. **独立的 Agent 预设**：新增 `InfoGen-DSH` Agent 预设。只有新建/切换到该预设的会话才会获得 InfoGen 工具、全局提示词和只读工具集；其他会话完全不受影响。
2. **InfoGen MCP 工具**：通过 MCP 连接 `http://localhost:8000/mcp`，暴露以下三个只读查询工具：
   - `QuerySystem`：查询指定系统的信息；
   - `QueryAllObjectsInSystem`：查询指定系统内某一类型的全部物体；
   - `QueryObject`：查询指定物体的轨道、物理、大气、水圈、生物圈等详情。
3. **全局提示词**：优先使用 MCP `initialize` 响应中的 `instructions` 字段下发 `InfoGen_Data/StreamingAssets/Prompts/Global.txt`；插件把 `maxInstructionBytes` 设为 64 KiB，避免长提示词被截断。
4. **只读权限**：只注册读取类工具（`read`、`read_image`、`glob`、`grep`）、用户交互工具和 MCP 查询工具；不注册 `write`、`edit`、`apply_patch`、`bash` 等任何可写或可执行工具。
5. **MCP 不可达提醒**：启用该 Agent 后，插件会在后台探测 MCP 服务；连续 3 次失败时在窗口顶部弹出一条静默提示。提示不拦截操作、不阻塞对话、不导致 DSH 崩溃；MCP 客户端同时保持后台重连，服务恢复后工具自动恢复。

## 目录结构

```
InfoGen_Data/Plug-ins/DSH/
├── package.json          # DSH bundle 清单、Client 模块声明
├── cordis.patch.yml      # 插件行 + InfoGen-DSH Agent 预设声明
├── index.js              # Host 半：MCP 健康探测、状态路由
├── client.js             # 浏览器半：窗口顶部提示
├── locale/
│   ├── zh.json           # 插件标题/描述（中文）
│   └── en.json           # 插件标题/描述（英文）
└── README.md
```

## 使用前提

1. 已安装可用的 DSH Web 或桌面版（需要 `agentPresets`、`webServer`，以及随 DSH 提供的 `@deepseek-ai/dsh-mcp-client`）。
2. 已安装 InfoGen，并启动 MCP 服务。
3. InfoGen 的数据库里已经用某个命名空间导入过行星系统数据。

### 启动 InfoGen MCP 服务

```bash
python InfoGen.py service mcp -n<namespace> -Dtransport=http --mcp-global-prompt-instruction
```

- `<namespace>` 必须与导入数据时使用的命名空间一致。
- `--mcp-global-prompt-instruction` 会让服务把 `Global.txt` 放进 MCP 的 `instructions` 字段；不加这个参数时，Agent 只能拿到工具，拿不到全局提示词。
- 默认监听 `8000` 端口，地址为 `http://localhost:8000/mcp`。如果需要 IPv6，可加 `-6`（监听 `::1`）；如果需要换端口，可用 `-Dport=<端口>`，但插件里的 MCP 地址也要同步修改（见“配置”）。
- 也可以用 `-C<提示词文件>` 指定其他全局提示词文件。

> 即使 MCP 服务暂时没有启动，也可以先导入并启用插件；只是此时 Agent 查不到数据，窗口顶部会出现一次不可达提示。

## 手动导入

本插件**不包含安装脚本，也不会发布到 npm**。导入由用户手动完成：

1. 打开 DSH 的 **设置 → 插件**（或插件管理）。
2. 选择“安装本地插件 / 从本地目录安装”。
3. 填入本目录的**绝对路径**（源码树或 `cmake --install` 安装前缀下的 `InfoGen_Data/Plug-ins/DSH` 均可），例如：
   - Linux/macOS：`/path/to/InfoGen/InfoGen_Data/Plug-ins/DSH`
   - Windows：`C:\path\to\InfoGen\InfoGen_Data\Plug-ins\DSH`
4. 安装完成后，在插件列表中启用 **InfoGen-DSH**。
5. 重启 DSH（如果插件列表没有立即刷新），然后新建会话，在 **Agent 预设** 中选择 **InfoGen-DSH**。

> 说明：DSH 的“本地目录安装”只会把本目录链接进当前 profile，不会执行本插件里的任何安装脚本；本插件也没有安装脚本。
>
> 不想使用插件管理器时，也可以由用户自行把本目录加入 DSH profile 的 bundle 列表，但这种方式需要手动维护 profile 配置，升级 DSH 时容易失效，不推荐普通用户使用。

### 启用与停用

- **启用**：新建会话时选择 `InfoGen-DSH`，或在空白会话中把 Agent 预设切换为 `InfoGen-DSH`。
- **停用**：新建会话时选择其他预设（如 `standard`）即可；插件不会把工具或提示词注入其他预设。
- **卸载**：在插件管理中禁用或移除 InfoGen-DSH。移除后，`InfoGen-DSH` 预设会从列表中消失，已打开的其他会话不受影响。

## 只读说明

本 Agent 从工具层面就是只读的：

| 能力 | 是否提供 | 说明 |
| --- | --- | --- |
| `read` / `read_image` | ✅ | 读取工作区文件、图片、用户设定文档 |
| `glob` / `grep` | ✅ | 只读检索文件内容 |
| `QuerySystem` 等 MCP 工具 | ✅ | 只查询数据库，不写入 |
| `ask_user_question` | ✅ | 向用户澄清需求 |
| `write` / `edit` / `apply_patch` | ❌ | 未注册 |
| `bash` / 终端 / 子进程 | ❌ | 未注册 |

“读取用户给的设定文档”也是只读操作：插件不会注册任何写回工具，因此 Agent 无法修改设定文档、数据库或工作区文件。

> 注意：Agent 预设不是安全沙箱。DSH 的插件本身拥有宿主进程权限，本插件的只读约束来自不注册写工具；请只从可信来源导入插件。

## 全局提示词

- 文件位置：`InfoGen_Data/StreamingAssets/Prompts/Global.txt`
- 下发方式：启动 MCP 服务时加 `--mcp-global-prompt-instruction`，服务会在 `initialize` 的 `instructions` 字段返回提示词；DSH 的 MCP 客户端会把它作为系统提示词的一部分。
- 字节上限：`maxInstructionBytes: 65536`（64 KiB）。当前 `Global.txt` 约 4 KiB，余量充足。
- 如果 MCP 服务没有下发 `instructions`，插件不会自行读取并重复注入该文件，以免与 MCP 下发的提示词重复。请确认启动参数正确。

## MCP 重连与窗口提示

MCP 连接由 DSH 的 `@deepseek-ai/dsh-mcp-client` 负责：

- 启动失败不会让 DSH 崩溃，也不会阻止会话创建（`failOnStartupError: false`）。
- 断线后按指数退避自动重连：首次等待 2 秒，最长 30 秒；后台持续重试，InfoGen 稍后启动时工具会自动恢复。
- 如果你希望 MCP 客户端在 3 次失败后彻底停止，可把预设中 `reconnect.maxAttempts` 改为 `3`；代价是服务恢复后需要重新加载插件。

窗口提示由本插件的 Host 守护逻辑负责：

- **只在用户启用 `InfoGen-DSH` Agent 后**才开始统计失败次数；普通会话不会看到任何提示。
- 连续探测失败 **3 次**后，向 `/infogen-dsh/status` 发布一条通知；`client.js` 轮询该路由，在**窗口顶部**显示一条约 7 秒的静默提示。
- 同一轮故障只发布一次通知；服务恢复后通知自动清除，再次故障才会重新提示。页面刷新时会重新读取当前仍有效的通知。
- 可通过只读接口查看当前状态：

```bash
curl http://127.0.0.1:3080/infogen-dsh/status
```

返回示例：

```json
{
  "plugin": "infogen-dsh",
  "preset": "infogen-dsh",
  "mcpUrl": "http://localhost:8000/mcp",
  "state": "failed",
  "attempts": 3,
  "notice": { "seq": 1, "at": 1762900000000 },
  "error": "fetch failed",
  "updatedAt": 1762900000000
}
```

`state` 取值：

| 值 | 含义 |
| --- | --- |
| `idle` | 当前没有会话使用 `InfoGen-DSH` 预设，未开始探测 |
| `checking` | 正在执行本轮重试 |
| `up` | MCP 服务可达 |
| `failed` | 连续 `attempts` 次探测失败，已发布提示 |

## 配置

插件的两行配置都可以在 DSH profile 的 `cordis.patch.yml` 中按行 `id` 覆盖。例如把 MCP 地址改成 IPv4 显式地址：

```yaml
- id: infogen-dsh
  config:
    mcpUrl: http://127.0.0.1:8000/mcp
```

守护插件的配置项：

| 键 | 默认值 | 说明 |
| --- | --- | --- |
| `mcpUrl` | `http://localhost:8000/mcp` | 健康探测地址 |
| `presetId` | `infogen-dsh` | 关联的 Agent 预设 id |
| `attempts` | `3` | 连续失败多少次后提示 |
| `retryDelaysMs` | `[1000, 2000, 4000]` | 每轮重试之间的等待时间 |
| `probeTimeoutMs` | `4000` | 单次探测超时 |
| `pollIntervalMs` | `10000` | 探测周期（毫秒） |

> 如果要更换 MCP 端口或地址，除了改上面的 `mcpUrl`，还要覆盖 `preset-infogen-dsh` 里 `infogen-mcp` 行的 `url`（或在 profile 补丁中整体覆盖 `preset-infogen-dsh` 的 `config`）。两处默认值都指向 `http://localhost:8000/mcp`。

## 工具清单

| 工具 | 用途 |
| --- | --- |
| `QuerySystem` | 按系统名/ID 查询系统级信息，返回 JSON |
| `QueryAllObjectsInSystem` | 列出系统内某一类型的全部物体，返回 TSV；可过滤质心、恒星、行星、矮行星、卫星、小卫星、小行星、彗星 |
| `QueryObject` | 按物体名/别名/ID 查询完整详情，返回格式化 JSON；可附加系统名和物体类型限定范围 |

工具名称在 DSH 中会带上服务器前缀，即 `mcp__infogen__QuerySystem`、`mcp__infogen__QueryAllObjectsInSystem`、`mcp__infogen__QueryObject`。

## 常见问题

| 现象 | 处理 |
| --- | --- |
| Agent 预设列表里看不到 InfoGen-DSH | 确认插件已启用并重启 DSH；确认本目录被正确安装到当前 profile |
| 会话里没有 MCP 工具 | 确认 MCP 服务已启动、命名空间与数据一致；访问 `/infogen-dsh/status` 查看状态 |
| 有工具但没有全局提示词 | 启动服务时漏了 `--mcp-global-prompt-instruction`，或客户端未读取 `instructions` |
| 窗口顶部没有提示 | 提示只在“已启用该 Agent”且“连续 3 次失败”后出现；MCP 正常时本来就不会提示 |
| 提示出现后信息仍查不到 | 先启动 InfoGen MCP；客户端会在后台自动重连，服务恢复后工具自动可用 |
| 改了端口后工具消失 | 同时修改 `infogen-dsh.mcpUrl` 和预设中 `infogen-mcp.url` 两处 |
| 中文界面显示英文描述 | DSH 会按语言加载 `locale/zh.json` 或 `locale/en.json`；确认 DSH 语言设置为中文 |

## 兼容性与版本

- SpaceEngine：0.99x，向下兼容 0.980
- DSH：使用 `@deepseek-ai/dsh-mcp-client`、`@deepseek-ai/dsh-agent-preset`、`@deepseek-ai/dsh-tool-fs` 等随 DSH 提供的插件包，不额外声明 npm 依赖

## 许可证

本插件与 InfoGen 主项目一致，使用 [GPL-3.0](../../../LICENSE) 许可证。
