# KI-3 In PowerShell `sl build` changes folder instead of building

The row of this entry is in `docs/backlog/COMMAND_LINE.md`. This file holds what did not fit the row.

- Origin: own
- Where: `cli/pyproject.toml`
- Cause: `sl` is PowerShell's built-in alias of `Set-Location`
- Recognise: the prompt moves to a folder named `build`
- Workaround: run `sushilog build`
- Fix: none, `sushilog` is the same command under its full name
