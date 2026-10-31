# project.py
# SushiLog - https://github.com/SushiSystems/SushiLog
# Copyright (c) 2026-present Mustafa Garip & Sushi Systems
# Licensed under PolyForm Noncommercial 1.0.0. See LICENSE.
# Commercial use requires a licence from Sushi Systems.
"""Drives SushiLog's build, test and check steps.

Holds the build policy: one tree per build type and sanitizer, the memory gate every compile
runs behind, the test sizes and their CTest labels, the install the package test builds against,
and the checkers. cmake and ctest run through ``sushicore.cmake_driver.CMakeDriver`` under the
environment ``sushilog.env`` assembles.
"""

from __future__ import annotations

import enum
import subprocess
import sys
from collections.abc import Sequence
from pathlib import Path

from sushicore import build_admission, build_report, cmake_cache
from sushicore.build_tree import K_DEFAULT_BUILD_TYPE, refuse_another_build_type, tree_dir
from sushicore.cmake_driver import CMakeDriver
from sushicore.proc import Runner
from sushicore.testing import ModuleTests, TestLabel, run_tests, select_labels
from sushicore.toolchain_args import c_compiler_for

from .. import console
from ..config import PROFILE, Config, find_project_root, load_config
from ..env import load_build_env


class BuildType(str, enum.Enum):
    """Names the build types `--type` accepts."""

    DEBUG = "debug"
    RELEASE = "release"
    RELWITHDEBINFO = "relwithdebinfo"


class Sanitizer(str, enum.Enum):
    """Names the sanitizer builds `--sanitize` accepts; each is an SL_SANITIZE value."""

    ADDRESS = "address"
    THREAD = "thread"


K_CMAKE_BUILD_TYPE = {
    BuildType.DEBUG: "Debug",
    BuildType.RELEASE: "Release",
    BuildType.RELWITHDEBINFO: "RelWithDebInfo",
}

K_DEFAULT_TYPE = BuildType(K_DEFAULT_BUILD_TYPE.lower())

K_TREE_NAME = {None: "default", Sanitizer.ADDRESS: "asan", Sanitizer.THREAD: "tsan"}

K_PACKAGE_SOURCE = Path("tests", "package")

K_CHECKERS = (
    Path("tools", "documentation", "check_source_comments.py"),
    Path("tools", "documentation", "check_docs_layout.py"),
    Path("tools", "documentation", "check_changelog.py"),
    Path("tools", "documentation", "check_docs_truth.py"),
    Path("tools", "layering", "check_layering.py"),
    Path("tools", "testing", "check_test_mirror.py"),
    Path("tools", "testing", "check_test_sizes.py"),
    Path("tools", "testing", "check_probes.py"),
    Path("tools", "logs", "check_logging.py"),
)

_RUNNER = Runner(console, PROFILE.program)
_DRIVER = CMakeDriver(console, _RUNNER)


def build_dir(root: Path, sanitizer: Sanitizer | None, build_type: BuildType,
              requested: str | None = None) -> Path:
    """Returns the build tree: *requested* below *root* when given, else the one of the pair.

    The pair is *sanitizer* and *build_type*: `build/default-debug`, `build/asan-debug`.
    """
    if requested:
        return root / requested
    return tree_dir(root, K_TREE_NAME[sanitizer], K_CMAKE_BUILD_TYPE[build_type])


def _compiler_arguments(cfg: Config) -> list[str]:
    """Returns the generator and compiler arguments of a configure."""
    arguments: list[str] = []
    generator = cfg.resolved_generator()
    if generator:
        arguments += ["-G", generator]
    if generator == "Ninja":
        arguments += [f"-DCMAKE_MAKE_PROGRAM={cfg.resolved_ninja()}"]
    compiler = cfg.resolved_compiler()
    if compiler:
        arguments += [f"-DCMAKE_CXX_COMPILER={compiler}",
                      f"-DCMAKE_C_COMPILER={c_compiler_for(compiler)}"]
    return arguments


def _needs_configure(cfg: Config, tree: Path, sanitizer: Sanitizer | None,
                     lean: bool = False) -> bool:
    """Returns whether *tree* must be configured before it is compiled."""
    if not (tree / "CMakeCache.txt").is_file():
        return True
    expect = {"SL_SANITIZE": sanitizer.value if sanitizer else "",
              "SL_BUILD_TESTS": _on_off(not lean)}
    generator = cfg.resolved_generator()
    if generator:
        return _DRIVER.needs_configure(tree, generator, expect=expect)
    return any(cmake_cache.cached_value(tree, key) != value for key, value in expect.items())


def _on_off(value: bool) -> str:
    """Returns the CMake spelling of a boolean option."""
    return "ON" if value else "OFF"


def build(build_type: BuildType = K_DEFAULT_TYPE, clean: bool = False,
          defines: Sequence[str] = (), sanitizer: Sanitizer | None = None,
          lean: bool = False, tree_path: str | None = None, jobs: int | None = None,
          report: bool = False) -> int:
    """Configures the tree of *sanitizer* and *build_type* when it needs it, then compiles it.

    Args:
        build_type: Picks the tree; a tree keeps the type it was configured with.
        clean: Removes the tree first.
        defines: `VAR=VALUE` cache variables; any of them reconfigures a configured tree.
        sanitizer: Builds in that sanitizer's own tree, build/asan-<type>/ or build/tsan-<type>/.
        lean: Builds no tests; what a library package is built as. SushiLog has no examples or
            benchmarks to leave out.
        tree_path: The tree to build in, below the project root, in place of the default one.
        jobs: Compile jobs; none uses the hardware threads, which the memory gate then admits.
        report: Prints and writes the build report after the compile.

    Raises:
        ConfigError: The tree was configured as another build type, or *defines* names one.
        BuildStoppedError: The memory gate's stop file exists.
    """
    root = find_project_root()
    cfg = load_config()
    tree = build_dir(root, sanitizer, build_type, tree_path)
    cmake_type = K_CMAKE_BUILD_TYPE[build_type]
    refuse_another_build_type(tree, cmake_type, defines)
    build_admission.refuse_a_stopped_gate(tree)
    if clean:
        _DRIVER.clean_tree(tree)
    env = load_build_env(cfg, tree)

    if defines or _needs_configure(cfg, tree, sanitizer, lean):
        command = [_DRIVER.cmake(cfg), "-S", str(root), "-B", str(tree),
                   f"-DCMAKE_BUILD_TYPE={cmake_type}",
                   f"-DSL_SANITIZE={sanitizer.value if sanitizer else ''}",
                   f"-DSL_BUILD_TESTS={_on_off(not lean)}",
                   *_compiler_arguments(cfg)]
        gate = build_admission.launcher_defines(
            tree, build_admission.compiler_cache(cfg.ccache_exe))
        for define in (*defines, *gate):
            command += ["-D", define]
        rc = _DRIVER.configure(command, cwd=root, env=env)
        if rc != 0:
            return rc

    offset = build_report.ninja_log_offset(tree)
    rc = _DRIVER.compile(cfg, tree, cwd=root, env=env, config=cmake_type,
                         jobs=jobs or build_admission.hardware_threads())
    if report:
        summary = build_report.of_build(tree, offset, build_admission.state_dir(tree))
        for line in summary.lines():
            console.info(line)
        console.info(f"Report: {summary.write(tree)}")
    return rc


def test(build_type: BuildType = K_DEFAULT_TYPE, sizes: Sequence[TestLabel] = (), clean: bool = False,
         filter: str | None = None, sanitizer: Sanitizer | None = None,
         jobs: int | None = None, package: bool = False, repeat: int = 0,
         every_size: bool = False) -> int:
    """Builds when needed, then runs the tests *sizes* and *filter* select through the shared test command.

    Args:
        sizes: The sizes to run; none runs small and medium.
        filter: A ctest -R regex over the `Suite.Case` names.
        sanitizer: Builds and tests that sanitizer's tree.
        jobs: Tests run at once; none leaves the count to ctest.
        package: Also installs the tree, builds tests/package/ against the install and runs
            its two tests, one per library target.
        repeat: Reruns each test up to this many times, stopping at its first failure.
        every_size: Runs every size, the large tests included.
    """
    rc = build(build_type, clean=clean, sanitizer=sanitizer)
    if rc != 0:
        return rc

    root = find_project_root()
    cfg = load_config()
    tree = build_dir(root, sanitizer, build_type)
    config = K_CMAKE_BUILD_TYPE[build_type]
    env = load_build_env(cfg, tree)
    module = ModuleTests(
        profile=PROFILE, project_root=lambda: root, console=lambda: console,
        build_tree=lambda: tree, run=_RUNNER.run,
        run_ctest=lambda directory, **options: _DRIVER.ctest_run(cfg, directory, env, config=config, **options))
    selection = select_labels(sizes, every_size=every_size)
    rc = run_tests(module, selection, name_filter=filter, repeat=repeat, jobs=jobs, every_size=every_size)
    if rc != 0 or not package:
        return rc
    return _test_package(root, cfg, tree, env, config)


def _test_package(root: Path, cfg: Config, tree: Path, env: dict[str, str], config: str) -> int:
    """Installs *tree*, then builds and runs the consumer of tests/package/ against the install."""
    prefix = tree / "package-prefix"
    consumer = tree / "package"
    console.header("Package consumer")
    rc = _RUNNER.run([_DRIVER.cmake(cfg), "--install", str(tree), "--config", config,
                      "--prefix", str(prefix)], root, env)
    if rc != 0:
        return rc
    rc = _DRIVER.configure([_DRIVER.cmake(cfg), "-S", str(root / K_PACKAGE_SOURCE),
                            "-B", str(consumer), f"-DCMAKE_BUILD_TYPE={config}",
                            f"-DCMAKE_PREFIX_PATH={prefix}", *_compiler_arguments(cfg)],
                           cwd=root, env=env)
    if rc != 0:
        return rc
    rc = _DRIVER.compile(cfg, consumer, cwd=root, env=env, config=config)
    if rc != 0:
        return rc
    return _DRIVER.ctest_run(cfg, consumer, env, config=config)


def check(report: bool = False, rule: str | None = None) -> int:
    """Runs the shared checkers under tools/.

    Args:
        report: Makes each checker print counts per rule and exit zero.
        rule: The one rule to run; only the checkers whose --list-rules names it run.

    Returns:
        The highest exit code among the checkers that ran, or 1 when no checker has *rule*.
    """
    root = find_project_root()
    checkers = [checker for checker in K_CHECKERS
                if rule is None or rule in rules_of(root / checker)]
    if not checkers:
        console.error(f"no checker under tools/ has the rule {rule}")
        return 1
    arguments = (["--report"] if report else []) + (["--rule", rule] if rule else [])
    return max(_RUNNER.run([sys.executable, str(root / checker), str(root), *arguments], root)
               for checker in checkers)


def rules_of(checker: Path) -> frozenset[str]:
    """Returns the rule names the checker at *checker* prints for --list-rules."""
    listed = subprocess.run([sys.executable, str(checker), "--list-rules"],
                            capture_output=True, text=True, check=False)
    return frozenset(listed.stdout.split()) if listed.returncode == 0 else frozenset()
