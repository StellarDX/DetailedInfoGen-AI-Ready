"""数据库表达式、NULL、别名去重和连接所有权的独立回归。"""
from argparse import Namespace
import json
import sqlite3
from types import SimpleNamespace
from unittest.mock import Mock

import numpy as np
import pandas as pd
import pytest
from sqlalchemy.dialects import sqlite
from sqlalchemy import Double

from support import REPO, load_python_modules


@pytest.fixture
def modules(monkeypatch):
    result = load_python_modules(monkeypatch)
    result.adbc._SQLVariation = sqlite.dialect()
    return result


@pytest.mark.unit
@pytest.mark.parametrize('missing', [np.nan, np.inf, -np.inf, pd.NA, pd.NaT])
def test_all_missing_literals_become_sql_null(modules, missing):
    frame = pd.DataFrame({'value': [missing, 1.5]}, dtype=object)
    original = frame.copy(deep=True)
    statement = modules.loader.Uploader.DataFrameToSQL('probe', frame, ManualMapping={'value':Double})
    with sqlite3.connect(':memory:') as connection:
        connection.execute('CREATE TABLE probe ("index" INTEGER, value DOUBLE)')
        connection.execute(statement)
        assert connection.execute('SELECT value FROM probe ORDER BY "index"').fetchall() == [(None,), (1.5,)]
    pd.testing.assert_frame_equal(frame, original)


@pytest.mark.unit
def test_raw_connections_are_fresh_and_owned(modules, monkeypatch):
    first, second = Mock(), Mock()
    monkeypatch.setattr(modules.adbc, 'ADBCConfig', lambda: {'Connection': {}})
    monkeypatch.setattr(modules.adbc, 'RunInit', lambda _: None)
    monkeypatch.setattr(modules.adbc.dbapi, 'connect', Mock(side_effect=[first, second]))
    assert modules.adbc.ADBCRawClient() is first
    first.close()
    assert modules.adbc.ADBCRawClient() is second


@pytest.mark.unit
@pytest.mark.parametrize('pooled', [False, True])
def test_init_failure_closes_connection(modules, monkeypatch, pooled):
    connection = Mock()
    monkeypatch.setattr(modules.adbc, 'ADBCConfig', lambda: {'Connection': {}})
    monkeypatch.setattr(modules.adbc.dbapi, 'connect', lambda **_: connection)
    monkeypatch.setattr(modules.adbc, '_ConnectionPool', lambda: SimpleNamespace(connection=lambda: connection))
    def fail(_):
        raise RuntimeError('初始化失败')
    monkeypatch.setattr(modules.adbc, 'RunInit', fail)
    with pytest.raises(RuntimeError, match='初始化失败'):
        (modules.adbc.ADBCClient if pooled else modules.adbc.ADBCRawClient)()
    connection.close.assert_called_once()


@pytest.fixture
def query_database(modules, monkeypatch):
    database = sqlite3.connect(':memory:')
    database.executescript((REPO / 'InfoGen_Data/Resources/Schema-ADBC.sql').read_text(encoding='utf-8'))
    database.execute("INSERT INTO ig_system(system_id,main_id,namespace) VALUES ('sys','Root','review')")
    database.execute("INSERT INTO ig_object(object_id,system_id,primary_name,otype) VALUES ('oid','sys','Root','Star')")
    database.executemany('INSERT INTO ig_identifiers VALUES (?,?)', [('oid', name) for name in ('Root','Alias1','Alias2')])
    connection = SimpleNamespace(close=Mock())
    monkeypatch.setattr(modules.query, 'ADBCClient', lambda: connection)
    monkeypatch.setattr(modules.query, 'ReadSQLToDataFrame', lambda sql, _: pd.read_sql_query(sql, database))
    try:
        yield database, connection
    finally:
        database.close()


@pytest.mark.smoke
@pytest.mark.unit
@pytest.mark.parametrize('name', [None, 'oid', 'Root', 'Alias1'])
def test_aliases_do_not_duplicate_objects(modules, query_database, name):
    rows = modules.query._QueryObject_Unchecked('review', name)
    assert len(rows) == 1
    assert rows[0]['object_id'] == 'oid'
    assert set(rows[0]['other_designations']) == {'Root','Alias1','Alias2'}
    query_database[1].close.assert_called_once()


@pytest.mark.unit
def test_all_namespaces_still_filters_requested_name(modules, query_database):
    assert modules.query._QueryObject_Unchecked('-A', 'absent') == []


@pytest.mark.unit
def test_query_rings_null_and_nonfinite_are_json_safe(modules, query_database):
    database, _ = query_database
    database.execute("INSERT INTO ig_rings(object_id,ring_type,inner_radius,width) VALUES ('oid','Colorful',1,2)")
    database.execute('INSERT INTO ig_physical(object_id,temperature) VALUES (?,?)', ('oid', float('inf')))
    rows = modules.query._QueryObject_Unchecked('review', 'Alias1', ['physic','rings'])
    encoded = json.dumps(rows, allow_nan=False)
    assert json.loads(encoded)[0]['physical_characteristics']['temperature'] is None
    assert rows[0]['rings'][0].get('object_count') is None


@pytest.mark.unit
@pytest.mark.parametrize('function,args', [
    ('_QuerySystem_Unchecked', ('review','Root')),
    ('_QueryAllObjectsInSystem_Unchecked', ('review','Root',None)),
    ('_QueryObject_Unchecked', ('review','oid')),
])
def test_query_failure_releases_owned_connection(modules, monkeypatch, function, args):
    connection = SimpleNamespace(close=Mock())
    monkeypatch.setattr(modules.query, 'ADBCClient', lambda: connection)
    def fail(*_):
        raise RuntimeError('查询失败')
    monkeypatch.setattr(modules.query, 'ReadSQLToDataFrame', fail)
    with pytest.raises(RuntimeError, match='查询失败'):
        getattr(modules.query, function)(*args)
    connection.close.assert_called_once()


@pytest.mark.unit
def test_absolute_orbit_does_not_require_transformed_angles(modules):
    orbit = dict(RefPlane='Ecliptic', Position=[1.,2.,3.], Period=10., PericenterDist=1.,
        AphelionDist=1., SemiMajorAxis=1., Eccentricity=0., Inclination=5., AscendingNode=6.,
        Epoch=2451545., ArgOfPericenter=7., MeanAnomaly=8., BinaryOrbit=False)
    source = {'MainID':'Root','Objects':{'Root':{'OType':'Planet','Identifiers':['Root'],
        'OrbitalCharacteristics':orbit}}}
    uploader = modules.loader.Uploader(source, Namespace(namespace='review'))
    with sqlite3.connect(':memory:') as database:
        database.executescript((REPO / 'InfoGen_Data/Resources/Schema-ADBC.sql').read_text(encoding='utf-8'))
        for attr, table, index in [('_ObjectsDataFrame','ig_object','object_id'),
            ('_IdentifiersDataFrame','ig_identifiers',['object_id','alias']),('_OrbitDataFrame','ig_orbit','object_id')]:
            frame = pd.DataFrame(columns=[row[1] for row in database.execute('PRAGMA table_info('+table+')')])
            frame.set_index(index, inplace=True)
            setattr(uploader, attr, frame)
    uploader._DFS_Iterate('Root',None,'system',None,['Root'],None)
    row = uploader._OrbitDataFrame.iloc[0]
    assert pd.isna(row['inclination_ecliptic'])
    assert row['inclination'] == 5.


@pytest.mark.unit
def test_metadata_failure_closes_raw_connection(modules, monkeypatch):
    connection = SimpleNamespace(close=Mock())
    monkeypatch.setattr(modules.loader, 'ADBCRawClient', lambda: connection)
    def fail(*_):
        raise RuntimeError('元数据失败')
    monkeypatch.setattr(modules.loader, 'LoadTableSchemaToDataFrame', fail)
    uploader = modules.loader.Uploader({'MainID':'Root'}, Namespace(namespace='review'))
    with pytest.raises(RuntimeError, match='元数据失败'):
        uploader._Load_Table()
    connection.close.assert_called_once()


@pytest.mark.unit
@pytest.mark.parametrize('count',[None,0,42])
def test_missing_ring_count_is_not_an_invented_zero(modules,count):
    ring=dict(Type='Colorful',InnerRadius=1.,Width=2.)
    if count is not None:
        ring['ObjectCount']=count
    source={'MainID':'Root','Objects':{'Root':{'OType':'Planet','Identifiers':['Root'],'Rings':[ring]}}}
    uploader=modules.loader.Uploader(source,Namespace(namespace='review'))
    with sqlite3.connect(':memory:') as database:
        database.executescript((REPO/'InfoGen_Data/Resources/Schema-ADBC.sql').read_text(encoding='utf-8'))
        for attr,table,index in [('_ObjectsDataFrame','ig_object','object_id'),
            ('_IdentifiersDataFrame','ig_identifiers',['object_id','alias']),('_RingsDataFrame','ig_rings',['object_id','ring_type'])]:
            frame=pd.DataFrame(columns=[row[1] for row in database.execute('PRAGMA table_info('+table+')')])
            frame.set_index(index,inplace=True)
            setattr(uploader,attr,frame)
    uploader._DFS_Iterate('Root',None,'system',None,['Root'],None)
    actual=uploader._RingsDataFrame.iloc[0]['object_count']
    assert pd.isna(actual) if count is None else actual==count
