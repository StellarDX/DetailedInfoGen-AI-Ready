#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""InfoGen MCP 连通性只读探测（供 PreToolUse 钩子调用）。

行为约定（与 InfoGen-WorkBuddy 专家一致）：
  - 仅发起只读的 MCP initialize 探测，不调用任何查询/写入工具；
  - 连续探测 3 次（含首次），全部失败才判定服务不可达；
  - 成功时静默退出，不输出任何内容，不影响工具调用；
  - 失败时输出一条 systemMessage（窗口顶部静默提醒）+ 阻断本次工具调用，
    并始终以退出码 0 结束，绝不抛异常、绝不崩端、不影响用户正常使用。

注意：本脚本不是「安装脚本」，也不写任何文件；它只是插件运行时的一段只读健康探测。
"""

import json
import os
import sys
import urllib.error
import urllib.request

DEFAULT_URL = "http://localhost:8000/mcp"

# 地址优先级：钩子传入的 ${user_config.mcp_url} > 专用环境变量 > 插件选项环境变量 > 默认值。
URL = (
    (sys.argv[1] if len(sys.argv) > 1 and sys.argv[1].strip() else None)
    or os.environ.get("INFOGEN_MCP_URL")
    or os.environ.get("CODEBUDDY_PLUGIN_OPTION_MCP_URL")
    or os.environ.get("CLAUDE_PLUGIN_OPTION_MCP_URL")
    or DEFAULT_URL
)

MAX_ATTEMPTS = 3
TIMEOUT_SECONDS = 2

REMINDER = (
    "InfoGen MCP 服务未启动或不可达（已重试 3 次），本次查询已结束；"
    "服务恢复后自动可用，其他功能不受影响。"
)

# MCP Streamable HTTP 的 initialize 请求；纯只读握手，不读取任何业务数据。
HANDSHAKE = json.dumps(
    {
        "jsonrpc": "2.0",
        "id": 1,
        "method": "initialize",
        "params": {
            "protocolVersion": "2025-06-18",
            "capabilities": {},
            "clientInfo": {"name": "InfoGen-WorkBuddy-Probe", "version": "26H2"},
        },
    }
).encode("utf-8")

HEADERS = {
    "Content-Type": "application/json",
    "Accept": "application/json, text/event-stream",
}


def reachable(url: str) -> bool:
    try:
        req = urllib.request.Request(url, data=HANDSHAKE, headers=HEADERS, method="POST")
        with urllib.request.urlopen(req, timeout=TIMEOUT_SECONDS):
            # 收到任何 HTTP 响应（2xx/3xx）都说明服务已启动并在监听。
            return True
    except urllib.error.HTTPError:
        # 4xx/5xx 同样是「服务已响应」，只是握手被拒或服务内部出错；不属于「未启动」。
        return True
    except Exception:
        # 连接被拒、超时、DNS 失败等网络层异常，才判定为服务未启动 / 不可达。
        return False


def main() -> int:
    for _ in range(MAX_ATTEMPTS):
        if reachable(URL):
            return 0  # 静默：不输出任何内容，工具调用正常继续

    # 3 次均失败：静默提醒 + 阻断本次工具调用，正常结束，不崩端。
    print(
        json.dumps(
            {
                "systemMessage": REMINDER,
                "hookSpecificOutput": {
                    "hookEventName": "PreToolUse",
                    "permissionDecision": "deny",
                    "permissionDecisionReason": REMINDER,
                },
            },
            ensure_ascii=False,
        )
    )
    return 0


if __name__ == "__main__":
    try:
        sys.exit(main())
    except Exception:
        # 兜底：任何意外都不得影响客户端。
        sys.exit(0)
