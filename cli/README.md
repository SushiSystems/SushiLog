# cli

The `sl` command line interface. `sushilog/cli.py` parses the commands `build`, `test` and `check`,
registers the shared `package` command, and `sushilog/services/project.py` does the work. The `docs` group, with its one command
`bundle`, the config loader, the console, the process runner and the CMake driver come from
sushicore 0.9.2. `tests/` is the pytest suite:
`cd cli && python -m pytest tests -q`.

## config.py

Configuration is layered, lowest precedence first: built-in defaults, `config.toml`,
`config.local.toml`, then `SL_*` environment variables such as `SL_CXX`. SushiLog pins no
toolchain: `sl` builds with the pinned `cxx`, else a clang++ on PATH, else CMake's platform
default. A found compiler with a found ninja takes the Ninja generator; with no compiler CMake
chooses its own.

## Package contract

SushiCore builds a SushiLog package for a consumer by running `sl build --type <type>
--build-dir build/pkg-<id> --lean` and then `sl package --build-dir build/pkg-<id> --prefix <p>
--no-archive --force` in this checkout. `--build-dir` names the tree below the checkout, and
`--lean` builds no tests (`SL_BUILD_TESTS=OFF`); SushiLog has no examples or benchmarks. `build`
always passes `SL_BUILD_TESTS`, so a tree switches between the two by reconfiguring. `package` is
sushicore's shared command. `SUSHISTACK_CXX` names a compiler that outranks the configured one.
`cli/sushistack.deps.toml` names this CLI in `[module] cli`.

## console.py

The console is built on first use, through sushicore's `LazyConsole`. Building it needs this
repository's `cli/` folder, so building it at import time would make `sl --help` fail outside a
checkout.

## env.py

With a compiler found, cmake and ctest run under the current environment, with the compiler's
folder first on PATH and, on Windows with `vs_vcvars` set, the Visual Studio environment that
`vcvars64.bat` produces, cached in the build tree.

## services/project.py

Each build type and each sanitizer has its own tree, `build/<name>-<type>/`: `default-debug`,
`default-release`, `asan-debug`, `tsan-debug`. Debug is the default type. A configure of a tree
with another type is refused, so switching never rebuilds a tree. Every configure sets the memory
gate of sushicore as the compiler and linker launcher, and a compile runs the hardware threads
unless `--jobs` says otherwise; `--report` prints the time and memory peak per kind of edge.
Another path is `--build-dir`. `sl test` leaves the quarantined tests out. `sl test --package`
installs the tree, builds `tests/package/` against the install and runs its two tests. `sl check`
runs the nine shared checkers and, with `--rule`, only those whose `--list-rules` names the rule.
