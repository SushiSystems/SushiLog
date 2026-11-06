# tests

The tests mirror `modules/`: the test of `modules/<tier>/<module>/include/SushiLog/<module>/<name>.hpp`
is `tests/<tier>/<module>/test_<name>.cpp`. Each module's tests build one program,
`sl_<module>_tests`, through `sushilog_add_module_tests` in `cmake/SushiLogTests.cmake`.

## Sizes

A test's size is the prefix of its GoogleTest suite. The build refuses a suite without one.

| Size | The test may use | Timeout |
| --- | --- | --- |
| `Small_` | The memory of its own process and one thread. The one process-wide instance it may reach is the logger, through a `LogCapture` (`SushiLog/log/log_capture.hpp`) that swaps the sink for the test and puts it back on exit. No environment variable, file, thread or real clock | 1 s |
| `Medium_` | Threads, the logger's thread, stderr captured through a temporary file, death tests; whatever it sets, such as the delivery mode, it puts back | 10 s |
| `Large_` | Nothing in SushiLog needs it | 120 s |

The sizes and timeouts are those of the `testing` skill in SushiSkills, the one home of these numbers.

`sl test --label small` runs one size; `sl test --package` also builds `tests/package/`, an
out-of-tree consumer of the installed package.
