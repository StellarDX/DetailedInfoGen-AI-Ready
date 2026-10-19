# 回归测试

安装项目 Python 依赖和 `pytest` 后，在仓库根目录运行：

```powershell
python -m pytest tests -m smoke -q
python -m pytest tests -m unit -q
python -m pytest tests -m integration -q
```

`smoke` 是小型子集，`unit` 不需要 native/PostgreSQL；`integration` 中缺失所需环境会明确报错，不把未运行记为通过。测试辅助加载器仅隔离包根 CLI/LLM 初始化，实际执行生产模块；不替换数据库或数值实现。

真实 fixture 的来源、SHA-256、对象数及完整根/缺根形状见 `fixtures/spaceengine/manifest.json`。只保留三个小型原样导出，不携带大型系统、开发报告或 MB 级快照。文件完整性测试不等于完整 native/uploader 兼容声明。

后续 native 和数据库修复在各自候选中增加边界回归；适用时使用 `INFOGEN_EXTENSION`、`INFOGEN_RUNTIME_DIRS`、`INFOGEN_NATIVE_TEST_EXE` 和 `INFOGEN_TEST_DATABASE_URI` 显式指定实际构建产物及隔离测试数据库。不得把生产数据库配置用作回归环境。


可选 native/parser 构建入口默认关闭：`-DINFOGEN_BUILD_TESTS=ON` 与 `-DINFOGEN_BUILD_PARSER_TESTS=ON`。相应修复候选分别提供 `tests/native.cmake`、`tests/parser.cmake` 及实际 C++ 测试；只应用测试基座时不声称存在这些目标。显式启用但目标文件缺失会配置失败。正常生产构建的 Python 组件要求保持原样；嵌入 Python 的 native 测试额外需要 Development.Embed。
