"""真实隔离 PostgreSQL 验证替换事务；故障注入不连接生产配置。"""
from argparse import Namespace
import os
from types import SimpleNamespace
from uuid import uuid4

import pytest
from sqlalchemy.dialects import postgresql
from adbc_driver_postgresql import dbapi

from support import REPO, load_python_modules
import json
import pandas as pd
from sqlalchemy import Double


class InjectedConnection:
    def __init__(self, connection, mode, observer):
        self.connection = connection
        self.mode = mode
        self.observer = observer
        self.closed = False
        self.rollbacks = 0
        self.commits = 0
        self.writes = 0
        self._autocommit = mode == 'autocommit'
        self._commit_supported = mode != 'unsupported_commit'

    def __getattr__(self, name):
        return getattr(self.connection, name)

    def cursor(self):
        actual = self.connection.cursor()
        owner = self
        class Cursor:
            def __enter__(self):
                return self
            def __exit__(self, *_):
                actual.close()
            def __getattr__(self, name):
                return getattr(actual, name)
            def execute(self, statement, *args, **kwargs):
                if statement.startswith(('DELETE','INSERT')):
                    owner.writes += 1
                if statement.startswith('INSERT INTO ig_object'):
                    if owner.mode in ('execute','rollback_failure'):
                        raise RuntimeError('注入 SQL 失败')
                    if owner.mode == 'interrupt':
                        raise KeyboardInterrupt('注入取消')
                result = actual.execute(statement, *args, **kwargs)
                if statement.startswith('DELETE'):
                    owner.observer()
                return result
        return Cursor()

    def commit(self):
        self.commits += 1
        if self.mode == 'commit_before':
            raise RuntimeError('提交前失败')
        self.connection.commit()
        if self.mode == 'commit_after':
            raise RuntimeError('提交后确认丢失')

    def rollback(self):
        self.rollbacks += 1
        if self.mode == 'rollback_failure':
            raise RuntimeError('回滚传输失败')
        self.connection.rollback()

    def close(self):
        self.closed = True
        self.connection.close()


@pytest.fixture
def database(monkeypatch):
    uri = os.environ.get('INFOGEN_TEST_DATABASE_URI')
    assert uri, '请设置隔离测试数据库 INFOGEN_TEST_DATABASE_URI'
    schema = 'contrib_' + uuid4().hex
    modules = load_python_modules(monkeypatch)
    modules.adbc._SQLVariation = postgresql.dialect()
    with dbapi.connect(uri) as connection:
        with connection.cursor() as cursor:
            cursor.execute('CREATE SCHEMA "'+schema+'"')
            cursor.execute('SET search_path TO "'+schema+'"')
        modules.adbc.ExecuteScript(connection,(REPO/'InfoGen_Data/Resources/Schema-ADBC.sql').read_text(encoding='utf-8'))
    def raw():
        connection = dbapi.connect(uri)
        with connection.cursor() as cursor:
            cursor.execute('SET search_path TO "'+schema+'"')
        connection.commit()
        return connection
    state = SimpleNamespace(mode=None, connections=[], observed=[])
    def rows():
        with raw() as connection:
            with connection.cursor() as cursor:
                cursor.execute('SELECT spectral_types,create_date FROM ig_system')
                return cursor.fetchall()
    def factory():
        connection = InjectedConnection(raw(),state.mode,lambda:state.observed.append(rows()))
        state.connections.append(connection)
        return connection
    monkeypatch.setattr(modules.loader,'ADBCClient',factory)
    monkeypatch.setattr(modules.loader,'ADBCRawClient',factory)
    monkeypatch.setattr(modules.loader,'UserConfirm',lambda _:True)
    source = dict(MainID='Root',Objects={'Root':dict(OType='Star',Identifiers=['Root','Alias'])},MinorObjects={},
        NStars=1,NPlanets=0,NDwarfPlanets=0,NSatellites=0,NMinorPlanets=0,NComets=0,StarSpectralType=['old'])
    args = Namespace(namespace='review',input=str(REPO/'tests/fixtures/spaceengine/se-0.991-real/Kepler-1512.sc'),
        code_page=65001,esi_estimator='SolarSys',absolute_orbit=False,common_plane_threshold=9,store_mode='dist-upgrade')
    def uploader(label):
        return modules.loader.Uploader({**source,'StarSpectralType':[label]},args)
    uploader('old').Run()
    old = rows()
    state.connections.clear()
    try:
        yield SimpleNamespace(modules=modules,state=state,raw=raw,rows=rows,old=old,uploader=uploader,monkeypatch=monkeypatch)
    finally:
        for connection in state.connections:
            if not connection.closed:
                connection.close()
        with dbapi.connect(uri) as connection:
            with connection.cursor() as cursor:
                cursor.execute('DROP SCHEMA "'+schema+'" CASCADE')
            connection.commit()


@pytest.mark.integration
@pytest.mark.parametrize('mode', ['success','cancel','compile','execute','interrupt','commit_before',
    'commit_after','rollback_failure','autocommit','unsupported_commit'])
def test_atomic_replacement_and_connection_release(database,mode):
    d = database
    d.state.mode = mode
    if mode == 'cancel':
        d.monkeypatch.setattr(d.modules.loader,'UserConfirm',lambda _:False)
    if mode == 'compile':
        original = d.modules.loader.Uploader.DataFrameToSQL
        def compile_statement(name,*args,**kwargs):
            if name == 'ig_object':
                raise RuntimeError('注入编译失败')
            return original(name,*args,**kwargs)
        d.monkeypatch.setattr(d.modules.loader.Uploader,'DataFrameToSQL',staticmethod(compile_statement))
    if mode in ('success','cancel'):
        d.uploader('new').Run()
    else:
        with pytest.raises((RuntimeError,KeyboardInterrupt)):
            d.uploader('new').Run()
    current = d.rows()
    assert all(connection.closed for connection in d.state.connections)
    if mode in ('success','commit_after'):
        assert current == [('new',d.old[0][1])]
    else:
        assert current == d.old
    assert all(snapshot == d.old for snapshot in d.state.observed)
    if mode in ('autocommit','unsupported_commit','compile','cancel'):
        assert sum(connection.writes for connection in d.state.connections) == 0
    if mode not in ('success','cancel'):
        assert d.state.connections[-1].rollbacks >= 1


@pytest.mark.integration
def test_postgresql_alias_null_and_repeated_upload(database):
    d = database
    d.uploader('new').Run()
    with d.raw() as connection:
        with connection.cursor() as cursor:
            cursor.execute('SELECT count(*) FROM ig_object')
            assert cursor.fetchone()[0] == 1
            cursor.execute('SELECT count(*) FROM ig_identifiers')
            assert cursor.fetchone()[0] == 2
    d.monkeypatch.setattr(d.modules.query,'ADBCClient',d.raw)
    rows = d.modules.query._QueryObject_Unchecked('review',None,['rings'])
    assert len(rows) == 1
    assert set(rows[0]['other_designations']) == {'Root','Alias'}


@pytest.mark.integration
def test_postgresql_null_storage_and_public_json(database):
    d = database
    with d.raw() as connection:
        with connection.cursor() as cursor:
            cursor.execute('SELECT object_id FROM ig_object')
            oid = cursor.fetchone()[0]
        frame = pd.DataFrame({'object_id':[oid],'surface_pressure':[float('inf')],
            'scale_height':[pd.NA]}).set_index('object_id')
        statement = d.modules.loader.Uploader.DataFrameToSQL('ig_atmosphere',frame,
            ManualMapping={'surface_pressure':Double,'scale_height':Double})
        with connection.cursor() as cursor:
            cursor.execute(statement)
            cursor.execute('SELECT surface_pressure,scale_height FROM ig_atmosphere')
            assert cursor.fetchall() == [(None,None)]
        connection.commit()
    d.monkeypatch.setattr(d.modules.query,'ADBCClient',d.raw)
    d.modules.query.SetNamespace('review')
    result = json.loads(d.modules.query.QueryObject.func(oid))
    assert len(result) == 1 and result[0]['object_id'] == oid
    assert len(json.loads(d.modules.query.QuerySystem.func('Root'))) == 1
