# DetailedInfoGen-AI-Ready

> 基于 **C++26** 与 **Python** 混编的 SpaceEngine 详细信息生成器（含 AI 能力）。

本项目使用 C++ 实现高性能核心逻辑，并通过 [pybind11](https://github.com/pybind/pybind11) 将其编译为 Python 扩展模块，
从而在 Python 侧（脚本、AI 调用、数据处理）直接复用 C++ 核心。构建系统采用 CMake，所有头文件、
源文件与第三方依赖均通过**自动查找**纳入构建，新增代码或依赖时无需手动修改 `CMakeLists.txt`。

---

## 目录结构

```
DetailedInfoGen-AI-Ready/
├── CMakeLists.txt              # 顶层构建脚本（自动查找头/源/第三方）
├── README.md                   # 本文档
├── LICENSE                     # 许可证
├── .gitignore                  # 忽略规则
├── InfoGen.py                  # Python 入口脚本
└── InfoGen_Data/               # 工程数据与源码包（Python 包）
    ├── __init__.py             # 包标识
    ├── requirements.txt        # Python 运行时依赖
    ├── Headers/                # C++ 头文件（自动查找，含子目录）
    │   └── InfoGen/
    │       └── InfoGen.h
    ├── Sources/                # C++ 源文件（自动查找，含子目录）
    ├── Resources/              # 资源文件（如语法/配置模板）
    ├── Config/                 # 配置文件
    └── 3rdParty/               # 第三方依赖（自动查找，每个含 CMakeLists.txt 的子目录均被引入）
        ├── antlr/              # ANTLR 运行时（占位目录）
        ├── fmtlib/             # {fmt} 格式化库（git 子模块）
        └── pybind/             # pybind11 绑定框架（git 子模块）
```

---

## 环境要求

| 工具 / 依赖        | 版本要求                                  | 说明                                   |
| ------------------ | ----------------------------------------- | -------------------------------------- |
| CMake              | >= 3.25                                   | 提供 `CXX_STANDARD 26` 支持            |
| C++ 编译器         | 支持 C++26 的工具链                       | 如 GCC 15+、Clang 19+、MSVC 17.10+ 等  |
| Python             | 3.x（含开发头文件与链接库）               | `Development.Module` 组件               |
| fmtlib / pybind11  | 随仓库附带的 3rdParty 子模块              | 无需另行安装                           |

> 提示：若使用 git 子模块拉取 `fmtlib` / `pybind`，克隆后请执行：
> ```bash
> git submodule update --init --recursive
> ```

---

## 构建步骤

推荐使用独立构建目录（out-of-source build）：

```bash
# 1. 创建并进入构建目录
mkdir build && cd build

# 2. 配置工程（会自动查找头文件、源文件与 3rdParty 依赖）
cmake .. -DCMAKE_BUILD_TYPE=Release

# 3. 编译
cmake --build . --config Release
```

构建成功后，会在 `build/lib/`（或 `build/bin/`）下生成可导入的 Python 扩展模块
`InfoGen.<平台后缀>`（Windows 上为 `InfoGen.pyd`，Linux/macOS 上为 `InfoGen.so`）。

在 Python 中即可使用：

```python
import InfoGen
# 调用由 C++ 暴露的接口 ...
```

---

## 自动查找机制说明

`CMakeLists.txt` 通过以下规则实现“零配置”扩展：

1. **头文件**：`file(GLOB_RECURSE ... CONFIGURE_DEPENDS)` 递归扫描
   `InfoGen_Data/Headers` 下的 `.h / .hpp / .hh / .hxx`。
2. **源文件**：同样递归扫描 `InfoGen_Data/Sources` 下的
   `.cpp / .cc / .cxx / .c++`。
   - 使用 `CONFIGURE_DEPENDS` 后，Ninja / Makefiles 生成器会在新增或删除文件时
     自动重新配置；使用 Visual Studio 生成器时请手动重新运行 `cmake`。
3. **第三方依赖**：遍历 `InfoGen_Data/3rdParty` 下的每个子目录，**凡是包含
   `CMakeLists.txt` 的子目录都会被 `add_subdirectory` 纳入构建**。
   当前内置 `fmtlib`、`pybind`、`antlr`；后续新增依赖只需把目录放入
   `3rdParty` 即可，无需改动构建脚本。

---

## 当前状态（骨架）

- `InfoGen_Data/Sources/` 目前为空：待添加 C++ 实现文件，且其中**至少包含一个
  `PYBIND11_MODULE(InfoGen, m)` 绑定入口**，编译后模块方可被 Python 导入。
- `InfoGen_Data/Headers/InfoGen/InfoGen.h` 为占位头文件，可在此定义对外接口。
- `InfoGen_Data/3rdParty/antlr/` 当前仅含占位 `CMakeLists.txt`，待补充 ANTLR
  C++ 运行时源码后由其目录下的构建脚本接入。

在源文件就绪前，`cmake` 配置阶段仍可正常完成（会打印警告提示尚未发现源文件）。
