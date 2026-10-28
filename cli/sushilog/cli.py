# cli.py
# SushiLog - https://github.com/SushiSystems/SushiLog
# Copyright (c) 2026-present Mustafa Garip & Sushi Systems
# Licensed under PolyForm Noncommercial 1.0.0. See LICENSE.
# Commercial use requires a licence from Sushi Systems.
"""Declares the commands of `sl`, the SushiLog developer CLI.

A thin Typer layer: each command parses its arguments and delegates to
``sushilog.services.project``. The root options, the `docs` group and the entry point come from
sushicore.
"""

from __future__ import annotations

from typing import Any, Optional

import typer
from sushicore.backlog import register_backlog_commands
from sushicore.docs_bundle import register_docs_commands
from sushicore.design import register_design_commands
from sushicore.entry import run as run_app
from sushicore.provision.commands import ModuleProvision
from sushicore.provision.package_command import register_package_command
from sushicore.root_options import register_root_options
from sushicore.typer_help import help_group

from . import console
from .config import PROFILE, find_project_root, load_config
from .services import project as project_service
from .services.project import K_DEFAULT_TYPE, BuildType, Sanitizer, Size

K_PROGRAM = "sl"
K_DISTRIBUTION = "sushilog-cli"
K_PANEL = "Build"

K_TYPE_HELP = ("Build type: debug | release | relwithdebinfo. Each type has its own tree. "
               f"Default: {K_DEFAULT_TYPE.value}.")

_help_group = help_group(console.current)

app = typer.Typer(
    name=K_PROGRAM,
    cls=_help_group,
    help="SushiLog developer CLI: build, test and check SushiLog.",
    no_args_is_help=True,
    add_completion=False,
    rich_markup_mode="rich",
)


def _type_option() -> Any:
    """Returns the default of a `--type/-t` parameter."""
    return typer.Option(K_DEFAULT_TYPE, "--type", "-t", case_sensitive=False, help=K_TYPE_HELP)


def _sanitize_option() -> Any:
    """Returns the default of a `--sanitize` parameter."""
    return typer.Option(None, "--sanitize", case_sensitive=False,
                        help="Sanitizer build in its own tree: address (ASan and UBSan) | "
                             "thread (TSan).")


def _build_dir_option() -> Any:
    """Returns the default of a `--build-dir` parameter."""
    return typer.Option(None, "--build-dir", metavar="PATH",
                        help="Build in this tree, below the checkout, instead of "
                             "build/default-<type>/.")


def _lean_option() -> Any:
    """Returns the default of a `--lean` parameter."""
    return typer.Option(False, "--lean",
                        help="Build no tests; what a consumer's package build of SushiLog uses.")


def _report_option() -> Any:
    """Returns the default of a `--report` parameter."""
    return typer.Option(False, "--report",
                        help="After the compile, print and write the build report: time, count "
                             "and memory peak per kind of edge, the slowest edges.")


def _clean_option() -> Any:
    """Returns the default of a `--clean` parameter."""
    return typer.Option(False, "--clean", help="Delete the build tree before configuring.")


@app.command(
    "build",
    rich_help_panel=K_PANEL,
    epilog="sl build\n"
           "sl build --type release\n"
           "sl build --report  # Time and memory peaks of the compile\n"
           "sl build --sanitize address  # ASan and UBSan, in build/asan-debug/\n"
           "sl build --build-dir build/pkg-x --lean  # What a consumer's package build runs",
)
def build(
    type: BuildType = _type_option(),
    clean: bool = _clean_option(),
    define: Optional[list[str]] = typer.Option(
        None, "--define", "-D", metavar="VAR=VALUE",
        help="Pass a CMake cache variable; a configured tree reconfigures. Repeatable."),
    sanitize: Optional[Sanitizer] = _sanitize_option(),
    build_dir: Optional[str] = _build_dir_option(),
    lean: bool = _lean_option(),
    jobs: Optional[int] = typer.Option(
        None, "--jobs", "-j", min=1,
        help="Compile jobs. When omitted: the hardware threads, behind the memory gate."),
    report: bool = _report_option(),
) -> None:
    """Configure when needed, then build the libraries and the tests."""
    raise typer.Exit(project_service.build(type, clean=clean, defines=define or (),
                                           sanitizer=sanitize, lean=lean, tree_path=build_dir,
                                           jobs=jobs, report=report))


@app.command(
    "test",
    rich_help_panel=K_PANEL,
    epilog="sl test  # Every size\n"
           "sl test --label small --label medium  # What a pull request runs\n"
           "sl test --filter 'Small_LogLevels.*' --jobs 4\n"
           "sl test --package  # Also build tests/package/ against the install",
)
def test(
    type: BuildType = _type_option(),
    label: Optional[list[Size]] = typer.Option(
        None, "--label", "-l", case_sensitive=False,
        help="Test size to run: small | medium | large; repeat for more. Default: every size."),
    filter: Optional[str] = typer.Option(
        None, "--filter", "-f", help="ctest -R regex over 'Suite.Case' test names."),
    jobs: Optional[int] = typer.Option(
        None, "--jobs", "-j", min=1,
        help="Tests run at once. When omitted ctest decides."),
    clean: bool = _clean_option(),
    sanitize: Optional[Sanitizer] = _sanitize_option(),
    package: bool = typer.Option(
        False, "--package",
        help="Install the tree, build tests/package/ against it and run its two tests."),
) -> None:
    """Build when needed, then run ctest over the sizes and the filter."""
    raise typer.Exit(project_service.test(type, label or (), clean=clean, filter=filter,
                                          sanitizer=sanitize, jobs=jobs, package=package))


@app.command(
    "check",
    rich_help_panel=K_PANEL,
    epilog="sl check\n"
           "sl check --report  # Counts per rule, without failing\n"
           "sl check --rule rule_console_output  # One rule",
)
def check(
    report: bool = typer.Option(False, "--report", "-r", help="Report counts without failing."),
    rule: Optional[str] = typer.Option(None, "--rule", help="Run only this rule."),
) -> None:
    """Run the shared checkers under tools/."""
    raise typer.Exit(project_service.check(report=report, rule=rule))


register_package_command(app, ModuleProvision(
    profile=PROFILE,
    project_root=find_project_root,
    load_config=load_config,
    console=console.current,
    panel=K_PANEL,
))

register_docs_commands(
    app,
    program=K_PROGRAM,
    panel=K_PANEL,
    project_root=find_project_root,
    report=lambda line: console.success(line),
    group_cls=_help_group,
)
register_backlog_commands(
    app,
    program=K_PROGRAM,
    panel=K_PANEL,
    project_root=find_project_root,
    group_cls=_help_group,
)
register_design_commands(
    app,
    program=K_PROGRAM,
    panel=K_PANEL,
    project_root=find_project_root,
    group_cls=_help_group,
)

register_root_options(app, distribution=K_DISTRIBUTION)


def _report_error(message: str) -> None:
    """Prints one error line through the console, which is built only at this call."""
    console.error(message)


def main() -> None:
    """Runs `sl` and exits; a failure sushicore reports becomes one line and exit code 1."""
    run_app(app, _report_error)


if __name__ == "__main__":
    main()
