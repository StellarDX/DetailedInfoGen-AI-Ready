# ============================================================================
# InfoGen —— C++26 / Python 混合项目构建脚本
#
# 设计要点：
#   1. 头文件自动查找：InfoGen_Data/Headers 及其子目录
#   2. 源文件自动查找：InfoGen_Data/Sources 及其子目录
#   3. 第三方依赖自动查找：InfoGen_Data/3rdParty 下的每个子目录
#      约定：包含 CMakeLists.txt 的子目录会被 add_subdirectory 纳入构建
#   4. 通过 pybind11（位于 3rdParty）将 C++ 核心编译为 Python 扩展模块
# ============================================================================

# 注：本构建脚本为腾讯云混元3模型生成

cmake_minimum_required(VERSION 4.0)

# 工程名（按需求显式设置）
set(PROJECT_NAME "InfoGen")
project(${PROJECT_NAME}
    VERSION "26.2"
    DESCRIPTION "Detailed SpaceEngine Info Generator with AI"
    LANGUAGES C CXX
)

# ----------------------------------------------------------------------------
# 全局编译选项（C++26）
# ----------------------------------------------------------------------------
set(CMAKE_CXX_STANDARD 26)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Release" CACHE STRING "构建类型 (Debug/Release)" FORCE)
endif()

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# 统一产物输出目录，便于查找生成物
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/InfoGen_Data)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/InfoGen_Data/Libraries)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/InfoGen_Data/Libraries)

# ----------------------------------------------------------------------------
# Python 环境（pybind11 在 add_subdirectory 前需要 Python 开发组件）
# ----------------------------------------------------------------------------
find_package(Python3 COMPONENTS Interpreter Development.Module REQUIRED)

# ----------------------------------------------------------------------------
# 自动查找第三方依赖（InfoGen_Data/3rdParty）
# 每个包含 CMakeLists.txt 的子目录都会作为子工程加入构建。
# 当前内置：fmtlib（格式化库）、pybind（pybind11 绑定框架）、antlr（占位）。
# 新增依赖时只需把目录放进 3rdParty 即可，无需修改本脚本。
# ----------------------------------------------------------------------------
set(INFOGEN_ADDED_3RDPARTY_DIRS "")
file(GLOB THIRD_PARTY_DIRS "${CMAKE_CURRENT_SOURCE_DIR}/InfoGen_Data/3rdParty/*")
foreach(_dir ${THIRD_PARTY_DIRS})
    if(IS_DIRECTORY "${_dir}" AND EXISTS "${_dir}/CMakeLists.txt")
        message(STATUS "[${PROJECT_NAME}] 引入第三方依赖: ${_dir}")
        add_subdirectory("${_dir}")
        list(APPEND INFOGEN_ADDED_3RDPARTY_DIRS "${_dir}")
    endif()
endforeach()

# pybind11 目标兜底：若 3rdParty 中未提供，则尝试系统安装版本
if(NOT TARGET pybind11::headers)
    find_package(pybind11 CONFIG REQUIRED)
endif()

# ----------------------------------------------------------------------------
# 动态收集「已发现依赖」的可消费目标（不硬编码具体目标名）
# 遍历每个被 add_subdirectory 引入的 3rdParty 子目录，读取其
# BUILDSYSTEM_TARGETS，再按目标类型分流：
#   - 真正的库目标（静态/共享/模块/对象/未知库）→ 后续用 target_link_libraries 链接；
#   - 接口库目标 → 同样链接到扩展模块。许多第三方依赖（如 ANTLR）通过
#     find_package 引入的导入目标默认仅在其子目录作用域可见，不能直接被
#     顶层工程按名引用；这类依赖通常提供一个「普通 INTERFACE 库包装目标」
#     （在其子目录作用域内链接真正的导入目标），链接该包装即可把库文件与
#     头文件包含目录一并传播给消费目标。纯头文件接口库（如 pybind11::headers）
#     被重复链接是无害且幂等的（pybind11_add_module 已链接等价目标）。
#   - 跳过 ALIAS 目标：其底层真实目标会被单独收集，避免重复链接。
# 新增依赖只需把目录放进 3rdParty 并实现相应 CMake 目标，无需改动此处。
# ----------------------------------------------------------------------------
set(INFOGEN_DEP_TARGETS "")
set(INFOGEN_DEP_INCLUDE_DIRS "")

foreach(_dir ${INFOGEN_ADDED_3RDPARTY_DIRS})
    get_property(_dir_targets DIRECTORY "${_dir}" PROPERTY BUILDSYSTEM_TARGETS)
    foreach(_t ${_dir_targets})
        if(NOT TARGET "${_t}")
            continue()
        endif()
        # 跳过 ALIAS：其底层真实目标会被单独收集，避免重复链接。
        get_target_property(_t_aliased "${_t}" ALIASED_TARGET)
        if(_t_aliased)
            continue()
        endif()
        get_target_property(_t_type "${_t}" TYPE)
        if(_t_type STREQUAL "INTERFACE_LIBRARY")
            # 接口库：链接它（传播其 INTERFACE 包含目录与经其链接的导入库），
            # 同时显式收集其公开包含目录作为兜底。
            list(APPEND INFOGEN_DEP_TARGETS "${_t}")
            get_target_property(_t_inc "${_t}" INTERFACE_INCLUDE_DIRECTORIES)
            if(_t_inc)
                list(APPEND INFOGEN_DEP_INCLUDE_DIRS ${_t_inc})
            endif()
        elseif(_t_type MATCHES "^(STATIC_LIBRARY|SHARED_LIBRARY|MODULE_LIBRARY|OBJECT_LIBRARY|UNKNOWN_LIBRARY)$")
            # 真正的库目标：稍后链接，其 INTERFACE 包含目录会随链接自动传播。
            list(APPEND INFOGEN_DEP_TARGETS "${_t}")
        endif()
    endforeach()
endforeach()

list(REMOVE_DUPLICATES INFOGEN_DEP_TARGETS)
list(REMOVE_DUPLICATES INFOGEN_DEP_INCLUDE_DIRS)

list(LENGTH INFOGEN_DEP_TARGETS _dep_count)
message(STATUS "[${PROJECT_NAME}] 收集到可链接依赖目标 ${_dep_count} 个: ${INFOGEN_DEP_TARGETS}")
if(INFOGEN_DEP_INCLUDE_DIRS)
    message(STATUS "[${PROJECT_NAME}] 收集到依赖接口包含目录 ${INFOGEN_DEP_INCLUDE_DIRS}")
endif()

# ----------------------------------------------------------------------------
# 自动查找头文件与源文件
# 使用 CONFIGURE_DEPENDS，使支持的生成器（Ninja / Makefiles）在新增/删除
# 文件时自动重新配置；使用 Visual Studio 生成器时请手动重新运行 cmake。
# ----------------------------------------------------------------------------
file(GLOB_RECURSE PROJECT_HEADERS
    CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/InfoGen_Data/Headers/*.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/InfoGen_Data/Headers/*.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/InfoGen_Data/Headers/*.hh"
    "${CMAKE_CURRENT_SOURCE_DIR}/InfoGen_Data/Headers/*.hxx"
)

file(GLOB_RECURSE PROJECT_SOURCES
    CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/InfoGen_Data/Sources/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/InfoGen_Data/Sources/*.cc"
    "${CMAKE_CURRENT_SOURCE_DIR}/InfoGen_Data/Sources/*.cxx"
    "${CMAKE_CURRENT_SOURCE_DIR}/InfoGen_Data/Sources/*.c++"
)

list(LENGTH PROJECT_HEADERS _hcount)
list(LENGTH PROJECT_SOURCES _scount)
message(STATUS "[${PROJECT_NAME}] 发现头文件 ${_hcount} 个，源文件 ${_scount} 个")

# ----------------------------------------------------------------------------
# 构建目标
# 将 C++ 核心源码（含绑定入口 PYBIND11_MODULE）编译为可导入的 Python 扩展模块。
# 命名空间头文件目录作为公共包含路径暴露给目标。
# ----------------------------------------------------------------------------
if(PROJECT_SOURCES)
    pybind11_add_module(${PROJECT_NAME}
        ${PROJECT_SOURCES}
        ${PROJECT_HEADERS}
    )

    target_include_directories(${PROJECT_NAME} PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/InfoGen_Data/Headers"
    )

    # 链接所有「已发现依赖」收集的库目标（动态收集，不硬编码具体目标名）。
    # 这些目标的 INTERFACE 包含目录会随链接自动传播，从而引入其头文件。
    if(INFOGEN_DEP_TARGETS)
        target_link_libraries(${PROJECT_NAME} PRIVATE ${INFOGEN_DEP_TARGETS})
    endif()

    # 显式引入接口库依赖（如 pybind11::headers）的公开头文件目录，
    # 保证即使未被直接链接，其头文件也对模块可见。
    if(INFOGEN_DEP_INCLUDE_DIRS)
        target_include_directories(${PROJECT_NAME} PRIVATE ${INFOGEN_DEP_INCLUDE_DIRS})
    endif()

    # 模块名保持为 InfoGen，便于 Python 侧 `import InfoGen`
    set_target_properties(${PROJECT_NAME} PROPERTIES OUTPUT_NAME "${PROJECT_NAME}")

    install(TARGETS ${PROJECT_NAME} LIBRARY DESTINATION "${PROJECT_NAME}_Data")
else()
    message(FATAL
        "[${PROJECT_NAME}] 未在 InfoGen_Data/Sources 中发现源文件"
        "添加 .cpp/.cc 源文件（含 PYBIND11_MODULE 入口）后即可构建。")
endif()

# ----------------------------------------------------------------------------
# 安装 Python 脚本与包（按原目录结构）
#   - 工程根目录下的 InfoGen.py
#   - InfoGen_Data 目录及其子目录下的全部 .py 文件
# 以与源码一致的目录层级安装到 CMAKE_INSTALL_PREFIX（PREFIX）下。
# 3rdParty 子树（纯 C++ 第三方依赖）整体排除。
# 使用 CONFIGURE_DEPENDS，在支持的生成器上新增/删除 .py 文件时自动重新配置；
# 使用 Visual Studio 生成器时请手动重新运行 cmake。
# ----------------------------------------------------------------------------
set(PYTHON_INSTALL_FILES "${CMAKE_CURRENT_SOURCE_DIR}/InfoGen.py")

file(GLOB_RECURSE _PYTHON_DATA_FILES
    CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/InfoGen_Data/*.py"
)
list(APPEND PYTHON_INSTALL_FILES ${_PYTHON_DATA_FILES})

foreach(_py ${PYTHON_INSTALL_FILES})
    # 跳过 3rdParty（C++ 相关依赖，无 Python 运行时安装需求）
    if(_py MATCHES "InfoGen_Data/3rdParty")
        continue()
    endif()

    # 计算相对于工程根目录的路径，从而原样保留目录结构
    file(RELATIVE_PATH _rel "${CMAKE_CURRENT_SOURCE_DIR}" "${_py}")
    get_filename_component(_dest_dir "${_rel}" DIRECTORY)

    # 根目录文件（如 InfoGen.py）的相对目录为空，安装到前缀根
    if(NOT _dest_dir)
        set(_dest_dir ".")
    endif()

    install(FILES "${_py}" DESTINATION "${_dest_dir}")
endforeach()

list(LENGTH PYTHON_INSTALL_FILES _pycount)
message(STATUS "[${PROJECT_NAME}] 将安装 Python 文件 ${_pycount} 个（已排除 3rdParty）")
