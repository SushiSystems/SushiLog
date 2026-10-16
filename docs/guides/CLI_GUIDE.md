# CLI guide

`sl` builds, tests and checks SushiLog. Install it from the checkout:

```bash
pip install -e ./cli
```

Every build and test goes through `sl`; do not call CMake, Ninja or CTest yourself.

In PowerShell `sl` is the built-in alias of `Set-Location`; type `sushilog`, the same command
under its full name, there.

## sl build

Configures the tree when it needs it, then builds the shared and the static library, the
tests and the header checks.

```bash
sl build                      # the type the tree carries, else debug
sl build --type release
sl build --sanitize address   # ASan and UBSan, in build-asan/
sl build --sanitize thread    # ThreadSanitizer, in build-tsan/
sl build -D SL_BUILD_TESTS=OFF
sl build --clean
```

## sl test

Builds when needed, then runs the tests. The quarantined ones are left out.

```bash
sl test                                # every size
sl test --label small --label medium   # what a pull request runs
sl test --filter 'Small_LogLevels.*'   # ctest -R over Suite.Case
sl test --jobs 4                       # four tests at once
sl test --sanitize thread --label medium
sl test --package                      # also the consumer of the installed package
```

`--package` installs the tree under `build/package-prefix/`, builds `tests/package/` against it
and runs its two programs, one per library target.

## sl check

Runs the nine shared checkers under `tools/`.

```bash
sl check
sl check --report                     # counts per rule, exit 0
sl check --rule rule_console_output   # one rule
```

## Choosing the compiler

`sl` builds with `cxx` from `cli/config.local.toml` or `SL_CXX`, else a clang++ on PATH, else
CMake's platform default. On Windows a clang++ needs the Visual Studio environment: set
`vs_vcvars` to `vcvars64.bat`, or run `sl` from a developer prompt with `SL_CXX=cl`.
