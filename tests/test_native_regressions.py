"""对子进程运行真实 C++ 回归，超时也判为失败。"""
import os
from pathlib import Path
import subprocess
import sys
import pytest

CASES = ['initialization', 'position', 'temperature', 'synchronous-temperature', 'esi', 'rotation', 'kepler', 'coplanar', 'sequential', 'binary-negative', 'binary-apsis', 'binary-wrap', 'binary-period', 'binary-single', 'binary-frame', 'binary-rounded', 'binary-small-period', 'binary-missing', 'binary-epoch', 'binary-eccentricity', 'binary-retrograde', 'binary-tie', 'binary-empty']


@pytest.mark.native
@pytest.mark.integration
@pytest.mark.parametrize('case', [pytest.param(name, marks=pytest.mark.smoke)
    if name in ('initialization','position','esi') else name for name in CASES])
def test_native_regression(case):
    executable = os.environ.get('INFOGEN_NATIVE_TEST_EXE')
    assert executable and Path(executable).is_file(), '请设置实际构建的 INFOGEN_NATIVE_TEST_EXE'
    environment = os.environ.copy()
    environment['PYTHONPATH'] = os.pathsep.join(sys.path)
    result = subprocess.run([executable, case], capture_output=True, text=True, env=environment,
                            encoding='utf-8', errors='replace', timeout=45 if case=='sequential' else 5)
    assert result.returncode == 0, result.stdout + result.stderr
