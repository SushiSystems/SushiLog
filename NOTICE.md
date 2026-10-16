# Notices

SushiLog is Copyright (c) 2026-present Mustafa Garip & Sushi Systems and licensed under the
PolyForm Noncommercial License 1.0.0; see `LICENSE`.

SushiLog holds no third-party code. No source is vendored in this repository, no file is ported
from third-party code, and the library links the C++ standard library only.

## Third-party components

| Component | Where | Source | Version | Licence |
| --- | --- | --- | --- | --- |
| GoogleTest | Tests only, never installed; `cmake/SushiLogTests.cmake` finds an installed one or fetches the release | https://github.com/google/googletest | v1.14.0, archive SHA-256 `8ad598c73ad796e0d8280b082cebd82a630d73e73cd3c70057938a6501bba5d7` | BSD-3-Clause; not read from the archive in this change |
| sushicore | `sl` command line; `cli/pyproject.toml` | https://github.com/SushiSystems/SushiCore | `==0.9.0` | A Sushi Systems package under its own licence |
| typer | `sl` command line; `cli/pyproject.toml` | https://pypi.org/project/typer/ | `>=0.12` | MIT; not read in this change |
| rich | `sl` command line; `cli/pyproject.toml` | https://pypi.org/project/rich/ | `>=13.0` | MIT; not read in this change |

## Redistributed binaries

The install ships SushiLog's own binaries and nothing else: `sushilog.dll` under `bin/` on
Windows, `libsushilog.so` or `sushilog.lib` and the static `sushilog_static` library under
`lib/`.
