# Logging interface

## Remaining work

| ID | Pri | State | Title | Done when | Blocked by | Source | Opened |
| --- | --- | --- | --- | --- | --- | --- | --- |
| RW-3 | P2 | open | Add a log-once and a rate limit per call site | a call site can log once per key and at most once per interval, and the runtime's `RulePlacement::is_first_warning` and `is_logged_attempt` throttles can be… |  |  | 2026-10-26 |
| RW-4 | P2 | open | Add a counter that reports itself once per interval | a relaxed atomic increment, safe on a real-time thread, feeds one line per interval naming each counter of a category that moved |  |  | 2026-10-26 |
| RW-5 | P2 | open | Carry structured fields and a scoped context on a record | a record holds key and value pairs beside its message, and a scope sets a field (a frame, a run, a peer) on every record made inside it |  |  | 2026-10-26 |
| RW-6 | P2 | open | Stamp each record on the steady clock SushiProfiler uses | a record carries a steady-clock time that SushiProfiler's record clock converts, so a WARN can be placed on a profile's timeline |  |  | 2026-10-26 |
| RW-7 | P2 | open | Carry the thread's name on a record | a record names its thread by the name the program gave it, the same name SushiProfiler's `Collector::set_thread_name` records |  |  | 2026-10-26 |
| RW-8 | P2 | open | Ship a capture sink for tests, and let a sink be restored | a test captures records through a shipped class, `set_log_sink` returns the sink it replaced, and two captures nest |  |  | 2026-10-26 |
| RW-9 | P2 | open | Add a record path for a real-time thread | a record is made with no heap allocation, lock or system call, as fixed-size arguments formatted on the logger thread |  |  | 2026-10-26 |
| RW-10 | P2 | open | Log the exception in flight from a catch block | one call inside `catch (...)` logs the exception's type and message under a category, level and context |  |  | 2026-10-26 |
| RW-11 | P2 | open | Let a program read the dropped-record count and the count per level | a program reads how many records were dropped and how many were made at each level since start |  |  | 2026-10-26 |
| RW-12 | P2 | open | Set levels, register categories and log from Python | the C interface and a Python binding over it set and read a level, register a category and emit a record |  |  | 2026-10-26 |

## Known issues

| ID | Origin | Symptom | Workaround | Fix | Source | Opened |
| --- | --- | --- | --- | --- | --- | --- |
