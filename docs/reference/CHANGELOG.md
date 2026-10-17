# Changelog

## Unreleased

- 2026-10-17 — tests: Made the package consumer ask for the version this tree builds, so it follows each release (`tests/package/CMakeLists.txt`).

## v0.2.0 — 2026-10-17

- 2026-10-17 — text: Added the text module, exported as SushiLog::text, with the format function that log records use (`text_format.hpp`).
- 2026-10-17 — text: Added helpers that write a value with a precision, in general or scientific form, in hex, or aligned in a width (`value_text.hpp`).
- 2026-10-17 — log: Breaking change: Removed `log_format.hpp` and `Detail::format_log_message`; the front formats through `format_text` (`log_front.hpp`).
