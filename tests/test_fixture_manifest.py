"""固定真实来源和文件字节；不以文本检查冒充 native 兼容验收。"""
import hashlib
import json
from pathlib import Path
import pytest

ROOT = Path(__file__).resolve().parent / 'fixtures/spaceengine'
MANIFEST = json.loads((ROOT / 'manifest.json').read_text(encoding='utf-8'))


@pytest.mark.smoke
@pytest.mark.unit
@pytest.mark.parametrize('entry', MANIFEST['fixtures'], ids=lambda entry: entry['path'])
def test_real_fixture_integrity(entry):
    path = ROOT / entry['path']
    assert path.stat().st_size == entry['bytes']
    assert hashlib.sha256(path.read_bytes()).hexdigest() == entry['sha256']
    assert entry['source_kind'] == 'real_export'
    assert entry['source_build'] == '0.991.50.2155 beta'


@pytest.mark.unit
def test_fixture_paths_and_scope_are_explicit():
    assert len({entry['path'] for entry in MANIFEST['fixtures']}) == 3
    assert sum(entry['complete_system_root'] for entry in MANIFEST['fixtures']) == 2
    assert MANIFEST['version_basis']['embedded_version_header'] is False
