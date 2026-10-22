"""通过现有 SQLAlchemy / PooledDB / ADBC 路径验证 PostgreSQL 数据往返。"""
from argparse import Namespace
import json
import os
from types import SimpleNamespace
from uuid import uuid4

import pandas as pd
import pytest
from sqlalchemy import Double
from sqlalchemy.dialects import postgresql

from support import REPO, load_python_modules


@pytest.fixture
def database(monkeypatch):
    from adbc_driver_postgresql import dbapi
    uri = os.environ.get('INFOGEN_TEST_DATABASE_URI')
    assert uri, '请设置隔离测试数据库 INFOGEN_TEST_DATABASE_URI'
    driver = os.environ.get('INFOGEN_TEST_ADBC_DRIVER')
    assert driver, '请设置 PostgreSQL ADBC 驱动路径 INFOGEN_TEST_ADBC_DRIVER'
    schema = 'contrib_' + uuid4().hex
    modules = load_python_modules(monkeypatch)
    modules.adbc._SQLVariation = postgresql.dialect()
    modules.adbc._ADBCConf = {
        'Connection': {'driver': driver, 'uri': uri},
        'ConnectionPool': {'maxconnections': 2, 'mincached': 0, 'blocking': False},
        'InitStatements': {'init_statements': ['SET search_path TO "' + schema + '"']},
    }
    with dbapi.connect(uri) as connection:
        with connection.cursor() as cursor:
            cursor.execute('CREATE SCHEMA "' + schema + '"')
            cursor.execute('SET search_path TO "' + schema + '"')
        modules.adbc.ExecuteScript(connection, (REPO/'InfoGen_Data/Resources/Schema-ADBC.sql').read_text(encoding='utf-8'))
        connection.commit()
    monkeypatch.setattr(modules.loader, 'UserConfirm', lambda _: True)
    source = dict(MainID='Root', Objects={'Root': dict(OType='Star', Identifiers=['Root', 'Alias1', 'Alias2'])},
        MinorObjects={}, NStars=1, NPlanets=0, NDwarfPlanets=0, NSatellites=0,
        NMinorPlanets=0, NComets=0, StarSpectralType=['old'])
    args = Namespace(namespace='review', input=str(REPO/'tests/fixtures/spaceengine/se-0.991-real/Kepler-1512.sc'),
        code_page=65001, esi_estimator='SolarSys', absolute_orbit=False,
        common_plane_threshold=9, store_mode='dist-upgrade')

    def upload(*, rings=None, label='old'):
        current = {**source, 'StarSpectralType': [label], 'Objects': {'Root': dict(source['Objects']['Root'])}}
        if rings is not None:
            current['Objects']['Root']['Rings'] = rings
        modules.loader.Uploader(current, args).Run()

    def read(statement):
        connection = modules.adbc.ADBCClient()
        try:
            with connection.cursor() as cursor:
                cursor.execute(statement)
                return cursor.fetchall()
        finally:
            connection.close()

    try:
        yield SimpleNamespace(modules=modules, upload=upload, read=read)
    finally:
        if modules.adbc._Connection is not None:
            modules.adbc._Connection.close()
        with dbapi.connect(uri) as connection:
            with connection.cursor() as cursor:
                cursor.execute('DROP SCHEMA "' + schema + '" CASCADE')
            connection.commit()


@pytest.mark.integration
@pytest.mark.parametrize('count', [None, 0, 42])
def test_ring_count_roundtrip_on_postgresql(database, count):
    d = database
    ring = dict(Type='Colorful', InnerRadius=1., Width=2.)
    if count is not None:
        ring['ObjectCount'] = count
    d.upload(rings=[ring])
    assert d.read('SELECT object_count FROM ig_rings') == [(count,)]
    d.modules.query.SetNamespace('review')
    rows = json.loads(d.modules.query.QueryObject.func('Alias1'))
    actual = rows[0]['rings'][0].get('object_count')
    assert actual == count


@pytest.mark.integration
@pytest.mark.parametrize('name', [None, 'oid', 'Root', 'Alias1', 'absent'])
def test_aliases_do_not_duplicate_objects(database, name):
    d = database
    d.upload()
    oid = d.read('SELECT object_id FROM ig_object')[0][0]
    rows = d.modules.query._QueryObject_Unchecked('review', oid if name == 'oid' else name)
    if name == 'absent':
        assert rows == []
    else:
        assert len(rows) == 1 and rows[0]['object_id'] == oid
        assert set(rows[0]['other_designations']) == {'Root', 'Alias1', 'Alias2'}


@pytest.mark.integration
def test_all_namespaces_keeps_existing_unfiltered_behavior(database):
    d = database
    d.upload()
    rows = d.modules.query._QueryObject_Unchecked('-A', 'absent')
    assert len(rows) == 1


@pytest.mark.integration
def test_primary_name_without_identifier_does_not_add_a_match(database):
    d = database
    d.upload()
    connection = d.modules.adbc.ADBCClient()
    try:
        with connection.cursor() as cursor:
            cursor.execute("UPDATE ig_object SET primary_name = 'display-only'")
        connection.commit()
    finally:
        connection.close()
    assert d.modules.query._QueryObject_Unchecked('review', 'display-only') == []


@pytest.mark.integration
def test_repeated_upload_uses_live_connections(database):
    d = database
    d.upload()
    d.upload(label='new')
    assert d.read('SELECT spectral_types FROM ig_system') == [('new',)]
    assert d.read('SELECT count(*) FROM ig_object') == [(1,)]
    assert d.read('SELECT count(*) FROM ig_identifiers') == [(3,)]


@pytest.mark.integration
def test_nonfinite_sql_and_public_json(database):
    d = database
    d.upload()
    oid = d.read('SELECT object_id FROM ig_object')[0][0]
    frame = pd.DataFrame({'object_id': [oid], 'surface_pressure': [float('inf')],
        'scale_height': [pd.NA]}).set_index('object_id')
    statement = d.modules.loader.Uploader.DataFrameToSQL('ig_atmosphere', frame,
        ManualMapping={'surface_pressure': Double, 'scale_height': Double})
    connection = d.modules.adbc.ADBCClient()
    try:
        with connection.cursor() as cursor:
            cursor.execute(statement)
        connection.commit()
    finally:
        connection.close()
    assert d.read('SELECT surface_pressure, scale_height FROM ig_atmosphere') == [(None, None)]
    d.modules.query.SetNamespace('review')
    def reject_constant(value):
        raise AssertionError('JSON 中出现非标准常量：' + value)
    rows = json.loads(d.modules.query.QueryObject.func(oid), parse_constant=reject_constant)
    assert len(rows) == 1 and rows[0]['object_id'] == oid
