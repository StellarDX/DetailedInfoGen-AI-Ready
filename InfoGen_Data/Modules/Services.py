from InfoGen_Data import InfoGenHelpFormatter, ArgToDict
from InfoGen_Data import SetNamespace, QuerySystem, QueryAllObjectsInSystem, QueryObject

import os
os.environ["FASTMCP_CHECK_FOR_UPDATES"] = "off"
from fastmcp import FastMCP
from fastmcp.tools import Tool

from typing import Literal

def MCP(args):
    RunArgs = {i: j for i, j in args.define}
    if args.ipv6:
        RunArgs["host"] = "::1"

    MCPServ = FastMCP(name = "InfoGen", version = "26H2")
    MCPServ.add_tool(Tool.from_function(QuerySystem))
    MCPServ.add_tool(Tool.from_function(QueryAllObjectsInSystem))
    MCPServ.add_tool(Tool.from_function(QueryObject))
    MCPServ.run(**RunArgs)

Services = {
    "mcp": MCP
}

def Register(MainArgParser):
    Parser = MainArgParser.add_parser("service", help = "服务", formatter_class = InfoGenHelpFormatter)
    Parser.add_argument("-n", "--namespace", type = str, required = True, help = "命名空间")
    Parser.add_argument("service", type = str, choices = Services.keys(), help = "服务名称")
    Parser.add_argument("-D", "--define", action = 'append', type = ArgToDict, default = [], help = "服务启动参数")
    Parser.add_argument("-6", "--ipv6", action = 'store_true', help = "使用IPv6链接")

def StartService(args):
    SetNamespace(args.namespace)
    Services[args.service](args)