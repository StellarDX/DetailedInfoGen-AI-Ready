# parser 回归测试

这组测试检查缺文件、语法错误、尾随 token、整数和实数越界，以及 signed minimum、有效标量、重复 block 和真实导出的通过控制。

```powershell
cmake -S . -B build -DINFOGEN_BUILD_PARSER_TESTS=ON
cmake --build build --target infogen_parser_tests
ctest --test-dir build -R '^parser_' --output-on-failure
python -m pytest tests -m unit -q
```

`INFOGEN_BUILD_PARSER_TESTS` 默认关闭。入口与 `tests/parser.cmake`、实际 C++ 回归一起提供，不依赖 native 修复。parser 测试程序本身不嵌入 Python，项目配置仍沿用生产构建的 Python 组件要求，不额外要求 Development.Embed。

pytest 集成测试通过 `INFOGEN_PARSER_TEST_EXE`、`INFOGEN_RUNTIME_DIRS` 指定实际测试程序和动态库目录，再运行 `python -m pytest tests -m integration -q`。多个目录按 `os.pathsep` 分隔，Windows 使用分号；缺少产物会明确失败。

有效输入的输出结构保持不变。过去被吞掉的不完整输入、非法 token 和标量溢出现在会报错；没有增加 SpaceEngine 版本特判，也没有更改 Class 映射或数组表示。三个仓库内真实 fixture 只覆盖各自声明的来源和形状，不代表完整版本认证。
