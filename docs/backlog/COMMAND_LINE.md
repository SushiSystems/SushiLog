# Command line

## Remaining work

| ID | Pri | State | Title | Done when | Blocked by | Source | Opened |
| --- | --- | --- | --- | --- | --- | --- | --- |
| RW-16 | P2 | blocked | Move the CLI tests onto sushicore's in-memory runner and console | cli/tests/conftest.py defines no runner or console and Workspace.runner is typed as sushicore's CommandRunner | external: sushicore release past 0.10.11 that ships proc_memory and console_memory | `cli/tests/conftest.py` | 2026-11-06 |

## Known issues

| ID | Origin | Symptom | Workaround | Fix | Source | Opened |
| --- | --- | --- | --- | --- | --- | --- |
| KI-3 | own | In PowerShell `sl build` changes folder instead of building | run `sushilog build` | none, `sushilog` is the same command under its full name |  | 2026-10-19 |
