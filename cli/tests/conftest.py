# conftest.py
# SushiLog - https://github.com/SushiSystems/SushiLog
# Copyright (c) 2026-present Mustafa Garip & Sushi Systems
# Licensed under the Apache License 2.0. See LICENSE.
"""Fixtures of the `sl` tests, and the size every test must carry.

Each test carries exactly one size mark, which gives it its pytest-timeout.
"""

from __future__ import annotations

from dataclasses import dataclass, field
from pathlib import Path

import pytest
from sushicore.build_tree import tree_dir
from sushicore.console_memory import RecordingConsole
from sushicore.proc_memory import RecordingRunner

from sushilog.config import Config
from sushilog.services.project import Workspace

K_SIZE_TIMEOUTS = {"small": 1, "medium": 10, "large": 120}


def _sizes_of(item: pytest.Item) -> list[str]:
    """Returns the size marks *item* carries."""
    return [mark.name for mark in item.iter_markers() if mark.name in K_SIZE_TIMEOUTS]


def pytest_collection_modifyitems(items: list[pytest.Item]) -> None:
    """Fails collection when a test carries no size or two, and gives each its timeout."""
    wrong = [f"{item.nodeid}: {_sizes_of(item) or 'no size'}"
             for item in items if len(_sizes_of(item)) != 1]
    if wrong:
        raise pytest.UsageError(f"every test carries exactly one size: {', '.join(wrong)}")
    for item in items:
        item.add_marker(pytest.mark.timeout(K_SIZE_TIMEOUTS[_sizes_of(item)[0]]))


@dataclass
class Project:
    """Holds a scratch checkout and what the project service did to it."""

    root: Path
    runner: RecordingRunner
    console: RecordingConsole
    workspace: Workspace
    tree: Path = field(init=False)

    @property
    def commands(self) -> list[list[str]]:
        """Returns the argv of every command the runner was handed, in order."""
        return [list(command.argv) for command in self.runner.commands]

    def __post_init__(self) -> None:
        """Derives the plain Debug build tree from the root."""
        self.tree = tree_dir(self.root, "default", "Debug")

    def configure_tree(self, build_type: str = "Debug", sanitize: str = "",
                       name: str = "default", tests: str = "ON") -> Path:
        """Plants the files a finished Ninja configure of *build_type* leaves behind.

        Returns:
            The tree, `build/<name>-<type>`.
        """
        tree = tree_dir(self.root, name, build_type)
        tree.mkdir(parents=True, exist_ok=True)
        (tree / "build.ninja").write_text("", encoding="utf-8")
        (tree / "CMakeCache.txt").write_text(
            f"CMAKE_BUILD_TYPE:STRING={build_type}\nCMAKE_GENERATOR:INTERNAL=Ninja\n"
            f"SL_SANITIZE:STRING={sanitize}\nSL_BUILD_TESTS:BOOL={tests}\n",
            encoding="utf-8")
        return tree


@pytest.fixture
def project(tmp_path) -> Project:
    """Returns a scratch checkout whose workspace records argv and starts nothing."""
    (tmp_path / "CMakeLists.txt").write_text("project(x)\n", encoding="utf-8")
    console = RecordingConsole()
    runner = RecordingRunner(console)
    cfg = Config(cxx="clang++", generator="Ninja", ninja_exe="ninja")
    workspace = Workspace(tmp_path, cfg, console, runner, environment={})
    return Project(tmp_path, runner, console, workspace)
