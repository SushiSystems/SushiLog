# Logging interface

## Remaining work

| ID | Pri | State | Title | Done when | Blocked by | Source | Opened |
| --- | --- | --- | --- | --- | --- | --- | --- |
| RW-3 | P2 | open | Add a log-once and a rate limit per call site | a call site can log once per key and at most once per interval, and the runtime's `RulePlacement::is_first_warning` and `is_logged_attempt` throttles can be… |  |  | 2026-10-26 |
| RW-4 | P2 | open | Add a counter that reports itself once per interval | a relaxed atomic increment, safe on a real-time thread, feeds one line per interval naming each counter of a category that moved |  |  | 2026-10-26 |
| RW-5 | P2 | open | Carry structured fields and a scoped context on a record | a record holds key and value pairs beside its message, and a scope sets a field (a frame, a run, a peer) on every record made inside it |  |  | 2026-10-26 |
| RW-6 | P2 | open | Stamp each record on the steady clock SushiProfiler uses | a record carries a steady-clock time that SushiProfiler's record clock converts, so a WARN can be placed on a profile's timeline |  |  | 2026-10-26 |
| RW-7 | P2 | open | Carry the thread's name on a record | a record names its thread by the name the program gave it, the same name SushiProfiler's `Collector::set_thread_name` records |  |  | 2026-10-26 |
| RW-9 | P2 | open | Add a record path for a real-time thread | a record is made with no heap allocation, lock or system call, as fixed-size arguments formatted on the logger thread |  |  | 2026-10-26 |
| RW-10 | P2 | open | Log the exception in flight from a catch block | one call inside `catch (...)` logs the exception's type and message under a category, level and context |  |  | 2026-10-26 |
| RW-11 | P2 | open | Let a program read the dropped-record count and the count per level | a program reads how many records were dropped and how many were made at each level since start |  |  | 2026-10-26 |
| RW-12 | P2 | open | Set levels, register categories and log from Python | the C interface and a Python binding over it set and read a level, register a category and emit a record |  |  | 2026-10-26 |
| RW-21 | P2 | open | Rule on an injected clock for the record time | Ruled 2026-11-06: a clock port exchanged like the sink; the time test Small_ on a set clock, one medium default test |  | `docs/agent/2026_11_05_TEST_REVIEW/REPORT.md` | 2026-11-06 |
| RW-22 | P2 | open | Rule on thread identities as values for the thread number | Ruled 2026-11-06: thread numbering is a generic brick tested Small_ on values; one medium two-thread test |  | `docs/agent/2026_11_05_TEST_REVIEW/REPORT.md` | 2026-11-06 |

## Known issues

| ID | Origin | Symptom | Workaround | Fix | Source | Opened |
| --- | --- | --- | --- | --- | --- | --- |
