# Known issues

Next id: KI-4

## Shutdown

### KI-1 A process that skips static destruction loses the records still queued
- Origin: own
- Where: `modules/foundation/log/source/logger.cpp`
- Cause: the writer drains its queue when the library's statics are destroyed
- Recognise: records logged just before a crash or an `_exit` never reach the sink
- Workaround: none
- Fix: none, a process that skips static destruction runs no code that could drain the queue

### KI-2 On Windows the logger's thread can end inside a sink call at exit
- Origin: own
- Where: `modules/foundation/log/source/sink_holder.cpp`,
  `tests/foundation/log/test_log_calls/test_exit.cpp`
- Cause: Windows ends every thread but the main one before SushiLog's statics are destroyed
- Recognise: at exit the records still queued are missing, and later records reach stderr
- Workaround: none
- Fix: none, the system ends the thread before any SushiLog code runs at exit

## Command line

### KI-3 In PowerShell `sl build` changes folder instead of building
- Origin: own
- Where: `cli/pyproject.toml`
- Cause: `sl` is PowerShell's built-in alias of `Set-Location`
- Recognise: the prompt moves to a folder named `build`
- Workaround: run `sushilog build`
- Fix: none, `sushilog` is the same command under its full name
