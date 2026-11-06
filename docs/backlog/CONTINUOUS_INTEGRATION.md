# Continuous integration

## Remaining work

| ID | Pri | State | Title | Done when | Blocked by | Source | Opened |
| --- | --- | --- | --- | --- | --- | --- | --- |
| RW-1 | P2 | open | Pin the Windows job's MSVC action by commit hash | `.github/workflows/ci.yml` takes `ilammy/msvc-dev-cmd` by a commit hash, not by `@v1` |  |  | 2026-10-19 |
| RW-15 | P2 | open | Clear the known findings of the shared checkers | tools/known_findings.toml holds no entry and tools/check_all.py is clean without it |  | `tools/check_all.py` | 2026-11-05 |
| RW-18 | P2 | open | Let check_test_mirror accept a header with no runtime promise | check_test_mirror passes log_category, log_project, log_record, log_sink and SushiLog.hpp without known findings |  | `tools/known_findings.toml` | 2026-11-06 |
| RW-24 | P2 | open | Register Characterization_ suites, then move the Windows exit pin to one | AThreadEndedInsideTheSinkLosesTheQueue opens with Characterization_ and runs through sl test |  | `tests/foundation/log/test_log_calls/test_exit.cpp` | 2026-11-06 |

## Known issues

| ID | Origin | Symptom | Workaround | Fix | Source | Opened |
| --- | --- | --- | --- | --- | --- | --- |
