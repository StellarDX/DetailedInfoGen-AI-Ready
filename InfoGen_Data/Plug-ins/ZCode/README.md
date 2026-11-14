# InfoGen-ZCode

ZCode 插件 Agent · by GLM-5.3-flash

加入了AI的SpaceEngine详细信息生成器，适配0.99x，可向下兼容0.980。
New SpaceEngine Detailed Information Generator with AI

## 简介

本插件把 InfoGen 的 MCP 查询能力接入 ZCode。通过 `/infogen` 指令启用专属 Agent 后，Agent 会调用 InfoGen MCP 服务的三个查询工具（`QuerySystem` / `QueryAllObjectsInSystem` / `QueryObject`）检索已导入数据库的行星系统数据，并结合你提供的设定文档生成扩展设定（文明、矿产、生态圈、宜居度评价、历史沿革等）。

Agent 仅在通过 `/infogen` 指令启用时生效；InfoGen MCP 服务未启动时会自动重试，仍失败则给出一条简短的静默提醒并正常结束，不影响会话与其他功能。

## 插件结构

```
InfoGen-ZCode/
├── .zcode-plugin/
│   └── plugin.json      # 插件清单（含 MCP 服务声明与用户配置项）
├── agents/
│   └── infogen.md       # InfoGen 生成 Agent
├── commands/
│   └── infogen.md       # /infogen 指令（Agent 的唯一启用入口）
├── marketplace.json     # 本地市场清单（供 ZCode 插件市场手动导入）
└── README.md
```

> 命名说明：ZCode 对插件清单的 `name` 字段有硬性校验（仅允许小写字母、数字、`.`、`_`、`-`），因此插件内部标识为 `infogen-zcode`，对外展示名为 `InfoGen-ZCode`（由 `marketplace.json` 的 `displayName` 提供）。

## 前置条件

1. 已安装 InfoGen，并通过 `create system --store` 将行星系统数据导入数据库。
2. MCP 服务已以 HTTP 模式启动（默认地址 `http://localhost:8000/mcp`）：

   ```bash
   python InfoGen.py service mcp -n<namespace> -Dtransport=http -Dport=8000
   ```

3. 建议附带 `--mcp-global-prompt-instruction` 启动，让 Agent 直接通过 MCP 的 `instructions` 字段读取全局提示词（`InfoGen_Data/StreamingAssets/Prompts/Global.txt`）：

   ```bash
   python InfoGen.py service mcp -n<namespace> -Dtransport=http -Dport=8000 --mcp-global-prompt-instruction
   ```

   未开启时，Agent 会尝试在派发消息提供的路径或工作目录的默认路径下读取 `Global.txt`；两者都拿不到时按内置的核心约束摘要执行，并在结果开头注明。

## 手动导入

本插件采用手动导入，不提供安装脚本，也不发布到任何插件市场源。

1. 打开 ZCode，进入 **插件市场 / Plugin Marketplace**。
2. 点击 **添加 / Add → 添加插件市场 / Add Plugin Marketplace**。
3. 粘贴本插件所在目录（即包含本 README 与 `marketplace.json` 的目录），点击 **添加 / Add**。
4. 在 **个人 / Personal** 下的 `dev-infogen-zcode` 市场中找到 **InfoGen-ZCode**，确认版本为 `26H2`。
5. 点击 **安装 / Install**。之后可在 **设置 → 插件** 中管理启用状态。

## 使用

安装后新建任务，输入 `/infogen` 加任务描述即可：

```text
/infogen 为 RS 8479-1793 生成文明设定，设定文档见 ./worldbuilding.md
/infogen 查询 Trappist-1 系统内所有行星，并为每颗行星生成矿产资源分布
/infogen 评价 Kepler-1649 c 的宜居度
```

- 生成结果直接在对话中返回，默认使用中文与 Markdown 格式。
- 任务目标存在歧义（如多个同名天体）时，Agent 会列出候选请你确认后再继续。
- 设定文档路径可以写在 `/infogen` 的参数里，Agent 会读取并以其约束世界观内容。

## 配置

- **MCP 服务地址**：打开 **设置 → 插件管理 → 已安装**，进入 InfoGen-ZCode 详情页并展开配置区域，即可在端内直接修改 `MCP 服务地址`（`mcp_url`，默认 `http://localhost:8000/mcp`）。保存后新建会话即按新地址连接；当前值保存在 `~/.zcode/cli/config.json` 的插件配置中。
- **切换 stdio 模式**：编辑 `.zcode-plugin/plugin.json`，把 `mcpServers` 替换为：

  ```json
  "mcpServers": {
    "InfoGen": {
      "type": "stdio",
      "command": "python",
      "args": ["InfoGen.py", "service", "mcp", "-n<namespace>", "--mcp-global-prompt-instruction"],
      "cwd": "<InfoGen 安装目录>"
    }
  }
  ```

  修改后重新安装或在插件管理中更新插件使配置生效。stdio 模式不使用 `mcp_url` 配置项。

## 服务未启动时的行为

InfoGen MCP 服务未启动时，`/infogen` 会先做连通性探测并自动重试，最多 3 次；仍失败则给出一条简短的静默提醒并正常结束本次请求，不会中断会话，也不影响其他功能。可在 **设置 → MCP** 中查看 `plugin:infogen-zcode:InfoGen` 服务的连接状态。

## 更新

插件不会自动更新。手动更新流程：

1. 替换或修改本目录中的插件源文件，并递增 `.zcode-plugin/plugin.json` 中的版本号。
2. 同步修改 `marketplace.json` 中对应条目的版本号与描述。
3. 在 **插件市场** 页面打开市场源的刷新按钮（**刷新该市场 / Refresh this marketplace**）。
4. 回到 **个人 → dev-infogen-zcode**，在 InfoGen-ZCode 详情中点击 **更新 / Update**。

## 故障排查

| 现象 | 处理 |
| --- | --- |
| `/infogen` 提示 MCP 服务未响应 | 确认 InfoGen MCP 服务已启动、地址与 `mcp_url` 配置一致；在 **设置 → MCP** 检查 `plugin:infogen-zcode:InfoGen` 的状态与错误信息 |
| 连接成功但查询无结果 | 确认启动 MCP 服务时的 `-n/--namespace` 与导入数据时使用的命名空间一致，且数据库配置仍有效 |
| 读不到全局提示词 | 服务启动时未加 `--mcp-global-prompt-instruction`，或 `Global.txt` 不在默认路径；也可把路径直接写进 `/infogen` 参数 |
| 找不到命令 `/infogen` | 确认插件已安装且处于启用状态；旧任务可能需要新建任务后重试 |

## 许可证

GPL-3.0（与 InfoGen 主项目一致）。
