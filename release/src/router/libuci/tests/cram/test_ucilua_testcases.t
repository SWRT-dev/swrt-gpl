set LUA_CPATH and ucilua for convenience:

  $ [ -n "$UCI_LUA" ] && export LUA_CPATH="$(dirname "$UCI_LUA")/?.so"
  $ alias ucilua="valgrind --quiet --leak-check=full lua -luci"

check that changes method doesnt leak memory:

  $ cp -R "$TESTDIR/config" .
  $ export CONFIG_DIR=$(pwd)/config
  $ ucilua $TESTDIR/lua/test_cases/changes_doesnt_leak.lua

check that set method with empty table value doesn't leak memory:

  $ cp -R "$TESTDIR/config" .
  $ export CONFIG_DIR=$(pwd)/config
  $ ucilua $TESTDIR/lua/test_cases/set_with_empty_table_doesnt_leak.lua
  false\tCannot set an uci option to an empty table value (esc)
