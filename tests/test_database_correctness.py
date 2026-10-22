"""编译、DataFrame 和连接释放回归；数据库执行另用真实 PostgreSQL。"""
from argparse import Namespace
import json
from types import SimpleNamespace
from unittest.mock import Mock
import numpy as np
import pandas as pd
import pytest
from sqlalchemy import Double
from sqlalchemy.dialects import postgresql,mysql

from support import REPO, load_python_modules

@pytest.fixture
def modules(monkeypatch):
    result=load_python_modules(monkeypatch)
    result.adbc._SQLVariation=postgresql.dialect()
    return result

@pytest.mark.parametrize('dialect',[postgresql.dialect(),mysql.dialect()],ids=['postgresql','mysql'])
@pytest.mark.parametrize('missing',[np.nan,np.inf,-np.inf,pd.NA,pd.NaT])
@pytest.mark.unit
def test_sql_null_compilation_only(modules,monkeypatch,dialect,missing):
    modules.adbc._SQLVariation=dialect
    # 只验证正式支持方言的表达式编译，不宣称对应驱动已完成执行验收。
    frame=pd.DataFrame({'value':[missing]},dtype=object)
    before=frame.copy(deep=True)
    statement=modules.loader.Uploader.DataFrameToSQL('probe',frame,ManualMapping={'value':Double})
    assert 'NULL' in statement and not any(x in statement.lower() for x in ['nan','inf'])
    pd.testing.assert_frame_equal(frame,before)

@pytest.mark.unit
@pytest.mark.smoke
def test_fresh_owned_raw_connections(modules,monkeypatch):
    first,second=Mock(),Mock()
    monkeypatch.setattr(modules.adbc,'ADBCConfig',lambda:{'Connection':{}})
    monkeypatch.setattr(modules.adbc,'RunInit',lambda _:None)
    monkeypatch.setattr(modules.adbc.dbapi,'connect',Mock(side_effect=[first,second]))
    assert modules.adbc.ADBCRawClient() is first
    first.close()
    assert modules.adbc.ADBCRawClient() is second

@pytest.mark.parametrize('pooled',[False,True])
@pytest.mark.unit
def test_init_failure_releases_connection(modules,monkeypatch,pooled):
    connection=Mock()
    monkeypatch.setattr(modules.adbc,'ADBCConfig',lambda:{'Connection':{}})
    monkeypatch.setattr(modules.adbc.dbapi,'connect',lambda **_:connection)
    monkeypatch.setattr(modules.adbc,'_ConnectionPool',lambda:SimpleNamespace(connection=lambda:connection))
    def fail(_):raise RuntimeError('初始化失败')
    monkeypatch.setattr(modules.adbc,'RunInit',fail)
    with pytest.raises(RuntimeError,match='初始化失败'):
        (modules.adbc.ADBCClient if pooled else modules.adbc.ADBCRawClient)()
    connection.close.assert_called_once()

@pytest.mark.parametrize('function,args',[
    ('_QuerySystem_Unchecked',('review','Root')),
    ('_QueryAllObjectsInSystem_Unchecked',('review','Root',None)),
    ('_QueryObject_Unchecked',('review','oid'))])
@pytest.mark.unit
def test_query_failure_releases_connection(modules,monkeypatch,function,args):
    connection=SimpleNamespace(close=Mock())
    monkeypatch.setattr(modules.query,'ADBCClient',lambda:connection)
    def fail(*_):raise RuntimeError('查询失败')
    monkeypatch.setattr(modules.query,'ReadSQLToDataFrame',fail)
    with pytest.raises(RuntimeError,match='查询失败'):getattr(modules.query,function)(*args)
    connection.close.assert_called_once()

@pytest.mark.unit
@pytest.mark.smoke
def test_metadata_failure_releases_connection(modules,monkeypatch):
    connection=SimpleNamespace(close=Mock())
    monkeypatch.setattr(modules.loader,'ADBCRawClient',lambda:connection)
    def fail(*_):raise RuntimeError('元数据失败')
    monkeypatch.setattr(modules.loader,'LoadTableSchemaToDataFrame',fail)
    uploader=modules.loader.Uploader({'MainID':'Root'},Namespace(namespace='review'))
    with pytest.raises(RuntimeError,match='元数据失败'):uploader._Load_Table()
    connection.close.assert_called_once()

@pytest.mark.unit
@pytest.mark.parametrize('read_fails', [False, True])
def test_dist_upgrade_releases_pooled_connection(modules, monkeypatch, read_fails):
    connection = SimpleNamespace(cursor=Mock(), commit=Mock(), close=Mock())
    monkeypatch.setattr(modules.loader, 'ADBCClient', lambda: connection)
    def read(*_):
        if read_fails:
            raise RuntimeError('读取失败')
        return pd.DataFrame({'system_id': ['existing'], 'create_date': [pd.Timestamp('2020-01-01')]})
    monkeypatch.setattr(modules.loader, 'ReadSQLToDataFrame', read)
    monkeypatch.setattr(modules.loader, 'UserConfirm', lambda _: False)
    uploader = modules.loader.Uploader({'MainID': 'Root'}, Namespace(namespace='review'))
    if read_fails:
        with pytest.raises(RuntimeError, match='读取失败'):
            uploader.DistUpgrade()
    else:
        uploader.DistUpgrade()
    connection.cursor.assert_not_called()
    connection.commit.assert_not_called()
    connection.close.assert_called_once()

@pytest.mark.unit
@pytest.mark.smoke
def test_json_boundary_without_database(modules):
    result=json.loads(modules.query._DumpJSON({'a':np.inf,'b':pd.NA,'c':0,'d':[float('nan')]}))
    assert result=={'a':None,'b':None,'c':0,'d':[None]}

def empty_frames(uploader,names):
    # 使用已有 schema 的列声明准备空 DataFrame，不启动数据库。
    import re
    schema=(REPO/'InfoGen_Data/Resources/Schema-ADBC.sql').read_text(encoding='utf-8')
    for attr,table,index in names:
        body=re.search(r'CREATE TABLE\s+(?:IF NOT EXISTS\s+)?'+table+r'\s*\((.*?)\n\);',schema,re.S|re.I).group(1)
        columns=[]
        for line in body.splitlines():
            match=re.match(r'\s*(\w+)\s+(?:VARCHAR|TEXT|CHAR|BIGINT|INT|INTEGER|SMALLINT|BOOLEAN|BOOL|DOUBLE|REAL|FLOAT|TIMESTAMP|DATE|DECIMAL|NUMERIC)\b',line,re.I)
            if match:columns.append(match.group(1))
        setattr(uploader,attr,pd.DataFrame(columns=columns).set_index(index))

@pytest.mark.unit
def test_absolute_mode_keeps_source_angles(modules):
    orbit=dict(RefPlane='Ecliptic',Position=[1.,2.,3.],Period=10.,PericenterDist=1.,AphelionDist=1.,SemiMajorAxis=1.,Eccentricity=0.,Inclination=5.,AscendingNode=6.,Epoch=2451545.,ArgOfPericenter=7.,MeanAnomaly=8.,BinaryOrbit=False)
    source={'MainID':'Root','Objects':{'Root':{'OType':'Planet','Identifiers':['Root'],'OrbitalCharacteristics':orbit}}}
    uploader=modules.loader.Uploader(source,Namespace(namespace='review'))
    empty_frames(uploader,[('_ObjectsDataFrame','ig_object','object_id'),('_IdentifiersDataFrame','ig_identifiers',['object_id','alias']),('_OrbitDataFrame','ig_orbit','object_id')])
    uploader._DFS_Iterate('Root',None,'system',None,['Root'],None)
    row=uploader._OrbitDataFrame.iloc[0]
    assert pd.isna(row['inclination_ecliptic']) and row['inclination']==5.

@pytest.mark.parametrize('count',[None,0,42])
@pytest.mark.unit
def test_ring_dataframe_preserves_missing_and_zero(modules,count):
    ring=dict(Type='Colorful',InnerRadius=1.,Width=2.)
    if count is not None:ring['ObjectCount']=count
    source={'MainID':'Root','Objects':{'Root':{'OType':'Planet','Identifiers':['Root'],'Rings':[ring]}}}
    uploader=modules.loader.Uploader(source,Namespace(namespace='review'))
    empty_frames(uploader,[('_ObjectsDataFrame','ig_object','object_id'),('_IdentifiersDataFrame','ig_identifiers',['object_id','alias']),('_RingsDataFrame','ig_rings',['object_id','ring_type'])])
    uploader._DFS_Iterate('Root',None,'system',None,['Root'],None)
    actual=uploader._RingsDataFrame.iloc[0]['object_count']
    assert pd.isna(actual) if count is None else actual==count
