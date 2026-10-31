"""
Versioning and upgrade of the pysimCoder diagram (.dgm) file format.

This module is the Qt-free backend: it decides the format of a diagram,
migrates older diagrams step by step to the current format and writes
the upgraded diagram to a new file (the original is never modified). The GUI only asks the user and calls
the functions below.

The format version is a plain integer stored in dataDict['init']['format'],
independent of the application version (const.VERSION). Diagrams without
it are format 0 (all legacy variants before versioning was introduced).

Changing the .dgm format:
  1. bump DGM_FORMAT_VERSION in const.py (N -> N+1),
  2. add a migration function _vN_to_vN+1(d) and register it in MIGRATIONS,
  3. add a test with a format N fixture.
"""

import copy
import glob
import json
import os
import shutil
from dataclasses import dataclass, field
from enum import Enum

from supsisim.const import DGM_FORMAT_VERSION, respath


class DgmFormatError(Exception):
    """The diagram cannot be read, migrated or written."""


class DgmFormatTooNew(DgmFormatError):
    """The diagram was created by a newer pysimCoder."""


class DgmKind(Enum):
    CURRENT = 'current'
    OLDER = 'older'
    NEWER = 'newer'
    INVALID = 'invalid'


@dataclass
class DgmStatus:
    kind: DgmKind
    file_format: int | None
    current_format: int = DGM_FORMAT_VERSION
    steps: list[str] = field(default_factory=list)
    error: str | None = None


@dataclass
class UpgradeResult:
    new_path: str | None
    from_format: int
    to_format: int
    steps: list[str]


# ============================================================
# Helpers
# ============================================================

def get_format(d: dict) -> int:
    """Return the format version of a diagram dict (0 if not present)."""
    init = d.get('init', {})
    fmt = init.get('format', 0) if isinstance(init, dict) else 0
    if isinstance(fmt, bool) or not isinstance(fmt, int) or fmt < 0:
        raise DgmFormatError('Invalid diagram format version: %r' % (fmt,))
    return fmt


def _walk_scenes(scene: dict):
    """Yield the scene dict and, recursively, all subsystem scenes."""
    yield scene
    for subs in scene.get('subsystems', []) or []:
        sub = subs.get('subitems')
        if isinstance(sub, dict):
            yield from _walk_scenes(sub)


def library_dirs() -> list[str]:
    """Directories searched for .xblk block definitions."""
    return [respath + 'blocks/blocks']


_lib_param_types = None

def _library_param_types() -> dict:
    """Map block function name -> {param name: type} from the .xblk library."""
    global _lib_param_types
    if _lib_param_types is not None:
        return _lib_param_types

    types = {}
    for d in library_dirs():
        for fn in glob.glob(os.path.join(d, '**', '*.xblk'), recursive=True):
            try:
                with open(fn, 'r') as f:
                    params = json.load(f)['params'].split('|')
            except (OSError, ValueError, KeyError, AttributeError):
                continue
            blk = types.setdefault(params[0].strip(), {})
            for p in params[1:]:
                par = p.split(':')
                if len(par) == 3:
                    blk.setdefault(par[0].strip(), par[2].strip())
    _lib_param_types = types
    return types


def infer_param_type(value: str) -> str | None:
    """
    Guess the type of an untyped parameter value. Same rules as
    Scene._recheck_param: quoted strings and number arrays only,
    anything else is unknown (None).
    """
    v = str(value).replace(' ', '')
    if len(v) >= 2 and v[0] == v[-1] and v[0] in '"\'':
        return 'str'
    if len(v) >= 2 and v[0] == '[' and v[-1] == ']':
        force_double = False
        for n in v[1:-1].split(','):
            try:
                int(n)
            except ValueError:
                try:
                    float(n)
                    force_double = True
                except ValueError:
                    return None
        return 'double' if force_double else 'int'
    return None


# ============================================================
# Migrations
# ============================================================

SIMULATE_DEFAULTS = {
    'template': 'sim.tmf',
    'Ts': '0.01',
    'AddObj': '',
    'AddCDefs': '',
    'AddMakeArgs': '',
    'script': '',
    'intgMethod': 'standard RK4',
    'epsAbs': '1e-6',
    'epsRel': '1e-6',
    'Tf': '10',
    'prio': '',
}


def _type_params(params: str) -> str:
    """Append a type to every 'name:value' parameter where it can be determined."""
    items = params.split('|')
    lib = _library_param_types().get(items[0].strip(), {})
    for i in range(1, len(items)):
        par = items[i].split(':')
        # the generic parameter dialog (RCPDlg) saved untyped params as 'name: value: None'
        if len(par) == 3 and par[2].strip() in ('', 'None'):
            par = par[:2]
        if len(par) != 2:
            continue
        ptype = lib.get(par[0].strip()) or infer_param_type(par[1])
        if ptype is not None:
            items[i] = ':'.join(par) + ':' + ptype
    return '|'.join(items)


def _v0_to_v1(d: dict) -> dict:
    sim = d.setdefault('simulate', {})
    # diagrams saved between 62fb819 and 993061a store Tf as 'relTf'
    if 'relTf' in sim and 'Tf' not in sim:
        sim['Tf'] = sim.pop('relTf')
    for key, val in SIMULATE_DEFAULTS.items():
        sim.setdefault(key, val)

    for scene in _walk_scenes(d):
        blocks = list(scene.get('blocks', []) or [])
        blocks += [s['block'] for s in scene.get('subsystems', []) or [] if 'block' in s]
        for blk in blocks:
            if 'width' in blk and 'dims' not in blk:
                blk['dims'] = blk.pop('width')
        for blk in scene.get('blocks', []) or []:
            if isinstance(blk.get('params'), str):
                blk['params'] = _type_params(blk['params'])
    return d


# format N -> (description, function migrating N to N+1)
MIGRATIONS = {
    0: ('Legacy diagram: rename block "width" to "dims" and "relTf" to "Tf", add missing '
        'simulation settings and add types to block parameters', _v0_to_v1),
}


# ============================================================
# Dict-level API
# ============================================================

def upgrade(d: dict) -> tuple[dict, list[str]]:
    """
    Migrate a diagram dict to DGM_FORMAT_VERSION. The input is not modified.
    Returns the upgraded dict and the descriptions of the applied steps.
    """
    if not isinstance(d, dict):
        raise DgmFormatError('The diagram is not a JSON object')
    fmt = get_format(d)
    if fmt > DGM_FORMAT_VERSION:
        raise DgmFormatTooNew(
            'The diagram has format %d, this pysimCoder supports up to %d. '
            'Please update pysimCoder.' % (fmt, DGM_FORMAT_VERSION))

    d = copy.deepcopy(d)
    steps = []
    for v in range(fmt, DGM_FORMAT_VERSION):
        if v not in MIGRATIONS:
            raise DgmFormatError('No migration from format %d to %d' % (v, v + 1))
        desc, fn = MIGRATIONS[v]
        try:
            d = fn(d)
        except Exception as e:
            raise DgmFormatError('Migration from format %d to %d failed: %s' % (v, v + 1, e)) from e
        steps.append('%d -> %d: %s' % (v, v + 1, desc))

    init = d.get('init')
    if not isinstance(init, dict):
        init = d['init'] = {'code': 'pysimCoder'}
    init['format'] = DGM_FORMAT_VERSION
    return d, steps


def pending_steps(fmt: int) -> list[str]:
    """Descriptions of the migrations needed to bring format fmt up to date."""
    return ['%d -> %d: %s' % (v, v + 1, MIGRATIONS[v][0])
            for v in range(fmt, DGM_FORMAT_VERSION) if v in MIGRATIONS]


# ============================================================
# File-level API
# ============================================================

def _read_bytes(path: str) -> bytes:
    try:
        with open(path, 'rb') as f:
            return f.read()
    except OSError as e:
        raise DgmFormatError('Cannot read %s: %s' % (path, e)) from e


def _parse(path: str, data: bytes) -> dict:
    try:
        d = json.loads(data)
    except ValueError as e:
        raise DgmFormatError('%s is not a valid diagram file: %s' % (path, e)) from e
    if not isinstance(d, dict):
        raise DgmFormatError('%s is not a valid diagram file' % path)
    return d


def _read_json(path: str) -> dict:
    return _parse(path, _read_bytes(path))


def inspect_file(path: str) -> DgmStatus:
    """Classify a diagram file. Problems are reported in the status."""
    try:
        fmt = get_format(_read_json(path))
    except DgmFormatError as e:
        return DgmStatus(DgmKind.INVALID, None, error=str(e))

    if fmt == DGM_FORMAT_VERSION:
        return DgmStatus(DgmKind.CURRENT, fmt)
    if fmt > DGM_FORMAT_VERSION:
        return DgmStatus(DgmKind.NEWER, fmt,
                         error='The diagram has format %d, this pysimCoder supports up to %d. '
                               'Please update pysimCoder.' % (fmt, DGM_FORMAT_VERSION))
    return DgmStatus(DgmKind.OLDER, fmt, steps=pending_steps(fmt))


def _upgraded_candidates(path: str, fmt: int):
    stem = path[:-4] if path.endswith('.dgm') else path
    yield '%s_v%d.dgm' % (stem, fmt)
    n = 1
    while True:
        yield '%s_v%d_%d.dgm' % (stem, fmt, n)
        n += 1


def upgraded_path_for(path: str, fmt: int = DGM_FORMAT_VERSION) -> str:
    """First free name for the upgraded diagram: <name>_v<fmt>.dgm, then <name>_v<fmt>_1.dgm, ..."""
    for candidate in _upgraded_candidates(path, fmt):
        if not os.path.exists(candidate):
            return candidate


def upgrade_file(path: str) -> UpgradeResult:
    """
    Upgrade a diagram file into a new file in the same directory
    (see upgraded_path_for). The original file is never modified and an
    existing file is never overwritten. On failure DgmFormatError is raised
    and no new file is left behind.
    """
    d = _read_json(path)
    fmt = get_format(d)
    new, steps = upgrade(d)
    if not steps:
        return UpgradeResult(None, fmt, DGM_FORMAT_VERSION, steps)

    for candidate in _upgraded_candidates(path, DGM_FORMAT_VERSION):
        try:
            f = open(candidate, 'x')
        except FileExistsError:
            continue
        except OSError as e:
            raise DgmFormatError('Cannot create the upgraded diagram %s: %s' % (candidate, e)) from e
        try:
            with f:
                f.write(json.dumps(new))
                f.flush()
                os.fsync(f.fileno())
            shutil.copymode(path, candidate)
        except OSError as e:
            os.remove(candidate)
            raise DgmFormatError('Cannot write the upgraded diagram %s: %s' % (candidate, e)) from e
        return UpgradeResult(candidate, fmt, DGM_FORMAT_VERSION, steps)


def load_dict(path: str) -> tuple[dict, list[str]]:
    """Read a diagram file and upgrade it in memory only (nothing is written)."""
    return upgrade(_read_json(path))
