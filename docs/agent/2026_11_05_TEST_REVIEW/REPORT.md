# Test review: report

**Status:** Open — every test has a verdict; the plan's jobs are next

Every test in the repository has a verdict from `skills/testing/REVIEWING.md` of SushiSkills, reviewed
on 2026-11-05 as part of J15 (`docs/agent/2026_11_03_INDUSTRIAL_TESTING/` in SushiSkills). The review read
the code and ran nothing. Each record names the files it read. The tests of the shared checkers under
`tools/` are reviewed once, in SushiSkills; this report covers this repository's own tests and its own
checkers' tests.

## What the review found

- **The CLI's `project` fixture** (`cli/tests/conftest.py`, lines 113 to 120) replaces
  `find_project_root`, `load_config`, `load_build_env` and the console, all repository-owned functions.
  The project service reads them from module functions, so a seam that passes them in comes first; with
  it, the tests use sushicore's in-memory runner and console.
- **Small tests reach the process-wide logger and the environment** (25 rewrites at question 1), and
  medium tests keep synchronous delivery on. SushiLog is the logger every repository uses, so the sink
  seam it needs is also the one sushitrack's `CapturedLog` needs.
- `test_levels.cpp` depends on order: `K_GRAPH_LOG_CATEGORY` is pinned by its siblings, and
  `set_project` skips pinned slots (`level_table.cpp`).

## Defects the review found

- `log_calls.hpp` documents a WARN for an unseen project; `level_table.cpp` writes none, and a test
  asserts none.
- `tests/README.md` defines `Small_` as 60 s and allows the process-wide logger; the testing skill says
  1 s and forbids it.
- `tools/testing/check_test_additions.py` and its test are a leftover: SushiSkills removed the checker in
  phase B, and the copier never removes a file upstream dropped. Both are deleted.
- `tools/tests/test_test_budget_settings.py` skips here, since the repository has no `skills/` folder, so
  its promise is checked nowhere; it also defines `read_budget` and `differences` in the test file.

## Decisions

The owner delegated these decisions on 2026-11-05. Each ruling below is final for the plan; its reason follows it.

1. The unseen-project WARN: the header changes. Levels are often set from configuration before any logger of that project registers, so a warning there would fire on correct start-up. The test that asserts no WARN stays.
2. SushiLog's seam is a scoped in-memory sink: a guard that swaps the process-wide logger's sink for one test and restores it on exit. Routing a logger through every call site in every repository would cost more than it returns, and small tests run one at a time in their process. The in-memory sink is public, and every repository's log captures move onto it.
3. The size timeouts follow the testing skill: 1 s, 10 s and 120 s. The skill is the one home of these numbers; a measurement on the owner's machine changes the skill, not a repository.

## Dispatches

dispatch: J15 review, 3 chunks — haiku high, 1 round each, passed, rung 1

## Totals

| Tests | Keep | Rewrite | Resize | Split | Merge | Delete | Characterize | Add |
| --- | --- | --- | --- | --- | --- | --- | --- | --- |
| 136 | 27 | 80 | 5 | 5 | 0 | 19 | 0 | 14 |

Kept as they stand: 27 of 136 (19%).

## Verdicts by question

| Verdict | Question | What it means | Tests |
| --- | --- | --- | --- |
| rewrite | 1 | a forbidden kind | 70 |
| delete | 1 | a forbidden kind | 12 |
| rewrite | 8 | wrong size | 9 |
| split | 3 | no single promise | 5 |
| resize | 8 | wrong size | 5 |
| delete | 3 | no single promise | 5 |

## Modules

| Module | Files | Tests | Keep | Rewrite | Resize | Split | Merge | Delete | Characterize |
| --- | --- | --- | --- | --- | --- | --- | --- | --- | --- |
| `foundation` | 20 | 64 | 10 | 40 | 2 | 0 | 0 | 12 | 0 |
| `cli/services` | 1 | 22 | 1 | 21 | 0 | 0 | 0 | 0 | 0 |
| `cli` | 6 | 27 | 8 | 15 | 1 | 1 | 0 | 2 | 0 |
| `tools` | 2 | 9 | 0 | 1 | 2 | 1 | 0 | 5 | 0 |
| `core` | 2 | 12 | 8 | 1 | 0 | 3 | 0 | 0 | 0 |
| `package` | 1 | 2 | 0 | 2 | 0 | 0 | 0 | 0 | 0 |

## Records

One line per test: `file::test | verdict | question | reason | files read`.

### `cli/tests/services/test_project.py`

```
test_build_configures_an_empty_tree_then_compiles | rewrite | 1 | project: build configures an empty tree, then compiles it; forbidden: the project fixture (conftest.py:113-120) replaces find_project_root, load_config, load_build_env and console, repository-owned functions | cli/tests/services/test_project.py; cli/tests/conftest.py; cli/sushilog/services/project.py; cli/sushilog/config.py; cli/sushilog/env.py
test_build_leaves_a_configured_tree_alone | rewrite | 1 | project: a configured tree is compiled without reconfiguring; forbidden: project fixture | same as above
test_build_with_a_define_reconfigures | rewrite | 1 | project: a define reconfigures a configured tree; forbidden: project fixture | same as above
test_build_with_a_sanitizer_uses_its_own_tree | rewrite | 1 | project: a sanitizer builds in its own tree with SL_SANITIZE set; forbidden: project fixture | same as above
test_test_passes_the_sizes_the_jobs_and_the_filter_to_ctest | rewrite | 1 | project: test runs the sizes, filter, jobs and JUnit output through ctest; forbidden: project fixture | same as above
test_test_without_a_size_runs_small_and_medium_and_refuses_an_unfiltered_large_run | rewrite | 1 | project: test with no size runs small and medium, and the large refusal is a second promise (split when rewritten); forbidden: project fixture | same as above
test_test_with_package_installs_then_builds_and_runs_the_consumer | rewrite | 1 | project: test with package installs, builds and runs tests/package; forbidden: project fixture | same as above
test_check_runs_every_checker_with_the_root | rewrite | 1 | project: check runs every checker with the root; forbidden: project fixture; the length check repeats the K_CHECKERS constant | same as above
test_check_with_a_rule_runs_only_the_checkers_that_list_it | rewrite | 1 | project: check with a rule runs only the checkers that list it; forbidden: project fixture and a replacement of project_service.rules_of (test_project.py:126) | same as above
test_check_with_an_unknown_rule_fails_and_says_so | rewrite | 1 | project: check with an unknown rule returns 1 and says so; forbidden: project fixture and a replacement of rules_of (test_project.py:137) | same as above
test_rules_of_reads_the_list_rules_flag | keep | 9 | project: rules_of reads the --list-rules output; no stub, a real checker script in tmp_path, one process, so medium | cli/tests/services/test_project.py; cli/sushilog/services/project.py
test_the_default_build_type_is_debug | rewrite | 1 | project: the default build type is debug; forbidden: project fixture, and the K_DEFAULT_TYPE assertion checks a default value | same as above
test_each_build_type_has_its_own_tree[build_type] | rewrite | 1 | project: each build type has its own tree; forbidden: project fixture, used only for the root; without it the case is small | cli/tests/services/test_project.py; cli/tests/conftest.py; cli/sushilog/services/project.py
test_a_sanitizer_tree_is_named_by_sanitizer_and_type | rewrite | 1 | project: a sanitizer tree is named by sanitizer and type; forbidden: project fixture, used only for the root | same as above
test_an_explicit_build_dir_overrides_the_named_tree | rewrite | 1 | project: an explicit build dir overrides the named tree; forbidden: project fixture, used only for the root | same as above
test_a_tree_of_another_type_is_not_configured | rewrite | 1 | project: a tree configured as another type is refused and nothing runs; forbidden: project fixture | same as above
test_a_define_naming_the_build_type_is_refused | rewrite | 1 | project: a define naming the build type is refused; forbidden: project fixture | same as above
test_a_stopped_gate_refuses_the_build | rewrite | 1 | project: a stopped gate refuses the build and nothing runs; forbidden: project fixture | same as above
test_a_configure_carries_both_gate_launchers | rewrite | 1 | project: a configure carries both compiler and linker launchers; forbidden: project fixture; the oracle is build_admission.launcher_defines, the function build calls (Q6 too) | same as above
test_a_compile_gets_the_hardware_threads_without_jobs | rewrite | 1 | project: a compile uses the hardware threads unless jobs are given; forbidden: project fixture; the oracle is build_admission.hardware_threads(), the function build calls (Q6 too) | same as above
test_a_compile_gets_the_jobs_the_user_gave | rewrite | 1 | project: a compile uses the jobs given; forbidden: project fixture | same as above
test_a_report_prints_after_the_compile | rewrite | 1 | project: a report prints after the compile and writes build_report.json; forbidden: project fixture | same as above
```

### `cli/tests/test_cli.py`

```
test_help_lists_the_three_commands | delete | 2 | cli: sl --help lists build, test, check and the docs group; the root help text names build, test and check (cli.py:46), so a removed command still passes; each command's own invoke test covers existence | cli/tests/test_cli.py; cli/sushilog/cli.py
test_build_passes_type_sanitizer_and_defines | rewrite | 1 | cli: sl build hands its options to the service; forbidden: the calls fixture (test_cli.py:25-34) replaces cli.project_service.build, a function the repository owns, and no other test reaches this promise | cli/tests/test_cli.py; cli/sushilog/cli.py; cli/sushilog/services/project.py
test_build_defaults_to_debug_and_forwards_jobs_and_report | rewrite | 1 | cli: sl build defaults to the debug type; forbidden: calls fixture replaces the service | cli/tests/test_cli.py; cli/sushilog/cli.py; cli/sushilog/services/project.py
test_test_passes_labels_jobs_filter_and_package | rewrite | 1 | cli: sl test hands sizes, jobs, filter, repeat and package to the service; forbidden: calls fixture | cli/tests/test_cli.py; cli/sushilog/cli.py; cli/sushilog/services/project.py
test_test_refuses_zero_jobs | rewrite | 1 | cli: sl test refuses --jobs 0 before the service runs; forbidden: calls fixture; exit_code != 0 accepts any failure, not only the jobs refusal, so the rewrite asserts the usage error and that it names --jobs | cli/tests/test_cli.py; cli/sushilog/cli.py
test_check_passes_report_and_rule | rewrite | 1 | cli: sl check hands report and rule to the service; forbidden: calls fixture | cli/tests/test_cli.py; cli/sushilog/cli.py; cli/sushilog/services/project.py
test_help_lists_the_docs_group | delete | 4 | cli: sl --help lists the docs group; test_docs_bundle_asks_for_a_release in the same file invokes docs bundle, which a missing group fails, so the same promise is checked there | cli/tests/test_cli.py; cli/sushilog/cli.py
test_docs_bundle_asks_for_a_release | keep | 9 | cli: sl docs bundle requires --release and exits 2 naming it; an optional --release would exit 0 and fail; the required option refuses before the command body (sushicore docs_bundle/commands.py:75), so no project lookup runs | cli/tests/test_cli.py; cli/sushilog/cli.py; sushicore docs_bundle/commands.py
test_a_failing_service_becomes_the_exit_code | rewrite | 1 | cli: a service's exit code becomes the process exit code; forbidden: lambda replaces cli.project_service.check (test_cli.py:103); no other test checks it with the real service | cli/tests/test_cli.py; cli/sushilog/cli.py
```

### `cli/tests/test_config.py`

```
test_a_pinned_compiler_wins | keep | 9 | config: a pinned compiler wins over clang++ on PATH | cli/tests/test_config.py; cli/sushilog/config.py
test_without_a_pin_the_clang_on_path_is_used | keep | 9 | config: without a pin, the clang++ on PATH is used | cli/tests/test_config.py; cli/sushilog/config.py
test_without_any_compiler_cmake_chooses | keep | 9 | config: with no compiler found, compiler and generator stay empty for CMake; one outcome, two returned values | cli/tests/test_config.py; cli/sushilog/config.py
test_a_found_compiler_and_ninja_choose_ninja | split | 3 | two results that can break apart: the Ninja choice (config: a found compiler and ninja choose Ninja) and is_multi_config false (config: Ninja is single-config); split into two tests | cli/tests/test_config.py; cli/sushilog/config.py
test_a_visual_studio_generator_is_multi_config | keep | 9 | config: a Visual Studio or Xcode generator is multi-config | cli/tests/test_config.py; cli/sushilog/config.py
```

### `cli/tests/test_console.py`

```
test_importing_the_console_builds_nothing | rewrite | 1 | console: importing the console builds nothing; forbidden: reads the private name _lazy and its built property (console.py:17), and reloads the module, which replaces a process-wide instance; no public signal is used | cli/tests/test_console.py; cli/sushilog/console.py; sushicore cli_console.py (LazyConsole.built)
test_help_works_outside_a_checkout | resize | 8 | console: sl --help works outside a checkout; the help page builds the console (sushicore typer_help.py:63-91) and that console resolves config files from the working directory (cli_console.py:66-72), so the test reaches the file system; medium, not small | cli/tests/test_console.py; cli/sushilog/console.py; cli/sushilog/config.py
```

### `cli/tests/test_env.py`

```
test_without_a_compiler_the_environment_is_a_copy | rewrite | 1 | env: without a compiler the environment is a copy; forbidden: monkeypatch replaces Config.resolved_compiler (test_env.py:21), a method the repository owns; rewrite with a PATH that holds no clang++ | cli/tests/test_env.py; cli/sushilog/env.py; cli/sushilog/config.py
test_a_found_compiler_comes_first_on_path | rewrite | 1 | env: a compiler path's folder comes first on PATH; forbidden: replaces Config.resolved_compiler (test_env.py:31); the test also writes into tmp_path (env.py:50), so it is medium once rewritten | cli/tests/test_env.py; cli/sushilog/env.py; cli/sushilog/config.py
```

### `cli/tests/test_main.py`

```
test_the_module_entry_prints_the_help | keep | 9 | cli: python -m sushilog prints the CLI help and exits zero; one process, which medium allows | cli/tests/test_main.py; cli/sushilog/__main__.py; cli/sushilog/cli.py
```

### `cli/tests/test_package_contract.py`

```
test_the_build_command_forwards_the_tree_and_lean | rewrite | 1 | cli: sl build hands --build-dir and --lean to the service; forbidden: lambda replaces cli.project_service.build (test_package_contract.py:38) | cli/tests/test_package_contract.py; cli/sushilog/cli.py
test_a_lean_build_configures_the_named_tree_without_tests | rewrite | 1 | package: a lean build configures the named tree with tests off; forbidden: the project fixture replaces find_project_root, load_config, load_build_env and console (conftest.py:113-120) | cli/tests/test_package_contract.py; cli/tests/conftest.py; cli/sushilog/services/project.py
test_a_default_build_asks_for_the_tests | rewrite | 1 | package: a default build asks for the tests; forbidden: project fixture | cli/tests/test_package_contract.py; cli/tests/conftest.py; cli/sushilog/services/project.py
test_a_lean_build_reconfigures_a_tree_that_has_tests | rewrite | 1 | package: a lean build reconfigures a tree that has tests; forbidden: project fixture; the assertion "-S" in argv holds for any configure | cli/tests/test_package_contract.py; cli/tests/conftest.py; cli/sushilog/services/project.py
test_the_package_command_is_registered_with_its_options | keep | 9 | cli: sl package --help lists the four options | cli/tests/test_package_contract.py; cli/sushilog/cli.py
test_the_compiler_the_environment_names_outranks_the_pinned_one | rewrite | 1 | config: SUSHISTACK_CXX outranks the pinned compiler; forbidden: replaces config._MODULE, the repository's loader (test_package_contract.py:78-79) | cli/tests/test_package_contract.py; cli/sushilog/config.py
test_without_the_variable_the_pinned_compiler_stays | rewrite | 1 | config: without SUSHISTACK_CXX the pinned compiler stays; forbidden: replaces config._MODULE (test_package_contract.py:86-87) | cli/tests/test_package_contract.py; cli/sushilog/config.py
test_the_fragment_names_the_cli | keep | 9 | package: the package fragment names the CLI sl; reads a repository file, so medium | cli/tests/test_package_contract.py; cli/sushistack.deps.toml
```

### `tests/core/text/test_text_format.cpp`

```
Small_TextFormat.EachPlaceholderTakesTheNextArgument | keep | 9 | format: format_text writes each {} with the next argument, in order | tests/core/text/test_text_format.cpp; modules/core/text/include/SushiLog/text/text_format.hpp
Small_TextFormat.DoubledBracesAreWrittenOnce | keep | 9 | format: {{ and }} are written once as literal braces | tests/core/text/test_text_format.cpp; modules/core/text/include/SushiLog/text/text_format.hpp
Small_TextFormat.APlaceholderWithNoArgumentShowsAsAQuestion | keep | 9 | format: a {} with no argument is written as {?} | tests/core/text/test_text_format.cpp; modules/core/text/include/SushiLog/text/text_format.hpp
Small_TextFormat.ArgumentsPastTheLastPlaceholderAreCounted | keep | 9 | format: arguments past the last {} are written as {+N} | tests/core/text/test_text_format.cpp; modules/core/text/include/SushiLog/text/text_format.hpp
Small_TextFormat.AnEightBitIntegerIsANumberAndAnEnumItsValue | split | 3 | two results that can break apart: an 8-bit integer as a number, and a scoped enum without operator<< as its underlying value; split into two tests | tests/core/text/test_text_format.cpp; modules/core/text/include/SushiLog/text/text_format.hpp
Small_TextFormat.ANullCStringIsWrittenAsNull | keep | 9 | format: a null C string is written as (null) | tests/core/text/test_text_format.cpp; modules/core/text/include/SushiLog/text/text_format.hpp
```

### `tests/core/text/test_value_text.cpp`

```
Small_ValueText.FixedWritesThePrecisionAsked | keep | 9 | value_text: fixed writes exactly the precision asked after the point | tests/core/text/test_value_text.cpp; modules/core/text/include/SushiLog/text/value_text.hpp
Small_ValueText.GeneralTakesTheShorterForm | keep | 9 | value_text: general writes the shorter of fixed and scientific, at the precision asked; the three inputs are rows of one promise | tests/core/text/test_value_text.cpp; modules/core/text/include/SushiLog/text/value_text.hpp
Small_ValueText.ScientificWritesThePrecisionAsked | keep | 9 | value_text: scientific writes the precision asked in scientific form | tests/core/text/test_value_text.cpp; modules/core/text/include/SushiLog/text/value_text.hpp
Small_ValueText.HexIsLowerCaseAndZeroPadded | rewrite | 2 | value_text: hex writes lower-case digits zero-padded to the width; neither input has an a-f digit, so a change to upper case still passes, and no other test holds the lower-case promise; the wider-than-width assertion is a second promise (see Adds) | tests/core/text/test_value_text.cpp; modules/core/text/include/SushiLog/text/value_text.hpp
Small_ValueText.RightAndLeftAlignAnyStreamableValue | split | 3 | two results that can break apart: right alignment and left alignment; split, with the left longer-text row added (see Adds) | tests/core/text/test_value_text.cpp; modules/core/text/include/SushiLog/text/value_text.hpp
Small_ValueText.HelpersComposeAndFeedAFormat | split | 3 | two results that can break apart: right over fixed(), and format_text fed by fixed(); split into two tests | tests/core/text/test_value_text.cpp; modules/core/text/include/SushiLog/text/value_text.hpp; modules/core/text/include/SushiLog/text/text_format.hpp
```

### `tests/foundation/log/test_delegate.cpp`

```
Small_Delegate.DefaultAndNullAreEmpty | keep | 9 | Delegate: an empty Delegate holds no callable | tests/foundation/log/test_delegate.cpp; modules/foundation/log/include/SushiLog/log/delegate.hpp
Small_Delegate.CallsTheHeldCallableWithItsArguments | keep | 9 | Delegate: a call invokes the held callable with its arguments and returns its result | tests/foundation/log/test_delegate.cpp; modules/foundation/log/include/SushiLog/log/delegate.hpp
Small_Delegate.HoldsAMoveOnlyCallable | keep | 9 | Delegate: a move-only callable is held and called | tests/foundation/log/test_delegate.cpp; modules/foundation/log/include/SushiLog/log/delegate.hpp
Small_Delegate.MoveLeavesTheSourceEmpty | keep | 9 | Delegate: a move leaves the source empty and the target holding the callable | tests/foundation/log/test_delegate.cpp; modules/foundation/log/include/SushiLog/log/delegate.hpp
Small_Delegate.AssigningNullDestroysTheCallableOnce | keep | 9 | Delegate: assigning nullptr destroys the held callable exactly once | tests/foundation/log/test_delegate.cpp; modules/foundation/log/include/SushiLog/log/delegate.hpp
Small_Delegate.MoveAssignmentDestroysTheOldCallable | keep | 9 | Delegate: move assignment destroys the old callable exactly once and holds the new one | tests/foundation/log/test_delegate.cpp; modules/foundation/log/include/SushiLog/log/delegate.hpp
```

### `tests/foundation/log/test_export.cpp`

```
Small_Export.ExportedFunctionsLinkFromTheSharedLibrary | delete | 1 | forbidden kind: the body is SUCCEED() with no assertion; the #error guard is a compile check that can stay outside a test; the link is exercised by every test that calls the library | tests/foundation/log/test_export.cpp; modules/foundation/log/include/SushiLog/log/export.hpp
```

### `tests/foundation/log/test_log_abi.cpp`

```
Small_LogAbi.LibraryHoldsTheVersionItsHeadersName | delete | 1 | forbidden kind: test of trivial code (a constant compared with the constant the library returns); every front's first use runs check_log_abi with K_ABI_VERSION and abi_version(), so the promise is checked by every log call | tests/foundation/log/test_log_abi.cpp; modules/foundation/log/include/SushiLog/log/log_abi.hpp; modules/foundation/log/source/log_abi.cpp; modules/foundation/log/include/SushiLog/log/log_front.hpp
Small_LogAbi.EqualVersionsPassTheCheck | delete | 1 | forbidden kind: the body is check_log_abi followed by SUCCEED(); equal versions are exercised by every front call's first use | tests/foundation/log/test_log_abi.cpp; modules/foundation/log/include/SushiLog/log/log_abi.hpp; modules/foundation/log/include/SushiLog/log/log_front.hpp
Medium_LogAbi.AMismatchNamesTheProjectBothVersionsAndTheLibraryThenEnds | keep | 9 | Abi: a version mismatch names the project, both versions and the library, then ends the process; the regex matches the message of log_abi.cpp | tests/foundation/log/test_log_abi.cpp; modules/foundation/log/include/SushiLog/log/log_abi.hpp; modules/foundation/log/source/log_abi.cpp
```

### `tests/foundation/log/test_log_c_sink.cpp`

```
Small_LogCSink.ACSinkReceivesEveryFieldWithItsSize | rewrite | 1 | forbidden kind for Small_: reaches the process-wide logger; only test of the C sink's field copy; also leaves sl_set_log_synchronous(1) on. Rewrite at a size the logger allows, with a fixture that restores the delivery mode | tests/foundation/log/test_log_c_sink.cpp; tests/foundation/log/recording_sink.hpp; tests/foundation/log/sample_front.hpp; modules/foundation/log/include/SushiLog/log/log_c_sink.h; modules/foundation/log/source/log_c_sink.cpp
Small_LogCSink.ForwardRecordKeepsTimeAndRenumbersTheThread | delete | 1 | same promise as tests/foundation/log/test_log_calls/test_forward.cpp::Small_LogForward.AForwardedRecordKeepsItsTimeAndCallSite and ...AForwardedRecordGetsThisLoggersThreadAndLine; both reach forwarded_record in forward_sink.cpp; the C++ door owns the promise | tests/foundation/log/test_log_c_sink.cpp; tests/foundation/log/test_log_calls/test_forward.cpp; tests/foundation/log/recording_sink.hpp; modules/foundation/log/source/forward_sink.cpp; modules/foundation/log/source/log_c_sink.cpp
Small_LogCSink.ForwardRecordReadsOnlyTheFieldsItsSizeCovers | rewrite | 1 | forbidden kind for Small_: reaches the process-wide logger; only test of the size rule of sl_forward_record | tests/foundation/log/test_log_c_sink.cpp; tests/foundation/log/recording_sink.hpp; modules/foundation/log/include/SushiLog/log/log_c_sink.h; modules/foundation/log/source/log_c_sink.cpp
Medium_LogCSink.ANullFunctionRestoresStderr | rewrite | 8 | size right and the C null branch is reached by no other test, but the test leaves sl_set_log_synchronous(1) on; the default is asynchronous (log_writer.hpp: synchronous_ = false), so the test leaves state behind; a restoring reset fixes it | tests/foundation/log/test_log_c_sink.cpp; modules/foundation/log/source/log_c_sink.cpp; modules/foundation/log/source/log_writer.hpp
```

### `tests/foundation/log/test_log_calls/test_environment.cpp`

```
Small_LogEnvironment.AVariableSetsEveryModuleAndThenEachPair | rewrite | 1 | forbidden kind for Small_: sets process environment variables; only test of the list grammar; the name joins two results and is a split candidate | tests/foundation/log/test_log_calls/test_environment.cpp; tests/foundation/log/environment_variable.hpp; modules/foundation/log/source/level_environment.cpp; modules/foundation/log/source/level_environment.hpp
Small_LogEnvironment.AVariableIsReadInAnyCaseAndWithSpaces | rewrite | 1 | forbidden kind for Small_: sets process environment variables; only test of case and space handling | tests/foundation/log/test_log_calls/test_environment.cpp; tests/foundation/log/environment_variable.hpp; modules/foundation/log/source/level_environment.cpp
Small_LogEnvironment.AVariableTouchesNoOtherProject | rewrite | 1 | forbidden kind for Small_: sets process environment variables; the check depends on ENVTHEIRS_LOG_LEVEL being unset on the machine (determinism rule 3) | tests/foundation/log/test_log_calls/test_environment.cpp; tests/foundation/log/environment_variable.hpp; modules/foundation/log/source/level_environment.cpp
Small_LogEnvironment.ACallBeforeTheFirstLogCallWinsOverTheVariable | rewrite | 1 | forbidden kind for Small_: sets process environment variables; the name describes an event, the promise is that an explicit set overrides the variable | tests/foundation/log/test_log_calls/test_environment.cpp; tests/foundation/log/environment_variable.hpp; modules/foundation/log/source/level_table.cpp
Medium_LogEnvironment.AnUnreadableValueIsReportedOnceAndLeavesWarn | resize | 8 | the parse and the report-once rule are logic on values (level_environment.cpp, level_table.cpp), reached only through the environment and the process-wide logger; resize to Small_ after a seam that takes the variable's value and returns the setting, with the report sent through an injected sink | tests/foundation/log/test_log_calls/test_environment.cpp; tests/foundation/log/environment_variable.hpp; modules/foundation/log/source/level_environment.cpp; modules/foundation/log/source/level_environment.hpp; modules/foundation/log/source/level_table.cpp
```

### `tests/foundation/log/test_log_calls/test_exit.cpp`

```
Small_LogExit.CallsAreValidDuringStaticInitialisation | rewrite | 1 | forbidden kind for Small_: reaches the process-wide logger during static initialisation; a process-lifecycle promise that belongs to Medium_ | tests/foundation/log/test_log_calls/test_exit.cpp; tests/foundation/log/sample_front.hpp; modules/foundation/log/source/logger.cpp
Medium_LogExit.AThreadEndedInsideTheSinkLosesTheQueue (Windows branch) | rewrite | 1 | expected stderr "^$" is pasted from current output (forbidden kind outside a Characterization_ suite); the promise it pins is a loss of records, which is not a promise; keep only as a Characterization_ pin with an entry the owner tracks (no backlog state read here, rule 9) | tests/foundation/log/test_log_calls/test_exit.cpp; modules/foundation/log/source/logger.cpp; modules/foundation/log/source/log_writer.cpp
Medium_LogExit.ACallAfterTheLoggerStoppedIsDelivered (Windows branch) | delete | 1 | forbidden kind: no assertion, the body is GTEST_SKIP on every Windows run; the promise is the non-Windows branch's, under the same name; a platform limit belongs in a comment or an entry the owner tracks | tests/foundation/log/test_log_calls/test_exit.cpp; modules/foundation/log/source/logger.cpp
Medium_LogExit.StaticDestructionDeliversWhatIsQueued (non-Windows branch) | keep | 9 | records queued at exit are delivered during static destruction, in order; the expected stderr is the message sequence, not current output | tests/foundation/log/test_log_calls/test_exit.cpp; modules/foundation/log/source/log_writer.cpp; modules/foundation/log/source/logger.cpp
Medium_LogExit.ACallAfterTheLoggerStoppedIsDelivered (non-Windows branch) | keep | 9 | a call after the logger stopped reaches the sink's stderr fallback; the expected text is a message the test logs, not current output | tests/foundation/log/test_log_calls/test_exit.cpp; modules/foundation/log/source/logger.cpp; modules/foundation/log/source/log_writer.cpp
```

### `tests/foundation/log/test_log_calls/test_fatal.cpp`

```
Medium_LogFatal.FatalFollowsTheQueueOnTheCallingThread | rewrite | 1 | forbidden kind: control flow in the body, a loop of 50 WARN records builds the input; the promise is one (FATAL follows the queue on the calling thread); rewrite straight line with one WARN and one FATAL | tests/foundation/log/test_log_calls/test_fatal.cpp; tests/foundation/log/sample_front.hpp; modules/foundation/log/source/log_writer.cpp
```

### `tests/foundation/log/test_log_calls/test_forward.cpp`

```
Small_LogForward.AForwardedRecordPassesNoLevelGate | rewrite | 1 | forbidden kind for Small_: reaches the process-wide logger; only test of the forward's pass of no level gate | tests/foundation/log/test_log_calls/test_forward.cpp; tests/foundation/log/recording_sink.hpp; modules/foundation/log/include/SushiLog/log/log_calls.hpp; modules/foundation/log/source/forward_sink.cpp
Small_LogForward.AForwardedRecordKeepsItsTimeAndCallSite | rewrite | 1 | forbidden kind for Small_: reaches the process-wide logger; this file owns forwarded_record's time path (the C copy is deleted above), so it is kept as the owner's promise at a size the logger allows | tests/foundation/log/test_log_calls/test_forward.cpp; tests/foundation/log/recording_sink.hpp; modules/foundation/log/source/forward_sink.cpp
Small_LogForward.AForwardedRecordGetsThisLoggersThreadAndLine | rewrite | 1 | forbidden kind for Small_: reaches the process-wide logger; owner of the thread and text rule for forwarded records | tests/foundation/log/test_log_calls/test_forward.cpp; tests/foundation/log/recording_sink.hpp; modules/foundation/log/source/forward_sink.cpp; modules/foundation/log/source/record_format.cpp
```

### `tests/foundation/log/test_log_calls/test_levels.cpp`

```
Small_LogLevels.ACategoryNothingSetsIsAtWarn | delete | 1 | forbidden kind for Small_ (process-wide logger), and the promise is tested elsewhere: ReadingAnUnseenProjectGivesItsDefaultAndWritesNothing in this file checks the same Warn default | tests/foundation/log/test_log_calls/test_levels.cpp; modules/foundation/log/source/level_table.cpp
Small_LogLevels.SettingAProjectSetsEveryCategoryOfIt | rewrite | 1 | forbidden kind for Small_: reaches the process-wide logger; order-dependent: set_project skips pinned slots (level_table.cpp), and K_GRAPH_LOG_CATEGORY is pinned by ALogCallPassesAtOrAboveItsCategoryLevel and LogAtWritesTheGivenCallSitePastEveryLevel, so the result depends on shuffle order (determinism rule 5) | tests/foundation/log/test_log_calls/test_levels.cpp; modules/foundation/log/source/level_table.cpp
Small_LogLevels.ACategorySetAloneKeepsItsLevelWhenTheProjectIsSet | rewrite | 1 | forbidden kind for Small_: reaches the process-wide logger; order-dependent: pins K_DEVICE_LOG_CATEGORY, which breaks SettingAProjectSetsEveryCategoryOfIt, and K_GRAPH pins from sibling tests break its own GRAPH check (level_table.cpp set_project) | tests/foundation/log/test_log_calls/test_levels.cpp; modules/foundation/log/source/level_table.cpp
Small_LogLevels.SettingAProjectTouchesNoOtherProject | rewrite | 1 | forbidden kind for Small_: reaches the process-wide logger; only test of the project isolation rule | tests/foundation/log/test_log_calls/test_levels.cpp; modules/foundation/log/source/level_table.cpp
Small_LogLevels.AProjectSetBeforeItsFirstCategoryAppliesToIt | rewrite | 1 | forbidden kind for Small_: reaches the process-wide logger; only test of a project set before its first category | tests/foundation/log/test_log_calls/test_levels.cpp; modules/foundation/log/source/level_table.cpp
Small_LogLevels.ReadingAnUnseenProjectGivesItsDefaultAndWritesNothing | rewrite | 1 | forbidden kind for Small_: reaches the process-wide logger; the test asserts no WARN is written, but log_calls.hpp documents one WARN for an unseen project and level_table.cpp writes none; header and test disagree, owner ruling first | tests/foundation/log/test_log_calls/test_levels.cpp; modules/foundation/log/include/SushiLog/log/log_calls.hpp; modules/foundation/log/source/level_table.cpp; modules/foundation/log/source/log_calls.cpp
Small_LogLevels.ALogCallPassesAtOrAboveItsCategoryLevel | rewrite | 1 | forbidden kind for Small_: reaches the process-wide logger; only test of the threshold at delivery; pins K_GRAPH_LOG_CATEGORY for sibling tests | tests/foundation/log/test_log_calls/test_levels.cpp; tests/foundation/log/recording_sink.hpp; modules/foundation/log/source/level_table.cpp
Small_LogLevels.LogAtWritesTheGivenCallSitePastEveryLevel | rewrite | 1 | forbidden kind for Small_: reaches the process-wide logger; only test of log_at's call site and gate bypass; pins K_GRAPH_LOG_CATEGORY for sibling tests | tests/foundation/log/test_log_calls/test_levels.cpp; tests/foundation/log/recording_sink.hpp; modules/foundation/log/include/SushiLog/log/log_calls.hpp; modules/foundation/log/source/log_calls.cpp
```

### `tests/foundation/log/test_log_calls/test_queue.cpp`

```
Medium_LogQueue.AFullQueueDropsCountsAndReportsBeforeTheNextRecord | rewrite | 1 | forbidden kind: control flow in the body, a loop of 4106 records builds the input; the test copies the capacity as the literal 4096 because K_LOG_QUEUE_CAPACITY is in a private header; after the rewrite the full-queue logic (log_queue.cpp) is reached only through threads, a Q8 seam to build next | tests/foundation/log/test_log_calls/test_queue.cpp; tests/foundation/log/recording_sink.hpp; tests/foundation/log/sample_front.hpp; modules/foundation/log/source/log_queue.cpp; modules/foundation/log/source/log_queue.hpp
```

### `tests/foundation/log/test_log_calls/test_record_line.cpp`

```
Small_LogRecordLine.LineHoldsTimeLevelCategoryThreadCallSiteAndMessage | rewrite | 1 | forbidden kind for Small_: reaches the process-wide logger; only test of the whole line shape (test_sink.cpp checks two fragments only) | tests/foundation/log/test_log_calls/test_record_line.cpp; tests/foundation/log/test_log_calls/test_sink.cpp; modules/foundation/log/source/record_format.cpp
Small_LogRecordLine.TimeIsTheSystemClockOfTheCall | rewrite | 1 | forbidden kind for Small_: reads the real system clock and the process-wide logger; only test of the timestamp; takes an injected clock once a seam exists | tests/foundation/log/test_log_calls/test_record_line.cpp; modules/foundation/log/source/record_format.cpp
Small_LogRecordLine.ALineBreakInTheMessageBecomesASpaceInTheLine | rewrite | 1 | forbidden kind for Small_: reaches the process-wide logger; only test of line-break handling in make_record | tests/foundation/log/test_log_calls/test_record_line.cpp; modules/foundation/log/source/record_format.cpp
Small_LogRecordLine.OneThreadKeepsOneNumber | rewrite | 1 | forbidden kind for Small_: reaches the process-wide logger; alone it passes a constant thread number, so only TwoThreadsGetTwoNumbers kills that mutant | tests/foundation/log/test_log_calls/test_record_line.cpp; modules/foundation/log/source/record_format.cpp
Medium_LogRecordLine.TwoThreadsGetTwoNumbers | resize | 8 | distinct numbers for distinct threads is logic over thread identities (thread_number in record_format.cpp), reached only through two real threads; resize to Small_ after a seam that gives thread identities as values | tests/foundation/log/test_log_calls/test_record_line.cpp; modules/foundation/log/source/record_format.cpp
```

### `tests/foundation/log/test_log_calls/test_reentry.cpp`

```
Medium_LogReentry.ARecordLoggedFromInsideTheSinkGoesToStderr | rewrite | 8 | promise and size are right; the test sets synchronous delivery and does not restore the asynchronous default, so it leaves state behind; a restoring fixture fixes it | tests/foundation/log/test_log_calls/test_reentry.cpp; tests/foundation/log/recording_sink.hpp; modules/foundation/log/source/sink_holder.cpp; modules/foundation/log/source/log_writer.cpp
Medium_LogReentry.ASinkThatSetsTheSinkChangesNothing | rewrite | 8 | promise and size are right; leaves synchronous delivery on, see above | tests/foundation/log/test_log_calls/test_reentry.cpp; tests/foundation/log/recording_sink.hpp; modules/foundation/log/source/sink_holder.cpp; modules/foundation/log/source/log_writer.cpp
Medium_LogReentry.ASinkThatSetsTheModeChangesNothing | rewrite | 8 | leaves synchronous delivery on; also asserts only the diagnostic, not that the mode is unchanged | tests/foundation/log/test_log_calls/test_reentry.cpp; modules/foundation/log/source/log_writer.cpp
```

### `tests/foundation/log/test_log_calls/test_sink.cpp`

```
Medium_LogSink.TheDefaultSinkWritesEachLineToStderr | rewrite | 8 | promise and size are right; leaves synchronous delivery on, the default being asynchronous | tests/foundation/log/test_log_calls/test_sink.cpp; modules/foundation/log/source/sink_holder.cpp; modules/foundation/log/source/log_writer.cpp
Medium_LogSink.AnEmptySinkRestoresStderr | rewrite | 8 | promise and size are right; leaves synchronous delivery on | tests/foundation/log/test_log_calls/test_sink.cpp; modules/foundation/log/source/sink_holder.cpp; modules/foundation/log/source/log_writer.cpp
Medium_LogSink.ASinkThatThrowsLosesNothing | rewrite | 8 | promise and size are right; leaves synchronous delivery on | tests/foundation/log/test_log_calls/test_sink.cpp; modules/foundation/log/source/sink_holder.cpp; modules/foundation/log/source/log_writer.cpp
Medium_LogSink.AsynchronousDeliveryRunsOnTheLoggerThread | rewrite | 8 | promise and size are right; the test ends with synchronous delivery set, not the asynchronous default | tests/foundation/log/test_log_calls/test_sink.cpp; modules/foundation/log/source/log_writer.cpp
Medium_LogSink.EnteringSynchronousModeWaitsForTheQueueInOrder | rewrite | 1 | forbidden kind: control flow in the body, a loop logs 100 records and EXPECT_EQ runs inside a loop; rewrite as rows or a fixed straight-line set of records | tests/foundation/log/test_log_calls/test_sink.cpp; tests/foundation/log/recording_sink.hpp; modules/foundation/log/source/log_writer.cpp
Medium_LogSink.SynchronousModeDeliversOnTheCallingThread | rewrite | 8 | promise and size are right; leaves synchronous delivery on | tests/foundation/log/test_log_calls/test_sink.cpp; modules/foundation/log/source/log_writer.cpp; modules/foundation/log/source/sink_holder.cpp
```

### `tests/foundation/log/test_log_category.cpp`

```
Small_LogCategory.HoldsItsProjectAndModuleAtCompileTime | delete | 1 | forbidden kinds: trivial getters and static_assert run as a test; the names are asserted through every record line and level lookup in other tests | tests/foundation/log/test_log_category.cpp; tests/foundation/log/test_log_calls/test_record_line.cpp; tests/foundation/log/test_log_calls/test_levels.cpp; modules/foundation/log/include/SushiLog/log/log_category.hpp
```

### `tests/foundation/log/test_log_front.cpp`

```
Small_LogFront.CeilingIsErrorWhenLoggingIsOff | delete | 1 | forbidden kind: static_asserts plus SUCCEED(), a static_assert run as a test; ceiling(false) is exercised by ACallPastTheCeilingEvaluatesNothing in this file; the values move beside log_level_ceiling as a compile check | tests/foundation/log/test_log_front.cpp; modules/foundation/log/include/SushiLog/log/log_front.hpp
Small_LogFront.ACallPastTheCeilingEvaluatesNothing | rewrite | 1 | forbidden kind for Small_: reaches the process-wide logger; only test of the compile-time ceiling's evaluation rule | tests/foundation/log/test_log_front.cpp; tests/foundation/log/recording_sink.hpp; modules/foundation/log/include/SushiLog/log/log_front.hpp
Small_LogFront.ACallBelowTheCategoryLevelEvaluatesNothing | rewrite | 1 | forbidden kind for Small_: reaches the process-wide logger; the delivery half is tested in test_levels.cpp, the argument-evaluation half only here | tests/foundation/log/test_log_front.cpp; tests/foundation/log/recording_sink.hpp; modules/foundation/log/include/SushiLog/log/log_front.hpp; tests/foundation/log/test_log_calls/test_levels.cpp
Small_LogFront.ACallPastBothGatesIsDeliveredOnce | rewrite | 1 | forbidden kind for Small_: reaches the process-wide logger; the delivery is tested in test_levels.cpp, the evaluated-once half only here | tests/foundation/log/test_log_front.cpp; tests/foundation/log/recording_sink.hpp; modules/foundation/log/include/SushiLog/log/log_front.hpp; tests/foundation/log/test_log_calls/test_levels.cpp
```

### `tests/foundation/log/test_log_level.cpp`

```
Small_LogLevel.NumbersRunFromTraceToOff | keep | 9 | the LogLevel numbers are part of the C interface and never change (log_level.hpp, log_c_sink.h) | tests/foundation/log/test_log_level.cpp; modules/foundation/log/include/SushiLog/log/log_level.hpp; modules/foundation/log/include/SushiLog/log/log_c_sink.h
Small_LogLevel.IsOneByte | rewrite | 1 | forbidden kind: a static_assert run as a test (an EXPECT_TRUE on a type trait); the one-byte size is a compile-time promise of the C interface; move it to a static_assert beside the enum | tests/foundation/log/test_log_level.cpp; modules/foundation/log/include/SushiLog/log/log_level.hpp; modules/foundation/log/include/SushiLog/log/log_c_sink.h
```

### `tests/foundation/log/test_log_project.cpp`

```
Small_LogProject.HoldsItsNameAndVariableAtCompileTime | delete | 1 | forbidden kinds: trivial getters and static_asserts; the name and the level variable are used by every record line and environment test | tests/foundation/log/test_log_project.cpp; tests/foundation/log/test_log_calls/test_environment.cpp; tests/foundation/log/test_log_calls/test_record_line.cpp; modules/foundation/log/include/SushiLog/log/log_project.hpp
```

### `tests/foundation/log/test_log_record.cpp`

```
Small_LogRecord.HoldsEachFieldApart | delete | 1 | forbidden kind: test of trivial code, a plain struct holding the values it is given; every field is read by the record-line and sink tests | tests/foundation/log/test_log_record.cpp; tests/foundation/log/test_log_calls/test_record_line.cpp; modules/foundation/log/include/SushiLog/log/log_record.hpp
```

### `tests/foundation/log/test_log_sink.cpp`

```
Small_LogSink.CallsItsCallableWithTheRecord | delete | 1 | same promise as tests/foundation/log/test_delegate.cpp::Small_Delegate.CallsTheHeldCallableWithItsArguments; LogSink is a Delegate alias | tests/foundation/log/test_log_sink.cpp; tests/foundation/log/test_delegate.cpp; modules/foundation/log/include/SushiLog/log/log_sink.hpp; modules/foundation/log/include/SushiLog/log/delegate.hpp
Small_LogSink.AnEmptySinkIsFalse | delete | 1 | same promise as tests/foundation/log/test_delegate.cpp::Small_Delegate.DefaultAndNullAreEmpty | tests/foundation/log/test_log_sink.cpp; tests/foundation/log/test_delegate.cpp; modules/foundation/log/include/SushiLog/log/log_sink.hpp; modules/foundation/log/include/SushiLog/log/delegate.hpp
```

### `tests/foundation/umbrella/test_SushiLog.cpp`

```
Small_SushiLog.UmbrellaNamesTheLevelsTheSinkAndTheFront | rewrite | 1 | forbidden kind for Small_: reaches the process-wide logger; only test of the umbrella's names, its delivery half is tested in test_levels.cpp; keep the names as a compile check of the header | tests/foundation/umbrella/test_SushiLog.cpp; modules/foundation/umbrella/include/SushiLog/SushiLog.hpp; tests/foundation/log/test_log_calls/test_levels.cpp
Small_SushiLog.UmbrellaLeavesTheCInterfaceOut | rewrite | 1 | forbidden kind: an assertion on a literal compared with itself (local.declared_here against 7); the promise is compile-time (the local struct conflicts if log_c_sink.h is included); keep as a compile check, not a test | tests/foundation/umbrella/test_SushiLog.cpp; modules/foundation/umbrella/include/SushiLog/SushiLog.hpp; modules/foundation/log/include/SushiLog/log/log_c_sink.h
```

### `tests/package/consumer.cpp`

```
Small_Package.LogsThroughTheSharedTarget (CTest; program main) | rewrite | 1 | forbidden kinds: a small label with a 60 s timeout (tests/package/CMakeLists.txt) on a program that loads a shared library and reaches the process-wide logger; control flow in the body (an if decides the exit code); the promise is medium | tests/package/consumer.cpp; tests/package/CMakeLists.txt; modules/foundation/umbrella/include/SushiLog/SushiLog.hpp; modules/foundation/log/include/SushiLog/log/log_c_sink.h
Small_Package.LogsThroughTheStaticTarget (CTest; program main) | rewrite | 1 | same kinds as the shared target; the static target is built from the same source | tests/package/consumer.cpp; tests/package/CMakeLists.txt; modules/foundation/umbrella/include/SushiLog/SushiLog.hpp; modules/foundation/log/include/SushiLog/log/log_c_sink.h
```

### `tools/tests/test_check_test_additions.py`

```
CheckTestAdditionsTest.test_a_commit_at_its_limit_passes_and_one_over_it_is_named | delete | 3 | tools/testing/check_test_additions.py was removed from the shared tools upstream (sushiskills tools/testing holds no such file); the copy in this repository and its test are a leftover | tools/tests/test_check_test_additions.py
CheckTestAdditionsTest.test_a_fix_has_the_smaller_limit | delete | 3 | the same removal from the shared tools; leftover copy | tools/tests/test_check_test_additions.py
CheckTestAdditionsTest.test_a_second_program_needs_the_owners_line | delete | 3 | the same removal from the shared tools; leftover copy | tools/tests/test_check_test_additions.py
CheckTestAdditionsTest.test_the_owners_line_approves_a_second_program | delete | 3 | the same removal from the shared tools; leftover copy | tools/tests/test_check_test_additions.py
CheckTestAdditionsTest.test_a_repository_that_sets_no_base_commit_is_not_checked | delete | 3 | the same removal from the shared tools; leftover copy | tools/tests/test_check_test_additions.py
```

### `tools/tests/test_test_budget_settings.py`

```
TestBudgetSettingsTest.test_the_skill_table_matches_the_settings | rewrite | 1 | forbidden kind: control flow in the body (the skipTest guard); it is the only test of the promise that the skill's Budget table equals K_TEST_BUDGET; this repository has no skills/ folder, so the test always skips and checks nothing here; rewrite with the skip moved into the fixture | tools/tests/test_test_budget_settings.py; tools/settings.py
TestBudgetSettingsTest.test_reads_a_number_with_a_thousands_space_and_a_trailing_note | split | 3 | several promises in one body: a thousands-spaced number parses, a trailing note is dropped, a row no key names is ignored, rows outside the Budget heading are ignored; read_budget is defined in this test file, not in a module, so it needs a home before the split tests can import it | tools/tests/test_test_budget_settings.py
TestBudgetSettingsTest.test_names_each_key_that_differs | resize | 8 | differences() names each differing key in sorted order, None for a missing side; plain values, so small; the file has no size label, which fails collection; resize to small, no seam needed | tools/tests/test_test_budget_settings.py; tools/settings.py
TestBudgetSettingsTest.test_every_row_of_the_table_has_a_constant | resize | 8 | the label map and K_TEST_BUDGET name the same keys; a comparison of values, so small; Q4: the skill-table test checks the same key set at a larger size, so this smaller copy stays; no size label, resize to small | tools/tests/test_test_budget_settings.py; tools/settings.py
```

## Adds

Promises a header or a port states that no test checks: `unit: promise | oracle | size`.

```
cli: sl build --clean hands clean=True to the service | captured service arguments | small
project: build --clean removes the tree before it configures | tree absent before the configure argv | medium
project: check returns the highest checker exit code | return code with runner stubbed per checker | medium
config: without a generator, is_multi_config follows the platform | is_multi_config with platform set | small
env: a bare compiler name leaves PATH unchanged | PATH equal to the environment's | medium
value_text: hex writes lower-case digits | exact string with a-f digits | small
value_text: left writes a value wider than the width whole | exact string | small
format: an enum with operator<< is written through that operator | exact string | small
Delegate: calling an empty Delegate asserts in a debug build | precondition | medium
Delegate: a callable larger than Capacity fails to compile | static_assert | compile check
log_level_ceiling(true) is Trace without NDEBUG and Debug with it | static_assert | compile check
sl_set_log_synchronous(0) returns delivery to the logger's thread | contract (log_c_sink.h) | medium
log_level(project) and set_log_level(project) on an unseen project write one WARN, as log_calls.hpp documents; level_table.cpp writes none; owner ruling needed before a test is written, either the header or the code changes
: none. The rules of check_test_additions are a leftover and are not in scope; the settings promise is tested by tools/tests/test_test_budget_settings.py, and its skill-side half cannot run in this repository. Headers and modules read for this section: tools/tests/test_check_test_additions.py; tools/tests/test_test_budget_settings.py; tools/settings.py.
```
