# SPDX-FileCopyrightText: Peter Pentchev <roam@ringlet.net>
# SPDX-License-Identifier: BSD-2-Clause
"""Common definitions for the check-build library."""

from __future__ import annotations

import dataclasses
from typing import TYPE_CHECKING


if TYPE_CHECKING:
    import logging
    import pathlib


VERSION = "0.1.0"


@dataclasses.dataclass
class CBuildError(Exception):
    """An error that occurred during the check-build routines operation."""


@dataclasses.dataclass
class SkippedProgramError(CBuildError):
    """A program had to be skipped."""

    prog: str


@dataclasses.dataclass(frozen=True)
class Commands:
    """The commands to build and test a program."""

    clean: list[list[str]]
    build: list[list[str]]
    test: list[list[str]]


@dataclasses.dataclass(frozen=True)
class Prerequisites:
    """The prerequisites to check for before attempting to build the program."""

    programs: list[str] | None


@dataclasses.dataclass(frozen=True)
class Program:
    """The definition of a single program."""

    executable: str
    prerequisites: Prerequisites | None
    commands: Commands


@dataclasses.dataclass(frozen=True)
class Config:
    """Runtime configuration for the check-build routines."""

    force: bool
    log: logging.Logger
    program: dict[str, Program]
    selected: list[str]
    topdir: pathlib.Path
