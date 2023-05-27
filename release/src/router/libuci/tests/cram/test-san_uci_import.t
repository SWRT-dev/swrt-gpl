check that uci import is producing expected results:

  $ [ -n "$BUILD_BIN_DIR" ] && export PATH="$BUILD_BIN_DIR:$PATH"
  $ export TEST_INPUTS="$TESTDIR/inputs"
  $ export FUZZ_CORPUS="$TESTDIR/../fuzz/corpus"

  $ for file in $(LC_ALL=C find $FUZZ_CORPUS -type f | sort ); do
  >   uci-san import -f $file; \
  > done
  uci-san: Parse error (package without name) at line 0, byte 68
  uci-san: I/O error
  uci-san: Parse error (invalid command) at line 0, byte 0
  uci-san: Parse error (invalid command) at line 1, byte 18
  uci-san: I/O error
  uci-san: I/O error
  [1]
