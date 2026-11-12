---
description: 启用 InfoGen Agent：基于 InfoGen 数据源生成行星系统扩展设定（文明、矿产、生态圈、宜居度评价等）
argument-hint: [任务描述，可含设定文档路径]
---

这是 InfoGen Agent 的唯一启用入口：只在本指令被调用时才使用 infogen Agent，其他任何情况下都不要主动派发它。严格按以下步骤执行本次请求：

1. 参数检查：若 `$ARGUMENTS` 为空，先简短询问用户本次要生成的目标（系统或天体、希望的输出维度、设定文档路径，如有），待用户补充后再继续；不要用空任务去探测或派发。
2. 连通性预检：调用 `mcp__InfoGen__QuerySystem` 做一次探测，SystemName 传用户任务中出现的系统名；没有就用 `__infogen_probe__`。工具返回「没找到任何资源」等正常查询结果，即说明 MCP 链路正常，直接进入第 4 步。
3. 重试：仅当预检出现连接类错误（连接被拒绝、超时、会话失效、fetch failed 等）时，等待约 5 秒后原样重试，最多重试 3 次。3 次仍失败：只输出一条简短的静默提醒（说明 InfoGen MCP 服务未响应、已重试 3 次，并给出启动命令 `python InfoGen.py service mcp -n<namespace> -Dtransport=http`），然后正常结束本次请求——不派发 Agent、不编造数据、不重复报错、不输出堆栈，用户后续对话与其他功能不受任何影响。
4. 派发 Agent：预检通过后，派发 `infogen` 子代理（subagent_type=infogen），派发消息需包含：(a) 用户请求原文（即 `$ARGUMENTS`，保留原意）；(b) 用户给出的设定文档或全局提示词路径（如有）；(c) 一句提示：若上下文没有注入 InfoGen 全局提示词，可尝试读取 `InfoGen_Data/StreamingAssets/Prompts/Global.txt`。
5. 结果转述：把 Agent 返回的完整结果原样转述给用户，不删减、不改写、不再加工。本次请求的整个处理过程中不做任何写入或修改操作。
