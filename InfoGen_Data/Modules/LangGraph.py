from InfoGen_Data import InfoGenHelpFormatter
from InfoGen_Data import ArgToDict
from InfoGen_Data import SetNamespace
from InfoGen_Data import QuerySystem, QueryAllObjectsInSystem, QueryObject

import tomllib
import httpx

from pathlib import Path
from typing import Sequence, Callable
from datetime import datetime
from langchain_core.messages import SystemMessage, HumanMessage, AIMessage
from langchain_core.prompts import PromptTemplate, ChatPromptTemplate
from langchain_core.tools import BaseTool
from langchain_openai import ChatOpenAI
from langgraph.graph import StateGraph, MessagesState, START, END
from langgraph.prebuilt import ToolNode

_ChatClient = None

def ChatClient():
    global _ChatClient
    if _ChatClient == None:
        with Path("./InfoGen_Data/Config/Chat.toml").open("rb") as Conf:
            CliConfig = tomllib.load(Conf)
        CliConfig["http_client"] = httpx.Client(proxy = None, trust_env = False)
        _ChatClient = ChatOpenAI(**CliConfig)
    return _ChatClient

class IGChatState(MessagesState):
    Client:ChatOpenAI
    QueryCount:int
    MaxQueryCount:int
    OutputFile:Path
    Verbose:bool

def SendMessage(State:IGChatState):
    print("生成内容...")

    Response = ""
    if State["Verbose"]:
        Response = None
        for Chunk in State["Client"].stream(State["messages"]):
            Response = Chunk if Response is None else Response + Chunk
            if Chunk.content:
                print(Chunk.content, end = "", flush = True)
        if Response is not None and len(Response.content) != 0:
            print()
        if Response is None:
            Response = AIMessage(content = "")
    else:
        Response = State["Client"].invoke(State["messages"])

    return {"messages":[Response]}

def ToolCond(State:IGChatState):
    Latest = State["messages"][-1]
    if Latest.tool_calls and State["QueryCount"] < State["MaxQueryCount"]:
        return "CallTool"
    elif State["QueryCount"] >= State["MaxQueryCount"]:
        return "TooMuchToolCalls"
    return "Continue"

def ToolCallInc(State:IGChatState):
    return {"QueryCount": State["QueryCount"] + 1}

def WriteFile(State:IGChatState):
    with open(State["OutputFile"], "w", encoding = "utf-8") as fout:
        fout.write(State["messages"][-1].content)
    print(f"文件已写入：{State["OutputFile"]}")
    return {}

def TooMuchToolCalls(State:IGChatState):
    print("工具调用次数太多")
    return {}

def InitState(args, Tools:Sequence[BaseTool | Callable]):
    State:IGChatState = {"messages": []}

    print("创建客户端...")
    State["Client"] = ChatClient().bind_tools(Tools)
    
    print("读取全局提示词...")
    GlobalPrompt = Path(args.global_prompt).read_text(encoding = 'utf-8')
    State["messages"].append(SystemMessage(GlobalPrompt))

    print("读取用户提示词...")
    UserPrompt = Path(args.input).read_text(encoding = 'utf-8')
    UserPromptTemplate = ChatPromptTemplate.from_messages([("human", UserPrompt)])
    FinalUserPrompt = UserPromptTemplate.invoke({i: j for i, j in args.define}).to_messages()[0] # 按理来说这里只会返回格式化后的提示词
    State["messages"].append(FinalUserPrompt)
    
    State["OutputFile"] = Path(args.output) / ("InfoGen-" + datetime.now().strftime("%Y-%m-%dT%H-%M-%S") + ".md")
    State["QueryCount"] = 0
    State["MaxQueryCount"] = args.max_tool_calls
    State["Verbose"] = args.verbose

    return State

def InitWorkflow(Tools:Sequence[BaseTool | Callable]):
    Workflow = StateGraph(IGChatState)

    # 节点
    Workflow.add_node("SendMsg", SendMessage)
    Workflow.add_node("ToolCall", ToolNode(Tools))
    Workflow.add_node("ToolCallInc", ToolCallInc)
    Workflow.add_node("WriteFile", WriteFile)
    Workflow.add_node("TooMuchToolCalls", TooMuchToolCalls)

    # 边
    Workflow.add_edge(START, "SendMsg")
    Workflow.add_conditional_edges("SendMsg", ToolCond, {"CallTool": "ToolCallInc", "Continue": "WriteFile", "TooMuchToolCalls": "TooMuchToolCalls"})
    Workflow.add_edge("ToolCallInc", "ToolCall")
    Workflow.add_edge("ToolCall", "SendMsg")
    Workflow.add_edge("WriteFile", END)
    Workflow.add_edge("TooMuchToolCalls", END)

    return Workflow.compile()

def Register(MainArgParser):
    ParserLLM = MainArgParser.add_parser("generate", help = "AI生成内容", formatter_class = InfoGenHelpFormatter)
    ParserLLM.add_argument("-S", "--input", type = str, required = True, help = "用户提示词文件")
    ParserLLM.add_argument("-B", "--output", type = str, default = "./Export", help = "输出目录")
    ParserLLM.add_argument("-C", "--global-prompt", type = str, default = "./InfoGen_Data/StreamingAssets/Prompts/Global.txt", help = "全局提示词文件")
    ParserLLM.add_argument("-D", "--define", action = 'append', type = ArgToDict, default = [], help = "用户提示词中如有定义变量，用这个参数赋值")
    ParserLLM.add_argument("-n", "--namespace", type = str, required = True, help = "命名空间")
    ParserLLM.add_argument("--max-tool-calls", type = int, default = 10, help = "最大调用工具次数")
    ParserLLM.add_argument("-v", "--verbose", action='store_true', help = "流式输出模型返回的内容")

def Generate(args):
    SetNamespace(args.namespace)
    ToolList = [QuerySystem, QueryAllObjectsInSystem, QueryObject]
    State = InitState(args, ToolList)
    WorkFlow = InitWorkflow(ToolList)
    print("初始化完成")

    print("开始生成（按ctrl+C中止）")
    State = WorkFlow.invoke(State)