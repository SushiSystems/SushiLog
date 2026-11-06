# test_env.py
# SushiLog - https://github.com/SushiSystems/SushiLog
# Copyright (c) 2026-present Mustafa Garip & Sushi Systems
# Licensed under the Apache License 2.0. See LICENSE.
"""Tests the environment `sl` hands cmake and ctest."""

from __future__ import annotations

import os

import pytest

from sushilog import env
from sushilog.config import Config

pytestmark = pytest.mark.medium


def test_without_a_compiler_the_environment_is_a_copy(tmp_path, monkeypatch):
    monkeypatch.setenv("PATH", str(tmp_path))

    built = env.load_build_env(Config(), tmp_path / "build")

    assert built == dict(os.environ)
    assert built is not os.environ


def test_a_compiler_path_puts_its_folder_first_on_path(tmp_path):
    compiler = tmp_path / "bin" / "clang++"

    built = env.load_build_env(Config(cxx=str(compiler), platform="linux"), tmp_path / "build")

    assert built["PATH"].split(os.pathsep)[0] == str(compiler.parent)


def test_a_bare_compiler_name_leaves_path_unchanged(tmp_path):
    built = env.load_build_env(Config(cxx="clang++", platform="linux"), tmp_path / "build")

    assert built["PATH"] == os.environ["PATH"]
