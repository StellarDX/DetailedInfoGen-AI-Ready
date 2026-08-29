# ============================================================================
# InfoGen —— ANTLR 第三方依赖接入
#
# 通过 find_package 搜索并引入「系统安装」的 ANTLR 库（C++ 运行时）。
# 搜索目录由可配置的 CMake 变量控制：
#   - ANTLR_SEARCH_DIRS : 额外搜索目录列表（分号分隔，可填多个）
#   - ANTLR4_ROOT       : ANTLR 安装根目录（便捷单前缀写法）
# 二者会在 find_package 之前并入 CMAKE_PREFIX_PATH，从而让系统优先在
# 用户指定位置查找；也可直接用标准 -DCMAKE_PREFIX_PATH=<路径> 传入。
#
# 引入后统一暴露「父作用域可见」的包装目标 antlr4::runtime 供其他子工程使用。
# 由于 find_package 导入的目标默认仅在其所在子目录作用域内可见，顶层工程
# 无法直接按名引用，故这里用普通 INTERFACE 库包装后再向上暴露。
# ============================================================================

# 注：本构建脚本为腾讯云混元3模型生成

cmake_minimum_required(VERSION 3.25)

# ----------------------------------------------------------------------------
# 1. 可配置的搜索目录变量
# ----------------------------------------------------------------------------
# ANTLR_SEARCH_DIRS：额外的搜索目录（分号分隔），可追加任意多个不标准位置。
#   Windows 常见：C:/Program Files/ANTLR/antlr4-cpp-runtime-v4.x.x
#   Linux/macOS 常见：/usr/local、/opt/antlr4
set(ANTLR_SEARCH_DIRS "" CACHE STRING
    "额外的 ANTLR 搜索目录（分号分隔）。这些目录会并入 CMAKE_PREFIX_PATH 供 find_package 查找。")

# ANTLR4_ROOT：ANTLR 安装根目录（便捷写法），应指向包含
#   lib/cmake/antlr4-runtime 的子目录。留空则仅使用系统默认路径。
set(ANTLR4_ROOT "" CACHE PATH
    "ANTLR 安装根目录（含 lib/cmake/antlr4-runtime 或 lib/cmake/antlr4 的子目录）。")

# 将可配置目录并入 CMAKE_PREFIX_PATH，使 find_package 优先在用户指定位置查找。
if(ANTLR4_ROOT)
    list(APPEND CMAKE_PREFIX_PATH "${ANTLR4_ROOT}")
endif()
if(ANTLR_SEARCH_DIRS)
    list(APPEND CMAKE_PREFIX_PATH ${ANTLR_SEARCH_DIRS})
endif()

# ----------------------------------------------------------------------------
# 2. 查找 ANTLR C++ 运行时（现代 CMake CONFIG 包）
# ----------------------------------------------------------------------------
# ANTLR4 官方 C++ 运行时自带 antlr4-runtime-config.cmake，导出目标
#   antlr4-runtime::antlr4-runtime（另有 antlr4_shared / antlr4_static 变体）。
find_package(antlr4-runtime CONFIG QUIET)

if(antlr4-runtime_FOUND)
    message(STATUS "[InfoGen/antlr] 找到 ANTLR C++ 运行时: ${antlr4-runtime_DIR}")
else()
    # 兜底：尝试经典包名 antlr4。
    find_package(antlr4 CONFIG QUIET)
    if(antlr4_FOUND)
        message(STATUS "[InfoGen/antlr] 找到 ANTLR (antlr4): ${antlr4_DIR}")
    endif()
endif()

# ----------------------------------------------------------------------------
# 3. 提供统一且「父作用域可见」的包装目标 antlr4::runtime
# ----------------------------------------------------------------------------
# find_package 导入的目标（如 antlr4-runtime::antlr4-runtime）默认仅在其
# 所在子目录及子目录内可见，父工程（顶层 CMakeLists）无法直接按名引用，
# 会导致链接与头文件包含目录传播失败。这里创建一个普通 INTERFACE 库
# antlr4_runtime，在本目录作用域内把真正的导入目标链接进来（链接引用在
# 本作用域被捕获，因此父工程链接该包装目标时可正确传播库文件与包含目录），
# 并额外显式补入其公开包含目录作为兜底。最后以 antlr4::runtime 别名对外
# 暴露稳定名称。整个过程仅依赖「发现到的导入目标名」，不硬编码具体变体
# 或安装路径。
if(NOT TARGET antlr4_runtime)
    # 不硬编码具体导入目标名：按常见导出名顺序取第一个已存在的目标。
    set(_antlr_imported_target "")
    foreach(_candidate antlr4-runtime::antlr4-runtime antlr4_shared antlr4_static antlr4-runtime antlr4)
        if(TARGET "${_candidate}")
            set(_antlr_imported_target "${_candidate}")
            break()
        endif()
    endforeach()

    if(_antlr_imported_target)
        add_library(antlr4_runtime INTERFACE)
        target_link_libraries(antlr4_runtime INTERFACE "${_antlr_imported_target}")

        # 兜底：显式补入导入目标的公开包含目录（链接本身也会自动传播）。
        get_target_property(_antlr_inc "${_antlr_imported_target}" INTERFACE_INCLUDE_DIRECTORIES)
        if(_antlr_inc)
            target_include_directories(antlr4_runtime INTERFACE ${_antlr_inc})
        endif()

        # 对外暴露稳定别名（命名空间写法），父工程与其他子工程均可引用。
        add_library(antlr4::runtime ALIAS antlr4_runtime)
    endif()
endif()

# ----------------------------------------------------------------------------
# 4. 兜底：CMake 内置 FindANTLR 模块（代码生成工具 + 旧版 C 运行时）
# ----------------------------------------------------------------------------
# 若未找到现代 CONFIG 包，再尝试内置 FindANTLR 模块，主要用于定位
# ANTLR 代码生成工具（Java）及较旧的 antlr C 库。该模块不提供导入库目标。
if(NOT TARGET antlr4::runtime)
    find_package(ANTLR QUIET)
    if(ANTLR_FOUND)
        message(STATUS "[InfoGen/antlr] 经 FindANTLR 找到工具: ${ANTLR_EXECUTABLE}")
    endif()
endif()

# ----------------------------------------------------------------------------
# 5. 结果诊断
# ----------------------------------------------------------------------------
if(TARGET antlr4::runtime)
    message(STATUS "[InfoGen/antlr] 已就绪，导入目标: antlr4::runtime")
else()
    message(WARNING
        "[InfoGen/antlr] 未找到 ANTLR C++ 运行时。可按以下任一方式提供：\n"
        "    - 设置 -DANTLR4_ROOT=<安装根目录>\n"
        "    - 设置 -DANTLR_SEARCH_DIRS=<目录1>;<目录2>\n"
        "    - 将安装路径加入 -DCMAKE_PREFIX_PATH=<路径>\n"
        "  ANTLR 官方 C++ 运行时下载：https://www.antlr.org/download.html")
endif()
