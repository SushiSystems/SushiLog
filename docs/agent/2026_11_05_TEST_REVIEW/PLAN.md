# Test review: plan

**Status:** Open — every job done and every entry it opened closed; the owner closes the folder

Applies the verdicts of `REPORT.md`, in hierarchy order: the shared helpers, adapters and seams the
rewrites need come first, then the defects the review found, then each module's tests. A module job
applies every record of its module: rewrite, resize, split, merge and delete as the record says, and
writes each add. Nothing in a module job changes production code except a seam a job above built.

## Jobs

| Job | Files | Waits on | Status |
| --- | --- | --- | --- |
| T1 | The scoped in-memory sink on the process-wide logger, public |  | done |
| T2 | `cli/sushilog/services/project.py`: root, config and environment passed in, with sushicore's in-memory runner and console | sushicore T1 | done |
| T3 | `tests/README.md` sizes aligned with the skill, and the header or code fix of decision 1 |  | done |
| T4 | `tools/testing/check_test_additions.py`, `tools/tests/test_check_test_additions.py` deleted |  | done |
| M1 | the tests of `cli` (6 files): 15 rewrite, 1 resize, 1 split, 2 delete | T1, T2, T3, T4 | done |
| M2 | the tests of `cli/services` (1 file): 21 rewrite | T1, T2, T3, T4 | done |
| M3 | the tests of `core` (2 files): 1 rewrite, 3 split | T1, T2, T3, T4 | done |
| M4 | the tests of `foundation` (20 files): 40 rewrite, 2 resize, 12 delete | T1, T2, T3, T4 | done; seams RW-19 to RW-23 built, RW-24 done |
| M5 | the tests of `package` (1 file): 2 rewrite | T1, T2, T3, T4 | done |
| M6 | the tests of `tools` (2 files): 1 rewrite, 2 resize, 1 split, 5 delete | T1, T2, T3, T4 | done; budget-settings test deleted (RW-17) |

## Acceptance

- **T1.** A small test checks a log line through the scoped in-memory sink, and the logger's sink is restored on exit. Build: the repository's test command.
- **T2.** The project service's tests use no `monkeypatch` of the repository's own functions. Build: the CLI tests.
- **T3.** The page says what the skill says; the WARN test and the header agree. Build: `python tools/check_all.py .`
- **T4.** Neither file exists and the checkers pass. Build: `python tools/check_all.py .`
- **M jobs.** Every record of the module is applied: no test the report deletes remains, every kept and
  new test passes at the size its label states, and the module's tests pass through the project's own
  test command. Build: that command over the module.

The owner chooses inline or parallel mode.
