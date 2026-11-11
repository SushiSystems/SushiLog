# Changelog

## Unreleased

- 2026-11-11 — cli: Required sushicore 0.13, whose test command fails a run of a current tree that breaks a time limit, and told it whether the tree is current.
## v0.4.1 — 2026-11-10

- 2026-11-10 — release: Carried the version numbers v0.4.0 lacked; the tag v0.4.0 holds the same code under version 0.3.1 (`CMakeLists.txt`).
- 2026-11-10 — log: Added the executor port `LogExecutor` with `set_log_executor` and `ScopedLogExecutor` (RW-25, `log_executor.hpp`).
- 2026-11-10 — log: Added `ManualLogClock` and `ManualLogExecutor`, so a test drives time and delivery by hand (RW-25, `log_manual.hpp`).
- 2026-11-10 — log: Delivered FATAL, synchronous mode and exit on the calling thread under any executor (RW-25, `log_writer.cpp`).
- 2026-11-10 — cli: Required sushicore 0.12.2, which narrows a sharded test run by `--filter` and reruns a failed shard's cases.
