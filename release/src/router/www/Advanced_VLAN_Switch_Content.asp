<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml">
<html xmlns:v>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=utf-8" />
<meta HTTP-EQUIV="Pragma" CONTENT="no-cache">
<meta HTTP-EQUIV="Expires" CONTENT="-1">
<link rel="shortcut icon" href="images/favicon.png">
<link rel="icon" href="images/favicon.png">
<title><#Web_Title#> - VLAN Switch</title>
<link rel="stylesheet" type="text/css" href="index_style.css"> 
<link rel="stylesheet" type="text/css" href="form_style.css">
<link rel="stylesheet" type="text/css" href="other.css">
<script type="text/javascript" src="/state.js"></script>
<script type="text/javascript" src="/general.js"></script>
<script type="text/javascript" src="/popup.js"></script>
<script type="text/javascript" src="/help.js"></script>
<script type="text/javascript" src="/detect.js"></script>
<script type="text/javascript" src="/js/jquery.js"></script>
<script type="text/javascript" src="/js/httpApi.js"></script>
<style>
	li {
		font-family: monospace;
	}
	.role {
		color:#A4B7C3;
		margin-left: 10px;
	}
	.model {
		font-size:20px;
		font-weight:bold;
		margin-top:-5px;
	}
	.status_note {
		color:#A4B7C3;
		margin-top:18px;
	}

	.ul-align{
		margin-left:-25px;
	}

	.center-align{
		text-align:center;
	}

	.FormTable thead td{
		text-align: center;
	}

	select {
		width: 90%;
	}

	.port_status_table{
		text-align: center;
		width: 99%;
		margin: 0 auto;
	}
.port_status_bg{
	display: flex;
	align-items: flex-start;
	height: auto;
	position: relative;
	margin-top: 15px;
}
.port_status_bg:before{
	content: "";
	position: absolute;
	left: 0%;
	bottom: -3px;
	height: 1px;
	width: 95%;
	border-bottom: 1px solid #3E4E59;
}
.port_status_bg > div{
	display: flex;
	flex-wrap: wrap;
	height: auto;
	position: relative;
}
.port_status_bg .label_W_bg{
	width: auto;
	max-width: 92px;
	margin-right: 6px;
}
.port_status_bg .label_W_bg:before{
	position: absolute;
	content: "";
	right: -4px;
	top: -4%;
	height: 100%;
	border-left: 1px solid #3E4E59;
}
.port_status_bg .label_L_bg:before{
	position: absolute;
	content: "";
	top: -4%;
	height: 100%;
	border-right: 1px solid #3E4E59;
	left: -6px;
}
.port_status_bg .label_W_bg > div, .port_status_bg .label_L_bg > div{
	display: flex;
	width: 46px;
	height: auto;
	flex-direction: column;
	align-items: center;
	margin-bottom: 8px;
}
.port_status_bg .label_L_bg{
	margin-left: 3px;
	flex-grow: 1;
	flex-shrink: 0;
	flex-basis: 60%;
}
.port_status_bg .port_icon{
	background-image: url(/images/New_ui/amesh/port_status.svg);
	background-size: 100px;
	width: 30px;
	height: 26px;
	display: flex;
	justify-content: center;
	align-items: center;
	border-radius: 2px;
	position: relative;
}
.port_status_bg .port_icon.conn{
	background-position: 0px 0px;
}
.port_status_bg .port_icon.warn{
	background-position: 0px -43px;
}
.port_status_bg .port_icon.unplug{
	background-position: 0px -86px;
}
</style>
<script>
var VLAN_Profile_select = [];
var VLAN_port_status = {};

var str_port_binding_note_tmp = "";
var str_port_binding_note = stringSafeGet("<#VLAN_port_binding_note#>");
var str_modify = stringSafeGet(" <#web_redirect_suggestion_modify#>");

var rate_map = [
		{
			value: 'X',
			text: '<#Status_Unplugged#>'
		},
		{
			value: '10',
			text: '10 Mbps'
		},
		{
			value: '100',
			text: '100 Mbps'
		},
		{
			value: '1000',
			text: '1 Gbps'
		},
		{
			value: '2500',
			text: '2.5 Gbps'
		},
		{
			value: '5000',
			text: '5 Gbps'
		},
		{
			value: '10000',
			text: '10 Gbps'
		}
];

httpApi.get_port_status = function(mac) {
	var retData = {};

	var capability_map = [{
			type: 'WAN',
			bit: 0
		},
		{
			type: 'LAN',
			bit: 1
		},
		{
			type: 'GAME',
			bit: 2
		},
		{
			type: 'PLC',
			bit: 3
		},
		{
			type: 'WAN2',
			bit: 4
		},
		{
			type: 'WAN3',
			bit: 5
		},
		{
			type: 'SFPP',
			bit: 6
		},
		{
			type: 'USB',
			bit: 7
		},
		{
			type: 'MOBILE',
			bit: 8
		},
		{
			type: 'IPTV_BRIDGE',
			bit: 26
		},
		{
			type: 'IPTV_VOIP',
			bit: 27
		},
		{
			type: 'IPTV_STB',
			bit: 28
		},
		{
			type: 'DUALWAN_SECONDARY_WAN',
			bit: 29
		},
		{
			type: 'DUALWAN_PRIMARY_WAN',
			bit: 30
		},
	];

	var rate_map_USB = [{
			value: '480',
			text: 'USB2.0'
		},
		{
			value: '5000',
			text: 'USB3.0'
		},
		{
			value: '10000',
			text: 'USB3.1'
		},
		{
			value: '20000',
			text: 'USB3.2'
		},
	];

	function get_port_status_handler(response) {
		var response_temp = JSON.parse(JSON.stringify(response));
		var port_info_temp = {};
		if (response_temp["port_info"] != undefined) {
			if (response_temp["port_info"][mac] != undefined) {
				port_info_temp = {
					"WAN": [],
					"LAN": [],
					"USB": []
				};
				var port_info = response_temp["port_info"][mac];
				$.each(port_info, function(index, data) {
					var label = index.substr(0, 1);
					var label_idx = index.substr(1, 1);
					data["label"] = label;
					data["label_priority"] = ((label == "W") ? 1 : ((label == "L") ? 2 : 3));
					data["label_idx"] = label_idx;
					data["label_port_name"] = (function() {
						if (data.cap_support.WAN) {
							if (label_idx == "0")
								return "WAN";
							else
								return "WAN " + label_idx;
						} else if (data.cap_support.LAN) {
							return "LAN " + label_idx;
						} else if (data.cap_support.USB) {
							return "USB";
						}
					})();
					var link_rate_data = rate_map.filter(function(item, index, array) {
						return (item.value == data.link_rate);
					})[0];

					data["link_rate_text"] = "";
					if (link_rate_data != undefined) {
						data["link_rate_text"] = link_rate_data.text;
					}

					if (data.cap_support.USB) {
						var max_rate_data = rate_map_USB.filter(function(item, index, array) {
							return (item.value == data.max_rate);
						})[0];
					} else {
						var max_rate_data = rate_map.filter(function(item, index, array) {
							return (item.value == data.max_rate);
						})[0];
					}

					data["max_rate_text"] = "";
					if (max_rate_data != undefined) {
						data["max_rate_text"] = max_rate_data.text;
						data["special_port_name"] = "";
						if (data["cap_support"]["GAME"] == true) {
							data["special_port_name"] = "Gaming Port";
						} else {
							if (data.cap_support.USB) {
								data["special_port_name"] = max_rate_data.text;
							} else {
								var max_rate = parseInt(max_rate_data.value);
								if (max_rate > 1000) {
									data["special_port_name"] = max_rate_data.text.replace(" Gbps", "");
									if (max_rate == 10000) {
										if (data["cap_support"]["SFPP"] == true)
											data["special_port_name"] = data["special_port_name"] + "G SFP+";
										else
											data["special_port_name"] = data["special_port_name"] + "G baseT";
									} else
										data["special_port_name"] = data["special_port_name"] + "G";
								}
							}
						}
					}

					var link_rate = isNaN(parseInt(data.link_rate)) ? 0 : parseInt(data.link_rate);
					var max_rate = isNaN(parseInt(data.max_rate)) ? 0 : parseInt(data.max_rate);
					data["link_rate_status"] = 1; //normal
					if (max_rate == 2500)
						max_rate = 1000;

					if (data.is_on == "1" && link_rate < max_rate)
						data["link_rate_status"] = 0; //abnormal

					var sort_key = "";
					if (data.cap_support.DUALWAN_PRIMARY_WAN || data.cap_support.DUALWAN_SECONDARY_WAN) {
						port_info_temp["WAN"].push(data);
						sort_key = "WAN";
					} else if (data.cap_support.USB) {
						port_info_temp['USB'].push(data);
						sort_key = 'USB';
					} else {
						port_info_temp["LAN"].push(data);
						sort_key = "LAN";
					}

					port_info_temp[sort_key].sort(function(a, b) {
						//first compare label priority, W>L>U
						var a_label_priority = parseInt(a.label_priority);
						var b_label_priority = parseInt(b.label_priority);
						var label_priority = ((a_label_priority == b_label_priority) ? 0 : ((a_label_priority > b_label_priority) ? 1 : -1));
						if (label_priority != 0) {
							return label_priority;
						} else { //second compare label idx
							var a_label_idx = parseInt(a.label_idx);
							var b_label_idx = parseInt(b.label_idx);
							return ((a_label_idx == b_label_idx) ? 0 : ((a_label_idx > b_label_idx) ? 1 : -1));
						}
					});
				});
			}
		}

		return response;
	}

	$.ajax({
		url: "/get_port_status.cgi?node_mac=" + mac,
		dataType: 'json',
		async: false,
		error: function() {},
		success: function(response) {
			if (response["port_info"] != undefined) {
				if (response["port_info"][mac] != undefined) {
					var port_info = response["port_info"][mac];

					//temporary to handle 'cd_good_to_go', wait it move to new JSON structure
					if (port_info.cd_good_to_go !== undefined) {
						delete port_info.cd_good_to_go;
					}

					$.each(port_info, function(index, data) {
						var cap = data.cap;
						if (data["cap_support"] == undefined)
							data["cap_support"] = {};
						$.each(capability_map, function(index, capability_item) {
							data["cap_support"][capability_item.type] = ((parseInt(cap) & (1 << parseInt(capability_item.bit))) > 0) ? true : false;
						});
					});
				}
			}

			retData = response;
		}
	});

	return get_port_status_handler(retData);
}

var convertRulelistToJson = function(attrArray, rulelist) {
	var rulelist_json = [];

	var each_rule = rulelist.split("<");
	var convertAtoJ = function(rule_array) {
		var rule_json = {}
		$.each(attrArray, function(index, value) {
			rule_json[value] = rule_array[index];
		});
		return rule_json;
	}

	$.each(each_rule, function(index, value) {
		if (value != "") {
			var one_rule_array = value.split(">");
			var one_rule_json = convertAtoJ(one_rule_array);
			if (!one_rule_json.error) rulelist_json.push(one_rule_json);
		}
	});

	return rulelist_json;
}

var add_back_missing_apg_dut_list = function(dut_list) {
	$.each(meshList, function(idx, meshNodeMac) {
		if (dut_list.indexOf(meshNodeMac) == -1) dut_list += "<" + meshNodeMac + ">>"
	})
	return dut_list;
}

apgRuleTable = [
	"mac",
	"wifiband",
	"lanport"
]

vlanRuleTable = [
	"vlan_idx",
	"vid",
	"port_isolation"
];

sdnRuleTable = [
	"idx",
	"sdn_name",
	"sdn_enable",
	"vlan_idx",
	"subnet_idx",
	"apg_idx",
	"vpnc_idx",
	"vpns_idx",
	"dns_filter_idx",
	"urlf_idx",
	"nwf_idx",
	"cp_idx",
	"gre_idx",
	"firewall_idx",
	"kill_switch",
	"access_host_service",
	"wan_idx",
	"pppoe-relay"
];

var orig_wans_dualwan = decodeURIComponent(httpApi.nvramCharToAscii(["wans_dualwan"]).wans_dualwan);
var orig_wans_lanport = decodeURIComponent(httpApi.nvramCharToAscii(["wans_lanport"]).wans_lanport);
var orig_aggregation = httpApi.nvramGet(["bond_wan","wanports_bond","lacp_enabled"], true);
var original_stb_port = httpApi.nvramGet(["switch_stb_x","iptv_stb_port","iptv_voip_port","iptv_bridge_port"], true);

var sdn_rl = decodeURIComponent(httpApi.nvramCharToAscii(["sdn_rl"]).sdn_rl)
var vlan_rl = decodeURIComponent(httpApi.nvramCharToAscii(["vlan_rl"]).vlan_rl)
var sdn_rl_json = convertRulelistToJson(sdnRuleTable, sdn_rl);
var vlan_rl_json = convertRulelistToJson(vlanRuleTable, vlan_rl);
var vlan_rl_vid_array = [];
var apg_dutList = {};
var meshNodelist = httpApi.hookGet("get_cfg_clientlist", true);
var meshList = [];

$.each(meshNodelist, function(idx, meshNode) {
	if (httpApi.aimesh_get_node_lan_port(meshNode).length == 0) return true;

	var oneNode = {};
	oneNode.macAddress = meshNode.mac;
	oneNode.model = meshNode.model_name;
	oneNode.location = meshNode.alias;
	oneNode.connection = meshNode.online;
	oneNode.port = [];

	if (httpApi.get_port_status(oneNode.macAddress).node_info != undefined) { //Viz add for disconnected node
		var oneNode_port_status = httpApi.get_port_status(oneNode.macAddress).port_info[oneNode.macAddress];
		for (var key in oneNode_port_status) {
			if (key.indexOf("L") == -1) continue;

			var oneLanPort = {};
			var portIndex = 0;
			portIndex = key.charAt(key.length - 1);
			oneLanPort.portLabel = key.replace("L", "LAN ");
			oneLanPort.status = oneNode_port_status[key].is_on;
			var link_rate_tmp = rate_map.filter(function(item, index, array) {
				return (item.value == oneNode_port_status[key].link_rate);
			})[0];
			link_rate_tmp = (link_rate_tmp == undefined)? rate_map[0]:link_rate_tmp;	//<#Status_Unplugged#>
			oneLanPort.speed = link_rate_tmp.text;
			var max_rate_tmp = rate_map.filter(function(item, index, array) {
				return (item.value == oneNode_port_status[key].max_rate);
			})[0];
			max_rate_tmp = (max_rate_tmp == undefined)? rate_map[0]:max_rate_tmp;	//<#Status_Unplugged#>
			oneLanPort.max_rate = max_rate_tmp.text;
			oneLanPort.detail = oneNode_port_status[key];
			oneLanPort.profile = "0";
			oneLanPort.mode = "";
			oneLanPort.wans_lanport = "";
			oneLanPort.wan_bonding = "";
			oneLanPort.link_aggregation = "";
			oneLanPort.iptv = "";

			//oneNode.port.push(oneLanPort);
			oneNode.port[portIndex - 1] = oneLanPort;
		}
	}

	VLAN_port_status[meshNode.mac] = oneNode;
	meshList.push(meshNode.mac)
})

//Collect VLAN_Profile_select from sdn_rl
$.each(sdn_rl_json, function(idx, _sdn_rl) {
	var oneRule = {};

	var vlanArrayIdx = -1;
	$.each(vlan_rl_json, function(idx, _vlan_rl) {
		if (_sdn_rl.vlan_idx == _vlan_rl.vlan_idx) vlanArrayIdx = idx;
	})
	if (vlanArrayIdx < 0) return true;

	// oneRule.name = _sdn_rl.sdn_name;
	oneRule.name = decodeURIComponent(httpApi.nvramCharToAscii(["apg" + _sdn_rl.apg_idx + "_ssid"])["apg" + _sdn_rl.apg_idx + "_ssid"]);
	oneRule.vid = vlan_rl_json[vlanArrayIdx].vid;
	oneRule.iso = vlan_rl_json[vlanArrayIdx].port_isolation;
	oneRule.apgIdx = _sdn_rl.apg_idx;

	var apg_dut_list = decodeURIComponent(httpApi.nvramCharToAscii([
		"apg" + _sdn_rl.apg_idx + "_dut_list"
	])["apg" + _sdn_rl.apg_idx + "_dut_list"])

	apg_dut_list = add_back_missing_apg_dut_list(apg_dut_list);

	apg_dutList["apg" + _sdn_rl.apg_idx + "_dut_list"] = convertRulelistToJson(apgRuleTable, apg_dut_list)

	VLAN_Profile_select.push(oneRule);
})

//Collect VLAN_Profile_select from vlan_rl
$.each(vlan_rl_json, function(idx, _vlan_rl) {
	var match_count=0;
	for(var y=0;y<VLAN_Profile_select.length;y++){
		if(VLAN_Profile_select[y].vid == _vlan_rl.vid)
			match_count++;
	}
	if(match_count==0){
		var oneRule = {};
		oneRule.name = _vlan_rl.vid;
		oneRule.vid = _vlan_rl.vid;
		oneRule.iso = _vlan_rl.port_isolation;
		oneRule.apgIdx = "";

		VLAN_Profile_select.push(oneRule);
	}
})

for (var key in apg_dutList) {
	var dutList = apg_dutList[key];
	var apgIdx = key.replace("apg", "").replace("_dut_list", "");
	var vlanId = "";
	var vlanIdx = "";

	$.each(sdn_rl_json, function(idx, sdn) {
		if (sdn.apg_idx == apgIdx) {
			vlanId = sdn.vlan_idx;
			return false;
		}
	})

	$.each(vlan_rl_json, function(idx, vlan) {
		if (vlan.vlan_idx == vlanId) {
			vlanIdx = vlan.vid;
			return false;
		}
	})

	$.each(dutList, function(idx, node) {
		if (node.lanport != "") {
			var lanPortArray = node.lanport.split(",");
			for (var i = 0; i < lanPortArray.length; i++) {
				if(VLAN_port_status[node.mac].port.length > 0){
					VLAN_port_status[node.mac].port[lanPortArray[i] - 1].profile = vlanIdx;
				}
			}
		}
	})

	for (var key in apg_dutList) {
		var dutList = apg_dutList[key];
		var lanport_array = [];
		$.each(dutList, function(key, node) {
			lanport_array = node.lanport.split(",");
			for (var a = 0; a < lanport_array.length; a++) {
				if (lanport_array[a] != "" && VLAN_port_status[node.mac].port.length > 0) {
					VLAN_port_status[node.mac].port[lanport_array[a] - 1].mode = "Access";
				}
			}
		})
	}
}

$.each(vlan_rl_json, function(idx, vlan) {
	vlan_rl_vid_array.push(vlan.vid);
});

var vlan_trunklist_tmp = decodeURIComponent(httpApi.nvramCharToAscii(["vlan_trunklist"]).vlan_trunklist);
var vlan_trunklist_array = vlan_trunklist_tmp.split("<");
var vlan_trunklist_port_array = [];

if (vlan_trunklist_array.length > 1) {
	for (var b = 1; b < vlan_trunklist_array.length; b++) {
		vlan_trunklist_port_array = vlan_trunklist_array[b].split(">");
		for (var c = 0; c < vlan_trunklist_port_array.length; c++) {
			if (c > 0) {
				vlan_rl_vid_array_tmp = vlan_rl_vid_array;
				vlan_trunklist_port = vlan_trunklist_port_array[c].split("#")[0];
				vlan_trunklist_port_vid_array = vlan_trunklist_port_array[c].split("#")[1].split(",");
				
				if(VLAN_port_status[vlan_trunklist_port_array[0]].port[parseInt(vlan_trunklist_port) - 1]){
					VLAN_port_status[vlan_trunklist_port_array[0]].port[parseInt(vlan_trunklist_port) - 1].mode = "Trunk";
					VLAN_port_status[vlan_trunklist_port_array[0]].port[parseInt(vlan_trunklist_port) - 1].profile = vlan_trunklist_port_vid_array;
				}
				else{
					console.log(vlan_trunklist_port_array[0]+": Not exist.");
				}
			}
		}
	}
}

var vlan_rl_vid_array_tmp = [];
var reportedNodeNumber = 0;

var get_VLAN_Status;

function initial() {
	show_menu();
	var vlan_switch_array = { "VLAN" : ["VLAN", "Advanced_VLAN_Switch_Content.asp"], "Profile" : ["Profile", "Advanced_VLAN_Profile_Content.asp"]};
	$('#divSwitchMenu').html(gen_switch_menu(vlan_switch_array, "VLAN"));

	get_VLAN_Status = update_VLAN_ports();
	gen_VLAN_port_table(get_VLAN_Status);

	setInterval(check_get_port_status_update, 3000);
}

function check_get_port_status_update(){
	var nodeInfo = [];

	$.ajax({
		url: "/get_port_status.cgi?node_mac=all",
		dataType: 'json',
		success: function(response){
			for(var node in response.node_info){nodeInfo.push(node)}
			if(reportedNodeNumber == 0){
				reportedNodeNumber = nodeInfo.length;
			}
			else if(nodeInfo.length != reportedNodeNumber){
				top.location.href = top.location.href;
			}
		}
	})
}

function arrayRemove(arr, value) {
	return arr.filter(function(ele) {
		return ele != value;
	});
}

function update_VLAN_ports() { //ajax update VLAN ports	

	return VLAN_port_status; //no update temporarily
}

function gen_VLAN_port_table(port_profile) {
	var mesh_mac = Object.keys(port_profile);

	var role_str, model_str, macaddr_str, loc_str, connected_flag = "";
	var port_length = 0;

	if (mesh_mac.length == 0) {
		$('#tableCAP').hide();
	}
	else {

		$('#tableCAP').empty().show();

		for (var i = 0; i < mesh_mac.length; i++) { //[for loop] mesh system start

			role_str = (i == 0) ? "<#Device_type_02_RT#>" : "<#AiMesh_Node#>";
			model_str = port_profile[mesh_mac[i]].model;
			macaddr_str = port_profile[mesh_mac[i]].macAddress;
			loc_str = port_profile[mesh_mac[i]].location;
			connected_flag = port_profile[mesh_mac[i]].connection;
			port_length = port_profile[mesh_mac[i]].port.length;


			if (i > 0) { //Node
				var $div_system_node = $("<div>").attr("id", "tableNode_" + mesh_mac[i]).appendTo($("#tableCAP"));
				var $target_div = $div_system_node;
				var table_id = "tableNode_" + mesh_mac[i] + "_id";
			} else { //CAP
				var $target_div = $("#tableCAP");
				var table_id = "tableCAP_id";
			}

			var $system_model_bg = $("<div>")
				.addClass("formfontdesc model")
				.html(model_str)
				.appendTo($target_div)
				.hide();

			var $system_role_bg = $("<span>")
				.addClass("formfontdesc role")
				.html(role_str)
				.appendTo($system_model_bg)
				.hide();

			//table head
			var $port_table_bg = $("<table>").addClass("FormTable").appendTo($target_div);

			$port_table_bg.attr("id", table_id)
				.attr('border', '1')
				.attr('cellpadding', '4')
				.attr('cellspacing', '0')
				.css('width', '100%');

			var $port_table_thead = $("<thead>").appendTo($port_table_bg);
			var $port_table_thead_tr = $("<tr>").appendTo($port_table_thead);
			var $port_table_thead2 = $("<td>")
				.attr('colSpan', '6')
				.appendTo($port_table_thead_tr)
				.css({
					"text-align": "left",
					"padding-left": "10px"
				})

			if (i > 0) {
				$port_table_bg.css({
				    "margin-top": "-30px"
				})
				$port_table_thead2.html("<#AiMesh_Node#>");
			} else {
				$port_table_thead2.html("<#AiMesh_Router#>");
			}

			//table content : status
			var $port_table_tr_status = $("<tr>").appendTo($port_table_bg);
			var $port_table_th_status = $("<th>").appendTo($port_table_tr_status);
			$port_table_th_status.css({"width":"20%"});
			$port_table_th_status.html("<ul class='ul-align'><li>" + model_str + "</li><li>" + macaddr_str + "</li></ul>");

			//table content : mode
			var $port_table_tr_mode = $("<tr>").appendTo($port_table_bg);
			var $port_table_th_mode = $("<th>").appendTo($port_table_tr_mode);
			$port_table_th_mode.css({"width":"20%"});
			$port_table_th_mode.html("<#DSL_Mode#>");

			//table content : profile
			var $port_table_tr_profile = $("<tr>").appendTo($port_table_bg);
			var $port_table_th_profile = $("<th>").appendTo($port_table_tr_profile);
			$port_table_th_profile.css({"width":"20%"});
			$port_table_th_profile.html("SDN (VLAN) Profile"); /* Untranslated */			

			//Note: modify array
			var wan_aggregation_array = [];
			var link_aggregation_array = [];
			var iptv_array = [];

			if (port_length > 0) { //With port_info
				//ports 1st row
				for (var j = 0; j < 4; j++) {

					if (j < port_length) {

						//gen port status 
						var $port_table_td_status = $("<td>").appendTo($port_table_tr_status);
						$port_table_td_status.css("width", "100px");
						//gen profile selector
						var $port_table_td_profile = $("<td>").appendTo($port_table_tr_profile);
						var $port_table_td_mode = $("<td>").appendTo($port_table_tr_mode);

						var $port_status_title, $port_status_icon, $port_status_idx, $port_status_note;
						var $port_max_rate;
						var aggressive_tag = 0;

						$port_status_title = $("<div>").appendTo($port_table_td_status);
						$port_status_title
							.css({
								"width": "30px",
								"border-bottom": "0px #000 solid !important",
								"margin": "0 auto"
							})
							.addClass("port_status_bg")
							.addClass("port_status_bg_vlan")

						var port_rate_txt = "";
						if (port_profile[mesh_mac[i]].port[j].speed) {
							port_rate_txt = port_profile[mesh_mac[i]].port[j].speed;
						}

						var port_max_rate_txt = "";
						if (port_profile[mesh_mac[i]].port[j].max_rate) {
							port_max_rate_txt = port_profile[mesh_mac[i]].port[j].max_rate;
						}

						switch (port_profile[mesh_mac[i]].port[j].status) {
							case '0': //unplug

								//title: speed
								$port_status_title.attr("title", port_rate_txt);
								//port_icon unplug
								$port_status_icon = $("<div>").appendTo($port_status_title);
								$port_status_icon.addClass("port_icon unplug");
								$("<div>").html(port_max_rate_txt).appendTo($port_table_td_status);
								break;
							case '1': //connected

								//title: speed
								$port_status_title.attr("title", port_rate_txt);
								//port_icon conn
								$port_status_icon = $("<div>").appendTo($port_status_title);
								$port_status_icon.addClass("port_icon conn");
								$("<div>").html(port_max_rate_txt).appendTo($port_table_td_status);
								break;
							case '2': //warning

								//title: speed
								$port_status_title.attr("title", port_rate_txt);
								//port_icon warn
								$port_status_icon = $("<div>").appendTo($port_status_title);
								$port_status_icon.addClass("port_icon warn");
								$("<div>").html(port_max_rate_txt).appendTo($port_table_td_status);
								break;
							default: //unplug

								//port_icon unplug
								$port_status_icon = $("<div>").appendTo($port_status_title);
								$port_status_icon.addClass("port_icon unplug");
								break;
						}

						//lan_idx
						$port_status_idx = $("<div>").appendTo($port_status_icon);
						$port_status_idx.html(j + 1)
							.addClass("status_idx");

						//wans_lanport
						if( i == 0 && orig_wans_dualwan.indexOf("lan") >= 0 && orig_wans_lanport == (j+1) )	//for CAP only
						{
							port_profile[mesh_mac[i]].port[j].wans_lanport = '1';
							aggressive_tag = 1;
							$port_status_note = $("<div>").appendTo($port_status_icon);
							$port_status_note.html("Aggregation") /* Untranslated */
								.addClass("status_note");							
						}

						//wan_bonding
						if( i == 0 && j == 3 && orig_aggregation.bond_wan == 1 ){	//CAP only because have no info for RE, general LAN4
							port_profile[mesh_mac[i]].port[j].wan_bonding = '1';
						}	

						//link aggregation
						if ( i == 0 && (j == 0 || j == 1)  && orig_aggregation.lacp_enabled == 1 ) {	//CAP only because have no info for RE, general LAN1 & LAN2
							port_profile[mesh_mac[i]].port[j].link_aggregation = '1';
						}

						//iptv
						if( i == 0 && original_stb_port.switch_stb_x != 0){	//with iptv settings
							if(original_stb_port.iptv_stb_port.indexOf(j+1) > 0 || original_stb_port.iptv_voip_port.indexOf(j+1) > 0 || original_stb_port.iptv_bridge_port.indexOf(j+1) > 0){
								port_profile[mesh_mac[i]].port[j].iptv = '1';
							}
						}

						//Aggregation tag
						if (port_profile[mesh_mac[i]].port[j].wan_bonding == '1' || port_profile[mesh_mac[i]].port[j].link_aggregation == '1' ||
							port_profile[mesh_mac[i]].port[j].iptv == '1') {

							aggressive_tag = 1;
							$port_status_note = $("<div>").appendTo($port_status_icon);
							$port_status_note.html("Aggregation") /* Untranslated */
								.addClass("status_note");
							//Note:Modify port array
							if (port_profile[mesh_mac[i]].port[j].wan_bonding == '1') {
								wan_aggregation_array.push(j + 1);
							}
							if (port_profile[mesh_mac[i]].port[j].link_aggregation == '1') {
								link_aggregation_array.push(j + 1);
							}
							if (port_profile[mesh_mac[i]].port[j].iptv == '1') {
								iptv_array.push(j + 1);
							}
						}

						$port_table_td_mode.empty();
						$port_table_td_mode.append(insert_vlan_mode_selector(macaddr_str, j + 1, port_profile[mesh_mac[i]].port[j].mode, aggressive_tag));

						$port_table_td_profile.empty();
						$port_table_td_profile.append(insert_vlan_profile_selector(macaddr_str, j + 1, port_profile[mesh_mac[i]].port[j].profile, aggressive_tag));
					}
				}

				if (port_length > 4) { //ports 2nd row
					//table content : status
					var $port_table_tr_status = $("<tr>").appendTo($port_table_bg);
					var $port_table_th_status = $("<th>").appendTo($port_table_tr_status);
					$port_table_th_status.css({"width":"20%"});
					$port_table_th_status.html("<ul class='ul-align'><li>" + macaddr_str + "</li><li>" + loc_str + "</li></ul>");

					//table content : mode
					var $port_table_tr_mode = $("<tr>").appendTo($port_table_bg);
					var $port_table_th_mode = $("<th>").appendTo($port_table_tr_mode);
					$port_table_th_mode.css({"width":"20%"});
					$port_table_th_mode.html("<#DSL_Mode#>");

					//table content : profile
					var $port_table_tr_profile = $("<tr>").appendTo($port_table_bg);
					var $port_table_th_profile = $("<th>").appendTo($port_table_tr_profile);
					$port_table_th_profile.css({"width":"20%"});
					$port_table_th_profile.html("SDN (VLAN) Profile"); /* Untranslated */

					//ports 2nd row
					for (var j = 4; j < 8; j++) {

						if (j < port_length) {

							//gen port status
							var $port_table_td_status = $("<td>").appendTo($port_table_tr_status);
							$port_table_td_status.css("width", "100px");
							//gen profile selector
							var $port_table_td_profile = $("<td>").appendTo($port_table_tr_profile);
							var $port_table_td_mode = $("<td>").appendTo($port_table_tr_mode);

							var $port_status_title, $port_status_icon, $port_status_idx, $port_status_note;
							var aggressive_tag = 0;


							//console.log(port_profile[mesh_mac[i]].port[j].status);
							$port_status_title = $("<div>").appendTo($port_table_td_status);
							$port_status_title
								.css({
									"width": "30px",
									"border-bottom": "0px #000 solid !important",
									"margin": "0 auto"
								})
								.addClass("port_status_bg")
								.addClass("port_status_bg_vlan")

							var port_rate_txt = "";
							if (port_profile[mesh_mac[i]].port[j].speed) {
								port_rate_txt = port_profile[mesh_mac[i]].port[j].speed;
							}

							var port_max_rate_txt = "";
							if (port_profile[mesh_mac[i]].port[j].max_rate) {
								port_max_rate_txt = port_profile[mesh_mac[i]].port[j].max_rate;
							}

							switch (port_profile[mesh_mac[i]].port[j].status) {
								case '0': //unplug

									//title: speed
									$port_status_title.attr("title", port_rate_txt);
									//port_icon unplug
									$port_status_icon = $("<div>").appendTo($port_status_title);
									$port_status_icon.addClass("port_icon unplug");
									$("<div>").html(port_max_rate_txt).appendTo($port_table_td_status);
									break;
								case '1': //connected

									//title: speed
									$port_status_title.attr("title", port_rate_txt);
									//port_icon conn
									$port_status_icon = $("<div>").appendTo($port_status_title);
									$port_status_icon.addClass("port_icon conn");
									$("<div>").html(port_max_rate_txt).appendTo($port_table_td_status);
									break;
								case '2': //warning

									//title: speed
									$port_status_title.attr("title", port_rate_txt);
									//port_icon warn
									$port_status_icon = $("<div>").appendTo($port_status_title);
									$port_status_icon.addClass("port_icon warn");
									$("<div>").html(port_max_rate_txt).appendTo($port_table_td_status);
									break;
								default: //unplug

									//port_icon unplug
									$port_status_icon = $("<div>").appendTo($port_status_title);
									$port_status_icon.addClass("port_icon unplug");
									break;
							}
							//lan_idx
							$port_status_idx = $("<div>").appendTo($port_status_icon);
							$port_status_idx.html(j + 1)
								.addClass("status_idx");

							//Aggregation tag
							if (port_profile[mesh_mac[i]].port[j].wan_bonding == '1' || port_profile[mesh_mac[i]].port[j].link_aggregation == '1' ||
								port_profile[mesh_mac[i]].port[j].iptv == '1') {
								aggressive_tag = 1;
								$port_status_note = $("<div>").appendTo($port_status_icon);
								$port_status_note.html("Aggregation") /* Untranslated */
									.addClass("status_note");
								//Note:Modify port array
								if (port_profile[mesh_mac[i]].port[j].wan_bonding == '1') {
									wan_aggregation_array.push(j + 1);
								}
								if (port_profile[mesh_mac[i]].port[j].link_aggregation == '1') {
									link_aggregation_array.push(j + 1);
								}
								if (port_profile[mesh_mac[i]].port[j].iptv == '1') {
									iptv_array.push(j + 1);
								}
							}

							$port_table_td_mode.empty();
							$port_table_td_mode.append(insert_vlan_mode_selector(macaddr_str, j + 1, port_profile[mesh_mac[i]].port[j].mode, aggressive_tag));

							$port_table_td_profile.empty();
							$port_table_td_profile.append(insert_vlan_profile_selector(macaddr_str, j + 1, port_profile[mesh_mac[i]].port[j].profile, aggressive_tag));
						}

					}
				}
			} 
			else{ //without port_profile

				if (connected_flag == "0") {
					$("<div>").html("<#Disconnected#>")
						.css("color", "#FFCC00")
						.css('margin-top', '30px')
						.appendTo($port_table_tr_status);
				}
				else{
					$("<img>").attr("src", "images/InternetScan.gif")
						.css('width', '30px')
						.css('height', '30px')
						.css('margin-top', '30px')
						.appendTo($port_table_tr_status);

					setTimeout("refreshpage()", 30000);
				}
			}

			//Note for LAN port
			var note_wans_lanport = "";
			var note_wan_aggregation = "";
			var note_link_aggregation = "";
			var note_iptv = "";			

			if( i == 0 && orig_wans_dualwan.indexOf("lan") >= 0 && orig_wans_lanport != "")	//for CAP only
			{
				note_wans_lanport += "<b>LAN ";
				note_wans_lanport += orig_wans_lanport;
				note_wans_lanport += "</b>: ";
				str_port_binding_note_tmp = str_port_binding_note.replace("%@", "<#dualwan#>");
				note_wans_lanport += str_port_binding_note_tmp;
				note_wans_lanport += str_modify.replace('<a>', '<a id="modify_wans">');
				$("<div>").html(note_wans_lanport).appendTo($target_div).css('text-align','left');
				$("#modify_wans").attr("href", "Advanced_WANPort_Content.asp")
					.css("color", "#FFCC00")
					.css("cursor", "pointer")
					.css("text-decoration", "underline");
			}

			if( i == 0 && orig_aggregation.bond_wan == 1 ){	//CAP only because have no info for RE
				if (wan_aggregation_array.length) {
					for (var b = 0; b < wan_aggregation_array.length; b++) {
						if (b > 0)
							note_wan_aggregation += " / ";
						note_wan_aggregation += "<b>LAN ";
						note_wan_aggregation += wan_aggregation_array[b];
						note_wan_aggregation += "</b>";
					}
					note_wan_aggregation += ": ";
					str_port_binding_note_tmp = str_port_binding_note.replace("%@", "<#WANAggregation#>");
					note_wan_aggregation += str_port_binding_note_tmp;
					note_wan_aggregation += str_modify.replace('<a>', '<a id="modify_wan_bonding">');
					$("<div>").html(note_wan_aggregation).appendTo($target_div).css('text-align','left');
					$("#modify_wan_bonding").attr("href", "Advanced_WAN_Content.asp")
						.css("color", "#FFCC00")
						.css("cursor", "pointer")
						.css("text-decoration", "underline");
				}
			}	

			if ( i == 0 && orig_aggregation.lacp_enabled ==1 ) {	//CAP only because have no info for RE
				if(link_aggregation_array.length) {
					for (var a = 0; a < link_aggregation_array.length; a++) {
						if (a > 0)
							note_link_aggregation += " / ";
						note_link_aggregation += "<b>LAN ";
						note_link_aggregation += link_aggregation_array[a];
						note_link_aggregation += "</b>";
					}
					note_link_aggregation += ": ";
					str_port_binding_note_tmp = str_port_binding_note.replace("%@", "<#NAT_lacp#>");
					note_link_aggregation += str_port_binding_note_tmp;
					$("<div>").html(note_link_aggregation).appendTo($target_div).css('text-align','left');
				}
			}
			
			if ( i == 0 && iptv_array.length) {
				for (var c = 0; c < iptv_array.length; c++) {
					if (c > 0)
						note_iptv += " / ";
					note_iptv += "<b>LAN ";
					note_iptv += iptv_array[c];
					note_iptv += "</b>";
				}
				note_iptv += ": ";
				str_port_binding_note_tmp = str_port_binding_note.replace("%@", "<#menu_dsl_iptv#>");
				note_iptv += str_port_binding_note_tmp;
				note_iptv += str_modify.replace('<a>', '<a id="modify_iptv">');
				$("<div>").html(note_iptv).appendTo($target_div).css('text-align','left');
				$("#modify_iptv").attr("href", "Advanced_IPTV_Content.asp")
					.css("color", "#FFCC00")
					.css("cursor", "pointer")
					.css("text-decoration", "underline");
			}

			$("<br><br>").appendTo($target_div);

		} //[for loop] mesh system end

	}
}

function insert_vlan_mode_selector(mac, idx, flag, aggregation) {

	var mac_str = mac.replace(/\:/g, '');
	var $insert_selector = $("<select>").attr({
			"name": "switch_mode_" + mac_str + "_" + idx,
			"id": "switch_mode_" + mac_str + "_" + idx
		})
		.addClass("input_option");

	var VLAN_mode_select = {
		"All": "<#All#>(<#Setting_factorydefault_value#>)",
		"Access": "Access",
		"Trunk": "Trunk"
	}

	for (var key in VLAN_mode_select) {

		if (key == flag) {
			matched = true;
		} else {
			matched = false;
		}
		$insert_selector.append($("<option></option>")
			.attr("value", key)
			.text(VLAN_mode_select[key])
			.attr("selected", matched));
	}


	if (aggregation) {
		var $default_selector = $("<select>").attr({
			"name": "switch_mode_" + mac_str + "_" + idx,
			"id": "switch_mode_" + mac_str + "_" + idx
		})
		.addClass("input_option")
		.addClass("input_max_length");

		$default_selector.attr("disabled", true);
		$default_selector.append(
			$("<option></option>").attr("value", "0") // defined 0 for default
			.text("<#All#>(<#Setting_factorydefault_value#>)")
		);

		return $default_selector;
	} 
	else {
		$insert_selector.change(function() {
			update_vlan_profile_selector(mac_str, idx, flag, $insert_selector);
		});
		return $insert_selector;
	}

}

function update_vlan_profile_selector(mac_str, idx, flag, obj) {

	$("#switch_vlan_" + mac_str + "_" + idx).empty();

	if(obj.val()=="All"){

		$("#switch_vlan_" + mac_str + "_" + idx).attr("disabled", true);
		$("#switch_vlan_" + mac_str + "_" + idx).append(
			$("<option></option>").attr("value", "0") // defined 0 for default
			.text("<#Setting_factorydefault_value#>")	// Untranslated
		);

	}
	else if(obj.val()=="Access"){
		if(VLAN_Profile_select.length > 0){	//with vlan_rl 
			$("#switch_vlan_" + mac_str + "_" + idx).attr("disabled", false);
			$.each(VLAN_Profile_select, function(i, VLAN_Profile_select) {
				text = VLAN_Profile_select.name;

				if (VLAN_Profile_select.vid == flag) { //suppose flag is vid binded for Access/Trunk mode
					matched = true;
				} 
				else {
					matched = false;
				}

				if(VLAN_Profile_select.apgIdx != ""){	//except pure VLAN profile
					$("#switch_vlan_" + mac_str + "_" + idx).append(
						$("<option></option>").attr("value", VLAN_Profile_select.vid)
							.text(text)
							.attr("selected", matched)
					);
				}
			});
		}
		else{		//without vlan_rl 
			$("#switch_vlan_" + mac_str + "_" + idx).attr("disabled", true);
			$("#switch_vlan_" + mac_str + "_" + idx).append(
				$("<option></option>").attr("value", "0") // defined 0 for default
									  .text("<#Setting_factorydefault_value#>")	// Untranslated
			);
		}
	}
	else{	//Trunk mode

		$("#switch_vlan_" + mac_str + "_" + idx).attr("disabled", false);
		$("#switch_vlan_" + mac_str + "_" + idx).append(
			$("<option></option>").attr("value", "all") // defined 0 for default
			.text("Allow all tagging(<#Setting_factorydefault_value#>)")	// Untranslated
		);
		$.each(VLAN_Profile_select, function(i, VLAN_Profile_select) {

			if(VLAN_Profile_select.apgIdx != ""){	//except pure VLAN profile
				text = VLAN_Profile_select.name;
			}
			else{
				text = "("+VLAN_Profile_select.vid+")";	
			}

			if (VLAN_Profile_select.vid == flag) { //suppose flag is vid binded for Access/Trunk mode
				matched = true;
			} 
			else {
				matched = false;
			}
			$("#switch_vlan_" + mac_str + "_" + idx).append(
				$("<option></option>").attr("value", VLAN_Profile_select.vid)
				.text(text)
				.attr("selected", matched)
			);
		});
	}
}

function insert_vlan_profile_selector(mac, idx, flag, aggregation) {
	
	var vlan_profile_length = VLAN_Profile_select.length;
	var mac_str = mac.replace(/\:/g, '');
	var $insert_selector = $("<select>").attr({
			"name": "switch_vlan_" + mac_str + "_" + idx,
			"id": "switch_vlan_" + mac_str + "_" + idx
		})
		.addClass("input_option")
		.addClass("input_max_length");

	if (aggregation) {
		var $default_selector = $("<select>").attr({
			"name": "switch_vlan_" + mac_str + "_" + idx,
			"id": "switch_vlan_" + mac_str + "_" + idx
		})
		.addClass("input_option")
		.addClass("input_max_length");

		$default_selector.attr("disabled", true);
		$default_selector.append(
			$("<option></option>").attr("value", "0") // defined 0 for default
			.text("<#Setting_factorydefault_value#>")
		);

		return $default_selector;
	} 
	else {

		if($("#switch_mode_" + mac_str + "_" + idx).val()=="All"){

			$insert_selector.attr("disabled", true);
			$insert_selector.append(
				$("<option></option>").attr("value", "0") // defined 0 for default
				.text("<#Setting_factorydefault_value#>")	// Untranslated
			);

		}
		else if($("#switch_mode_" + mac_str + "_" + idx).val()=="Access"){
			
			$insert_selector.attr("disabled", false);
			$.each(VLAN_Profile_select, function(i, VLAN_Profile_select) {
				text = VLAN_Profile_select.name;

				if (VLAN_Profile_select.vid == flag) { //suppose flag is vid binded for Access/Trunk mode
					matched = true;
				} 
				else {
					matched = false;
				}

				if(VLAN_Profile_select.apgIdx != ""){	//except pure VLAN profile
					$insert_selector.append(
						$("<option></option>").attr("value", VLAN_Profile_select.vid)
							.text(text)
							.attr("selected", matched)
					);
				}
			});

		}
		else{	//Trunk mode

			$insert_selector.attr("disabled", false);
			$insert_selector.append(
				$("<option></option>").attr("value", "all") // defined 0 for default
				.text("Allow all tagging(<#Setting_factorydefault_value#>)")	// Untranslated
			);
			$.each(VLAN_Profile_select, function(i, VLAN_Profile_select) {

				if(VLAN_Profile_select.apgIdx != ""){	//except pure VLAN profile
					text = VLAN_Profile_select.name;
				}
				else{
					text = "("+VLAN_Profile_select.vid+")";	
				}

				if (VLAN_Profile_select.vid == flag) { //suppose flag is vid binded for Access/Trunk mode
					matched = true;
				} 
				else {
					matched = false;
				}
				$insert_selector.append(
					$("<option></option>").attr("value", VLAN_Profile_select.vid)
					.text(text)
					.attr("selected", matched)
				);
			});
		}
		return $insert_selector;
	}
}

function applyRule() {
	var each_port_assigned = collect_assigned_from_table_array();
	if(each_port_assigned){
		if(!confirm(stringSafeGet("<#VLAN_eachport_assign#>"))){
			return;
		}
	}	

	collect_Access_mode_from_table_array();
	var vlanSwitchPost = getApgDutListPostData(apg_dutList);
	var vlanTrunkListPost = getvlanTrunkListPostData();
	vlanSwitchPost.vlan_trunklist = vlanTrunkListPost;

	vlanSwitchPost.action_mode = "apply";
	vlanSwitchPost.rc_service = "restart_net_and_phy";

	var reboot_time = httpApi.hookGet("get_default_reboot_time");
	if(isWLclient()){
		showLoading(reboot_time);
		setTimeout(function(){
			showWlHintContainer();
		}, reboot_time*1000);
		check_isAlive_and_redirect({"page": "Advanced_VLAN_Switch_Content.asp", "time": reboot_time});
	}
	httpApi.nvramSet(vlanSwitchPost, function() {
		if(isWLclient()) return;
		showLoading(reboot_time);
		check_isAlive_and_redirect({"page": "Advanced_VLAN_Switch_Content.asp", "time": reboot_time});
	});
}

function collect_assigned_from_table_array(){	//check if all ports were assigned
	var mac_id_str_cap = Object.keys(VLAN_port_status)[0].replace(/\:/g, '');
	var port_length = VLAN_port_status[Object.keys(VLAN_port_status)[0]].port.length;
	for (var x = 1; x < VLAN_port_status[Object.keys(VLAN_port_status)[0]].port.length + 1; x++) {	//each port
		if ($("#switch_mode_" + mac_id_str_cap + "_" + x).val() != "All") {
			port_length -= 1;
		}
	}

	return (port_length==0)? true:false;
}

var mac_id_str = "";
function collect_Access_mode_from_table_array() {
	clean_apgX_dut_list_lanport(apg_dutList);
	for (var y = 0; y < Object.keys(VLAN_port_status).length; y++) {	//how many macAddr
		mac_id_str = Object.keys(VLAN_port_status)[y].replace(/\:/g, '');
		for (var x = 1; x < VLAN_port_status[Object.keys(VLAN_port_status)[y]].port.length + 1; x++) {	//each port

			if ($("#switch_mode_" + mac_id_str + "_" + x).val() == "Access") {

				for (var z = 0; z < VLAN_Profile_select.length; z++) {

					if (VLAN_Profile_select[z].vid == $("#switch_vlan_" + mac_id_str + "_" + x).val()) {
						var apgX = VLAN_Profile_select[z].apgIdx;
						for (var i = 0; i < apg_dutList['apg' + apgX + '_dut_list'].length; i++) {

							if (apg_dutList['apg' + apgX + '_dut_list'][i].mac == Object.keys(VLAN_port_status)[y]) {
								if (apg_dutList['apg' + apgX + '_dut_list'][i].lanport == "") {
									apg_dutList['apg' + apgX + '_dut_list'][i].lanport = x;
								} else {
									apg_dutList['apg' + apgX + '_dut_list'][i].lanport = apg_dutList['apg' + apgX + '_dut_list'][i].lanport + "," + x;
								}
							}

						}
					}
				}

			}

		}
	}
}

var apgPostData = {};

function getApgDutListPostData(apgJson) {
	for (var apg in apgJson) {
		var apgFlatArray = [];

		$.each(apgJson[apg], function(idx, dutList) {
			var apgDutList = [];
			for (var attr in dutList) {
				apgDutList.push(dutList[attr])
			}

			apgFlatArray.push(apgDutList.join(">"))
		})

		apgPostData[apg] = "<" + apgFlatArray.join("<");
	}

	return apgPostData;
}

function clean_apgX_dut_list_lanport(apg_dutList) {
	$.each(apg_dutList, function(idx, mac_array) {
		for (var x = 0; x < mac_array.length; x++) {
			mac_array[x].lanport = ""; //clean apgX_dut_list lanport
		}
	})
}

function getvlanTrunkListPostData() {
	var TrunkListPostData = "";
	for (var y = 0; y < Object.keys(VLAN_port_status).length; y++) {	//by MAC
		mac_id_str = Object.keys(VLAN_port_status)[y].replace(/\:/g, '');
		var TrunkListByLanport = "";
		var TrunkListbyMac = "";
		for (var x = 1; x < VLAN_port_status[Object.keys(VLAN_port_status)[y]].port.length + 1; x++) {	//by port

			if ($("#switch_mode_" + mac_id_str + "_" + x).val() == "Trunk" && $("#switch_vlan_" + mac_id_str + "_" + x).val() != "all") {
				for (var z = 0; z < VLAN_Profile_select.length; z++) {
					TrunkListByLanport = getTrunkListVID($("#switch_vlan_" + mac_id_str + "_" + x).val());
				}

				TrunkListbyMac += ">" + x + "#" + TrunkListByLanport;
			}
			else if ($("#switch_mode_" + mac_id_str + "_" + x).val() == "Trunk" && $("#switch_vlan_" + mac_id_str + "_" + x).val() == "all") {
				TrunkListbyMac += ">" + x + "#all";
			}
		}
		if (TrunkListbyMac != "") {
			TrunkListPostData += "<" + Object.keys(VLAN_port_status)[y] + TrunkListbyMac;
		}
	}

	return TrunkListPostData;
}
function getTrunkListVID(flag) {	//suppose flag(=flag[0]) is the only one vid binded for trunk mode
	var TrunkListVIDbyLanport = "";

	$.each(VLAN_Profile_select, function(i, Profile) {
		if (Profile.vid == flag) {
			TrunkListVIDbyLanport += (TrunkListVIDbyLanport == "") ? Profile.vid : "," + Profile.vid;
		}
	});

	return TrunkListVIDbyLanport;
}

function getvlanBlkListPostData() {
	var blkListPostData = "";
	for (var y = 0; y < Object.keys(VLAN_port_status).length; y++) {
		mac_id_str = Object.keys(VLAN_port_status)[y].replace(/\:/g, '');
		//Object.keys(VLAN_port_status)[y]
		var blkListByLanport = "";
		var blkListbyMac = "";
		for (var x = 1; x < VLAN_port_status[Object.keys(VLAN_port_status)[y]].port.length + 1; x++) {
			if ($("#switch_mode_" + mac_id_str + "_" + x).val() == "Trunk") {
				for (var z = 0; z < VLAN_Profile_select.length; z++) {
					blkListByLanport = getBlkListVID($("#switch_vlan_" + mac_id_str + "_" + x).val());
				}
				blkListbyMac += ">" + x + "#" + blkListByLanport;
			}
		}
		if (blkListbyMac != "") {
			blkListPostData += "<" + Object.keys(VLAN_port_status)[y] + blkListbyMac;
		}
	}

	return blkListPostData;
}

function getBlkListVID(flag) {
	var blkListVIDbyLanport = "";

	$.each(VLAN_Profile_select, function(i, VLAN_Profile_select) {
		if (VLAN_Profile_select.vid != flag) {
			blkListVIDbyLanport += (blkListVIDbyLanport == "") ? VLAN_Profile_select.vid : "," + VLAN_Profile_select.vid;
		}
	});

	return blkListVIDbyLanport;
}

function get_showLoading_status(_rc_service){
	var result = {"time": 20, "disconnect": false};//restart_net_and_phy will disconnect and logout
	if(_rc_service != undefined){
		if(_rc_service.indexOf("restart_net_and_phy;") >= 0){
			result.time = httpApi.hookGet("get_default_reboot_time");
			result.disconnect = true;
		}
		else if(_rc_service.indexOf("restart_chilli;restart_uam_srv;") >= 0){
			result.time = 30;
		}
	}
	return result;
}

function check_isAlive_and_redirect(_parm){
	var page = "";
	var time = 30;
	var interval_time = 2;
	if(_parm != undefined){
		if(_parm.page != undefined && _parm.page != "") page = _parm.page;
		if(_parm.time != undefined && _parm.time != "" && !isNaN(_parm.time)) time = parseInt(_parm.time);
	}
	var lan_ipaddr = httpApi.nvramGet(["lan_ipaddr"]).lan_ipaddr;
	setTimeout(function(){
		var interval_isAlive = setInterval(function(){
			httpApi.isAlive("", lan_ipaddr, function(){ clearInterval(interval_isAlive); top.location.href = "/" + page + "";});
		}, 1000*interval_time);
	}, 1000*(time - interval_time));
}
</script>
</head>

<body onload="initial();" class="bg">
<div id="TopBanner"></div>
<div id="Loading" class="popup_bg"></div>

<iframe name="hidden_frame" id="hidden_frame" src="" width="0" height="0" frameborder="0"></iframe>
<form method="post" name="form" id="form" action="/start_apply.htm" target="hidden_frame">
<input type="hidden" name="productid" value="<% nvram_get("productid"); %>">
<input type="hidden" name="current_page" value="Advanced_VLAN_Switch_Content.asp">
<input type="hidden" name="next_page" value="Advanced_VLAN_Switch_Content.asp">
<input type="hidden" name="modified" value="0">
<input type="hidden" name="action_mode" value="apply_new">
<input type="hidden" name="action_script" value="restart_net">
<input type="hidden" name="action_wait" value="10">
<input type="hidden" name="preferred_lang" id="preferred_lang" value="<% nvram_get("preferred_lang"); %>">
<input type="hidden" name="firmver" value="<% nvram_get("firmver"); %>">
<!--input type="hidden" name="vlan_rulelist" value="<% nvram_char_to_ascii("","vlan_rulelist"); %>"-->
<table class="content" align="center" cellpadding="0" cellspacing="0">
	<tr>
		<td width="17">&nbsp;</td>	
		<!--=====Beginning of Main Menu=====-->
		<td valign="top" width="202">
			<div id="mainMenu"></div>
			<div id="subMenu"></div>
		</td>
		<td valign="top">
			<div id="tabMenu" class="submenuBlock"></div>
			<!--===================================Beginning of Main Content===========================================-->
			<table width="98%" border="0" align="left" cellpadding="0" cellspacing="0">
				<tr>
					<td align="left" valign="top">
						<table width="760px" border="0" cellpadding="5" cellspacing="0" class="FormTitle" id="FormTitle">
						<tbody>
							<tr>
								<td bgcolor="#4D595D" valign="top" style="padding: 0px;">
									<div>&nbsp;</div>
									<div class="formfonttitle">VLAN</div>
									
									<div style="margin:10px 0 10px 5px;" class="splitLine"></div>
									<div class="formfontdesc"><#VALN_Switch_desc#></div>
									<div id="divSwitchMenu" style="margin-top:-40px;float:right;"></div>
									
									<div id="divVLANTable" style="width:100%;margin-top:25px;">
										<div id="tableCAP" class="port_status_table"></div>
									</div>
									
									<div class="apply_gen">
										<input class="button_gen" onclick="applyRule()" type="button" value="<#CTL_apply#>"/>
									</div>
								</td>
							</tr>
						</tbody>	
						</table>					
					</td>	
				</tr>
			</table>				
			<!--===================================End of Main Content===========================================-->
		</td>
		<td width="10" align="center" valign="top">&nbsp;</td>
	</tr>
</table>
</form>	
<div id="footer"></div>
</body>
</html>

