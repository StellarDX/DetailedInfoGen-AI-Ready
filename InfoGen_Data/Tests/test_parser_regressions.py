"""实际 parser 可执行文件回归，保留解析与完整系统计算的区别。"""
import json
import os
from pathlib import Path
import subprocess

import pytest
from support import REPO

CASES=['missing','invalid-token','trailing','real-overflow','int-overflow','signed-min','valid-values','repeated-blocks']


def run(*arguments):
    executable=os.environ.get('INFOGEN_PARSER_TEST_EXE')
    assert executable and Path(executable).is_file(),'请设置实际构建的 INFOGEN_PARSER_TEST_EXE'
    result=subprocess.run([executable,*arguments],capture_output=True,text=True,
        encoding='utf-8',errors='replace',timeout=10)
    assert result.returncode==0,result.stdout+result.stderr


@pytest.mark.native
@pytest.mark.integration
@pytest.mark.parametrize('case',[pytest.param(case,marks=pytest.mark.smoke) if case=='trailing' else case for case in CASES])
def test_parser_boundaries(case):
    run(case)


@pytest.mark.native
@pytest.mark.integration
@pytest.mark.parametrize('Res',json.loads((REPO/'Tests/Resources/SpaceEngine/manifest.json').read_text(encoding='utf-8'))['resources'],ids=lambda entry:entry['path'])
def test_real_0991_parse(Res):
    run('real',str(REPO/'Tests/Resources/SpaceEngine'/Res['path']))
