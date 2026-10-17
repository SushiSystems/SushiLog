# Changelog

## Unreleased

- 2026-10-17 — text: Added the text module, exported as SushiLog::text, with the format function that log records use (`text_format.hpp`).
- 2026-10-17 — text: Added helpers that write a value with a precision, in general or scientific form, in hex, or aligned in a width (`value_text.hpp`).
- 2026-10-17 — log: Breaking change: Removed `log_format.hpp` and `Detail::format_log_message`; the front formats through `format_text` (`log_front.hpp`).

## v0.1.2 — 2026-10-17

- 2026-10-17 — log: Made a project level call add the project, so a level set before the project's first log call holds, and dropped the warning it wrote instead (`level_table.cpp`, `log_calls.cpp`).

- 2026-10-17 — docs: Stated that SushiLog exports functions only, so one set of objects compiled with `SUSHILOG_STATIC` links either target (`LOGGING_GUIDE.md`).
