# Shutdown

## Remaining work

| ID | Pri | State | Title | Done when | Blocked by | Source | Opened |
| --- | --- | --- | --- | --- | --- | --- | --- |

## Known issues

| ID | Origin | Symptom | Workaround | Fix | Source | Opened |
| --- | --- | --- | --- | --- | --- | --- |
| KI-1 | own | A process that skips static destruction loses the records still queued | none | none, a process that skips static destruction runs no code that could drain the queue |  | 2026-10-19 |
| KI-2 | own | On Windows the logger's thread can end inside a sink call at exit | none | none, the system ends the thread before any SushiLog code runs at exit |  | 2026-10-19 |
