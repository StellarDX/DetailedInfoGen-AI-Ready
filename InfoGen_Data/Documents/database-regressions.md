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

每个测试使用独立随机 schema，完成后删除；不要使用生产数据库配置。查询和上传通过现有 SQLAlchemy、PooledDB、ADBC 路径执行，覆盖多个 alias、重复上传、NULL、公开 JSON，以及普通环缺失计数和旧库的零值哨兵。

本 PR 已撤掉事务重写和私有驱动属性检查。`DistUpgrade` 只保留连接释放，不在这轮改变提交顺序。作者在 main 的 `5299c0b` 已去掉 DELETE 后的中间 commit；本 PR 不复制该提交，由作者决定分支同步。本组用例不宣称跨驱动原子更新已验证。

元数据读取原本使用 `ADBCRawClient`；这里保留每次调用获得有效连接的生命周期修复，避免重用前一次已关闭的全局连接。上传不再以 raw connection 替代连接池。

DataFrameToSQL 将缺失值和 NaN/Inf 转成 SQL NULL。查询不再增加整树 JSON 清洗，恢复普通 json.dumps。真实查询发现的 SQL NULL → pandas NaN 遗漏，只在 BasicLabels 和子系统 subsystem_class 的构造处处理；已有物理/轨道等字段继续使用原来的 notna 处理。

ObjectCount 只在 native 的 CircumplanetaryDisc 上有意义。HasAsteroids 非空才创建该条目，ObjectCount 等于其大小，正常值至少为1。普通环不提供 ObjectCount，新写入使用 NULL；旧数据库用0代表缺失，查询继续隐藏0。此前“0是需要保留的业务数量”的解释和人工42用例已撤回。历史0兼容用例先复现失败，再验证恢复隐藏；没有改 native Rings 算法。

本批完整系统中没有生成 CircumplanetaryDisc：旧520对象系统的小行星挂在恒星A下，0.991两份含Asteroid的系统分别挂在质心和恒星B下。gbuffers_rings 只处理 Planet/DwarfPlanet/Satellite，所以现有语料无法提供真实非零 ObjectCount 的往返样本；这项覆盖仍缺失，没有用人工数字或修改对象类型代替。

alias 只改为 EXISTS 去重，恢复原有 `-A` 取全部行为，不额外增加 primary_name 匹配。absolute-orbit 缺少转换后黄道角时使用 NULL，不补造角度。

`from __future__ import annotations` 单独解决 Python 3.13 导入时求值未定义的 pd/Optional 注解的问题，不改变运行时数据。没有 schema migration。

PostgreSQL 已执行验证，其他正式支持驱动待验证；MySQL 方言编译通过不等于 MySQL 驱动执行通过。
