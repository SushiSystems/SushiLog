# Remaining work

The single backlog of this repository. An item names where it sits.

## Release

- **The first build and test run.** The library, the tests and the package consumer were
  written and syntax-checked; the first `sl build`, `sl test --label small --label medium
  --package` and the CI run on main are the owner's.
- **Tag v0.1.0** with the owner's approval, task 6 of SushiSkills'
  `docs/agent/2026_10_16_SUSHILOG_REPOSITORY/PLAN.md`.

## CI

- **The CI's Windows job takes `ilammy/msvc-dev-cmd@v1` by tag**, not by commit hash
  (`.github/workflows/ci.yml`).
