from InfoGen_Data import InfoGenHelpFormatter, ArgToDict
from InfoGen_Data import QuerySystem, QueryAllObjectsInSystem, QueryObject

import os
os.environ["FASTMCP_CHECK_FOR_UPDATES"] = "off"
from fastmcp import FastMCP
from fastmcp.tools import Tool

from typing import Literal

def MCP(args):
    MCPServ = FastMCP(name = "InfoGen", version = "26H2")
    MCPServ.add_tool(Tool.from_function(QuerySystem))
    MCPServ.add_tool(Tool.from_function(QueryAllObjectsInSystem))
    MCPServ.add_tool(Tool.from_function(QueryObject))
    MCPServ.run(**{i: j for i, j in args.define})

Services = {
    "mcp": MCP
}

def Register(MainArgParser):
    Parser = MainArgParser.add_parser("service", help = "服务", formatter_class = InfoGenHelpFormatter)
    Parser.add_argument("service", type = str, choices = Services.keys(), help = "服务名称")
    Parser.add_argument("-D", "--define", action = 'append', type = ArgToDict, default = [], help = "服务启动参数")

def StartService(args):
    Services[args.service](args)