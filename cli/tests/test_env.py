# test_env.py
# SushiLog - https://github.com/SushiSystems/SushiLog
# Copyright (c) 2026-present Mustafa Garip & Sushi Systems
# Licensed under PolyForm Noncommercial 1.0.0. See LICENSE.
# Commercial use requires a licence from Sushi Systems.
"""Tests the environment `sl` hands cmake and ctest."""

from __future__ import annotations

import os

import pytest

from sushilog import env
from sushilog.config import Config

pytestmark = pytest.mark.small


def test_without_a_compiler_the_environment_is_a_copy(tmp_path, monkeypatch):
    monkeypatch.setattr(Config, "resolved_compiler", lambda self: "")

    built = env.load_build_env(Config(), tmp_path)

    assert built == dict(os.environ)
    assert built is not os.environ


def test_a_found_compiler_comes_first_on_path(tmp_path, monkeypatch):
    compiler = tmp_path / "bin" / "clang++"
    monkeypatch.setattr(Config, "resolved_compiler", lambda self: str(compiler))

    built = env.load_build_env(Config(platform="linux"), tmp_path / "build")

    assert built["PATH"].split(os.pathsep)[0] == str(compiler.parent)
