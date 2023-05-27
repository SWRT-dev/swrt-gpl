local c = uci.cursor(os.getenv("CONFIG_DIR"))
print(pcall(c.set, c, "network", "lan", "dns", {}))
