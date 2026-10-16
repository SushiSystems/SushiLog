# Known issues

Defects that are recorded and not yet fixed. The work each one needs is in
[`REMAINING_WORK.md`](../design/REMAINING_WORK.md).

| Issue | Where |
| --- | --- |
| A process ended without static destruction, such as by `_exit` or a crash, loses the records still queued | `modules/foundation/log/source/logger.cpp` |
| On Windows the system ends the logger's thread before SushiLog's statics are destroyed; when it ends inside a sink call, the records still queued are lost and later records go to stderr | `modules/foundation/log/source/sink_holder.cpp`, `tests/foundation/log/test_log_calls/test_exit.cpp` |
