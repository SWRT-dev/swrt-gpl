local A = assert
local c = uci.cursor(os.getenv("CONFIG_DIR"))

A(c:set("network", "lan", "dns", {
	"ns1.king.banik.cz",
	"ns2.openwrt.org",
}))

local changes = c:changes()
A(changes.network.lan.dns[1] == "ns1.king.banik.cz")
A(changes.network.lan.dns[2] == "ns2.openwrt.org")
