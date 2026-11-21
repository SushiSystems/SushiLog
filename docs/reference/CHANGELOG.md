# Changelog

## Unreleased

## v0.5.0 — 2026-11-21

- 2026-11-21 — cli: Breaking change: `sl build` refuses a tree of another generator or source folder and names `sl clean`; a build never removes a tree (RW-28, `09a47bd`).
- 2026-11-21 — cli: Breaking change: Removed `--clean` from build and test, and added `sl clean`, the shared command, which removes trees only with `--yes` (RW-27, `61e4789`).
- 2026-11-21 — cli: Added `sl run`, which runs a built program by name or picks one of the built examples, each with its description; `--all` lists every program (RW-26, `0af906b`).
- 2026-11-16 — tools: Took the shared tools from SushiCore v0.14.1, which leaves out its two own checkers and writes this repository's licence header, through `hub bump` (`tools/shared_tools.lock`).
- 2026-11-16 — tools: Took the shared tools from SushiCore v0.14.0 into a lock file and moved the SushiCore pin to 0.14 (`tools/shared_tools.lock`).
- 2026-11-11 — cli: Required sushicore 0.13, whose test command fails a run of a current tree that breaks a time limit, and told it whether the tree is current.
