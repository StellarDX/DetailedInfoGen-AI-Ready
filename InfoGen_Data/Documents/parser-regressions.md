# parser 回归测试

结构完整性测试检查缺文件、lexer/parser 错误和尾随未消费输入。数值测试单独检查 fast_float 路径中的相关整数/十进制转换错误、signed minimum，以及有效标量和重复 block。

```powershell
cmake -S . -B build -DINFOGEN_BUILD_PARSER_TESTS=ON
cmake --build build --target infogen_parser_tests
ctest --test-dir build -R '^parser_' --output-on-failure
python -m pytest tests -m unit -q
```

`INFOGEN_BUILD_PARSER_TESTS` 默认关闭。入口与 `tests/parser.cmake`、实际 C++ 回归一起提供，不依赖 native 修复。parser 测试程序本身不嵌入 Python，项目配置仍沿用生产构建的 Python 组件要求，不额外要求 Development.Embed。

pytest 集成测试通过 `INFOGEN_PARSER_TEST_EXE`、`INFOGEN_RUNTIME_DIRS` 指定实际测试程序和动态库目录，再运行 `python -m pytest tests -m integration -q`。多个目录按 `os.pathsep` 分隔，Windows 使用分号；缺少产物会明确失败。

当前12份真实输入（旧520对象系统 + 本批11份、1891对象的0.991导出）未观察到输出对象数变化。它们没有覆盖所有合法标量形式，不能据此声称所有有效输入都不变。仓库保留的三个小型真实 fixture 只覆盖其声明的来源和形状，不代表完整版本认证。

数值错误检查主要覆盖 fast_float 分支；hexadecimal floating 使用的 double-conversion 路径没有同样的完整范围保证，`0x1p99999` 等探针仍会被接受。十进制下溢如 `1e-9999` 现在会拒绝。这些是数值转换策略，与语法完整性检查分开审阅。

2026-10-22 的性能对照使用 Ryzen 5 5600H、相同 GCC 16.2.0 / ANTLR 4.13.2 依赖和 `-O2` 构建，两进程固定同一逻辑 CPU。2轮预热，21轮按文件交替 AB/BA；计时包括 ParseFile 的读文件、解析、listener 和生产入口启用的 profiling 报告，不含进程启动、结果计数和返回对象析构。

| 整批11份0.991导出 | 基线中位 / P95（秒） | PR中位 / P95（秒） | 中位开销 |
|---|---:|---:|---:|
| 首轮，有其他审计任务运行 | 2.3378 / 3.0932 | 2.3501 / 3.4988 | +0.53% |
| 停止其他重任务后的独立复测 | 2.4035 / 2.8818 | 2.3874 / 2.6002 | -0.67% |

独立复测的21轮累计时间按每文件一次归一化为52.662→51.254秒；小文件每测点实际重复10次，所有调用累计为56.018→54.608秒。旧520对象系统及857/740/286对象系统都已测；普通无 profiling 模式也完成21轮。

在这台机器和这批真实输入上，没有稳定观察到明显的有效输入整体性能退化。首轮和独立复测原始数据均保留，未删离群点；微小负开销不算性能优化，也不外推其他平台。

完整性检查没有新增从头重扫 token 的流程。ReadInteger 增加了字符串复制及可能的分配。synthetic-error 更快主要是拒绝后不再走 listener 等后续处理，不能算正常输入收益；内部语法错误仍可能等 Parser.table() 返回才拒绝，并非全部立即失败。
