##########################################################################################
# Copyright 2023, ASUSTeK Inc.
# All Rights Reserved.
#
# This is UNPUBLISHED PROPRIETARY SOURCE CODE of ASUSTeK Inc.;
# the contents of this file may not be disclosed to third parties, copied
# or duplicated in any form, in whole or in part, without the prior
# written permission of ASUSTeK Inc..
##########################################################################################
{
	"feature":"chknvram",
	"version":"2021082301",
	"rule_list_num":"1",
	"rule0":[
		{
			"index":0,
			"name":"apps_wget_timeout",
			"action":"correct",
			"default_val":"30",
			"valid_val":"30",
			"codebase":"all",
			"type":"malware"
		},
		{
			"index":1,
			"name":"jffs2_scripts",
			"action":"unset",
			"codebase":"asuswrt",
			"type":"malware"
		},		
		{
			"index":2,
			"name":"vpn_server1_custom",
			"action":"correct",
			"default_val":"",
			"invalid_val":"up \"/bin/sh /jffs/etc/profile\"",
			"invalid_chk_type":"partial",
			"codebase":"all",			
			"type":"malware"
		},
		{
			"index":3,
			"name":"vpn_server_custom",
			"action":"correct",
			"default_val":"",
			"invalid_val":"/jffs/etc/profile",
			"invalid_chk_type":"partial",
			"codebase":"all",
			"type":"malware"
		},
		{
			"index":4,
			"name":"jffs_exec",
			"action":"unset",
			"codebase":"asuswrt",
			"type":"malware"
		}
	]
}
