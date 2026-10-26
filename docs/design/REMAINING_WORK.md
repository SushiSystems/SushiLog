# Remaining work

Next id: RW-13

## Continuous integration

### RW-1 Pin the Windows job's MSVC action by commit hash
- State: open
- Done when: `.github/workflows/ci.yml` takes `ilammy/msvc-dev-cmd` by a commit hash, not by `@v1`

## Documentation

### RW-2 Give the root README its six sections
- State: open
- Done when: `README.md` holds the sections the `documentation` skill gives the root README,
  `## Status` among them

## Logging interface

### RW-3 Add a log-once and a rate limit per call site
- State: open
- Done when: a call site can log once per key and at most once per interval, and the runtime's
  `RulePlacement::is_first_warning` and `is_logged_attempt` throttles can be deleted for it

### RW-4 Add a counter that reports itself once per interval
- State: open
- Done when: a relaxed atomic increment, safe on a real-time thread, feeds one line per interval
  naming each counter of a category that moved

### RW-5 Carry structured fields and a scoped context on a record
- State: open
- Done when: a record holds key and value pairs beside its message, and a scope sets a field
  (a frame, a run, a peer) on every record made inside it

### RW-6 Stamp each record on the steady clock SushiProfiler uses
- State: open
- Done when: a record carries a steady-clock time that SushiProfiler's record clock converts, so a
  WARN can be placed on a profile's timeline

### RW-7 Carry the thread's name on a record
- State: open
- Done when: a record names its thread by the name the program gave it, the same name
  SushiProfiler's `Collector::set_thread_name` records

### RW-8 Ship a capture sink for tests, and let a sink be restored
- State: open
- Done when: a test captures records through a shipped class, `set_log_sink` returns the sink it
  replaced, and two captures nest

### RW-9 Add a record path for a real-time thread
- State: open
- Done when: a record is made with no heap allocation, lock or system call, as fixed-size
  arguments formatted on the logger thread

### RW-10 Log the exception in flight from a catch block
- State: open
- Done when: one call inside `catch (...)` logs the exception's type and message under a category,
  level and context

### RW-11 Let a program read the dropped-record count and the count per level
- State: open
- Done when: a program reads how many records were dropped and how many were made at each level
  since start

### RW-12 Set levels, register categories and log from Python
- State: open
- Done when: the C interface and a Python binding over it set and read a level, register a
  category and emit a record
