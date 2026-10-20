"""同步边界：显示回退不覆盖源分类，也不要求 native 或数据库。"""
import importlib.util
from pathlib import Path

import pytest

pytestmark = pytest.mark.unit

spec = importlib.util.spec_from_file_location(
    'sync_classifications', Path(__file__).resolve().parents[1] / 'InfoGen_Data/Modules/Classifications.py')
classifications = importlib.util.module_from_spec(spec)
spec.loader.exec_module(classifications)


@pytest.mark.parametrize('source', ['ExoticSourceClass', 'O5IIIe', 'G2Vpec'])
def test_unknown_spectral_class_keeps_literal(source):
    assert classifications.RegexStellarClassification(source) == source


@pytest.mark.parametrize('source', ['', None])
def test_missing_spectral_class_does_not_crash(source):
    assert classifications.RegexStellarClassification(source) == '特殊的恒星'


@pytest.mark.parametrize('source,label', [
    ('SC9', '碳星'), ('MS5', 'S型恒星'), ('WD2', '白矮星'),
    ('G2V', '黄矮星'), ('K5V', '橙矮星'), ('M2V', '红矮星'),
    ('WN6', '沃尔夫-拉叶星'), ('L3', '褐矮星'), ('Pulsar', '中子星'),
])
def test_existing_spectral_families(source, label):
    assert classifications.RegexStellarClassification(source) == label


@pytest.mark.parametrize('source', ['Selena', 'Titan', 'ExoticPlanet'])
def test_unknown_planet_class_is_not_silently_reclassified(source):
    physics = dict(Mass=1., MeanRadius=1., Temperature=250.)
    assert classifications.PlanetClassification(source, physics) == source
    assert physics == dict(Mass=1., MeanRadius=1., Temperature=250.)
