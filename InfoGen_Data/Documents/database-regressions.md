# 数据库回归测试

这组测试检查连接释放、alias 查询去重、SQL/JSON 的缺失值，以及 absolute-orbit 和 Rings 的数据往返。

单元测试不启动数据库。MySQL 和 PostgreSQL 只在表达式编译测试中作为方言使用：

```powershell
python -m pytest tests -m unit -q
python -m pytest tests -m smoke -q
```

真实 PostgreSQL 回归需要设置 `INFOGEN_TEST_DATABASE_URI`，指向允许创建和删除 schema 的专用测试数据库；`INFOGEN_TEST_ADBC_DRIVER` 指向 PostgreSQL ADBC 驱动文件。然后运行：

```powershell
python -m pytest tests -m integration -q
```

每个测试使用独立随机 schema，完成后删除；不要使用生产数据库配置。查询和上传通过现有 SQLAlchemy、PooledDB、ADBC 路径执行，覆盖多个 alias、重复上传、NULL、公开 JSON，以及 ObjectCount 缺失、0、42。显式 0 的用例在修改前实际失败，修改后要求数据库和公开查询都返回 0。

本 PR 已撤掉事务重写和私有驱动属性检查。`DistUpgrade` 恢复原来的连接池及删除、提交、插入顺序，只保留连接释放；**更新失败仍可能丢失旧系统，原子替换尚未修复**。连接池的事务固定机制和两个正式支持驱动的故障恢复，需要单独验证，不由本组用例宣称通过。

元数据读取原本使用 `ADBCRawClient`；这里保留每次调用获得有效连接的生命周期修复，避免重用前一次已关闭的全局连接。上传不再以 raw connection 替代连接池。

NaN/Inf 在 SQL/JSON 边界转成 NULL/null；缺失 ObjectCount 存 NULL，查询可省略该键，显式 0 和 42 原样返回。旧库中曾用 0 代表缺失的记录无法事后区分，这些 0 也会显示；不会自动猜测或迁移旧数据。

alias 只改为 EXISTS 去重，恢复原有 `-A` 取全部行为，不额外增加 primary_name 匹配。absolute-orbit 缺少转换后黄道角时使用 NULL，不补造角度。

`from __future__ import annotations` 单独解决 Python 3.13 导入时求值未定义的 pd/Optional 注解的问题，不改变运行时数据。没有 schema migration。

PostgreSQL 已执行验证，其他正式支持驱动待验证；MySQL 方言编译通过不等于 MySQL 驱动执行通过。
