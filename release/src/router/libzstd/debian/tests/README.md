<!--
SPDX-FileCopyrightText: Peter Pentchev <roam@ringlet.net>
SPDX-License-Identifier: BSD-2-Clause
-->

# check-build -- check whether example programs can be built

The `check-build` tool can be used to build and test different
programs in a temporary build directory. The programs are listed in
a TOML configuration file, along with the commands to build and
test them.

## Configuration file structure

The file describing the subdirectories with the programs to be built has
the following structure:

### The "format" section

The `format` section contains a single table named `version` with two
keys, `major` and `minor`, both integers. The `major.minor` pair
specifies the version of the format specification for the `programs.toml`
file itself.

The only valid combination of values is `major = 0` and `minor = 1`.

### The "program" section

This table contains the definitions for the programs that `check-build`
should try to build and run. It may have one or more entries with
names corresponding to the names of the subdirectories where the files for
each program may be found.

#### The "program.NAME.prerequisites" section

This table currently only has a single valid key, `programs`: a list of
program names that will be looked for in the current execution search path.

If any of these prerequisites are not met, `check-build` will not
attempt to build the program, but it will report it as skipped, not failed.
This may be overridden by the `--force` command-line option
(or the `check_build.defs.Config.force` boolean variable): if it is
specified, then `check-build` will attempt to build and run the program
even if some of its prerequisites are not met.

#### The "program.NAME.commands" section

This table contains the lists of commands to be executed in order to
build and test the program. Each value is a list of commands to be
executed, and each command in its turn is a list of strings:
a program name and arguments to pass to it.

There are currently three valid keys in this table:

- `clean`: not used yet, since `check-build` always copies the program
  files to a newly-created temporary directory, so there should be no
  previously-existing build artifacts
- `build`: one or more commands to execute to build the program
- `test`: one or more commands to execute to make sure that the program
  that was just built is operational and correct

## The `check-build` command-line tool

The most common way to use `check-build` is through the command-line tool:
run `check-build`, specify the path to the TOML configuration file using
the `-c` option, optionally pass the `-v` / `--verbose` flag. By default,
the `check-build` tool will try to build and test all the programs
defined in the configuration file; pass one or more program names on
the command line to only build and test those.

## Examples

Given the following contents of the `ztest/programs.toml` configuration file:

    [format.version]
    major = 0
    minor = 1

    [program.pkg-make]
    executable = "ztest"

    [program.pkg-make.commands]
    clean = [["make", "clean"]]
    build = [["make"]]
    test = [["make", "test"]]

    [program.cmake]
    executable = "ztest"

    [program.cmake.commands]
    clean = [["make", "clean"]]
    build = [["cmake", "."], ["make"]]
    test = [["./ztest"]]

Build and test both programs:

    check-build -c ztest/programs.toml

Only build and test the CMake version with some `check-build` diagnostic messages:

    check-build -c ztest/programs.toml -v cmake
