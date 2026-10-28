# RW-3 Add a log-once and a rate limit per call site

The row of this entry is in `docs/backlog/LOGGING_INTERFACE.md`. This file holds what did not fit the row.

- State: open
- Done when: a call site can log once per key and at most once per interval, and the runtime's `RulePlacement::is_first_warning` and `is_logged_attempt` throttles can be deleted for it
