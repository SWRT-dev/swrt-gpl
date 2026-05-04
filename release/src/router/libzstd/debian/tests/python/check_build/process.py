# SPDX-FileCopyrightText: Peter Pentchev <roam@ringlet.net>
# SPDX-License-Identifier: BSD-2-Clause
"""Build and test a single program."""

from __future__ import annotations

import dataclasses
import os
import pathlib
import shlex
import shutil
import subprocess
import tempfile
from typing import Final

from check_build import defs


@dataclasses.dataclass
class ProcessError(defs.CBuildError):
    """An error that occurred during the build and test process."""


@dataclasses.dataclass
class RunError(ProcessError):
    """Could not run a program for a build stage."""

    tag: str
    """The stage at which a program failed."""

    cmdstr: str
    """The command we attempted to run."""

    cwd: pathlib.Path
    """The directory we attempted to run the program in."""

    err: Exception
    """The error that occurred while running the program."""

    def __str__(self) -> str:
        """Provide a human-readable description of the error."""
        return (
            f"A {self.tag} command failed: could not run `{self.cmdstr}` in {self.cwd}: {self.err}"
        )


@dataclasses.dataclass
class NoProgramError(ProcessError):
    """A program was not defined in the configuration."""

    prog: str
    """The program we expected to be able to be build."""

    def __str__(self) -> str:
        """Provide a human-readable description of the error."""
        return f"No {self.prog!r} program defined in the configuration"


@dataclasses.dataclass
class CopyError(ProcessError):
    """Could not copy a source directory to a temporary one."""

    origdir: pathlib.Path
    """The original directory in the source tree."""

    srcdir: pathlib.Path
    """The temporary directory to build and run the program in."""

    err: Exception
    """The error that occurred while running the program."""

    def __str__(self) -> str:
        """Provide a human-readable description of the error."""
        return f"Could not copy {self.origdir} to the temporary {self.srcdir}: {self.err}"


@dataclasses.dataclass
class NotSourceError(ProcessError):
    """The expected directory does not contain our configuration files."""

    origdir: pathlib.Path
    """The directory that was not as expected."""

    def __str__(self) -> str:
        """Provide a human-readable description of the error."""
        return f"Not a source directory: {self.origdir}"


def _run_commands(
    cfg: defs.Config, tag: str, commands: list[list[str]], *, cwd: pathlib.Path
) -> None:
    """Run a series of commands."""
    for cmd in commands:
        cmdstr = shlex.join(cmd)
        cfg.log.debug("- running `%(cmdstr)s`", {"cmdstr": cmdstr})
        try:
            subprocess.run(cmd, check=True, cwd=cwd, shell=False)  # noqa: S603
        except (OSError, subprocess.CalledProcessError) as err:
            raise RunError(tag, cmdstr, cwd, err) from err


def _prereqs_programs_ok(
    cfg: defs.Config, prereqs: defs.Prerequisites, srcdir: pathlib.Path
) -> bool:
    """If any programs have been specified as prerequisites, look for them in the search path."""
    programs: Final = prereqs.programs
    if not programs:
        return True

    def _check_prog(cmd: str) -> bool:
        """Check for the presence of a single program."""
        prog_path: Final = pathlib.Path(cmd)
        if len(prog_path.parts) != 1 or prog_path.is_absolute():
            target: Final = srcdir / prog_path
            if not os.access(target, os.F_OK | os.X_OK):
                cfg.log.info(
                    "- the %(target)s prerequisite program is not present",
                    {"target": shlex.quote(str(target))},
                )
                return False

            return True

        if shutil.which(cmd) is None:
            cfg.log.info(
                "- the %(cmd)s prerequisite external program is not present",
                {"cmd": shlex.quote(cmd)},
            )
            return False

        return True

    cfg.log.info("Checking for programs present: %(progs)s", {"progs": shlex.join(programs)})
    return all(_check_prog(prog) for prog in programs)


def _prereqs_ok(cfg: defs.Config, prog: str, progdef: defs.Program, srcdir: pathlib.Path) -> bool:
    """If any prerequisites have been specified, check whether they are present."""
    prereqs: Final = progdef.prerequisites
    if prereqs is None:
        return True

    cfg.log.info("Checking for the prerequisites of %(prog)s", {"prog": prog})

    if not _prereqs_programs_ok(cfg, prereqs, srcdir):
        return False

    return True


def build_and_test(cfg: defs.Config, prog: str) -> None:
    """Build and test a single program in a temporary directory."""
    progdef: Final = cfg.program.get(prog)
    if progdef is None:
        raise NoProgramError(prog)

    origdir: Final = cfg.topdir / prog
    if not origdir.is_dir():
        raise NotSourceError(origdir)

    with tempfile.TemporaryDirectory(prefix="check-build.") as tempd_obj:
        tempd: Final = pathlib.Path(tempd_obj)
        srcdir: Final = tempd / prog
        cfg.log.debug("Using %(tempd)s to build %(prog)s", {"tempd": tempd, "prog": prog})

        # Copy the source files
        cfg.log.debug("Copying %(origdir)s to %(srcdir)s", {"origdir": origdir, "srcdir": srcdir})
        try:
            shutil.copytree(origdir, srcdir, symlinks=True)
        except OSError as err:
            raise CopyError(origdir, srcdir, err) from err

        if not _prereqs_ok(cfg, prog, progdef, srcdir):
            if not cfg.force:
                cfg.log.info("SKIPPING %(prog)s, prerequisites not satisfied", {"prog": prog})
                raise defs.SkippedProgramError(prog)

            cfg.log.info("NOT SKIPPING %(prog)s despite unsatisfied prerequisites", {"prog": prog})

        cfg.log.info("Building %(prog)s in %(srcdir)s", {"prog": prog, "srcdir": srcdir})
        _run_commands(cfg, "build", progdef.commands.build, cwd=srcdir)

        cfg.log.info("Testing %(prog)s in %(srcdir)s", {"prog": prog, "srcdir": srcdir})
        _run_commands(cfg, "test", progdef.commands.test, cwd=srcdir)

        cfg.log.info(
            "Successfully built and run %(prog)s in %(srcdir)s", {"prog": prog, "srcdir": srcdir}
        )
