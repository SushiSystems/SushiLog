# Changelog

## Unreleased

- 2026-10-19 — ci: Added the release job that builds the documentation bundle after CI and attaches it to the tag's release (`.github/workflows/release.yml`, `ci.yml`).
- 2026-10-19 — cli: Added `sl docs bundle`, registered from SushiCore, and moved the sushicore pin to 0.9.2 (`cli/sushilog/cli.py`, `cli/pyproject.toml`).

## v0.2.2 — 2026-10-17

- 2026-10-17 — build: Set C++17 through the CXX_STANDARD property, so a compiler whose features CMake cannot detect still configures (`SushiLogLibrary.cmake`).
