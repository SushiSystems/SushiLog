# Command line

## Remaining work

| ID | Pri | State | Title | Done when | Blocked by | Source | Opened |
| --- | --- | --- | --- | --- | --- | --- | --- |
| RW-26 | P2 | open | Add sl run, which picks and runs a built program like the sibling CLIs | sl run NAME runs a built program and sl run --pick lists the built examples with --all for every program |  | `cli/sushilog/services/project.py` | 2026-11-21 |

## Known issues

| ID | Origin | Symptom | Workaround | Fix | Source | Opened |
| --- | --- | --- | --- | --- | --- | --- |
| KI-3 | own | In PowerShell `sl build` changes folder instead of building | run `sushilog build` | none, `sushilog` is the same command under its full name |  | 2026-10-19 |
