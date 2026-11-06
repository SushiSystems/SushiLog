# test_package_contract.py
# SushiLog - https://github.com/SushiSystems/SushiLog
# Copyright (c) 2026-present Mustafa Garip & Sushi Systems
# Licensed under the Apache License 2.0. See LICENSE.
"""Tests the contract a consumer's package build runs `sl` under.

The contract is `build --build-dir --lean`, `package` and the compiler `SUSHISTACK_CXX` names.
"""

from __future__ import annotations

import re
from pathlib import Path

import pytest
from typer.testing import CliRunner

from sushicore import deps_fragment
from sushilog import cli, config
from sushilog.services import project as project_service
from sushilog.services.project import BuildType

pytestmark = pytest.mark.medium

_RUNNER = CliRunner()
_ANSI_ESCAPE = re.compile(r"\x1b\[[0-9;]*m")


def _value_after(command: list[str], flag: str) -> str:
    """Returns the argument that follows *flag* in *command*."""
    return command[command.index(flag) + 1]


def test_the_build_command_hands_the_tree_and_lean_to_the_service(project):
    result = _RUNNER.invoke(cli.app, ["build", "--build-dir", "build/pkg-x", "--lean"],
                            obj=project.workspace)

    assert result.exit_code == 0
    configure = project.runner.commands[0]
    assert Path(_value_after(configure, "-B")) == project.root / "build" / "pkg-x"
    assert "-DSL_BUILD_TESTS=OFF" in configure


def test_a_lean_build_configures_the_named_tree_without_tests(project):
    assert project_service.build(BuildType.RELEASE, lean=True, tree_path="build/pkg-x",
                                 workspace=project.workspace) == 0

    configure = project.runner.commands[0]
    assert Path(_value_after(configure, "-B")) == project.root / "build" / "pkg-x"
    assert "-DSL_BUILD_TESTS=OFF" in configure


def test_a_default_build_asks_for_the_tests(project):
    assert project_service.build(BuildType.RELEASE, workspace=project.workspace) == 0

    assert "-DSL_BUILD_TESTS=ON" in project.runner.commands[0]


def test_a_lean_build_reconfigures_a_tree_that_has_tests(project):
    project.configure_tree("Release", tests="ON")

    assert project_service.build(BuildType.RELEASE, lean=True, workspace=project.workspace) == 0

    assert "-DSL_BUILD_TESTS=OFF" in project.runner.commands[0]


def test_the_package_command_is_registered_with_its_options():
    result = _RUNNER.invoke(cli.app, ["package", "--help"])
    text = _ANSI_ESCAPE.sub("", result.output)

    assert result.exit_code == 0
    for option in ("--build-dir", "--prefix", "--no-archive", "--force"):
        assert option in text


def test_load_config_reads_the_compiler_the_environment_names(monkeypatch):
    monkeypatch.setenv(config.K_STACK_CXX_ENV, "/stack/clang++")

    assert config.load_config().stack_cxx == "/stack/clang++"


def test_the_fragment_names_the_cli():
    fragment = deps_fragment.read(Path(__file__).resolve().parents[1] / "sushistack.deps.toml")

    assert fragment.cli == "sl"
