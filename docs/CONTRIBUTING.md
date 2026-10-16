# Contributing to SushiLog

Contributions from outside Sushi Systems are not accepted yet. SushiLog is source-available
under the PolyForm Noncommercial License 1.0.0; see [`LICENSE`](../LICENSE) and
[`COMMERCIAL.md`](../COMMERCIAL.md).

## Building and testing

```bash
pip install -e ./cli
sl build
sl test --label small --label medium --package
sl check
```

Every build and test goes through `sl`; [`CLI_GUIDE.md`](guides/CLI_GUIDE.md) lists the commands.

## What a change contains

- The contract is the interface and the rules of the logging skill in SushiSkills. A
  change to a type or a call of the interface lands there first, raises `K_ABI_VERSION` and is a
  breaking change: its commit subject carries `!`.
- One responsibility per source file under `modules/foundation/log/source/`; the module README
  lists them.
- Every public header has its test at the mirrored path under `tests/`, and every test names
  its size in its suite: `Small_`, `Medium_` or `Large_`.
- The module README, the pages under `docs/` that describe the change and a changelog line land
  in the same commit.
- `python tools/check_all.py .` is clean before the change is reported finished.
