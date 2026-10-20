# 数据库回归测试

这组测试检查系统替换事务、连接释放、alias 查询去重，以及 SQL/JSON 边界的缺失值处理。

不需要 PostgreSQL 的单元测试可以直接运行：

```powershell
python -m pytest tests -m unit -q
```

真实 PostgreSQL 回归需要设置 `INFOGEN_TEST_DATABASE_URI`，指向允许创建和删除 schema 的专用测试数据库，然后运行：

```powershell
python -m pytest tests -m integration -q
```

每个测试使用独立随机 schema，完成后删除；不要把生产数据库 URI 用作测试配置。PostgreSQL 覆盖成功更新、取消、SQL 失败、提交前失败、提交后确认丢失、rollback 失败、autocommit/事务能力拒绝、NULL、alias 和重复上传。SQLite 用于表达式与单元回归，不代表其他数据库后端也完成了整链验证。

系统替换先准备全部 INSERT，再在一个事务中删除和写入，异常时尝试 rollback，最后关闭连接。提交后确认丢失会继续报错，不能据此宣称数据库已经回滚。不能可靠提供事务的连接在写入前拒绝。

不需要 schema migration。NaN/Inf 等在 SQL/JSON 边界转成 NULL/null，显式 0 保留；alias 不再让一个对象重复返回。API 参数和原有 subsystem/biosphere 输出形状保持不变。
