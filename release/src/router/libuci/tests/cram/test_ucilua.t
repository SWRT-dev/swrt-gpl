set LUA_CPATH and ucilua for convenience:

  $ export LC_ALL=C
  $ [ -n "$UCI_LUA" ] && export LUA_CPATH="$(dirname "$UCI_LUA")/?.so"
  $ alias ucilua="valgrind --quiet --leak-check=full lua -luci"

check available methods:

  $ ucilua -e 'table.foreach(uci,function(m) print(m) end)'
  add_history
  add_delta
  close
  set_confdir
  save
  cursor
  get_all
  foreach
  __gc
  delete
  set_savedir
  set
  revert
  get_savedir
  changes
  reorder
  get_confdir
  list_configs
  commit
  unload
  rename
  add
  load
  get

run basic Lua tests:

  $ cp -R "$TESTDIR/config" .
  $ export CONFIG_DIR=$(pwd)/config
  $ ucilua $TESTDIR/lua/basic.lua
  ---------------
  enabled: off
  .anonymous: false
  ipaddr: 2.3.4.5
  .index: 2
  .name: lan
  test: 123
  .type: interface
  ifname: eth0
  proto: static
  ---------------
  .name: wan
  .type: interface
  .index: 3
  enabled: on
  ifname: eth1
  proto: dhcp
  .anonymous: false
  aliases: c d
  nil\tuci: Parse error (EOF with unterminated ') at line 1, byte 18 (esc)
