# SPDX-FileCopyrightText: Peter Pentchev <roam@ringlet.net>
# SPDX-License-Identifier: BSD-2-Clause
"""Common utilities for the check-build library."""

import functools
import logging
import sys


@functools.lru_cache(maxsize=2)
def build_logger(*, verbose: bool) -> logging.Logger:
    """Build a logger that outputs messages to the standard output stream."""
    name_prefix = "" if verbose else "non-"
    logger = logging.getLogger(f"check-build/{name_prefix}verbose")
    logger.setLevel(logging.DEBUG)

    stdout_handler = logging.StreamHandler(stream=sys.stdout)
    stdout_handler.addFilter(lambda record: record.levelno == logging.INFO)
    stdout_handler.setLevel(logging.INFO)
    logger.addHandler(stdout_handler)

    stderr_handler = logging.StreamHandler(stream=sys.stderr)
    if verbose:
        stderr_handler.addFilter(lambda record: record.levelno != logging.INFO)
        stderr_handler.setLevel(logging.DEBUG)
    else:
        stderr_handler.setLevel(logging.WARNING)
    logger.addHandler(stderr_handler)

    return logger
