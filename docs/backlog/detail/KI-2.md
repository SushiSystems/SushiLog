# KI-2 On Windows the logger's thread can end inside a sink call at exit

The row of this entry is in `docs/backlog/SHUTDOWN.md`. This file holds what did not fit the row.

- Origin: own
- Where: `modules/foundation/log/source/sink_holder.cpp`, `tests/foundation/log/test_log_calls/test_exit.cpp`
- Cause: Windows ends every thread but the main one before SushiLog's statics are destroyed
- Recognise: at exit the records still queued are missing, and later records reach stderr
- Workaround: none
- Fix: none, the system ends the thread before any SushiLog code runs at exit
