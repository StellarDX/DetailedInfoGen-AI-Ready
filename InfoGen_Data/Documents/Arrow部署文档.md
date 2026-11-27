# Apache Arrow部署文档

Apache Arrow在InfoGen中主要承担数据库与DataFrame之间的数据传输的功能，由于这个中间件编译难度大，所以专门写个快速部署的教程

参考：

1. [Arrow底座部署文档](https://arrow.apache.ac.cn/docs/developers/cpp/building.html)
2. [Arrow Python部署文档](https://arrow.apache.ac.cn/docs/developers/python/building.html)

## 安装前准备

[点此链接下载Arrow底座](https://github.com/apache/arrow)

### 配置需求

| 项目 | 要求 | 说明 |
| --- | --- | --- |
| 内存 | 预留1GB以上，全量编译推荐4GB | 由Arrow官方声明 |
| C++编译器 | 支持C++20 | 由Arrow官方声明 |
| Python | 3.12以上 | 推荐配置 |
| CMake | 最低3.25 | 由Arrow官方声明 |
| Ninja构建系统 | 1.13以上 | Arrow构建系统锁Ninja |
| Boost | 推荐1.90以上 | 可选项，后续下载依赖包时会顺带下载，但由于这个库较大，会拖慢编译速度，推荐提前准备好 |
| Rustup | 1.100以上 | 可选安装，Python构建环境安装时部分包要求，若触发构建则需要安装。 |

### 下载并解压Arrow底座

```bash
cd /path/to/Arrow # Arrow源码目录
git clone git@github.com:apache/arrow.git ./ --depth=1
```

### 下载依赖包

在Arrow源码目录下执行：

```bash
cd ./cpp/thirdparty
./download_dependencies.sh [ > ./vars.sh ]
```

注：下载脚本的链接大部分都是GitHub地址，如果下载速度慢，可将./versions.txt中的Github链接替换为国内镜像源。

./download_dependencies.sh开始下载后会输出一大堆环境变量，**务必保存并设置好，要不然后续CMake读取不到已经下载的依赖包**。一个简单的方式是，执行`./download_dependencies.sh | grep export > ./vars.sh`提取所有环境变量（即export开头的行）并重定向输出到一个文件中，随后source那个文件即可一键设置。

## CMake编译Arrow底座

**重要：开始部署前，请务必检查以下内容**

1. Ninja的安装目录是否有被添加到PATH环境变量中，Arrow的构建脚本是典型的CMake套娃结构，通过CMAKE_MAKE_PROGRAM变量设置的工具链传不给下一层。
2. （如果使用自定义GCC）GCC的库目录有没有在LD_LIBRARY_PATH中，不同GCC版本的abi不兼容，不设置的情况下LD会应用系统GCC的库导致ABI不兼容报错

### 配置项目

```bash
cmake \
    --preset ninja-release-python \
    -DCMAKE_C_COMPILER=<C语言编译器> \
    -DCMAKE_CXX_COMPILER=<C++编译器> \
    -DPython3_EXECUTABLE=<Python解释器> \
    -S <Arrow C++源码目录（/path/to/Arrow/cpp）> \
    -B <Arrow构建中间目录> \
    -DCMAKE_INSTALL_PREFIX=<Arrow安装目录> \
    -DBoost_DIR=<包含Boost.cmake的目录> \ # 自行准备Boost时使用
    -DBoost_INCLUDE_DIR=<Boost头文件目录> # （自行准备Boost时必须，FindBoost读取的头文件目录也传不给下级CMake）
```

### 编译

```bash
cmake --build <Arrow构建中间目录>
```

注：自行准备了Boost的情况下，Arrow编译过程仍需要1个小时左右

### 安装

```bash
cmake --install <Arrow构建中间目录>
```

安装完成后，将<Arrow安装目录>/lib添加到LD_LIBRARY_PATH中

## PIP安装PyArrow

创建Python虚拟环境并激活，然后安装Python构建环境：

```bash
source "$HOME/.cargo/env" # 可选，若触发构建则需要（安装好rustup）
CC=<C语言编译器> CXX=<C++编译器> pip install build cython libcst numpy scikit_build_core setuptools_scm
```

安装PyArrow：

```bash
CMAKE_PREFIX_PATH=<Arrow安装目录>:$CMAKE_PREFIX_PATH CC=<C语言编译器> CPP=<C++编译器> \
pip install --no-build-isolation --editable <PyArrow源码目录（/path/to/Arrow/python> -vv
```

## 验证安装

在Python的库目录（site_packages）下找到PyArrow说明安装完成了，可执行`python -c "import pyarrow as pa; print(pa.__version__)"`进一步验证。