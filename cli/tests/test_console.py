# test_console.py
# SushiLog - https://github.com/SushiSystems/SushiLog
# Copyright (c) 2026-present Mustafa Garip & Sushi Systems
# Licensed under the Apache License 2.0. See LICENSE.
"""Tests that importing the console reads nothing until a printer is used."""

from __future__ import annotations

import subprocess
import sys

import pytest
from typer.testing import CliRunner

from sushilog import cli

pytestmark = pytest.mark.medium

# Run in a fresh interpreter, so the check sees an import and nothing a test did before it.
K_IMPORT_ONLY = "import sys, sushilog.console; sys.exit('rich.console' in sys.modules)"


def test_importing_the_console_builds_nothing():
    imported = subprocess.run([sys.executable, "-c", K_IMPORT_ONLY], check=False)

    assert imported.returncode == 0


def test_help_works_outside_a_checkout(tmp_path, monkeypatch):
    monkeypatch.chdir(tmp_path)

    result = CliRunner().invoke(cli.app, ["--help"])

    assert result.exit_code == 0
