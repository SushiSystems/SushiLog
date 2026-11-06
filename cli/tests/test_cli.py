# test_cli.py
# SushiLog - https://github.com/SushiSystems/SushiLog
# Copyright (c) 2026-present Mustafa Garip & Sushi Systems
# Licensed under the Apache License 2.0. See LICENSE.
"""Tests that each `sl` command parses its options and hands them to the project service.

Each command runs against the `project` fixture's workspace, handed in as the context object, so
what a command passed on shows in the argv the workspace's runner recorded.
"""

from __future__ import annotations

import re
from pathlib import Path

import pytest
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

    def test_build_with_clean_removes_the_tree(self, project):
        tree = project.configure_tree("Debug")

        result = _invoke(project, "build", "--clean")

        assert result.exit_code == 0
        assert not (tree / "build.ninja").exists()


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
