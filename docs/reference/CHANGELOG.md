# Changelog

## Unreleased

## v0.3.0 — 2026-10-29

- 2026-10-29 — cli: Added the `backlog` and `design` groups and asked for sushicore 0.10.2 (`cli.py`).
- 2026-10-28 — docs: Described installing standalone and with SushiHub, and named the file that pins sushicore (`README.md`, `INSTALL.md`, `NOTICE.md`).
- 2026-10-28 — build: Took the release C runtime, no iterator checking and Release imports in every build type, with line tables for a Clang Debug (`SushiLogAbiPinning.cmake`).
- 2026-10-28 — cli: Changed `sl build` and `sl test` to build Debug by default, one tree per type and sanitizer in `build/<name>-<type>/`, and refused a configure of another type into a tree (`project.py`).
- 2026-10-28 — cli: Ran every configure with the sushicore memory gate as launcher and every compile on the hardware threads, and added `sl build --jobs` and `--report` (`project.py`, `cli.py`).
- 2026-10-26 — deps: Moved the sushicore pin to 0.9.6 (`cli/pyproject.toml`).
- 2026-10-26 — cli: Added `sl build --build-dir` and `--lean`, the contract of a consumer's package build (`cli.py`, `project.py`).
- 2026-10-26 — cli: Registered the shared `package` command from sushicore (`cli.py`).
- 2026-10-26 — cli: Made `sl` honour `SUSHISTACK_CXX` over its configured compiler (`config.py`).
- 2026-10-26 — deps: Added `cli/sushistack.deps.toml`, which names the CLI `sl` in `[module] cli`.
- 2026-10-19 — docs: Moved the backlog and the known issues to the SushiSkills entry format and took the backlog checker with the shared tools (`REMAINING_WORK.md`, `KNOWN_ISSUES.md`, `check_backlog.py`).
