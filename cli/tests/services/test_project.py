# test_project.py
# SushiLog - https://github.com/SushiSystems/SushiLog
# Copyright (c) 2026-present Mustafa Garip & Sushi Systems
# Licensed under the Apache License 2.0. See LICENSE.
"""Tests the command lines the project service hands cmake, ctest and the checkers.

The medium tests run against the `project` fixture, a scratch checkout whose workspace records
argv and starts no process. The small tests name build trees from a root and touch nothing.
"""

from __future__ import annotations

import sys
from pathlib import Path

import pytest
from sushicore import build_admission
from sushicore.errors import ConfigError
from sushicore.testing import TestLabel

from sushilog.services import project as project_service
from sushilog.services.project import BuildType, Sanitizer

K_ROOT = Path("checkout")


def _is_configure(command: list[str]) -> bool:
    """Returns whether a recorded command is a cmake configure."""
    return "-S" in command


def _is_compile(command: list[str]) -> bool:
    """Returns whether a recorded command is a cmake build."""
    return "--build" in command


def _value_after(command: list[str], flag: str) -> str:
    """Returns the argument that follows *flag* in *command*."""
    return command[command.index(flag) + 1]


def _defines_of(command: list[str]) -> list[str]:
    """Returns the `-D NAME=VALUE` entries of a configure, without the flag."""
    return [command[i + 1] for i, part in enumerate(command) if part == "-D"]


def _plant_checker(root: Path, checker: Path, rules: str) -> None:
    """Writes a checker at *checker* below *root* that lists *rules* for --list-rules."""
    path = root / checker
    path.parent.mkdir(parents=True, exist_ok=True)
    path.write_text(f"print({rules!r})\n", encoding="utf-8")


@pytest.mark.small
class TestBuildDir:
    """The tree a build type and a sanitizer name."""

    @pytest.mark.parametrize("build_type", list(BuildType))
    def test_each_build_type_has_its_own_tree(self, build_type):
        expected = K_ROOT / "build" / f"default-{build_type.value}"

        assert project_service.build_dir(K_ROOT, None, build_type) == expected

    def test_a_sanitizer_tree_is_named_by_sanitizer_and_type(self):
        tree = project_service.build_dir(K_ROOT, Sanitizer.ADDRESS, BuildType.RELEASE)

        assert tree == K_ROOT / "build" / "asan-release"

    def test_an_explicit_build_dir_overrides_the_named_tree(self):
        tree = project_service.build_dir(K_ROOT, None, BuildType.DEBUG, "build/pkg-x")

        assert tree == K_ROOT / "build" / "pkg-x"


@pytest.mark.medium
class TestBuild:
    """What `build` hands cmake for a scratch checkout."""

    def test_build_configures_an_empty_tree_then_compiles(self, project):
        assert project_service.build(BuildType.RELEASE, workspace=project.workspace) == 0

        configure, compile_ = project.runner.commands
        assert _is_configure(configure) and _is_compile(compile_)
        assert Path(_value_after(configure, "-B")) == project.root / "build" / "default-release"
        assert "-DCMAKE_BUILD_TYPE=Release" in configure
        assert "-DSL_SANITIZE=" in configure
        assert "-DCMAKE_CXX_COMPILER=clang++" in configure

    def test_build_leaves_a_configured_tree_alone(self, project):
        project.configure_tree("Debug")

        assert project_service.build(workspace=project.workspace) == 0

        assert [_is_compile(command) for command in project.runner.commands] == [True]

    def test_build_with_a_define_reconfigures(self, project):
        project.configure_tree("Debug")

        assert project_service.build(defines=["FOO=1"], workspace=project.workspace) == 0

        configure = project.runner.commands[0]
        assert _is_configure(configure) and _value_after(configure, "-D") == "FOO=1"

    def test_build_with_a_sanitizer_uses_its_own_tree(self, project):
        assert project_service.build(BuildType.DEBUG, sanitizer=Sanitizer.THREAD,
                                     workspace=project.workspace) == 0

        configure = project.runner.commands[0]
        assert Path(_value_after(configure, "-B")).name == "tsan-debug"
        assert "-DSL_SANITIZE=thread" in configure

    def test_build_with_clean_removes_the_tree_before_it_configures(self, project):
        tree = project.configure_tree("Debug")

        assert project_service.build(clean=True, workspace=project.workspace) == 0

        assert not (tree / "build.ninja").exists()
        assert _is_configure(project.runner.commands[0])

    def test_build_without_a_type_configures_debug_in_the_default_tree(self, project):
        assert project_service.build(workspace=project.workspace) == 0

        configure = project.runner.commands[0]
        assert "-DCMAKE_BUILD_TYPE=Debug" in configure
        assert Path(_value_after(configure, "-B")) == project.tree

    def test_build_refuses_a_tree_of_another_type_and_runs_nothing(self, project):
        tree = project.configure_tree("Debug")
        cache = tree / "CMakeCache.txt"
        cache.write_text(cache.read_text(encoding="utf-8").replace("=Debug", "=Release"),
                         encoding="utf-8")

        with pytest.raises(ConfigError, match="Release tree"):
            project_service.build(BuildType.DEBUG, workspace=project.workspace)

        assert project.runner.commands == []

    def test_build_refuses_a_define_naming_the_build_type(self, project):
        with pytest.raises(ConfigError, match="CMAKE_BUILD_TYPE"):
            project_service.build(defines=["CMAKE_BUILD_TYPE=Release"],
                                  workspace=project.workspace)

    def test_build_refuses_a_stopped_gate_and_runs_nothing(self, project):
        stop = build_admission.stop_file(project.tree)
        stop.parent.mkdir(parents=True)
        stop.write_text("", encoding="utf-8")

        with pytest.raises(build_admission.BuildStoppedError):
            project_service.build(workspace=project.workspace)

        assert project.runner.commands == []

    def test_build_configures_with_the_gate_as_compiler_and_linker_launcher(self, project):
        assert project_service.build(workspace=project.workspace) == 0

        launchers = {define.split("=", 1)[0]: define.split("=", 1)[1]
                     for define in _defines_of(project.runner.commands[0])
                     if "_LAUNCHER=" in define}
        assert set(launchers) == {"CMAKE_CXX_COMPILER_LAUNCHER", "CMAKE_CXX_LINKER_LAUNCHER"}
        assert all("build_gate" in value for value in launchers.values())

    def test_build_compiles_on_the_stated_parallel_level_without_jobs(self, project, monkeypatch):
        monkeypatch.setenv("CMAKE_BUILD_PARALLEL_LEVEL", "7")
        project.configure_tree("Debug")

        assert project_service.build(workspace=project.workspace) == 0

        assert _value_after(project.runner.commands[0], "--parallel") == "7"

    def test_build_compiles_on_the_jobs_the_user_gave(self, project):
        project.configure_tree("Debug")

        assert project_service.build(jobs=3, workspace=project.workspace) == 0

        assert _value_after(project.runner.commands[0], "--parallel") == "3"

    def test_build_with_report_prints_and_writes_it_after_the_compile(self, project):
        project.configure_tree("Debug")

        assert project_service.build(report=True, workspace=project.workspace) == 0

        assert project.console.of("info")[-1].startswith("Report: ")
        assert (project.tree / "build_report.json").is_file()


@pytest.mark.medium
class TestTest:
    """What `test` hands ctest for a scratch checkout."""

    def test_test_passes_the_sizes_the_jobs_and_the_filter_to_ctest(self, project):
        project.configure_tree("Debug")

        assert project_service.test(BuildType.DEBUG, (TestLabel.MEDIUM, TestLabel.SMALL),
                                    filter="Small_.*", jobs=4, workspace=project.workspace) == 0

        ctest = project.runner.commands[-1]
        assert _value_after(ctest, "-L") == "^(small|medium)$"
        assert _value_after(ctest, "-R") == "Small_.*"
        assert _value_after(ctest, "--parallel") == "4"
        assert _value_after(ctest, "-LE") == "^quarantined$"
        assert "--output-junit" in ctest
        assert _value_after(ctest, "-C") == "Debug"

    def test_test_without_a_size_runs_small_and_medium(self, project):
        project.configure_tree("Debug")

        assert project_service.test(workspace=project.workspace) == 0

        assert _value_after(project.runner.commands[-1], "-L") == "^(small|medium)$"

    def test_test_refuses_an_unfiltered_large_run_after_the_build(self, project):
        project.configure_tree("Debug")

        assert project_service.test(sizes=(TestLabel.LARGE,), workspace=project.workspace) == 2

        assert _is_compile(project.runner.commands[-1])

    def test_test_with_package_installs_then_builds_and_runs_the_consumer(self, project):
        project.configure_tree("Debug")

        assert project_service.test(package=True, workspace=project.workspace) == 0

        install, configure, compile_, ctest = project.runner.commands[-4:]
        prefix = _value_after(install, "--prefix")
        assert "--install" in install and Path(prefix).name == "package-prefix"
        assert Path(_value_after(configure, "-S")) == project.root / "tests" / "package"
        assert f"-DCMAKE_PREFIX_PATH={prefix}" in configure
        assert _is_compile(compile_)
        assert Path(_value_after(ctest, "--test-dir")).name == "package"


@pytest.mark.medium
class TestCheck:
    """Which checkers `check` runs, and what it returns."""

    def test_check_runs_every_checker_with_the_root(self, project):
        assert project_service.check(workspace=project.workspace) == 0

        assert [Path(command[1]) for command in project.runner.commands] == [
            project.root / checker for checker in project_service.K_CHECKERS]
        assert all(command[0] == sys.executable and command[2] == str(project.root)
                   for command in project.runner.commands)

    def test_check_returns_the_highest_checker_exit_code(self, project):
        project.runner.script(0, 3, 1)

        assert project_service.check(workspace=project.workspace) == 3

    def test_check_with_a_rule_runs_only_the_checkers_that_list_it(self, project):
        _plant_checker(project.root, Path("tools", "logs", "check_logging.py"), "rule_x")

        assert project_service.check(rule="rule_x", workspace=project.workspace) == 0

        (command,) = project.runner.commands
        assert command[1].endswith("check_logging.py") and command[-2:] == ["--rule", "rule_x"]

    def test_check_with_an_unknown_rule_fails_and_says_so(self, project):
        assert project_service.check(rule="rule_none", workspace=project.workspace) == 1

        assert project.console.of("error") == ["no checker under tools/ has the rule rule_none"]

    def test_rules_of_reads_the_list_rules_flag(self, tmp_path):
        checker = tmp_path / "check_fake.py"
        checker.write_text(
            "import sys\nif sys.argv[1:] == ['--list-rules']:\n    print('rule_a\\nrule_b')\n",
            encoding="utf-8")

        assert project_service.rules_of(checker) == frozenset({"rule_a", "rule_b"})
