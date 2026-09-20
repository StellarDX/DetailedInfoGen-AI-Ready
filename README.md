# DetailedInfoGen-AI-Ready

> 基于 **C++26** 与 **Python** 混编的 SpaceEngine 详细信息生成器（含 AI 能力）。

从SpaceEngine导出的行星系统文件（`.sc`）解析行星系统结构并生成详细信息文档。

## 目录

- [产品介绍](#产品介绍)
- [技术栈](#技术栈)
- [项目结构](#项目结构)
- [安装部署](#安装部署)
- [使用方法](#使用方法)
- [在做的功能](#在做的功能)
- [计划添加的功能](#计划添加的功能)
- [许可证](#许可证)

## 产品介绍

DetailedInfoGen（简称 InfoGen）将C++侧的ANTLR解析与天文计算功能（使用pybind11编译成Python模块`InfoGen`）与Python侧的命令行、国际化及文档渲染能力相结合，用于从SpaceEngine导出的行星系统文件中提取天体数据并输出结构化文档。

### 当前已完成的功能

1. 从SpaceEngine导出的文件生成行星系统基本信息
   - 解析行星系统文件（使用ANTLR）
   - 重建行星系统层级：自动定位系统根节点（质心），并按广度优先遍历重建父/子系统树
   - 汇总系统级信息：恒星/行星/矮行星/卫星/小行星/彗星数量、恒星光谱型
   - 输出单天体信息：轨道数据（轨道根数、双星轨道）、物理数据（半径、质量、密度、温度、光度等）、大气、海洋、生物圈
   - 生成小行星/彗星列表，支持按半径、质量、自转、逆行、高倾角等条件筛选与排序
   - 中文友好的分类命名（恒星光谱型、行星类型分级）
   - 输出结果为Markdown或用户自定义Jinja2模板格式
   - （可选）把读取的行星系统保存到数据库，支持的数据库详见[配置文件](./InfoGen_Data/Config/ADBC.toml.def)

## 技术栈

| 层次 | 技术 |
| --- | --- |
| 构建 | CMake 4.0+ |
| 核心 | C++26（`<flat_map>` / `<flat_set>` / `ranges` / `concepts` / `jthread` 等） |
| 绑定 | pybind11（C++代码编译为Python模块`InfoGen`） |
| 解析 | ANTLR4（解析SpaceEngine文件语法） |
| 数值 | Eigen（定长/动态向量与矩阵） |
| 日志 | spdlog（复用外部 fmtlib，避免重复符号） |
| 格式化/转换 | fmtlib、fast-float、google-double-conversion |
| 脚本 | Python 3.12+，argparse / gettext / polib / pandas |
| 数据库 | Apache ADBC + SQLAlchemy |

## 安装部署

### 环境要求

| 项目 | 要求 | 说明 |
| --- | --- | --- |
| 操作系统 | Windows / Linux | 仓库仅提供这两个平台的依赖清单 |
| CMake | 4.0以上 | 构建脚本声明 `cmake_minimum_required(VERSION 4.0)` |
| C++编译器 | 支持 C++26（推荐 GCC-16.1） | 需 `<flat_map>` / `<flat_set>` / `<ranges>` / `std::jthread` 等特性 |
| Python | 3.12以上 | 需`Development.Module`组件（Python头文件与库） |
| ANTLR4 C++ Runtime | v4 | 唯一需要手动准备的依赖 |

### 依赖说明

以下依赖已内置于`InfoGen_Data/3rdParty`，配置阶段由CMake按目录名排序后自动`add_subdirectory`引入，**无需手动安装，下载源码后记得git submodule update一下**：

| 依赖 | 用途 |
| --- | --- |
| fmtlib | 字符串格式化（`fmt::format`） |
| spdlog | 日志输出（配置为复用外部 fmtlib，避免`fmt::v12`符号重复定义） |
| pybind11 | 将C++代码编译为可直接`import`的Python模块 |
| Eigen | 向量/矩阵类型（`SEVec2` / `SEVec3` / `SEArray`等） |
| fast-float | 快速浮点解析 |
| google-double-conversion | 浮点数与字符串互转 |
| antlr | ANTLR接入脚本，本身不含运行时，通过`find_package`定位系统安装的ANTLR C++ Runtime |

> 注：唯一无法自动处理的依赖是**ANTLR**，必须手动下载安装（见第1步）。由于ANTLR生成的词法器/语法器源码已自带，因此**无需安装Java或ANTLR工具本身**，只需C++运行时库。

### 安装步骤

注：由于这一版本功能比以前的版本强大的多，因此安装部署的难度也提高了某些白嫖党难以接受的程度

先修课程：C++程序设计，Python程序设计，数据库原理与应用，人工智能基础，天体物理学

#### 1. 编译并安装 ANTLR C++ Runtime

从 <https://www.antlr.org/download.html> 下载 `antlr4-cpp-runtime-4.x.x-source.zip` 并解压。

```bash
cd antlr4-cpp-runtime-4.x.x-source
cmake -S . -B build \
      -DANTLR4_INSTALL=ON \
      -DCMAKE_POLICY_VERSION_MINIMUM=3.5 \
      -DCMAKE_INSTALL_PREFIX=<ANTLR4安装目录>
cmake --build build --parallel
cmake --install build
```

两个关键选项：

- `-DANTLR4_INSTALL=ON`：让ANTLR生成并安装`antlr4-runtime-config.cmake`配置包，供`find_package(antlr4-runtime CONFIG)`定位。**不开启则本项目无法找到ANTLR。**
- `-DCMAKE_POLICY_VERSION_MINIMUM=3.5`：ANTLR的`cmake_minimum_required`版本较旧，CMake 4.x已不兼容低于3.5的策略版本，必须显式指定。

安装后确认存在`lib/cmake/antlr4-runtime/antlr4-runtime-config.cmake`，该目录即为后续`ANTLR4_ROOT`的取值依据。

#### 2. 配置项目

```bash
cmake -S <源码目录> -B <构建目录> \
      -DCMAKE_BUILD_TYPE=Release \
      -DCMAKE_INSTALL_PREFIX=<安装前缀> \
      -DANTLR4_ROOT=<ANTLR4安装目录> \
      -DPython3_EXECUTABLE=/path/to/python
```

可用的 CMake 变量：

| 变量 | 说明 |
| --- | --- |
| `-DANTLR4_ROOT=<dir>` | ANTLR安装根目录（需含 `lib/cmake/antlr4-runtime`或`lib/cmake/antlr4`），推荐写法 |
| `-DANTLR_SEARCH_DIRS=<dir1>;<dir2>` | 额外的ANTLR搜索目录，分号分隔，可填多个 |
| `-DCMAKE_PREFIX_PATH=<dir>` | CMake 标准前缀路径，与前两者等价，三者可任选 |
| `-DPython3_EXECUTABLE=<python>` | 指定Python解释器，用于解析`Development.Module`组件 |
| `-DCMAKE_INSTALL_PREFIX=<dir>` | 安装前缀，**必填** |

如果终端输出`[InfoGen/antlr] 已就绪，导入目标: antlr4::runtime`，以及`发现头文件 N 个，源文件 N 个`、`收集到可链接依赖目标 N 个`说明配置成功。若出现`[InfoGen/antlr] 未找到 ANTLR C++ 运行时`错误，说明ANTLR路径未正确传入。

#### 3. 编译并安装

```bash
cmake --build <构建目录> --parallel
cmake --install <构建目录>
```

安装后，`CMAKE_INSTALL_PREFIX` 下会得到与源码一致的目录结构：

```
<安装前缀>/
├── InfoGen.py                          # 命令行入口
└── InfoGen_Data/
    ├── InfoGen.(so|pyd)                # C++ 核心编译出的 Python 扩展模块
    ......
    └── requirements-linux.txt / requirements-win.txt
```

#### 4. 复制运行时动态库

C++模块在Linux下使用`$ORIGIN`作为RPATH（构建脚本设置了`INSTALL_RPATH "$ORIGIN"`），即只会在**模块自身所在目录**查找依赖库，因此运行时库必须放到`<安装前缀>/InfoGen_Data/`下：

- **ANTLR**：将ANTLR的DLL复制到`<安装前缀>/InfoGen_Data/`。
- **GCC 运行时（仅当使用 GCC 编译，且其运行时库不在系统搜索路径时）**：
  - Windows：将 `libstdc++-6.dll`、`libgcc_s_seh-1.dll`、`libwinpthread-1.dll`等复制到`<安装前缀>/InfoGen_Data/`。
  - Linux：
    ```bash
    export LD_LIBRARY_PATH=/path/to/gcc/lib64:$LD_LIBRARY_PATH
    ```

#### 5. 安装 Python 依赖

```bash
python3 -m venv ./venv
source ./venv/bin/activate                 # Windows: .\venv\Scripts\activate
pip install -r <安装前缀>/InfoGen_Data/requirements-linux.txt   # Windows 改用 requirements-win.txt
```

依赖清单包含argparse之外的中文帮助、国际化（`polib`）、数据处理（`pandas`），以及后续AI/向量化功能所需的`langchain`、`chromadb`等组件。

#### 6. 验证安装

```bash
cd <安装前缀>
python InfoGen.py --help
```

若正常输出中文帮助信息（而非报错或乱码），则安装配置完成。

### 常见问题排查（AI生成，仅供参考！！！）

| 现象 | 原因与处理 |
| --- | --- |
| 第 1 步配置 ANTLR 时报 CMake 策略/兼容性错误 | 缺少 `-DCMAKE_POLICY_VERSION_MINIMUM=3.5` |
| 第 2 步出现 `[InfoGen/antlr] 未找到 ANTLR C++ 运行时` | 未传 `-DANTLR4_ROOT` / `-DANTLR_SEARCH_DIRS` / `-DCMAKE_PREFIX_PATH`，或该路径下没有 `lib/cmake/antlr4-runtime` |
| 链接报 `relocation R_X86_64_32 ... can not be used when making a shared object` | 静态依赖未以 `-fPIC` 编译；顶层已全局开启 `CMAKE_POSITION_INDEPENDENT_CODE`，若某依赖自行覆盖该属性需检查其 CMake |
| 链接报 `multiple definition of fmt::v12::...` 或 `duplicate section` | spdlog 捆绑 fmt 与外部 fmtlib 冲突；构建脚本已强制 `SPDLOG_FMT_EXTERNAL_HO=ON`，请勿关闭或改为 `OFF` |
| `import InfoGen` 失败，或提示找不到动态库 | 未执行第 4 步，把 ANTLR / GCC 的运行时库放到 `InfoGen_Data/` 下 |
| Linux 运行前需每次设置 `LD_LIBRARY_PATH` | 属于第 4 步 GCC 运行时的正常处理，可写入 shell 配置或启动脚本 |
| 配置时报找不到 Python 开发组件 | 安装 `python3-dev`（Linux）或在 Windows 安装包中勾选「Python 开发文件」，或用 `-DPython3_EXECUTABLE` 指定正确的解释器 |
| Windows 控制台中文乱码 | `InfoGen_Data/__init__.py` 已自动把控制台输出代码页切到 UTF-8（65001），若仍乱码请确认终端字体支持中文 |

## 使用方法

### 从SpaceEngine导出的文件生成行星系统基本信息

```
python InfoGen.py create system -S /SpaceEngine/Export/RS-xxxx.sc [-B /OutputPath]
```

解析SpaceEngine导出的文件、重建行星系统结构，并将结果写入输出目录下的`<系统主天体ID>.md`。

#### `create system` 参数说明

| 参数 | 说明 | 默认值 |
| --- | --- | --- |
| `-S, --input` | 输入的 SpaceEngine 文件（必填） | — |
| `-B, --output` | 输出目录 | `./Export` |
| `--code-page` | 输入文件的编码代码页（用于防止乱码） | `65001` |
| `--absolute-orbit` | 轨道是否为绝对参照系（如黄道、天球）；默认相对参照系（如赤道） | 关闭 |
| `--common-plane-threshold` | `10^(-x)`，当计算的黄道轨道面与上一级物体的黄道轨道面相差小于该值时视为共面（仅 `--absolute-orbit=False` 生效） | `9` |
| `--esi-estimator` | 地球相似指数算法：`SolarSys`（半径、密度、逃逸速度、温度）、`Extrasolar`（恒星辐射通量、行星半径） | `SolarSys` |
| `--sort-system` | 对输出内容按半长轴顺序排序 | 关闭 |
| `--exceptional-asteroids-pred` | 小行星列表排序依据（取值见下表） | `LargestByDiameter` |
| `--exceptional-asteroids-limit` | 小行星列表最大数量 | `50` |
| `-L, --lcid` | 输出文档的语言 ID（如 `2052` 简体中文、`1033` 英文）（TODO） | `2052` |
| `-f, --format` | 输出文档格式（当前支持 `Markdown`） | `Markdown` |
| `-D, --format-args` | 输出文档格式的额外参数，可重复传入（`key=value`，如 `-Dprecision=6`） | 空 |
| `--store` | 将生成的原始数据保存到数据库 | 关闭 |
| `--namespace` | 保存数据的命名空间 | — |

`--exceptional-asteroids-pred` 可选值：

| 取值 | 含义 |
| --- | --- |
| `LargestByDiameter` | 半径降序 |
| `MostMassive` | 质量降序 |
| `SlowestRotators` | 自转周期降序 |
| `FastestRotators` | 自转周期升序 |
| `Retrograde` | 筛选轨道倾角大于90°小于270°的小行星并按倾角升序排序 |
| `HighlyInclined` | 轨道面与黄道面夹角降序 |

### 数据库联动

```
python InfoGen.py adbc init --driver="/path/to/libadbc_driver_xxxx.dll" --uri="<数据库链接>"
```

此命令会自动在URL对应的数据库里建立表结构，并在./InfoGen_Data/Config目录下生成一个配置文件，内容就是命令里填的驱动和数据库URL。目标数据库必须是一个空的数据库。

#### `adbc init` 参数说明

| 参数 | 说明 | 默认值 |
| --- | --- | --- |
| `--driver` | ADBC驱动路径（必填） | — |
| `--uri` | 连接目标：文件路径或DSN | — |
| `-F, --force` | 覆盖已存在的配置 | 关闭 |

### 查看帮助

```
python InfoGen.py --help          # 查看全部命令
python InfoGen.py create --help   # 查看某命令的详细参数
```

## 在做的功能

1. 国际化翻译管理系统
2. ChromaDB持久化

## 计划添加的功能

2. AI模型接入

## 许可证

本项目基于 [GPL-3.0](LICENSE) 许可协议开源。
