# test_cli.py
# SushiLog - https://github.com/SushiSystems/SushiLog
# Copyright (c) 2026-present Mustafa Garip & Sushi Systems
# Licensed under the Apache License 2.0. See LICENSE.
"""Tests that each `sl` command parses its options and hands them to the project service.

Each command runs against the `project` fixture's workspace, handed in as the context object, so
what a command passed on shows in the argv the workspace's runner recorded.
"""

from __future__ import annotations

import dataclasses
import os
import re
from pathlib import Path

import pytest
from sushicore.console_memory import RecordingConsole
from typer.testing import CliRunner

from sushilog import cli

_RUNNER = CliRunner()
_ANSI_ESCAPE = re.compile(r"\x1b\[[0-9;]*m")


def _value_after(command: list[str], flag: str) -> str:
    """Returns the argument that follows *flag* in *command*."""
    return command[command.index(flag) + 1]


def _invoke(project, *arguments: str):
    """Runs `sl` with *arguments* on the workspace of *project*."""
    return _RUNNER.invoke(cli.app, list(arguments), obj=project.workspace)


@pytest.mark.medium
class TestBuildCommand:
    """What `sl build` hands the service."""

    def test_build_hands_the_type_the_sanitizer_and_the_defines_to_the_service(self, project):
        result = _invoke(project, "build", "--type", "release", "--sanitize", "address",
                         "-D", "A=1")

        assert result.exit_code == 0
        configure = project.commands[0]
        assert Path(_value_after(configure, "-B")).name == "asan-release"
        assert "-DSL_SANITIZE=address" in configure
        assert _value_after(configure, "-D") == "A=1"

    def test_build_without_a_type_builds_debug(self, project):
        result = _invoke(project, "build")

        assert result.exit_code == 0
        assert "-DCMAKE_BUILD_TYPE=Debug" in project.commands[0]

    def test_build_hands_the_jobs_to_the_compile(self, project):
        result = _invoke(project, "build", "--jobs", "2")

        assert result.exit_code == 0
        assert _value_after(project.commands[-1], "--parallel") == "2"

    def test_build_with_report_prints_the_report(self, project):
        result = _invoke(project, "build", "--report")

        assert result.exit_code == 0
        assert project.console.texts("info")[-1].startswith("Report: ")

    def test_build_no_longer_takes_clean(self, project):
        tree = project.configure_tree("Debug")

        result = _invoke(project, "build", "--clean")

        assert result.exit_code == 2
        assert (tree / "build.ninja").exists()


@pytest.mark.medium
class TestTestCommand:
    """What `sl test` hands the service."""

    def test_test_hands_the_sizes_the_jobs_and_the_filter_to_ctest(self, project):
        project.configure_tree("Debug")

        result = _invoke(project, "test", "--label", "small", "--jobs", "3", "--filter", "X.*")

        assert result.exit_code == 0
        ctest = project.commands[-1]
        assert _value_after(ctest, "-L") == "^(small)$"
        assert _value_after(ctest, "--parallel") == "3"
        assert _value_after(ctest, "-R") == "X.*"

    def test_test_with_package_runs_the_consumer(self, project):
        project.configure_tree("Debug")

        result = _invoke(project, "test", "--package")

        assert result.exit_code == 0
        assert Path(_value_after(project.commands[-1], "--test-dir")).name == "package"

    def test_test_refuses_zero_jobs_naming_the_option_and_runs_nothing(self, project):
        result = _invoke(project, "test", "--jobs", "0")

        assert result.exit_code == 2
        assert "--jobs" in _ANSI_ESCAPE.sub("", result.output)
        assert project.commands == []


def _plant(directory: Path, name: str) -> Path:
    """Writes an empty file that the discovery of built programs takes for the program *name*."""
    directory.mkdir(parents=True, exist_ok=True)
    path = directory / (f"{name}.exe" if os.name == "nt" else name)
    path.write_bytes(b"")
    path.chmod(0o755)
    return path


@pytest.fixture
def built(project):
    """The project whose tree holds one registered example beside a test program."""
    examples = project.root / "applications" / "examples"
    examples.mkdir(parents=True)
    (examples / "CMakeLists.txt").write_text("add_executable(demo demo.cpp)\n", encoding="utf-8")
    _plant(project.tree, "sushilog_tests")
    _plant(project.tree, "demo")
    return project


def _answering(project, answer: str):
    """Returns the project with a console that answers the prompt with *answer*."""
    console = RecordingConsole(answer=answer)
    workspace = dataclasses.replace(project.workspace, console=console)
    return dataclasses.replace(project, console=console, workspace=workspace)


@pytest.mark.medium
class TestRunCommand:
    """What `sl run` starts, and what it asks for first."""

    def test_run_starts_the_program_the_name_matches_with_the_arguments_after_the_dashes(
            self, built):
        result = _invoke(built, "run", "sushilog_t", "--", "--gtest_filter=A.*")

        assert result.exit_code == 0
        (command,) = built.commands
        assert (Path(command[0]).stem, command[1:]) == ("sushilog_tests", ["--gtest_filter=A.*"])

    def test_run_with_no_target_picks_among_the_registered_examples(self, built):
        picking = _answering(built, "2")

        result = _invoke(picking, "run")

        assert result.exit_code == 1
        assert picking.console.texts("error") == ["'2' is not a number from 1 to 1."]

    def test_run_pick_with_all_reaches_a_program_that_is_not_an_example(self, built):
        picking = _answering(built, "2")

        result = _invoke(picking, "run", "--pick", "--all")

        assert result.exit_code == 0
        assert Path(picking.commands[0][0]).stem == "sushilog_tests"

    def test_run_without_a_build_tree_names_sl_build(self, project):
        result = _invoke(project, "run", "sushilog_tests")

        assert result.exit_code == 1
        assert "Run `sl build` first." in project.console.texts("error")[0]
        assert project.commands == []


@pytest.mark.medium
class TestCleanCommand:
    """What `sl clean` lists, and what it removes only with --yes."""

    def test_clean_without_yes_says_what_it_would_remove_and_removes_none(self, project):
        debug = project.configure_tree("Debug")
        release = project.configure_tree("Release")

        result = _invoke(project, "clean")

        assert result.exit_code == 2
        assert debug.is_dir() and release.is_dir()

    def test_clean_with_a_type_and_yes_removes_the_plain_tree_of_that_type_only(self, project):
        debug = project.configure_tree("Debug")
        release = project.configure_tree("Release")

        result = _invoke(project, "clean", "--type", "debug", "--yes")

        assert result.exit_code == 0
        assert not debug.exists() and release.is_dir()

    def test_clean_with_a_sanitizer_and_yes_removes_the_trees_of_that_sanitizer(self, project):
        debug = project.configure_tree("Debug")
        thread = project.configure_tree("Debug", name="tsan")

        result = _invoke(project, "clean", "--sanitize", "thread", "--yes")

        assert result.exit_code == 0
        assert debug.is_dir() and not thread.exists()

    def test_clean_with_yes_alone_removes_every_tree(self, project):
        debug = project.configure_tree("Debug")
        release = project.configure_tree("Release")

        assert _invoke(project, "clean", "--yes").exit_code == 0

        assert not debug.exists() and not release.exists()


@pytest.mark.medium
class TestCheckCommand:
    """What `sl check` hands the service, and what it returns."""

    def test_check_hands_the_report_and_the_rule_to_the_checker(self, project):
        checker = project.root / "tools" / "logs" / "check_logging.py"
        checker.parent.mkdir(parents=True)
        checker.write_text("print('rule_a')\n", encoding="utf-8")

        result = _invoke(project, "check", "--report", "--rule", "rule_a")

        assert result.exit_code == 0
        (command,) = project.commands
        assert command[-3:] == ["--report", "--rule", "rule_a"]

    def test_the_service_exit_code_becomes_the_process_exit_code(self, project):
        project.runner.script(2)

        assert _invoke(project, "check").exit_code == 2


@pytest.mark.small
def test_docs_bundle_asks_for_a_release():
    result = _RUNNER.invoke(cli.app, ["docs", "bundle"])

    assert result.exit_code == 2
    assert "--release" in _ANSI_ESCAPE.sub("", result.output)
