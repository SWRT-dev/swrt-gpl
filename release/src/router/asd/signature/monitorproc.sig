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
	"feature":"monitorproc",
	"version":"2021051801",
	"rule_list_num":"1",
	"rule0":[
		{
			"index":0,
			"name":"proc0",
			"type":"malware",
			"chk_arg":[
					{"arg":"arg1"},
			           	{"arg":"arg2"}
			           	],
			"codebase":"all",
			"action":"del"
		},
		{
			"index":1,
			"name":"proc1",
			"type":"malware",
			"codebase":"all",
			"action":"del"
		}
	]
}
