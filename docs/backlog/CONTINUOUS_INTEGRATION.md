# Continuous integration

## Remaining work

| ID | Pri | State | Title | Done when | Blocked by | Source | Opened |
| --- | --- | --- | --- | --- | --- | --- | --- |
| RW-1 | P2 | open | Pin the Windows job's MSVC action by commit hash | `.github/workflows/ci.yml` takes `ilammy/msvc-dev-cmd` by a commit hash, not by `@v1` |  |  | 2026-10-19 |
| RW-15 | P2 | open | Clear the known findings of the shared checkers | tools/known_findings.toml holds no entry and tools/check_all.py is clean without it |  | `tools/check_all.py` | 2026-11-05 |
| RW-17 | P2 | decision | Rule on tools/tests/test_test_budget_settings.py, which SushiSkills deleted in a528a1d | The file is deleted with K_TEST_BUDGET and its known finding, or rewritten as REPORT.md rules |  | `docs/agent/2026_11_05_TEST_REVIEW/REPORT.md` | 2026-11-06 |
| RW-18 | P2 | open | Let check_test_mirror accept a header with no runtime promise | check_test_mirror passes log_category, log_project, log_record, log_sink and SushiLog.hpp without known findings |  | `tools/known_findings.toml` | 2026-11-06 |

## Known issues

| ID | Origin | Symptom | Workaround | Fix | Source | Opened |
| --- | --- | --- | --- | --- | --- | --- |
