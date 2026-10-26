# Changelog

## Unreleased

- 2026-10-26 — cli: Added `sl build --build-dir` and `--lean`, the contract of a consumer's package build (`cli.py`, `project.py`).
- 2026-10-26 — cli: Registered the shared `package` command from sushicore (`cli.py`).
- 2026-10-26 — cli: Made `sl` honour `SUSHISTACK_CXX` over its configured compiler (`config.py`).
- 2026-10-26 — deps: Added `cli/sushistack.deps.toml`, which names the CLI `sl` in `[module] cli`.
- 2026-10-19 — docs: Moved the backlog and the known issues to the SushiSkills entry format and took the backlog checker with the shared tools (`REMAINING_WORK.md`, `KNOWN_ISSUES.md`, `check_backlog.py`).

## v0.2.3 — 2026-10-19

- 2026-10-19 — ci: Added the release job that builds the documentation bundle after CI and attaches it to the tag's release (`.github/workflows/release.yml`, `ci.yml`).
- 2026-10-19 — cli: Added `sl docs bundle`, registered from SushiCore, and moved the sushicore pin to 0.9.2 (`cli/sushilog/cli.py`, `cli/pyproject.toml`).
