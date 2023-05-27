local A = assert
local c = uci.cursor(os.getenv("CONFIG_DIR"))

c:foreach("network", "interface", function(s)
	print("---------------")
	for k, v in pairs(s) do
		print(k .. ': ' .. tostring(v))
	end
end)

local t = c:get_all("network")

A(t.wan.ifname == 'eth1')
A(t.wan.proto == 'dhcp')
A(c:get("network", "wan", "ifname") == "eth1")
A(c:get("network", "wan", "proto") == "dhcp")

A(t.lan.ifname == 'eth0')
A(t.lan.enabled == 'off')
A(c:get("network", "lan", "ifname") == "eth0")
A(c:get("network", "lan", "enabled") == "off")

A(c:set("network", "lan", "ifname", "eth5"))
A(c:get("network", "lan", "ifname") == "eth5")
A(c:revert("network"))
A(c:get("network", "lan", "ifname") == "eth0")

A(c:set("network", "lan", "ifname", "eth5"))
A(c:get("network", "lan", "ifname") == "eth5")
A(c:commit("network"))
A(c:set("network", "lan", "ifname", "eth0"))
A(c:revert("network"))
A(c:commit("network"))
A(c:get("network", "lan", "ifname") == "eth5")

A(c:set("network", "lan", "dns", {
	"ns1.king.banik.cz",
	"ns2.openwrt.org",
}))

local t = c:get("network", "lan", "dns")
A(#t == 2)
A(t[1] == "ns1.king.banik.cz")
A(t[2] == "ns2.openwrt.org")

print(c:get_all("broken"))
