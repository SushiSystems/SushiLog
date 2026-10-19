# Backlog migration: inventory

**Status:** Shipped — 2026-10-19. The owner approved the inventory as proposed.

The migration of this repository to the backlog format of SushiSkills'
`skills/documentation/BACKLOG_FORMAT.md`, step 2 of section 8 of SushiSkills'
`docs/agent/2026_10_19_BACKLOG_FORMAT/SPEC.md`. Every entry was read against the code and
`git log` on 2026-10-19.

## Shared tools

`copy_shared_tools.py` brought fourteen files up to date and added `check_backlog.py`,
`design_status.py`, their tests and the two community files under `.github/`. None of the files
it replaced held a local edit: each matched an earlier SushiSkills version. After the copy,
`check_all.py` fails only on what this migration converts.

## Backlog

| Entry today | Verdict | Evidence | Proposal |
| --- | --- | --- | --- |
| The first build and test run | Closed | Seven releases, v0.1.0 to v0.2.3; the CI and Release runs of v0.2.3 passed on 2026-10-18 | Delete |
| Tag v0.1.0 | Closed | `v0.1.0` is on the remote, at `f2916a5` | Delete |
| The CI's Windows job takes `ilammy/msvc-dev-cmd@v1` by tag | True | `.github/workflows/ci.yml` line 75 | `RW-1`, open, area CI |

## Known issues

`logger.cpp`, `sink_holder.cpp` and the `sl` script have no commit since the known issues were
last written (`3a7e113`).

| Issue today | Verdict | Proposal |
| --- | --- | --- |
| A process ended without static destruction loses the records still queued | True. `WriterLifetime` drains the queue only when the statics die | `KI-1`, origin own. Its `Fix:` is the owner's: `none, <reason>` if this is the design, or a new backlog entry |
| On Windows the system ends the logger's thread before the statics die | True. `test_exit.cpp` covers the stderr fallback | `KI-2`, origin own; `Fix:` as for `KI-1` |
| In PowerShell `sl` is `Set-Location` | True. `cli/pyproject.toml` still installs `sl` beside `sushilog` | `KI-3`, origin own, `Fix: none, sushilog is the same command under its full name` |

The intro of `KNOWN_ISSUES.md` says the work each issue needs is in the backlog. None of the
three has a backlog entry today.

## Other files

| File | Finding | Proposal |
| --- | --- | --- |
| `docs/agent/2026_10_16_INITIAL_IMPLEMENTATION/PLAN.md` | Status "written; the build and the tests wait", in no known form; the work shipped as v0.1.0 | `**Status:** Shipped — 2026-10-17` |
| `README.md` | Holds a title and prose, not the six sections `documentation` gives the root README; there is no `## Status` | A new backlog entry, `RW-2`; not changed in this migration |
| `docs/design/` | No design document, so nothing for `rule_design_coverage` | None |
| Roadmap, journal, handover, milestone or priority files | None | None |
| Foreign entries | None after the two closed entries go | None |

## Outcome

The owner approved every proposal on 2026-10-19 and ruled `KI-1` and `KI-2` limits of the
design, so each carries `Fix: none, <reason>`. The backlog holds `RW-1` and `RW-2`; the known
issues hold `KI-1` to `KI-3`. `tools/README.md`, which this repository keeps, took the shared
rows of SushiSkills' copy, the backlog checker among them.

```
$ python tools/check_all.py .
clean: 10 checkers, tests
```
