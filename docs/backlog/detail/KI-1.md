# KI-1 A process that skips static destruction loses the records still queued

The row of this entry is in `docs/backlog/SHUTDOWN.md`. This file holds what did not fit the row.

- Origin: own
- Where: `modules/foundation/log/source/logger.cpp`
- Cause: the writer drains its queue when the library's statics are destroyed
- Recognise: records logged just before a crash or an `_exit` never reach the sink
- Workaround: none
- Fix: none, a process that skips static destruction runs no code that could drain the queue
