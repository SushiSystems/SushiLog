# Report: initial implementation

Status: done. Tasks 2 to 5 and 5a of the SushiLog plan in SushiSkills
(`docs/agent/2026_10_16_SUSHILOG_REPOSITORY/PLAN.md`), written by one subagent (opus, low) in two
rounds; the orchestrator ran every build and test.

## Verification, Windows, MSVC, Debug

```
$ sl build                    clean
$ sl test --label small       100% tests passed, 0 tests failed out of 48
$ sl test --label medium      100% tests passed, 0 tests failed out of 17
                              (Medium_LogExit.ACallAfterTheLoggerStoppedIsDelivered skipped on Windows, reason in its message)
$ sl test --package           100% tests passed, 0 tests failed out of 2
$ python tools/check_all.py . clean: 9 checkers, tests
$ cd cli && python -m pytest  28 passed
```

Linux, ASan with UBSan and ThreadSanitizer run in CI.

## What was built

- `modules/foundation/log/`: the interface of `skills/logging/INTERFACE.md`, one public header
  per symbol, and one source per responsibility: level table, level environment, level names,
  record format, own record, queue, sink holder, writer, forward sink, library path, stderr line
  and the logger that composes them. `log_c_sink.h` stays out of the C++ umbrella.
- `SushiLog::SushiLog` (shared) and `SushiLog::SushiLogStatic` (hidden symbols, for plug-ins),
  installed and exported; the runtime DLL is copied beside every executable that loads it.
- The ABI check: `K_ABI_VERSION`, `abi_version()` and the front's first-call comparison.
- The `sl` CLI on sushicore (`build`, `test`, `check`), CI (checkers, CLI, Linux and Windows,
  sanitizers, a package consumer), the documentation tree and the logging guide.

The requirements carried over from SushiRuntime's and SushiDSP's SOLID reports are each met: the
four responsibilities of SushiRuntime's `logger.cpp` are separate sources, the module is
compiled and testable alone, no header includes what it does not use, the environment is
applied by a named call, `Logger` only composes, a level call for another project warns, and
`sl check` reads `--list-rules`.

## Changes made after the first run

- `cmake/SushiLogTests.cmake` builds the pinned GoogleTest always; an installed one from another
  toolchain had a different runtime and broke the Debug link.
- `test_exit.cpp` asserts the exit contract per platform with a gated sink instead of a sleep. On
  Windows the system ends the logger's thread before SushiLog's statics are destroyed, so a
  thread ended inside a sink call loses the queue; that is in `KNOWN_ISSUES.md`.

## Not done

- No test loads two static copies in one process; the static target is covered by the package
  consumer only.
- The `FATAL` flush is not asserted.
- `ci.yml` takes `ilammy/msvc-dev-cmd` by tag, not by hash; in `REMAINING_WORK.md`.

## SOLID breaches and unneeded dependencies

- Kept: `Delegate` lives in the log module, its only user.
- Kept: `LogWriter` holds the thread, synchronous mode, `FATAL` order and shutdown, as the plan's
  table assigns.
- Kept: `log_calls.cpp` forwards each call and composes the unowned-project warning.
- Kept: `level_named` relies on its caller to trim.
- Dependencies: `Threads::Threads` and, outside Windows, `${CMAKE_DL_LIBS}` for the library path
  in the ABI message; GoogleTest for the tests only, pinned by version and hash. No unneeded
  dependency.
