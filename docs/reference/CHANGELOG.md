# Changelog

## Unreleased

## v0.4.0 — 2026-11-10

- 2026-11-10 — log: Added the executor port `LogExecutor` with `set_log_executor` and `ScopedLogExecutor` (RW-25, `log_executor.hpp`).
- 2026-11-10 — log: Added `ManualLogClock` and `ManualLogExecutor`, so a test drives time and delivery by hand (RW-25, `log_manual.hpp`).
- 2026-11-10 — log: Delivered FATAL, synchronous mode and exit on the calling thread under any executor (RW-25, `log_writer.cpp`).
- 2026-11-10 — cli: Required sushicore 0.12.2, which narrows a sharded test run by `--filter` and reruns a failed shard's cases.

## v0.3.1 — 2026-11-07

- 2026-11-06 — tools: Took the shared checkers that accept a compile check and a `Characterization_` size, and cleared every known finding (RW-15, RW-18, `29d7a40`).
- 2026-11-06 — test: Registered `Characterization_<Size>_` suites at their size and moved the Windows exit pin to one (RW-24, `a843b1b`).
- 2026-11-06 — log: Took the record time from `LogClock`, the system clock by default, with `exchange_log_clock` and `ScopedLogClock` (RW-21, `01f2938`).
- 2026-11-06 — log: Numbered threads through `ThreadNumbers`, generic over the identity (RW-22, `01f2938`).
- 2026-11-06 — log: Made `parse_level_setting` and `LevelSetting` public in `level_setting.hpp` (RW-19) (`f326cac`).
- 2026-11-06 — log: Made `K_LOG_QUEUE_CAPACITY` public in `log_calls.hpp` (RW-20) (`168e7a0`).
- 2026-11-06 — log: Added `delegate_fits_v` and `Delegate::fits`, which constrain the Delegate constructor to callables that fit its storage (RW-23, `c960ede`).
- 2026-11-06 — cli: Moved the CLI tests onto sushicore's `RecordingRunner` and `RecordingConsole`, typed the workspace on sushicore's protocols, and asked for sushicore 0.10.12 (RW-16, `a1ecd34`).
- 2026-11-06 — tools: Deleted the leftover budget-settings test and `K_TEST_BUDGET`, which SushiSkills had dropped (RW-17, `94dab1f`).
- 2026-11-06 — cli: Handed the Typer context object to the project service as its workspace, so the command tests run the real service on a scratch checkout (`cli.py`).
- 2026-11-06 — cli: Passed the root, configuration, console, runner and environment into the project service as a `Workspace`, so its tests replace no function of the repository (`project.py`).
- 2026-11-06 — test: Set the size timeouts to 1, 10 and 120 seconds, as the testing skill gives, and stopped promising a WARN for an unseen project (`SushiLogTests.cmake`, `log_calls.hpp`).
- 2026-11-06 — licence: Relicensed SushiLog under the Apache License 2.0 and removed `COMMERCIAL.md`; every source file's licence block follows (`LICENSE`, `tools/settings.py`).
- 2026-11-05 — log: Shipped `LogCapture` and `LogRecorder` for tests, with `exchange_log_sink` and `log_synchronous`, so a capture puts back the sink and mode it found (RW-8, `log_capture.hpp`).
- 2026-11-05 — tools: Took the shared checkers of 2026-11-05 with a baseline of the findings they meet, which RW-15 clears.
- 2026-10-31 — cli: Ran `sl test` through the shared test options and `run_tests` of sushicore, so it runs small and medium by default and prints the budget line (RW-14, `a866aa4`).
- 2026-10-30 — continuous_integration: Turn the red CI green: install sushicore for the checkers, map Debug in the package test (RW-13, `e890c45`).
- 2026-10-29 — build: Let a Debug tree link an imported package built as `None`, such as Ubuntu's GTest (`cmake/`).
