"""隔离 CLI 初始化，测试实际生产模块；不加载 LLM 或本地扩展。"""
import argparse
import gettext
import importlib.util
from pathlib import Path
import sys
from types import ModuleType, SimpleNamespace

REPO = Path(__file__).resolve().parents[1]


def load_python_modules(monkeypatch):
    package = ModuleType('InfoGen_Data')
    package.__path__ = [str(REPO / 'InfoGen_Data')]
    modules = ModuleType('InfoGen_Data.Modules')
    modules.__path__ = [str(REPO / 'InfoGen_Data/Modules')]
    monkeypatch.setitem(sys.modules, package.__name__, package)
    monkeypatch.setitem(sys.modules, modules.__name__, modules)
    package.InfoGenHelpFormatter = argparse.HelpFormatter
    package.InfoGen = SimpleNamespace()
    package.LoadLocale = lambda *_: gettext.NullTranslations()
    package.ArgToDict = lambda value: tuple(value.split('=', 1))
    package.CheckNamespace = lambda _: None

    def load(name):
        fullname = 'InfoGen_Data.Modules.' + name
        spec = importlib.util.spec_from_file_location(fullname, REPO / 'InfoGen_Data/Modules' / (name + '.py'))
        module = importlib.util.module_from_spec(spec)
        monkeypatch.setitem(sys.modules, fullname, module)
        setattr(modules, name, module)
        spec.loader.exec_module(module)
        return module

    adbc = load('ADBC')
    for name in ('ADBCClient', 'ADBCRawClient', 'ReadSQLToDataFrame',
                 'LoadTableSchemaToDataFrame', 'UserConfirm', 'CurrentSQLVariation'):
        setattr(package, name, getattr(adbc, name))
    package.Classifications = load('Classifications')
    return SimpleNamespace(adbc=adbc, loader=load('SystemLoader'), query=load('SQLQuery'))
