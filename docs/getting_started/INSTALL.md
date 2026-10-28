# Install SushiLog

SushiLog installs in two ways. Both give the same checkout and the same `sl` command, and
neither needs the other: SushiHub is not required to build SushiLog, and a checkout made
without it joins a SushiHub workspace later without any change.

## Standalone

You need:

- Git, Python 3.11 or later and CMake 3.21 or later.
- A C++17 compiler. `sl` takes `cxx` from its configuration, else a clang++ on the path, else
  the compiler CMake picks; [`CLI_GUIDE.md`](../guides/CLI_GUIDE.md), "Choosing the compiler",
  has the Windows detail.
- Ninja, optional. `sl` uses it when it finds one.
- Network access once, for the googletest release that the tests fetch. `-D SL_BUILD_TESTS=OFF`
  builds the library without it.

SushiLog needs no sibling repository.

```bash
git clone https://github.com/SushiSystems/SushiLog.git
cd SushiLog
pip install -e ./cli
sl build
sl test --label small --label medium
```

`pip install` also installs `sushicore`, the command-line layer `sl` shares with the other Sushi
modules, from PyPI at the version `cli/pyproject.toml` pins. In PowerShell `sl` is the alias of
`Set-Location`; type `sushilog` there.

## With SushiHub

```bash
hub add sushilog            # clone it into the workspace
hub install-cli sushilog    # put sl on the path
hub link sushilog <path>    # adopt a checkout you already have, instead of hub add
```

`hub link` records where the checkout is and changes nothing in it. After either route, `sl build`
and `sl test` are the commands above.
