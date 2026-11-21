# CLI guide

`sl` builds, tests and checks SushiLog. Install it from the checkout, or through SushiHub as
[`INSTALL.md`](../getting_started/INSTALL.md) describes:

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
sl build                      # Debug, in build/default-debug/
sl build --type release       # build/default-release/
sl build --sanitize address   # ASan and UBSan, in build/asan-debug/
sl build --sanitize thread    # ThreadSanitizer, in build/tsan-debug/
sl build --jobs 4 --report    # four compile jobs; time and memory peaks after
sl build -D SL_BUILD_TESTS=OFF
```

Each build type and sanitizer has its own tree, and a tree keeps the type it was configured
with: `sl build --type release` never touches the Debug tree. Compiles run behind a memory gate
that admits an edge only when its expected peak fits the free memory. Creating the file
`build/build_gate/stop` makes the gate refuse the build until the file is deleted.

## sl test

Builds when needed, then runs the tests. The quarantined ones are left out. Without a label it runs `small`
and `medium`; `large` needs `--filter` or `--all`. The tests run in shards of up to 300 small, 25 medium or 1 large case, and each shard writes its
per-case GoogleTest XML under the tree's `test_results/gtest`. CTest writes a JUnit file under the tree's
`Testing` folder, and a repository that carries the shared budget reader prints one budget line after the run.

```bash
sl test                                # small and medium
sl test --label small --label medium   # what a pull request runs
sl test --filter 'Small_LogLevels.*'   # a GoogleTest filter over Suite.Case
sl test --jobs 4                       # four tests at once
sl test --all                          # every size; the large ones need real hardware
sl test --sanitize thread --label medium
sl test --package                      # also the consumer of the installed package
```

`--package` installs the tree under `build/<tree>/package-prefix/`, builds `tests/package/` against it
and runs its two programs, one per library target.

## sl run

Runs a program the tree already holds. It does not build first; without the tree it names
`sl build` and exits 1.

```bash
sl run                                 # pick one of the built examples
sl run --pick --all                    # pick from every program, tests and benchmarks included
sl run sushilog_tests                  # a program by name: an exact match, then a substring
sl run sushilog_tests -- --gtest_filter='Small_LogLevels.*'   # what follows `--` reaches it
sl run --sanitize thread sushilog_tests
```

A pick prints the examples the build registered under `applications/examples/`, each with the
`@brief` of its source; `--all` lists every program of the tree. With no name, the pick runs.
`--type` and `--sanitize` choose the tree, as they do for `sl build`.

## sl clean

Says which build trees it would remove, with their sizes, and removes them only with `--yes`.
Without it nothing is removed and the exit code is 2, so a script that expected the trees gone
sees that they are not. `sl build` and `sl test` take no `--clean`: a build never removes a tree. One
configured with another generator, or for another source folder, is refused with a message that
names `sl clean`, because CMake cannot change either in place.

```bash
sl clean                              # list every tree, remove none
sl clean --yes                        # remove every tree
sl clean --type debug --yes           # remove build/default-debug only
sl clean --sanitize thread --yes      # remove the trees of one sanitizer
```

`--type` and `--sanitize` are repeatable. `--type` selects the plain tree of that type,
`--sanitize` the trees of that sanitizer, and with neither every tree is selected.

## sl check

Runs the nine shared checkers under `tools/`.

```bash
sl check
sl check --report                     # counts per rule, exit 0
sl check --rule rule_console_output   # one rule
```

## sl docs bundle

Writes the archive `docs.sushisystems.io` reads: `docs-bundle-X.Y.Z.tar.gz` and its `.sha256`
file, in `build/docs/bundle`. `docs/publish.toml` lists what goes in: the manual pages of the
sections it names. It sets `api = false`, so the bundle carries no Doxygen XML.

```bash
sl docs bundle --release 0.2.2
sl docs bundle --release 0.2.2 --out some/folder
```

`--release` is required; without it the command exits 2. A page with no level-one heading, a page
`docs/README.md` does not link, or a link to a file that does not exist stops the command with one
line naming the page and exit code 1. The archive's layout is in SushiCore's
`docs/reference/DOCS_BUNDLE.md`.

The release workflow runs this command on a pushed version tag and attaches the archive to the
tag's GitHub release.

## Choosing the compiler

`sl` builds with `cxx` from `cli/config.local.toml` or `SL_CXX`, else a clang++ on PATH, else
CMake's platform default. On Windows a clang++ needs the Visual Studio environment: set
`vs_vcvars` to `vcvars64.bat`, or run `sl` from a developer prompt with `SL_CXX=cl`.
