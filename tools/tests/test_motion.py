import json
import shutil
import subprocess
import tempfile
import unittest
from pathlib import Path

ROOT=Path(__file__).resolve().parents[2]
class PetMotionTests(unittest.TestCase):
    def test_generated_motion_matches_one_definition(self):
        subprocess.run(['python3',str(ROOT/'tools/build_motion.py'),'--check'],check=True)
    def test_life_motion_and_touch_boundaries(self):
        compiler=shutil.which('clang++') or shutil.which('g++')
        self.assertIsNotNone(compiler)
        with tempfile.TemporaryDirectory() as tmp:
            output=Path(tmp)/'pet-test'
            subprocess.run([compiler,'-std=c++17','-Wall','-Wextra','-Werror','-I',str(ROOT/'main/teloa'),
                str(ROOT/'tools/tests/teloa_motion_test.cc'),str(ROOT/'main/teloa/teloa_motion.cc'),'-o',str(output)],check=True)
            subprocess.run([str(output)],check=True)
    def test_approval_and_pairing_have_still_frames(self):
        spec=json.loads((ROOT/'main/teloa/motion.json').read_text())
        for mode in ('approval','pairing'):
            frames=spec['clips'][mode]['frames']
            self.assertTrue(all((f['x'],f['y'],f['angle'],f['sx'],f['sy'])==(0,0,0,256,256) for f in frames))
if __name__=='__main__':unittest.main()
