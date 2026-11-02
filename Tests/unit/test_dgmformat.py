"""
Qt-free tests of the .dgm format versioning backend (supsisim.dgmformat).

Run from the repository root:
    python -m unittest discover -s Tests/unit -v
"""

import copy
import json
import os
import subprocess
import sys
import tempfile
import unittest
from unittest import mock

ROOT = os.path.abspath(os.path.join(os.path.dirname(__file__), '..', '..'))
os.environ.setdefault('PYSUPSICTRL', ROOT)
sys.path.insert(0, os.path.join(ROOT, 'toolbox', 'supsisim'))

from supsisim import dgmformat as dgf
from supsisim.const import DGM_FORMAT_VERSION

DIAGRAMS = os.path.join(ROOT, 'Tests', 'diagrams')
FIXTURES = os.path.join(ROOT, 'Tests', 'unit', 'fixtures')


def legacy_block(name='Sine', params='sineBlk|Amplitude: 1|Freq [Hz]: 1', **extra):
    b = {'name': name, 'inp': 0, 'outp': 1, 'inset': False, 'outset': False,
         'icon': 'SINE', 'params': params, 'help': 'set the width of the pulse',
         'width': [80, 60], 'flip': False, 'pos': [0.0, 0.0]}
    b.update(extra)
    return b


def legacy_dgm():
    """Format 0 diagram mixing all known legacy variants."""
    inner = {'blocks': [legacy_block('Gain', 'matmultBlk|Gains: 1')],
             'connections': [], 'subsystems': []}
    return {
        'init': {'code': 'pysimCoder', 'ver': 0.95, 'date': '01.01.2024 - 00:00:00'},
        'simulate': {'template': 'nuttx.tmf', 'Ts': '0.001', 'AddObj': '', 'AddCDefs': '',
                     'script': 'my.py', 'Tf': '20', 'prio': '5'},
        'blocks': [
            legacy_block(),
            legacy_block('AI', 'ar2inoAIBlk|Pin:18|Umin [V]:0.0|Umax [V]:5.0'),
            legacy_block('X', 'unknownBlk|Txt:"abc"|Arr:[1, 2.5]|Ints:[1,2]|Scalar:3'),
        ],
        'connections': [{'pos1': [0, 0], 'pos2': [10, 0], 'points': []}],
        'subsystems': [{'block': legacy_block('Subs', 'subsBlk'), 'subitems': inner}],
    }


def write_json(path, d):
    with open(path, 'w') as f:
        json.dump(d, f)


def read_bytes(path):
    with open(path, 'rb') as f:
        return f.read()


class TestNoQt(unittest.TestCase):
    def test_imports_without_qt(self):
        code = (
            "import sys, importlib.abc\n"
            "class Block(importlib.abc.MetaPathFinder):\n"
            "    def find_spec(self, name, path, target=None):\n"
            "        if name.split('.')[0] in ('PyQt5','PyQt6','PySide2','PySide6') "
            "or name == 'supsisim.qtvers':\n"
            "            raise ImportError('blocked: ' + name)\n"
            "sys.meta_path.insert(0, Block())\n"
            "sys.path.insert(0, sys.argv[1])\n"
            "import supsisim.dgmformat\n"
            "bad = [m for m in sys.modules if m.split('.')[0] in "
            "('PyQt5','PyQt6','PySide2','PySide6') or m == 'supsisim.qtvers']\n"
            "assert not bad, bad\n"
        )
        env = dict(os.environ, PYSUPSICTRL=ROOT)
        r = subprocess.run([sys.executable, '-c', code, os.path.join(ROOT, 'toolbox', 'supsisim')],
                           capture_output=True, text=True, env=env)
        self.assertEqual(r.returncode, 0, r.stderr)


class TestUpgradeDict(unittest.TestCase):
    def setUp(self):
        self.new, self.steps = dgf.upgrade(legacy_dgm())

    def test_format_set(self):
        self.assertEqual(dgf.get_format(legacy_dgm()), 0)
        self.assertEqual(self.new['init']['format'], DGM_FORMAT_VERSION)
        self.assertEqual(len(self.steps), DGM_FORMAT_VERSION)
        self.assertEqual(self.new['init']['ver'], 0.95)

    def test_input_not_modified(self):
        d = legacy_dgm()
        dgf.upgrade(d)
        self.assertEqual(d, legacy_dgm())

    def test_width_renamed_everywhere(self):
        blocks = self.new['blocks'] + [self.new['subsystems'][0]['block']] \
            + self.new['subsystems'][0]['subitems']['blocks']
        for b in blocks:
            self.assertNotIn('width', b)
            self.assertEqual(b['dims'], [80, 60])
            # help text containing "width" is left alone
            self.assertEqual(b['help'], 'set the width of the pulse')

    def test_simulate_keys_complete(self):
        sim = self.new['simulate']
        self.assertEqual(set(dgf.SIMULATE_DEFAULTS), set(sim))
        # existing values are preserved, missing ones get defaults
        self.assertEqual((sim['script'], sim['Tf'], sim['prio'], sim['template']),
                         ('my.py', '20', '5', 'nuttx.tmf'))
        self.assertEqual(sim['AddMakeArgs'], '')
        self.assertEqual(sim['intgMethod'], 'standard RK4')

    def test_relTf_renamed(self):
        d = legacy_dgm()
        d['simulate']['relTf'] = d['simulate'].pop('Tf')
        new, _ = dgf.upgrade(d)
        self.assertEqual(new['simulate']['Tf'], '20')
        self.assertNotIn('relTf', new['simulate'])
        # an existing Tf wins, relTf is left alone
        d = legacy_dgm()
        d['simulate']['relTf'] = '99'
        new, _ = dgf.upgrade(d)
        self.assertEqual(new['simulate']['Tf'], '20')

    def test_missing_simulate_section(self):
        d = legacy_dgm()
        del d['simulate']
        new, _ = dgf.upgrade(d)
        self.assertEqual(new['simulate'], dgf.SIMULATE_DEFAULTS)

    def test_params_typed_from_library(self):
        self.assertEqual(self.new['blocks'][0]['params'],
                         'sineBlk|Amplitude: 1:double|Freq [Hz]: 1:double')
        self.assertEqual(self.new['blocks'][1]['params'],
                         'ar2inoAIBlk|Pin:18:int|Umin [V]:0.0:double|Umax [V]:5.0:double')
        # nested subsystem blocks are typed too
        self.assertEqual(self.new['subsystems'][0]['subitems']['blocks'][0]['params'],
                         'matmultBlk|Gains: 1:double')

    def test_params_typed_by_inference(self):
        # unknown block: quoted string and arrays inferred, plain scalar left untyped
        self.assertEqual(self.new['blocks'][2]['params'],
                         'unknownBlk|Txt:"abc":str|Arr:[1, 2.5]:double|Ints:[1,2]:int|Scalar:3')

    def test_typed_params_untouched(self):
        d = legacy_dgm()
        d['blocks'][0]['params'] = 'sineBlk|Amplitude: 1: double'
        new, _ = dgf.upgrade(d)
        self.assertEqual(new['blocks'][0]['params'], 'sineBlk|Amplitude: 1: double')

    def test_idempotent(self):
        again, steps = dgf.upgrade(self.new)
        self.assertEqual(again, self.new)
        self.assertEqual(steps, [])
        # v0->v1 applied twice to the same data gives the same result
        twice = dgf._v0_to_v1(copy.deepcopy(self.new))
        twice['init']['format'] = DGM_FORMAT_VERSION
        self.assertEqual(twice, self.new)

    def test_existing_diagrams(self):
        for fn in ('simple_linux_rt.dgm', 'subsystem_linux_rt.dgm'):
            with open(os.path.join(DIAGRAMS, fn)) as f:
                d = json.load(f)
            self.assertEqual(dgf.get_format(d), DGM_FORMAT_VERSION, fn)
            self.assertEqual(dgf.upgrade(d), (d, []), fn)
            # the same content without a format (format 0) upgrades unchanged
            legacy = copy.deepcopy(d)
            del legacy['init']['format']
            self.assertEqual(dgf.upgrade(legacy)[0], d, fn)

    def test_real_legacy_diagram(self):
        # Tests/ControlDesign/Disks/disk_simSub.dgm as saved in 2022 (git 993061a^)
        with open(os.path.join(FIXTURES, 'disk_simSub_v0.dgm')) as f:
            d = json.load(f)
        self.assertEqual(dgf.get_format(d), 0)
        new, steps = dgf.upgrade(d)
        self.assertEqual(len(steps), DGM_FORMAT_VERSION)
        self.assertNotIn('"width"', json.dumps(new))
        sim = new['simulate']
        self.assertEqual(set(dgf.SIMULATE_DEFAULTS), set(sim))
        self.assertEqual((sim['script'], sim['Tf']), ('disk1.py', '20'))
        self.assertEqual(new['blocks'][1]['params'],
                         'squareBlk|Amplitude: 1:double|Period: 10:double|Width: 5:double'
                         '|Bias: 0:double|Delay: 0:double')
        sub = new['subsystems'][0]['subitems']['blocks']
        self.assertEqual(sub[1]['params'], 'sumBlk|Gains: [1,1]:double')
        self.assertEqual(sub[3]['params'],
                         'saturBlk|Upper saturation: 3000:double|Lower saturation: -3000:double')
        self.assertEqual(dgf.upgrade(new), (new, []))

    def test_none_type_from_param_dialog(self):
        d = legacy_dgm()
        d['blocks'][0]['params'] = 'sineBlk|Amplitude: 1: None|Freq [Hz]: 1: '
        d['blocks'][2]['params'] = 'unknownBlk|Scalar: 3: None'
        new, _ = dgf.upgrade(d)
        self.assertEqual(new['blocks'][0]['params'],
                         'sineBlk|Amplitude: 1:double|Freq [Hz]: 1:double')
        # no type can be determined: left as it was
        self.assertEqual(new['blocks'][2]['params'], 'unknownBlk|Scalar: 3: None')

    def test_width_and_dims_both_present(self):
        d = legacy_dgm()
        d['blocks'][0]['dims'] = [100, 40]
        new, _ = dgf.upgrade(d)
        self.assertEqual(new['blocks'][0]['dims'], [100, 40])

    def test_missing_or_bad_init(self):
        for init in (None, 'x', [1]):
            d = legacy_dgm()
            if init is None:
                del d['init']
            else:
                d['init'] = init
            self.assertEqual(dgf.get_format(d), 0)
            new, _ = dgf.upgrade(d)
            self.assertEqual(new['init']['format'], DGM_FORMAT_VERSION)

    def test_too_new(self):
        d = legacy_dgm()
        d['init']['format'] = DGM_FORMAT_VERSION + 1
        with self.assertRaises(dgf.DgmFormatTooNew):
            dgf.upgrade(d)

    def test_invalid_format_value(self):
        for bad in ('1', 1.0, -1, True, None):
            d = legacy_dgm()
            d['init']['format'] = bad
            with self.assertRaises(dgf.DgmFormatError, msg=repr(bad)):
                dgf.upgrade(d)
        with self.assertRaises(dgf.DgmFormatError):
            dgf.upgrade([1, 2])

    def test_missing_migration(self):
        with mock.patch.dict(dgf.MIGRATIONS, clear=True):
            with self.assertRaises(dgf.DgmFormatError):
                dgf.upgrade(legacy_dgm())

    def test_failing_migration(self):
        def boom(d):
            raise KeyError('x')
        with mock.patch.dict(dgf.MIGRATIONS, {0: ('boom', boom)}):
            with self.assertRaises(dgf.DgmFormatError):
                dgf.upgrade(legacy_dgm())


class TestFiles(unittest.TestCase):
    def setUp(self):
        self.tmp = tempfile.TemporaryDirectory()
        self.dir = self.tmp.name
        self.legacy = os.path.join(self.dir, 'legacy.dgm')
        write_json(self.legacy, legacy_dgm())

    def tearDown(self):
        self.tmp.cleanup()

    def test_inspect_kinds(self):
        current = os.path.join(self.dir, 'current.dgm')
        write_json(current, dgf.upgrade(legacy_dgm())[0])
        newer = os.path.join(self.dir, 'newer.dgm')
        d = legacy_dgm()
        d['init']['format'] = 999
        write_json(newer, d)
        broken = os.path.join(self.dir, 'broken.dgm')
        with open(broken, 'w') as f:
            f.write('{not json')
        badfmt = os.path.join(self.dir, 'badfmt.dgm')
        d = legacy_dgm()
        d['init']['format'] = 1.0
        write_json(badfmt, d)
        notdict = os.path.join(self.dir, 'list.dgm')
        write_json(notdict, [1, 2])

        self.assertEqual(dgf.inspect_file(current).kind, dgf.DgmKind.CURRENT)
        st = dgf.inspect_file(self.legacy)
        self.assertEqual((st.kind, st.file_format), (dgf.DgmKind.OLDER, 0))
        self.assertEqual(len(st.steps), DGM_FORMAT_VERSION)
        st = dgf.inspect_file(newer)
        self.assertEqual((st.kind, st.file_format), (dgf.DgmKind.NEWER, 999))
        self.assertTrue(st.error)
        for p in (broken, badfmt, notdict, os.path.join(self.dir, 'missing.dgm')):
            st = dgf.inspect_file(p)
            self.assertEqual(st.kind, dgf.DgmKind.INVALID)
            self.assertTrue(st.error)

    def test_upgrade_file(self):
        original = read_bytes(self.legacy)
        new_path = os.path.join(self.dir, 'legacy_v%d.dgm' % DGM_FORMAT_VERSION)
        self.assertEqual(dgf.upgraded_path_for(self.legacy), new_path)
        res = dgf.upgrade_file(self.legacy)
        self.assertEqual(res.new_path, new_path)
        self.assertEqual((res.from_format, res.to_format), (0, DGM_FORMAT_VERSION))
        # the original is not modified, the upgraded diagram is a new file next to it
        self.assertEqual(read_bytes(self.legacy), original)
        self.assertEqual(dgf.inspect_file(self.legacy).kind, dgf.DgmKind.OLDER)
        self.assertEqual(dgf.inspect_file(new_path).kind, dgf.DgmKind.CURRENT)
        self.assertEqual(dgf.load_dict(new_path)[0], dgf.upgrade(legacy_dgm())[0])
        self.assertEqual(sorted(os.listdir(self.dir)), sorted(['legacy.dgm', os.path.basename(new_path)]))

    def test_permissions_kept(self):
        os.chmod(self.legacy, 0o644)
        res = dgf.upgrade_file(self.legacy)
        self.assertEqual(os.stat(res.new_path).st_mode & 0o777, 0o644)

    def test_symlink_untouched(self):
        link = os.path.join(self.dir, 'link.dgm')
        os.symlink(self.legacy, link)
        original = read_bytes(self.legacy)
        res = dgf.upgrade_file(link)
        self.assertTrue(os.path.islink(link))
        self.assertEqual(read_bytes(self.legacy), original)
        self.assertEqual(res.new_path, os.path.join(self.dir, 'link_v%d.dgm' % DGM_FORMAT_VERSION))
        self.assertFalse(os.path.islink(res.new_path))

    def test_existing_file_never_overwritten(self):
        existing = os.path.join(self.dir, 'legacy_v%d.dgm' % DGM_FORMAT_VERSION)
        with open(existing, 'w') as f:
            f.write('keep me')
        self.assertEqual(dgf.upgraded_path_for(self.legacy),
                         os.path.join(self.dir, 'legacy_v%d_1.dgm' % DGM_FORMAT_VERSION))
        res = dgf.upgrade_file(self.legacy)
        self.assertEqual(res.new_path, os.path.join(self.dir, 'legacy_v%d_1.dgm' % DGM_FORMAT_VERSION))
        with open(existing) as f:
            self.assertEqual(f.read(), 'keep me')

    def test_upgrade_current_file_is_noop(self):
        new_path = dgf.upgrade_file(self.legacy).new_path
        upgraded = read_bytes(new_path)
        res = dgf.upgrade_file(new_path)
        self.assertIsNone(res.new_path)
        self.assertEqual(read_bytes(new_path), upgraded)
        self.assertEqual(len(os.listdir(self.dir)), 2)

    def test_failed_write_leaves_nothing(self):
        original = read_bytes(self.legacy)
        with mock.patch('supsisim.dgmformat.os.fsync', side_effect=OSError('disk full')):
            with self.assertRaises(dgf.DgmFormatError):
                dgf.upgrade_file(self.legacy)
        self.assertEqual(read_bytes(self.legacy), original)
        self.assertEqual(os.listdir(self.dir), ['legacy.dgm'])

    def test_unwritable_directory(self):
        os.chmod(self.dir, 0o555)
        try:
            with self.assertRaises(dgf.DgmFormatError):
                dgf.upgrade_file(self.legacy)
        finally:
            os.chmod(self.dir, 0o755)
        self.assertEqual(os.listdir(self.dir), ['legacy.dgm'])

    def test_upgrade_file_too_new(self):
        d = legacy_dgm()
        d['init']['format'] = 999
        write_json(self.legacy, d)
        original = read_bytes(self.legacy)
        with self.assertRaises(dgf.DgmFormatTooNew):
            dgf.upgrade_file(self.legacy)
        self.assertEqual(read_bytes(self.legacy), original)
        self.assertEqual(os.listdir(self.dir), ['legacy.dgm'])

    def test_load_dict_writes_nothing(self):
        original = read_bytes(self.legacy)
        d, steps = dgf.load_dict(self.legacy)
        self.assertEqual(d['init']['format'], DGM_FORMAT_VERSION)
        self.assertTrue(steps)
        self.assertEqual(read_bytes(self.legacy), original)
        self.assertEqual(os.listdir(self.dir), ['legacy.dgm'])

    def test_load_dict_too_new(self):
        d = legacy_dgm()
        d['init']['format'] = 999
        write_json(self.legacy, d)
        with self.assertRaises(dgf.DgmFormatTooNew):
            dgf.load_dict(self.legacy)


if __name__ == '__main__':
    unittest.main()
