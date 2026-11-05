#!/usr/bin/env python3
# Copyright (c) 2026 Aregtech
"""Regression tests for coverage report paths across Bash and Python."""

import json
import os
from pathlib import Path
import shutil
import subprocess
import sys
import tempfile
import unittest


ROOT = Path(__file__).resolve().parents[2]
DRIVER = ROOT / 'tools' / 'intern' / 'coverage.sh'
GCOVR = """#!/usr/bin/env bash
set -eu
if [ "$1" = --version ]; then
  echo 'gcovr 8.8'
  exit 0
fi
while [ $# -gt 0 ]; do
  case "$1" in
    --cobertura)
      shift
      printf '%s\\n' '<coverage><packages><package><classes><class filename="framework/fixture.cpp"><lines><line number="1" hits="1"/></lines></class></classes></package></packages></coverage>' > "$1"
      ;;
    --json-summary)
      shift
      printf '%s\\n' '{"line_total":1,"line_covered":1,"function_total":1,"function_covered":1,"branch_total":0,"branch_covered":0,"files":[]}' > "$1"
      ;;
    --html-nested)
      shift
      printf '%s\\n' '<html>coverage fixture</html>' > "$1"
      ;;
  esac
  shift
done
"""


def find_bash():
    if os.name == 'nt':
        git = shutil.which('git')
        bash = (Path(subprocess.check_output([git, '--exec-path'], text=True).strip())
                .parents[2] / 'bin' / 'bash.exe') if git else None
    elif sys.platform == 'cygwin':
        bash = Path('/usr/bin/bash')
    else:
        bash = shutil.which('bash')
    if not bash or not Path(bash).is_file():
        raise RuntimeError('The CI script tests require Bash (Git Bash on Windows)')
    return str(bash)


@unittest.skipUnless(sys.platform == 'cygwin', 'Cygwin prebuilt coverage dependency')
class CygwinLxmlTest(unittest.TestCase):
    # Reproduces the defect: the prebuilt extension imported an unavailable libxml2 DLL.
    def test_prebuilt_lxml_loads_and_transforms_xml(self):
        from lxml import etree
        document = etree.fromstring(b'<root>coverage</root>')
        stylesheet = etree.XML(
            b'<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">'
            b'<xsl:template match="/"><result><xsl:value-of select="root"/>'
            b'</result></xsl:template></xsl:stylesheet>')
        self.assertEqual(etree.XSLT(stylesheet)(document).getroot().text, 'coverage')


class CoveragePathTest(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.bash = find_bash()

    def shell_path(self, path):
        if os.name != 'nt':
            return str(path)
        return subprocess.check_output(
            [str(self.bash), '--noprofile', '--norc', '-c', 'cygpath -u "$1"', '--', str(path)],
            text=True).strip()

    def run_report(self, location):
        with tempfile.TemporaryDirectory(prefix='areg coverage ') as temporary:
            root = Path(temporary)
            home = root / 'home'
            home.mkdir()
            report = root / 'report'
            report.mkdir()
            for kind in ('unit', 'examples'):
                (report / (kind + '.json')).write_text('{}', encoding='ascii')
            gcovr = root / 'gcovr-fixture'
            gcovr.write_text(GCOVR, encoding='ascii', newline='\n')
            gcovr.chmod(0o755)
            env = dict(os.environ, HOME=self.shell_path(home), GCOVR=gcovr.as_posix(),
                       PYTHON=Path(sys.executable).as_posix(), MSYS2_ARG_CONV_EXCL='*',
                       GITHUB_STEP_SUMMARY=(root / 'summary.md').as_posix())
            env.pop('AREG_COV_DIR', None)
            arguments = [str(self.bash), '--noprofile', '--norc', DRIVER.as_posix(),
                         'report', '--in-place', '--build', (root / 'build').as_posix(),
                         '--out', report.as_posix()]
            history = home / 'areg-gcov'
            if location != 'home':
                history = root / 'history dir'
                if location == 'environment':
                    env['AREG_COV_DIR'] = self.shell_path(history)
                else:
                    arguments += ['--dir', self.shell_path(history)]
            result = subprocess.run(arguments, env=env, cwd=ROOT, capture_output=True, text=True)
            self.assertEqual(result.returncode, 0, result.stdout + result.stderr)
            saved = json.loads((history / 'last.json').read_text(encoding='utf-8'))
            self.assertEqual(saved['files'], {'framework/fixture.cpp': [1, 1, 0, 0]})
            self.assertIn('Code coverage', (report / 'coverage.md').read_text(encoding='utf-8'))

    # Reproduces the defect: native Python could not save the report history at a Bash home path
    # when MSYS argument conversion was disabled.
    def test_saves_history_below_the_bash_home(self):
        self.run_report('home')

    def test_saves_history_at_a_bash_environment_path(self):
        self.run_report('environment')

    def test_saves_history_at_an_explicit_bash_path(self):
        self.run_report('argument')


if __name__ == '__main__':
    unittest.main()
