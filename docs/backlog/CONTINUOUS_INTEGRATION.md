# Continuous integration

## Remaining work

| ID | Pri | State | Title | Done when | Blocked by | Source | Opened |
| --- | --- | --- | --- | --- | --- | --- | --- |
| RW-1 | P2 | open | Pin the Windows job's MSVC action by commit hash | `.github/workflows/ci.yml` takes `ilammy/msvc-dev-cmd` by a commit hash, not by `@v1` |  |  | 2026-10-19 |
| RW-15 | P2 | open | Clear the known findings of the shared checkers | tools/known_findings.toml holds no entry and tools/check_all.py is clean without it |  | `tools/check_all.py` | 2026-11-05 |

## Known issues

| ID | Origin | Symptom | Workaround | Fix | Source | Opened |
| --- | --- | --- | --- | --- | --- | --- |
