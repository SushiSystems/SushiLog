# Changelog

## Unreleased

## v0.2.1 — 2026-10-17

- 2026-10-17 — text: Marked a placeholder with no argument as {?} and extra arguments as {+N}, and wrote 8-bit integers, scoped enums and null C strings readably (`text_format.hpp`).
- 2026-10-17 — log: Added `log_at`, which writes a record at a given call site past both level gates (`log_calls.hpp`).

- 2026-10-17 — tests: Made the package consumer ask for the version this tree builds, so it follows each release (`tests/package/CMakeLists.txt`).
