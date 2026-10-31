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
sl build --clean
```

Each build type and sanitizer has its own tree, and a tree keeps the type it was configured
with: `sl build --type release` never touches the Debug tree. Compiles run behind a memory gate
that admits an edge only when its expected peak fits the free memory. Creating the file
`build/build_gate/stop` makes the gate refuse the build until the file is deleted.

## sl test

Builds when needed, then runs the tests. The quarantined ones are left out. Without a label it runs `small`
and `medium`; `large` needs `--filter` or `--all`. CTest writes a JUnit file under the tree's `Testing` folder,
and a repository that carries the shared budget reader prints one budget line after the run.

```bash
sl test                                # small and medium
sl test --label small --label medium   # what a pull request runs
sl test --filter 'Small_LogLevels.*'   # ctest -R over Suite.Case
sl test --jobs 4                       # four tests at once
sl test --all                          # every size; the large ones need real hardware
sl test --sanitize thread --label medium
sl test --package                      # also the consumer of the installed package
```

`--package` installs the tree under `build/<tree>/package-prefix/`, builds `tests/package/` against it
and runs its two programs, one per library target.

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
