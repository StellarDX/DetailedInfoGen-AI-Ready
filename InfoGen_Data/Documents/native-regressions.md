# native 回归测试

这组测试覆盖轨道成员初始化、自转线速度、Extrasolar ESI、Kepler 角度、binary 判定、显示分类以及同一进程连续导入时的状态和引用释放。

准备好项目依赖后，在仓库根目录运行：

```powershell
cmake -S . -B build -DINFOGEN_BUILD_TESTS=ON
cmake --build build --target InfoGen infogen_native_tests
ctest --test-dir build -R '^native_' --output-on-failure
python -m pytest tests -m unit -q
```

`INFOGEN_BUILD_TESTS` 默认关闭，开启时额外查找 Python Development.Embed。入口与 `tests/native.cmake`、实际 C++ 回归一起提供，不依赖 parser 修复。

pytest 的 native 集成测试还需要设置 `INFOGEN_EXTENSION`、`INFOGEN_NATIVE_TEST_EXE`、`INFOGEN_RUNTIME_DIRS`，分别指向本次构建的扩展、测试程序和动态库目录，再运行 `python -m pytest tests -m integration -q`。多个动态库目录按 `os.pathsep` 分隔，Windows 使用分号。缺少产物会明确失败，不把未运行记为通过。

轨道缺失值初始化为 NaN；不符合相容条件的 binary 候选会被拒绝。显示分类的修正只影响显示回退，不修改 native 输出的分类值或现有恒星重分类逻辑。自转同步温度和多星温度求和保持现有 upstream 模型。
