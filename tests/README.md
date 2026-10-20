# 回归测试

安装项目 Python 依赖和 `pytest` 后，在仓库根目录运行：

```powershell
python -m pytest tests -m smoke -q
python -m pytest tests -m unit -q
```

本测试基座仅提供真实 fixture 的完整性测试及 Python 模块加载辅助器。`smoke` 是小型子集，`unit` 不需要 native/PostgreSQL。测试辅助加载器仅隔离包根 CLI/LLM 初始化，实际执行生产模块；不替换数据库或数值实现。

真实 fixture 的来源、SHA-256、对象数及完整根/缺根形状见 `fixtures/spaceengine/manifest.json`。只保留三个小型原样导出，不携带大型系统、开发报告或 MB 级快照。文件完整性测试不等于完整 native/uploader 兼容声明。

本基座不新增 CMake 测试选项、目标或 include，不依赖后续 PR 的文件。native/parser 回归的构建入口、实际 C++ 测试及运行说明，将随各自修复一并提供；数据库修复同样在自己的 PR 中提供隔离环境要求和集成测试。
