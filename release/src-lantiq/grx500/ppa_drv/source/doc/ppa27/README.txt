This patch contains following enhancements and fixes:

Changes:
=========
1) Feature supports
	DS-Lite (IPv4-to-6) tunnel acceleration
	Offchip bonding: on VRX200 PTM WAN mode
	Onchip bonding: on VRX200 PTM WAN mode
	xRX300 support
	Support PPA per-session hit counter handling
	Support packets up to 1530 byte (following TR-124)
	PPE FW QID map to switch queue

2) Bug fixes
   Fix wrong-acceleration hairpin related traffics which will need src/dst ip editing.
   Fix ifx_ppa_mc_group_update only update part action issue
   Fix xRX200 Hang issue in PPE FW ( wrongly parse pause frame )
   Fix socket cannot send out UDP packet issue from CPU path via re-allocate new skb buffer
	
	

List of files:
===============
Please run the command "0002-000-12-UGW-5.1.1-SW-CD-PPA-NAPT-patch.sh --list" to get a complete list of files.

