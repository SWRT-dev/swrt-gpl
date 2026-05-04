# SPDX-FileCopyrightText: Peter Pentchev <roam@ringlet.net>
# SPDX-License-Identifier: BSD-2-Clause
"""Check whether some programs will build or not."""

import pathlib
import sys

import click

from check_build import defs
from check_build import parse
from check_build import process
from check_build import util


@click.command(name="check-build")
@click.option(
    "-c",
    "--config",
    required=True,
    type=click.Path(exists=True, dir_okay=False, resolve_path=True, path_type=pathlib.Path),
)
@click.option("-f", "--force", is_flag=True, default=False)
@click.option("-v", "--verbose", is_flag=True, default=False)
@click.argument("programs", type=str, nargs=-1)
def main(
    config: pathlib.Path, force: bool, verbose: bool, programs: tuple[str, ...]  # noqa: FBT001
) -> None:
    """Parse command-line arguments, build things."""
    try:
        cfg = parse.load_config(
            config=config, force=force, programs=programs, logger=util.build_logger(verbose=verbose)
        )
    except defs.CBuildError as err:
        sys.exit(f"Could not parse the configuration file: {err}")
    if not cfg.selected:
        sys.exit("No programs specified")

    skipped = []
    built_ok = []

    def build_or_skip(prog: str) -> None:
        """Build a program or skip it, updating the stats lists."""
        try:
            process.build_and_test(cfg, prog)
        except defs.SkippedProgramError:
            skipped.append(prog)
        else:
            built_ok.append(prog)

    for prog in cfg.selected:
        try:
            build_or_skip(prog)
        except defs.CBuildError as err:
            sys.exit(f"Could not build the {prog} program: {err}")

    cfg.log.info(
        "%(built_ok)d program%(built_plu)s built and tested successfully, %(skipped)d skipped",
        {
            "built_ok": len(built_ok),
            "built_plu": "" if len(built_ok) == 1 else "s",
            "skipped": len(skipped),
        },
    )


if __name__ == "__main__":
    main()
