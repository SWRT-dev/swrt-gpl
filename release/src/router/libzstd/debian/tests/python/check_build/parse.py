# SPDX-FileCopyrightText: Peter Pentchev <roam@ringlet.net>
# SPDX-License-Identifier: BSD-2-Clause
"""Parse the configuration file."""

from __future__ import annotations

import dataclasses
import functools
import sys
from typing import TYPE_CHECKING, Any

import typedload.dataloader

from check_build import defs


if sys.version_info >= (3, 11):
    import tomllib
else:
    import tomli as tomllib

if TYPE_CHECKING:
    import logging
    import pathlib
    from typing import Final


@dataclasses.dataclass
class ParseError(defs.CBuildError):
    """An error that occurred while parsing the configuration file."""


@dataclasses.dataclass
class ConfigError(ParseError):
    """An error that occurred while loading or parsing the configuration file."""

    config: pathlib.Path
    """The path to the config file that could not be parsed."""


@dataclasses.dataclass
class ConfigExcError(ConfigError):
    """An error that occurred while loading the config file with an exception attached."""

    err: Exception
    """The exception that occurred while loading the config file."""


@dataclasses.dataclass
class ConfigAbsoluteError(ConfigError):
    """The path to the config file was not an absolute one."""

    def __str__(self) -> str:
        """Provide a human-readable description of the error."""
        return f"Not an absolute path to the config file: {self.config}"


@dataclasses.dataclass
class ConfigReadError(ConfigExcError):
    """Could not read the config file."""

    def __str__(self) -> str:
        """Provide a human-readable description of the error."""
        return f"Could not read the {self.config} file: {self.err}"


@dataclasses.dataclass
class ConfigUTF8Error(ConfigExcError):
    """Could not parse the config file as valid UTF-8 text."""

    def __str__(self) -> str:
        """Provide a human-readable description of the error."""
        return f"Could not parse the {self.config} file as valid UTF-8: {self.err}"


@dataclasses.dataclass
class ConfigTOMLError(ConfigExcError):
    """Could not parse the config file as valid TOML."""

    def __str__(self) -> str:
        """Provide a human-readable description of the error."""
        return f"Could not parse the {self.config} file as valid TOML: {self.err}"


@dataclasses.dataclass
class ConfigTableError(ConfigError):
    """The config file did not contain a top-level TOML table."""

    def __str__(self) -> str:
        """Provide a human-readable description of the error."""
        return f"Expected a table as the top-level object in the {self.config} file"


@dataclasses.dataclass
class ConfigParseError(ConfigExcError):
    """Could not parse the config file as per our expected structure."""

    def __str__(self) -> str:
        """Provide a human-readable description of the error."""
        return f"Could not parse the {self.config} file: {self.err}"


@dataclasses.dataclass
class FormatParseError(ConfigExcError):
    """Could not parse the config file to obtain the format version."""

    def __str__(self) -> str:
        """Provide a human-readable description of the error."""
        return f"Could not determine the format version of the {self.config} file: {self.err}"


@dataclasses.dataclass
class FormatInvalidError(ConfigError):
    """The format version in the config file did not consist of two integers."""

    def __str__(self) -> str:
        """Provide a human-readable description of the error."""
        return f"The format.version.major/minor values in the {self.config} file must be integers"


@dataclasses.dataclass
class FormatUnsupportedError(ConfigError):
    """The format version is not supported by this version of check-build."""

    major: int
    """The major number of the format version."""

    minor: int
    """The minor number of the format version."""

    def __str__(self) -> str:
        """Provide a human-readable description of the error."""
        return f"Unsupported format version {self.major}.{self.minor} in the {self.config} file"


@dataclasses.dataclass
class UnknownProgramsError(ParseError):
    """The programs specified on the command line are not defined in the config file."""

    unknown: list[str]
    """The unknown programs specified on the command line."""

    def __str__(self) -> str:
        """Provide a human-readable description of the error."""
        return f"Unknown programs specified: {', '.join(self.unknown)}"


@dataclasses.dataclass(frozen=True)
class TCommands:
    """The commands to build and test a program."""

    clean: list[list[str]]
    build: list[list[str]]
    test: list[list[str]]


@dataclasses.dataclass(frozen=True)
class TPrereq:
    """The prerequisites for building a program."""

    programs: list[str] | None = None


@dataclasses.dataclass(frozen=True)
class TProgram:
    """The definition of a single program."""

    executable: str
    commands: TCommands
    prerequisites: TPrereq | None = None


@dataclasses.dataclass(frozen=True)
class TConfig:
    """The top-level parsed configuration settings."""

    program: dict[str, TProgram]


def _validate_format_version(config: pathlib.Path, progs: dict[str, Any]) -> None:
    """Make sure we have the appropriate format.version table."""
    try:
        major, minor = progs["format"]["version"]["major"], progs["format"]["version"]["minor"]
    except (TypeError, KeyError) as err:
        raise FormatParseError(config, err) from err
    if not isinstance(major, int) or not isinstance(minor, int):
        raise FormatInvalidError(config)
    if (major, minor) != (0, 1):
        raise FormatUnsupportedError(config, major, minor)


def _select_programs(cfg: TConfig, programs: tuple[str, ...]) -> list[str]:
    """Validate the specified list of programs or return them all."""
    if not programs:
        return list(cfg.program)

    selected: Final = list(programs)
    unknown: Final = [name for name in selected if name not in cfg.program]
    if unknown:
        raise UnknownProgramsError(unknown)

    return selected


@functools.lru_cache(maxsize=1)
def typed_loader() -> typedload.dataloader.Loader:
    """Instantiate a typed loader that can parse type annotations."""
    return typedload.dataloader.Loader(basiccast=False, failonextra=True, pep563=True)


def load_config(
    *, config: pathlib.Path, force: bool, programs: tuple[str, ...], logger: logging.Logger
) -> defs.Config:
    """Load the configuration file describing the list of programs to build."""
    if not config.is_absolute():
        raise ConfigAbsoluteError(config)
    try:
        contents: Final = config.read_text(encoding="UTF-8")
    except OSError as err:
        raise ConfigReadError(config, err) from err
    except ValueError as err:
        raise ConfigUTF8Error(config, err) from err

    try:
        progs: Final = tomllib.loads(contents)
    except ValueError as err:
        raise ConfigTOMLError(config, err) from err
    if not isinstance(progs, dict):
        raise ConfigTableError(config)

    _validate_format_version(config, progs)
    del progs["format"]
    try:
        cfg: Final = typed_loader().load(progs, TConfig)
    except ValueError as err:
        raise ConfigParseError(config, err) from err

    return defs.Config(
        force=force,
        log=logger,
        program={
            name: defs.Program(
                executable=prog.executable,
                prerequisites=defs.Prerequisites(programs=prog.prerequisites.programs)
                if prog.prerequisites is not None
                else None,
                commands=defs.Commands(
                    clean=prog.commands.clean, build=prog.commands.build, test=prog.commands.test
                ),
            )
            for name, prog in cfg.program.items()
        },
        selected=_select_programs(cfg, programs),
        topdir=config.parent,
    )
