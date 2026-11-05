# tests/foundation/log

The tests of the `log` module. `test_log_calls/` holds the tests of `log_calls.hpp`, one file per
behaviour: levels, the level variable, the record line, the sink, the queue, `FATAL`, re-entry,
forwarding and process exit. `test_log_capture.cpp` tests `LogRecorder` and `LogCapture` of
`log_capture.hpp`.

| Helper | Does |
| --- | --- |
| `sample_front.hpp` | `SAMPLE_LOG_*`, a front as a Sushi project writes one, over `SL_LOG_FRONT_AT` |
| `environment_variable.hpp` | `set_environment_variable`, for the level-variable tests |
