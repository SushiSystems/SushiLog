# test_project.py
# SushiLog - https://github.com/SushiSystems/SushiLog
# Copyright (c) 2026-present Mustafa Garip & Sushi Systems
# Licensed under PolyForm Noncommercial 1.0.0. See LICENSE.
# Commercial use requires a licence from Sushi Systems.
"""Tests the command lines the project service hands cmake, ctest and the checkers.

Every test runs against the `project` fixture, which records argv and starts no process.
"""

from __future__ import annotations

import sys
from pathlib import Path

import pytest
from sushicore import build_admission
from sushicore.errors import ConfigError

from sushilog.services import project as project_service
from sushicore.testing import TestLabel
from sushilog.services.project import BuildType, Sanitizer

pytestmark = pytest.mark.medium


def _is_configure(command: list[str]) -> bool:
    """Returns whether a recorded command is a cmake configure."""
    return "-S" in command


def _is_compile(command: list[str]) -> bool:
    """Returns whether a recorded command is a cmake build."""
    return "--build" in command


def _value_after(command: list[str], flag: str) -> str:
    """Returns the argument that follows *flag* in *command*."""
    return command[command.index(flag) + 1]


def test_build_configures_an_empty_tree_then_compiles(project):
    assert project_service.build(BuildType.RELEASE) == 0

    configure, compile_ = project.runner.commands
    assert _is_configure(configure) and _is_compile(compile_)
    assert Path(_value_after(configure, "-B")) == project.root / "build" / "default-release"
    assert "-DCMAKE_BUILD_TYPE=Release" in configure
    assert "-DSL_SANITIZE=" in configure
    assert "-DCMAKE_CXX_COMPILER=clang++" in configure


def test_build_leaves_a_configured_tree_alone(project):
    project.configure_tree("Debug")

    assert project_service.build() == 0

    assert [_is_compile(command) for command in project.runner.commands] == [True]


def test_build_with_a_define_reconfigures(project):
    project.configure_tree("Debug")

    assert project_service.build(defines=["FOO=1"]) == 0

    configure = project.runner.commands[0]
    assert _is_configure(configure) and _value_after(configure, "-D") == "FOO=1"


def test_build_with_a_sanitizer_uses_its_own_tree(project):
    assert project_service.build(BuildType.DEBUG, sanitizer=Sanitizer.THREAD) == 0

    configure = project.runner.commands[0]
    assert Path(_value_after(configure, "-B")).name == "tsan-debug"
    assert "-DSL_SANITIZE=thread" in configure


def test_test_passes_the_sizes_the_jobs_and_the_filter_to_ctest(project):
    project.configure_tree("Debug")

    assert project_service.test(BuildType.DEBUG, (TestLabel.MEDIUM, TestLabel.SMALL), filter="Small_.*",
                                jobs=4) == 0

    ctest = project.runner.commands[-1]
    assert _value_after(ctest, "-L") == "^(small|medium)$"
    assert _value_after(ctest, "-R") == "Small_.*"
    assert _value_after(ctest, "--parallel") == "4"
    assert _value_after(ctest, "-LE") == "^quarantined$"
    assert "--output-junit" in ctest
    assert _value_after(ctest, "-C") == "Debug"


def test_test_without_a_size_runs_small_and_medium_and_refuses_an_unfiltered_large_run(project):
    project.configure_tree("Debug")

    assert project_service.test() == 0
    assert _value_after(project.runner.commands[-1], "-L") == "^(small|medium)$"

    assert project_service.test(sizes=(TestLabel.LARGE,)) == 2
    assert _is_compile(project.runner.commands[-1])


def test_test_with_package_installs_then_builds_and_runs_the_consumer(project):
    project.configure_tree("Debug")

    assert project_service.test(package=True) == 0

    install, configure, compile_, ctest = project.runner.commands[-4:]
    prefix = _value_after(install, "--prefix")
    assert "--install" in install and Path(prefix).name == "package-prefix"
    assert Path(_value_after(configure, "-S")) == project.root / "tests" / "package"
    assert f"-DCMAKE_PREFIX_PATH={prefix}" in configure
    assert _is_compile(compile_)
    assert Path(_value_after(ctest, "--test-dir")).name == "package"


def test_check_runs_every_checker_with_the_root(project):
    assert project_service.check() == 0

    assert len(project.runner.commands) == len(project_service.K_CHECKERS)
    assert all(command[0] == sys.executable and command[2] == str(project.root)
               for command in project.runner.commands)


def test_check_with_a_rule_runs_only_the_checkers_that_list_it(project, monkeypatch):
    monkeypatch.setattr(project_service, "rules_of",
                        lambda checker: frozenset({"rule_x"}) if "logs" in checker.parts
                        else frozenset())

    assert project_service.check(rule="rule_x") == 0

    (command,) = project.runner.commands
    assert command[1].endswith("check_logging.py") and command[-2:] == ["--rule", "rule_x"]


def test_check_with_an_unknown_rule_fails_and_says_so(project, monkeypatch):
    monkeypatch.setattr(project_service, "rules_of", lambda checker: frozenset())

    assert project_service.check(rule="rule_none") == 1

    assert project.console.of("error") == ["no checker under tools/ has the rule rule_none"]


def test_rules_of_reads_the_list_rules_flag(tmp_path):
    checker = tmp_path / "check_fake.py"
    checker.write_text(
        "import sys\nif sys.argv[1:] == ['--list-rules']:\n    print('rule_a\\nrule_b')\n",
        encoding="utf-8")

    assert project_service.rules_of(checker) == frozenset({"rule_a", "rule_b"})


def test_the_default_build_type_is_debug(project):
    assert project_service.K_DEFAULT_TYPE is BuildType.DEBUG

    assert project_service.build() == 0

    configure = project.runner.commands[0]
    assert "-DCMAKE_BUILD_TYPE=Debug" in configure
    assert Path(_value_after(configure, "-B")) == project.tree


@pytest.mark.parametrize("build_type", list(BuildType))
def test_each_build_type_has_its_own_tree(project, build_type):
    expected = project.root / "build" / f"default-{build_type.value}"

    assert project_service.build_dir(project.root, None, build_type) == expected


def test_a_sanitizer_tree_is_named_by_sanitizer_and_type(project):
    tree = project_service.build_dir(project.root, Sanitizer.ADDRESS, BuildType.RELEASE)

    assert tree == project.root / "build" / "asan-release"


def test_an_explicit_build_dir_overrides_the_named_tree(project):
    tree = project_service.build_dir(project.root, None, BuildType.DEBUG, "build/pkg-x")

    assert tree == project.root / "build" / "pkg-x"


def test_a_tree_of_another_type_is_not_configured(project):
    tree = project.configure_tree("Debug")
    cache = tree / "CMakeCache.txt"
    cache.write_text(cache.read_text(encoding="utf-8").replace("=Debug", "=Release"),
                     encoding="utf-8")

    with pytest.raises(ConfigError, match="Release tree"):
        project_service.build(BuildType.DEBUG)

    assert project.runner.commands == []


def test_a_define_naming_the_build_type_is_refused(project):
    with pytest.raises(ConfigError, match="CMAKE_BUILD_TYPE"):
        project_service.build(defines=["CMAKE_BUILD_TYPE=Release"])


def test_a_stopped_gate_refuses_the_build(project):
    stop = build_admission.stop_file(project.tree)
    stop.parent.mkdir(parents=True)
    stop.write_text("", encoding="utf-8")

    with pytest.raises(build_admission.BuildStoppedError):
        project_service.build()

    assert project.runner.commands == []


def test_a_configure_carries_both_gate_launchers(project):
    assert project_service.build() == 0

    configure = project.runner.commands[0]
    gate = build_admission.launcher_defines(project.tree)
    defines = [configure[i + 1] for i, part in enumerate(configure) if part == "-D"]
    assert any(d.startswith("CMAKE_CXX_COMPILER_LAUNCHER=") for d in defines)
    assert any(d.startswith("CMAKE_CXX_LINKER_LAUNCHER=") for d in defines)
    assert [d.split("=", 1)[0] for d in gate] == [d.split("=", 1)[0] for d in defines
                                                  if "LAUNCHER" in d]


def test_a_compile_gets_the_hardware_threads_without_jobs(project):
    project.configure_tree("Debug")

    assert project_service.build() == 0

    compile_ = project.runner.commands[0]
    assert _value_after(compile_, "--parallel") == str(build_admission.hardware_threads())


def test_a_compile_gets_the_jobs_the_user_gave(project):
    project.configure_tree("Debug")

    assert project_service.build(jobs=3) == 0

    assert _value_after(project.runner.commands[0], "--parallel") == "3"


def test_a_report_prints_after_the_compile(project):
    project.configure_tree("Debug")

    assert project_service.build(report=True) == 0

    assert project.console.of("info")[-1].startswith("Report: ")
    assert (project.tree / "build_report.json").is_file()
