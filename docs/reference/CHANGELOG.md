# Changelog

## Unreleased

- 2026-10-17 — docs: Stated that SushiLog exports functions only, so one set of objects compiled with `SUSHILOG_STATIC` links either target (`LOGGING_GUIDE.md`).

## v0.1.1 — 2026-10-17

- 2026-10-17 — package: Exported the build tree, so a project that adds SushiLog as a subdirectory and exports its own build tree links it (`SushiLogInstall.cmake`).
- 2026-10-17 — build: Copied the warning and sanitizer settings onto each target instead of linking them, so the static library's export names neither (`SushiLogBuildPolicy.cmake`).
