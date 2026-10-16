# Changelog

## Unreleased

## v0.1.0 — 2026-10-17

- 2026-10-17 — ci: Added checkers, CLI tests, Linux and Windows small and medium with the package consumer, ASan with UBSan and ThreadSanitizer (`ci.yml`, `setup-sl`).
- 2026-10-17 — package: Installed SushiLog::SushiLog and SushiLog::SushiLogStatic, and added a consumer that logs through both (`SushiLogInstall.cmake`, `tests/package`).
- 2026-10-17 — log: Added the logger in one brick per responsibility, with a bounded queue, the FATAL order, the C interface and the ABI check (`log_writer.cpp`, `level_table.cpp`, `log_abi.hpp`).
- 2026-10-17 — log: Added the public headers of the logging interface and the umbrella (`log_calls.hpp`, `log_front.hpp`, `log_c_sink.h`, `SushiLog.hpp`).
- 2026-10-17 — cli: Added `sl` with build, test and check on sushicore 0.9.0 (`cli.py`, `project.py`).
