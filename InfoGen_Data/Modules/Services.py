from InfoGen_Data import InfoGenHelpFormatter, ArgToDict
from InfoGen_Data import SetNamespace, QuerySystem, QueryAllObjectsInSystem, QueryObject

from pathlib import Path
from argparse import SUPPRESS

import os
os.environ["FASTMCP_CHECK_FOR_UPDATES"] = "off"
from fastmcp import FastMCP
from fastmcp.tools import Tool

def MCP(args):
    if args.namespace:
        SetNamespace(args.namespace)
    else:
        raise ValueError("必须指定一个命名空间")

    SysArgs = {
        "name": "InfoGen", 
        "version": "26H2"
    }
    if args.mcp_global_prompt_instruction:
        SysArgs["instructions"] = Path(args.mcp_global_prompt_file).read_text(encoding = 'utf-8')

    RunArgs = {i: j for i, j in args.define}
    if args.mcp_ipv6:
        RunArgs["host"] = "::1"

    MCPServ = FastMCP(**SysArgs)
    MCPServ.add_tool(Tool.from_function(QuerySystem))
    MCPServ.add_tool(Tool.from_function(QueryAllObjectsInSystem))
    MCPServ.add_tool(Tool.from_function(QueryObject))
    MCPServ.run(**RunArgs)

def Columbina(args): # 空月之歌彩蛋
    Moon1_ = r"""
                                        .
                                             -.
                                                 .
                                                    .
                                                      .
                                                        .
                                                          .
                                                           \
                                                            \
                                                             \
                                                              \
                                                               \
                                                               |
                                                                \
                                                                |
                                                                |
                                                                |
                                                                |
                                                                /
                                                               /
                                                               /
                                                              /
                                                             /
                                                            /
                                                           /
                                                          '
                                                        '
                                                      '
                                                    '
                                                 '
                                             -'
                                        '
"""

    Moon2_ = r"""
                                --------.
                                    .    `----.
                                .          .   `-.
                                   .              `-.
                                     @@@@@@@@    .   `.
                                    @@@@@@@@@@         `.
                                    @@@@@@@@@@      o    `.
                                 @   @@@@@@@@@@  @@     .  \
                                .  @@   @@@@@@@@@@@@     @@ \
                                   @   @@@@@@@@@@@@@@ o @@@@ \
                                       @@@@@@@@@@@@@@@  @@@@@ \
                                    .   @@@@@@@@@@@@@@   @@@   \
                                         @@@@@@@@@@@@ @@@    . |
                                          @@       . @@@@@@     \
                                         @@    .     @@@@@@     |
                                   o       @@@   o   @@@@@@.    |
                                          @@@@@       @@@       |
                                          @@@@@     .    .      |
                                -.      .  @@@  .           .   /
                                -'                     .       /
                                     .            o            /
                                            .                 /
                                 .                    o      /
                                      o           .         /
                                _         .   .     .--.   /
                                                   `--'  .'
                                  .                    .'
                                        O      .     .'
                                   o              .-'
                                        .      .-'
                                 .       .----'
                                --------'
"""

    Moon3_ = r"""
                         .--------------.
                   .----'  o        .    `----.
                .-'     .    O  .          .   `-.
             .-'      @@@@@@       .              `-.
           .'@     @@@@@@@@@@@       @@@@@@@@    .   `.
         .'@@    @@@@@@@@@@@@@@     @@@@@@@@@@         `.
       .'@@@ o   @@@@@@@@@@@@@@     @@@@@@@@@@      o    `.
      /@@@       @@@@@@@@@@@@@@  @   @@@@@@@@@@  @@     .  \
     /            @@@@@@@@@@@   .  @@   @@@@@@@@@@@@     @@ \
    /@  o     .     @@@@@@ o       @   @@@@@@@@@@@@@@ o @@@@ \
   /@@@                        .       @@@@@@@@@@@@@@@  @@@@@ \
  /@@@@@                     @      .   @@@@@@@@@@@@@@   @@@   \
  |@@@@@     o      `.-./  .             @@@@@@@@@@@@ @@@    . |
 / @@@@@           __`-'       o          @@       . @@@@@@     \
 |@ @@@@ .        @    `    @            @@    .     @@@@@@     |
 |   @@       @                    o       @@@   o   @@@@@@.    |
 |          @                             @@@@@       @@@       |
 |  . .  @      @  @       o              @@@@@     .    .      |
 \            @                .-.      .  @@@  .           .   /
  |    @   @   @      @        `-'                     .       /
  \   .      @   @                   .            o            /
   \     o          @@@@   .                .                 /
    \       @@@    @@@@@@        .                    o      /
     \     @@@@@   @@\@@    /         o           .         /
      \  o  @@@       \ \  /  ___         .   .     .--.   /
       `.      .       \.-.---                     `--'  .'
         `.             `-'       .                    .'
           `.    o     / |              O      .     .'
             `-.      /  |         o              .-'
                `-.           .         .      .-'
                   `----.        .       .----'
                         `--------------'
"""

    Columbina_ = r"""
                                                                            ..
             @.=@%                                                      @@...@
             @.... @:                                              @@@@  ....@
              @....    @@.                                    :@%       ....@
              .@...  .    :@                 :#%%%#+:       @*      .  ...=@
                @...  .      @     %@@##############@%###%@@      ..  ..:@
                @.:...        %@####@@#########################@.  ......@
                @.......    @####@#########%@@%######@@@@@########@ ....+@
                 @+... . :%##############@@################%@#######%..@
                   @@:  @##############@@#####################%#######@
                   #%.######@#########@#########################%######+@
 #.  :%@@@@@@%-   .%@@###############@######################@#####@####++@@@@@@@#*@@
  +@..    .         @@#######################################@###+#@#+++++@@   ...@
    #@...    .   @###########*######%##################*+#+###@#*++###*###+@#@=%@
       -@::+   @########@####+++++#@##+++######+++#+++++++#+#########%######@###%@
           @@%##############++##++#@##*#+###@##*+##*#######*###@######@#####*###*****###-
         @%############@###################@@##*#########***+##@-#############%#*##*#%.@
  .*@#***#################################@ -+#%##*#********+%*@+@@#**#@#####%#%..@@@
@-*******#######%%%#@-%###########%######@ %@@*****#********#@*@    ***@#####@**%:
  @*.......::.:....-@%##############**##@     **%#**@******+@%*  ... :*%**###%*****#+ *
      @=.:.%@..:@@#@+***##########*%***#       %*.@+**%#%@%   .  .     #***###%#***##.
                 @#@****@#########*@*@            .    .          . *@-##*%###%
                 @@*****@###########@           .      .       ---=:   %*%#####
               :********@##########**#::%%@--@@*    .   .       .....:%#@#**###%
              #@#**#@@%#@########@****@ :    .                   ......  %+***#@
                 #######@#######***@****%+-...                          @#***@ @
                @#######@+#####+*****#  .....                         @@#+*% #*#
                @##@#####: %#***+****=*-                           @@#**-  #***@
                ##@@%####@*%  .@**+  ***@#                    @%:  @%- %+++* @@
                #%==@####%******%      #%@:::.:.:::::::::::::::::@ .@#++++++@.
                @@+*@####%@****= %++++++*@::::::::::::::::::::::::@  =@*  @
                 @*+*####%%%@% %**+*+*++@*:::::::::::::::::::::::::*@**++++#
                    .@###%%%%%%%%@@#@%@@+++@     .:::::::::::::.    @++++++@
                     @###%%%%%%%%@:@***+++++@                        =@@@+*+@%
                     @###%%#%%%%@::%++*++++@@      :            :      @
                     @###%%%%%%%:   -@*@@*+#@        +         +        .
                    .####%%%%%%-                      #.       @        @
                    @##**#%%##@          ..             @               =
                    @#****%###.            -             #    #         -
                    ##***#%###              @             ..  +         @ @
                   -******###%              .%=            @   @       =   :.
                   #******####                   #@       -@    #@: -@%     @
                    *******###                      @. =*@.       .:       :@.=
                @%##@******###@                      @                    %:  ::@
              @######@******###                  @  . @         .           .=:-@
             %##*##*#*@#*****@#%                  -:==:+       ::          :-==@
            @##****++*+*+%@@%@##@*               ::==::-   =@@@@%#:       :::#:
            @#**++++++++****@%%@@-##            .:===:@                -#@@-
             @**++++++++***#%@        @@%-.     :::*@.
               *@#++++++++@@@*
"""

    match args.verbose:
        case 0 | None:
            print(Moon1_, end = "")
            print("""
    A luna, cara cantica, ne me in atra dedas.
    Aves, ex urbe aurea, ferte indicia mea.
    Aves, ferte cantam cara, ut lugeam et fata mala.
    Num in immundorum terra. Clavus ille adhuc claudit pia.
""")
        case 1:
            print(Moon2_, end = "")
            print("""
    Dormi cara columbula.
    O columbula mea.
    Splendeat fenestra, adsint somnia flora.
    O cara filia lunae.
    Neve plumam pulvis foedet tuam.
    Nec iam complexum alma.
    Capias bona somnias a luna.
""")
        case 2:
            print(Moon3_, end = "")
            print("""
    Vola, cara columbula, sine fine vola.
    Vola, cara columbula, ad sublunaria.
    Audite fati campana sonat.
    Aestus attulit spemque liberam.
    Annorum mille mala, atra mala.
    A patria sub luna ablutum.
    Ad caeruleum mare rapietur.
""")
        case _:
            print(Columbina_, end = "")
            print("""
    Luna, claras terras lustra.
    Sicut aves per umbras vola.
    Et unda vasta foeda purga.
    Pro turba fida, spem crastinam serva.
    A luna nova, surgat luna pura casta.
    Fugio aerumnam, tecum nulla fleta.
""")

Services = {
    "luna": Columbina,
    "mcp": MCP
}

def Register(MainArgParser):
    Parser = MainArgParser.add_parser("service", help = "服务", formatter_class = InfoGenHelpFormatter)
    Parser.add_argument("-n", "--namespace", type = str, help = "命名空间")
    Parser.add_argument("service", type = str, choices = Services.keys(), help = "服务名称")
    Parser.add_argument("-D", "--define", action = 'append', type = ArgToDict, default = [], help = "服务启动参数")
    Parser.add_argument("-v", "--verbose", action = 'count', help = SUPPRESS)

    # MCP服务参数
    Parser.add_argument("-6", "--mcp-ipv6", action = 'store_true', help = "使用IPv6链接，仅mcp的http模式生效")
    # 有的客户端会尝试从mcp的instructions读取全局提示词，但官方文档说这不是一个强制性实现的特性，因此提供这个选项
    Parser.add_argument("--mcp-global-prompt-instruction", action = 'store_true', help = "把prompt通过mcp的instruction传出")
    Parser.add_argument("-C", "--mcp-global-prompt-file", type = str, default = "./InfoGen_Data/StreamingAssets/Prompts/Global.txt", help = "mcp全局提示词文件")

def StartService(args):
    Services[args.service](args)