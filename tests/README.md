# tests

The tests mirror `modules/`: the test of `modules/<tier>/<module>/include/SushiLog/<module>/<name>.hpp`
is `tests/<tier>/<module>/test_<name>.cpp`. Each module's tests build one program,
`sl_<module>_tests`, through `sushilog_add_module_tests` in `cmake/SushiLogTests.cmake`.

## Sizes

A test's size is the prefix of its GoogleTest suite. The build refuses a suite without one.

| Size | The test may use | Timeout |
| --- | --- | --- |
| `Small_` | One process and one thread; synchronous delivery, no stderr capture | 60 s |
| `Medium_` | Threads, the logger's thread, stderr captured through a temporary file, death tests | 300 s |
| `Large_` | Nothing in SushiLog needs it | 900 s |

`sl test --label small` runs one size; `sl test --package` also builds `tests/package/`, an
out-of-tree consumer of the installed package.
