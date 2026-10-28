# tests/package

An out-of-tree consumer of an installed SushiLog. It finds the package with `find_package` and
nothing else, refuses a target that hands it SushiLog's warnings or sanitizer, and builds
`consumer.cpp` twice: against `SushiLog::SushiLog` and against `SushiLog::SushiLogStatic`. Each
program logs one record through a C++ sink and one through the C interface, and fails unless
both arrive. On Windows the shared program gets `sushilog.dll` beside it after each link.

`sl test --package` installs the build tree under `build/<tree>/package-prefix/`, builds this project
in `build/<tree>/package/` against it and runs the two programs as tests.
