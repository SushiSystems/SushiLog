# Command line

## Remaining work

| ID | Pri | State | Title | Done when | Blocked by | Source | Opened |
| --- | --- | --- | --- | --- | --- | --- | --- |
| RW-27 | P1 | open | Add the shared clean command to sl and drop --clean from build and test | sl clean says what it would remove and removes trees only with --yes, and sl build and test have no --clean |  | `cli/sushilog/cli.py` | 2026-11-21 |

## Known issues

| ID | Origin | Symptom | Workaround | Fix | Source | Opened |
| --- | --- | --- | --- | --- | --- | --- |
| KI-3 | own | In PowerShell `sl build` changes folder instead of building | run `sushilog build` | none, `sushilog` is the same command under its full name |  | 2026-10-19 |
