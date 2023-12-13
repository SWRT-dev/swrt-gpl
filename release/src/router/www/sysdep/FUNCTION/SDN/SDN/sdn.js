if(typeof stringSafeGet != "function"){
	function stringSafeGet(str){
		return str.replace(new RegExp("&#39;", 'g'), "'");
	}
}
if(typeof Get_Component_PWD_Strength_Meter != "function"){
	function Get_Component_PWD_Strength_Meter(id){
		var postfix = (id == undefined)? "": ("_"+id);
		var $pwd_strength_container = $("<div>").addClass("pwd_strength_container").attr("id", "scorebarBorder"+postfix).attr("title", "<#LANHostConfig_x_Password_itemSecur#>");
		var $strength_text = $("<div>").addClass("strength_text").appendTo($pwd_strength_container).attr("id", "score"+postfix);
		var $strength_color = $("<div>").addClass("strength_color").appendTo($pwd_strength_container).attr("id", "scorebar"+postfix);
		return $pwd_strength_container;
	}
}

var terms_service_template = "Welcome to our Wi-Fi service (the \"Service\"). By accessing and using this Service, you represent and acknowledge that you are of legal age, and you have read and agree to be bound by the following terms and conditions:\n1. Accessing the Service\n1.1 The Service is provided by the store (the \"Store\") for your personal use. The Store reserves the right to change or terminate the Service or change this Terms and Conditions at any time. You are responsible for reviewing this Terms and Conditions each time you use the Service.\n1.2 Access to the Service may be blocked, suspended or terminated at any time for any reason including but not limited to your violation of this Terms and Conditions or applicable laws.\n1.3 The Store does not guarantee availability to the Internet, the minimum Internet connection speeds on the network, or that the Service will be stable, fault-free, timely, reliable, operational or suitable for supporting your intended use.\n1.4 The Store does not guarantee the quality of the information on the internet. It is your responsibility to determine the validity, quality, and relevance of the information obtained.\n1.5 The Service is provided on an open and public basis and the Store cannot guarantee the security of the Wi-Fi service, you acknowledge and agree that there are inherent security risks associated with using the Service and that you do so solely at your own risk.\n2. Restrictions\nYou acknowledge and agree that when using the Service you will comply with all applicable laws and that you will not;\na. use the Service for commercial purposes;\nb. use the Service to send unsolicited bulk emails;\nc. reveal or publish proprietary or confidential information;\nd. infringe the intellectual property rights of any person;\ne. collect or harvest any information or data from the Service or the servers used to provide the Service;\nf. attempt to intercept, interfere with or decipher any transmission to or from the servers used to provide the Service;\ng. connect to \"Peer to Peer\" file sharing networks, download large files, or run any programs, services, systems, processes, or servers that may substantially degrade network performance or accessibility;\nh. use the Service to transmit, send, upload, receive, download, publish, post, distribute, disseminate, encourage or solicit receipt of any material which is abusive, defamatory, harassing, indecent, offensive, obscene, menacing, racist, pornographic, threatening, unlawful or in breach of any right of any person;\ni. use the Service to transmit, store, publish or upload any electronic material, such as viruses, malware, Trojan horses, worms ,or time bombs, which will or are likely to cause damage to, or to destroy or limit the functionality of, any computer, software, hardware, electronic device or telecommunications equipment;\nj. obtain unauthorised access to any other person's computer, email account, bank account, computer network or equipment; or\nk. use the Service to invade the privacy of another person or to cause annoyance, inconvenience or anxiety to another person.\n3. Privacy\nYou acknowledge and agree that the Service will capture and process information about the web browser type and/or operating system information used by the enabled device to determine the more effective and/or customized means of displaying the requested website on your device. And the Store will collect the IP and MAC address of the enabled device that has accessed the Wi-Fi service, once the Terms and Conditions have been agreed to.\n4. Release and Indemnity\n4.1 The Store is not liable for loss of data due to service delays or interruptions or any kind of loss or damages you may sustain as a result of your use of the Service.\n4.2 You release and discharge the Store from any liability which might arise from your use of the Service, including liability in relation to defamatory or offensive material or any breach of copyright which may occur.\n4.3 You agree to indemnify and must defend and hold harmless the Store, its employees and agents, from and against all loss, damage, liability, charge, expense (including but not limited to attorneys' fees) of any nature or kind arising from your breach of these Terms and Conditions.";
var ui_lang = httpApi.nvramGet(["preferred_lang"]).preferred_lang;
$.getJSON("/ajax/freewifi_tos.json", function(data){
	if(data[ui_lang] != "")
		terms_service_template = data[ui_lang];
});
var str_SDN_choose = stringSafeGet("<#GuestNetwork_SDN_choose#>");
var str_WiFi_sche = stringSafeGet("<#GuestNetwork_WiFi_Sche#>");
var str_Scheduled = stringSafeGet("<#Time_Scheduled#>");
var str_Scheduled_one_time = stringSafeGet("<#Time_Scheduled_one_time#>");
var str_noProfile_hint = stringSafeGet("<#GuestNetwork_noProfile_hint#>");
var str_GuestNetwork_WiFi_Sche_desc = stringSafeGet("<#GuestNetwork_WiFi_Sche_desc#>");
var str_Scenario_Classroom = stringSafeGet("<#Scenario_Classroom#>");
var str_Scenario_coffee = stringSafeGet("<#Scenario_coffee#>");
var str_Scenario_Study = stringSafeGet("<#Scenario_Study#>");
var str_Scenario_EV_charging = stringSafeGet("<#Scenario_EV_charging#>");
var str_Scenario_smartHome = stringSafeGet("<#Scenario_smartHome#>");
var str_Scenario_branchOffice = stringSafeGet("<#Scenario_branchOffice#>");
var str_Scenario_Hotel = stringSafeGet("<#Scenario_Hotel#>");
var str_Scenario_Mall = stringSafeGet("<#Scenario_Mall#>");
var str_Scenario_Friends = stringSafeGet("<#Scenario_Friends#>");
var str_GuestNetwork_Portal_desc2 = stringSafeGet("<#GuestNetwork_Portal_desc2#>");
var str_GuestNetwork_Wizard_type_desc0 = stringSafeGet("<#GuestNetwork_Wizard_type_desc0#>");
var str_GuestNetwork_Employee_recommend = stringSafeGet("<#GuestNetwork_Employee_recommend#>");
var sdn_maximum = ((isSupport("MaxRule_SDN") == "0") ? 6 : parseInt(isSupport("MaxRule_SDN")));
var wifi_band_maximum = 6;
var selected_sdn_idx = "";
var selected_vlan_idx = "";
var sdn_all_rl_json = [];
var sdn_schedule = "";
var cfg_clientlist = httpApi.hookGet("get_cfg_clientlist");
var interval_AccTime = false;
var VLAN_Profile_select = [];
var is_Web_iframe = ($(parent.document).find("#mainMenu").length > 0) ? true : false;
var page_container = window.location.pathname.split("/").pop();
var cap_mac = (httpApi.hookGet('get_lan_hwaddr')) ? httpApi.hookGet('get_lan_hwaddr') : '';
var is_QIS_flow = (function(){
	var result = false;
	if(typeof apply == "object"){
		if(typeof apply.SDN_Profile == "function"){
			result = true;
		}
	}
	return result;
})();
var is_cfg_ready = (cfg_clientlist.length > 0) ? true : false;
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
	"wan_unit",
	"pppoe-relay",
	"wan6_unit",
	"createby",
	"mtwan_idx",
	"mswan_idx"
];

vlanRuleTable = [
	"vlan_idx",
	"vid",
	"port_isolation"
];

apgRuleTable = [
	"mac",
	"wifiband",
	"lanport"
];

vlanTrunklistTable = [
	"mac",
	"port",
	"profile"
];

vlanBlklistTable = [
	"mac",
	"port",
	"profile"
];
var vlan_rl = decodeURIComponent(httpApi.nvramCharToAscii(["vlan_rl"]).vlan_rl);
var vlan_rl_json = convertRulelistToJson(vlanRuleTable, vlan_rl);

var vlan_rl_vid_array = [];
$.each(vlan_rl_json, function(idx, vlan) {
	vlan_rl_vid_array.push(vlan.vid);
});

var vlan_trunklist_orig = decodeURIComponent(httpApi.nvramCharToAscii(["vlan_trunklist"]).vlan_trunklist);
var vlan_trunklist_tmp = vlan_trunklist_orig+"_";	//easy to do replace
var vlan_trunklist_array = vlan_trunklist_orig.split("<");
var vlan_trunklist_port_array = [];
var vlan_trunklist_port_array_one_mac = [];	//2-dim by mac
var vlan_trunklist_port = "";
var vlan_trunklist_port_vid_array = [];
var vlan_rl_vid_array_tmp = [];

var vlan_trunklist_string = "";
var vlan_trunklist_json = [];
var vlan_trunklist_json_tmp = [];

if (vlan_trunklist_array.length > 1) {
	for (var b = 1; b < vlan_trunklist_array.length; b++) {	//DUT Macs
		vlan_trunklist_port_array_one_mac[b] = vlan_trunklist_array[b].split(">");
		for (var c = 0; c < vlan_trunklist_port_array_one_mac[b].length; c++) {	//binded LAN ports

			vlan_trunklist_port_array.push(vlan_trunklist_port_array_one_mac[b][c]);
			//collect each mac & port binding

			if (c > 0) {
				vlan_rl_vid_array_tmp = vlan_rl_vid_array;
				vlan_trunklist_port = vlan_trunklist_port_array_one_mac[b][c].split("#")[0];
				vlan_trunklist_port_vid_array = vlan_trunklist_port_array_one_mac[b][c].split("#")[1].split(",");
				vlan_rl_vid_array_tmp = vlan_trunklist_port_vid_array;
				
				//suppose 1st vid (vlan_rl_vid_array_tmp[0]) to be the only one vid binded
				vlan_trunklist_string += "<"+vlan_trunklist_port_array_one_mac[b][0]+">"+vlan_trunklist_port+">"+vlan_rl_vid_array_tmp[0];
			}
		}
	}
	vlan_trunklist_json = convertRulelistToJson(vlanTrunklistTable, vlan_trunklist_string);
	vlan_trunklist_json_tmp = convertRulelistToJson(vlanTrunklistTable, vlan_trunklist_string);
}

function arrayRemove(arr, value) {
	return arr.filter(function(ele) {
		return ele != value;
	});
}

var cp_type_support = {};
var cp_type_rl_json = (function(){
	if(!isSwMode("rt")) return [];

	var cp_type_json = [];
	var cp_type_rl_attr = function(){
		this.cp_idx = "";
		this.cp_type = "";
		this.cp_text = "";
		this.sdn_idx = "";
		this.profile = [];
	};

	var cp_text_map = [];
	if(isSupport("captivePortal") && isSupport("cp_advanced"))
		cp_text_map.push({"id":"1", "text":"<#Captive_Portal#> WiFi"});
	if(isSupport("captivePortal") && isSupport("cp_freewifi")){
		cp_text_map.push({"id":"2", "text":"Free WiFi"});
		if(!isSupport("BUSINESS"))
			cp_text_map.push({"id":"4", "text":"Message Board"});
	}
	if(isSupport("fbwifi_sdn") && !is_QIS_flow)
		cp_text_map.push({"id":"3", "text":"Facebook WiFi"});

	var cp_type_rl = decodeURIComponent(httpApi.nvramCharToAscii(["cp_type_rl"]).cp_type_rl);
	var each_cp_type_rl = cp_type_rl.split("<");
	$.each(each_cp_type_rl, function(index, value){
		if(value != ""){
			var profile_data = value.split(">");
			var specific_cp_text = cp_text_map.filter(function(item, index, array){
				return (item.id == profile_data[1]);
			})[0];
			if(specific_cp_text != undefined){
				var cp_type = JSON.parse(JSON.stringify(new cp_type_rl_attr()));
				cp_type.cp_text = specific_cp_text.text;
				cp_type.cp_idx = profile_data[0];
				cp_type.cp_type = profile_data[1];
				cp_type_json.push(cp_type);
				cp_type_support["type_"+cp_type.cp_type+""] = true;
			}
		}
	});

	return cp_type_json;
})();
var vpns_rl_json = (function(){
	if(!isSwMode("rt")) return [];

	var vpns_type_text_map = [
		{"proto": "PPTP", "text": "PPTP"},
		{"proto": "OpenVPN", "text": "OpenVPN"},
		{"proto": "IPSec", "text": "IPSec VPN"},
		{"proto": "WireGuard", "text": "WireGuard VPN"}
	];

	var vpns_rl = decodeURIComponent(httpApi.nvramCharToAscii(["vpns_rl"]).vpns_rl);
	var vpn_type_json = convertRulelistToJson(["desc","proto","unit", "activate", "vpns_idx"], vpns_rl);
	if(!isSupport("pptpd"))
		vpn_type_json = remove_proto("PPTP");
	if(!isSupport("openvpnd"))
		vpn_type_json = remove_proto("OpenVPN");
	if(!isSupport("ipsec_srv"))
		vpn_type_json = remove_proto("IPSec");
	if(!isSupport("wireguard"))
		vpn_type_json = remove_proto("WireGuard");

	$.each(vpn_type_json, function(index, vpn_type){
		var spec_vpns_type = vpns_type_text_map.filter(function(item){
			return item.proto == vpn_type.proto;
		})[0];
		if(spec_vpns_type != undefined){
			vpn_type.text = spec_vpns_type.text;
		}
		else{
			vpn_type.text = vpn_type.proto;
		}
		if(vpn_type.unit > 1)
			vpn_type.text += " " + vpn_type.unit;
		if(vpn_type.proto == "WireGuard"){
			var activate = httpApi.nvramGet(["wgs"+vpn_type.unit+"_enable"])[["wgs"+vpn_type.unit+"_enable"]];
			if(activate == "1")
				vpn_type.activate = "1";
		}
	});

	return vpn_type_json

	function remove_proto(_proto) {
		return vpn_type_json.filter(function(item){
			if(item.proto == _proto){
				return false;
			}
			return true;
		});
	}
})();
var wizard_type_text = [
	{"type":"Employee", "text":"<#GuestNetwork_Employee#>", "desc":"<#GuestNetwork_Employee_desc#>"},
	{"type":"Portal", "text":"<#GuestNetwork_Portal#>", "desc":"<#GuestNetwork_Portal_desc2#>"},
	{"type":"Guest", "text":"<#Guest_Network#>", "desc":"<#GuestNetwork_Guests_desc#>."},
	{"type":"Kids", "text":"<#GuestNetwork_Kid#>", "desc":"<#GuestNetwork_Kid_desc#>"},
	{"type":"IoT", "text":"<#GuestNetwork_IoT#>", "desc":"<#GuestNetwork_IoT_desc#>"},
	{"type":"VPN", "text":"<#GuestNetwork_VPN#>", "desc":"<#GuestNetwork_VPN_desc#>"},
	{"type":"Customized", "text":"<#GuestNetwork_Customized#>", "desc":""},
];
if(isSupport("BUSINESS")){
	var specific_wizard_type = wizard_type_text.filter(function(item, index, array){
		return (item.type == "Kids");
	})[0];
	if(specific_wizard_type != undefined){
		specific_wizard_type.type = "Sched";
		specific_wizard_type.text = "<#GuestNetwork_Sche_network#>";
		specific_wizard_type.desc = str_GuestNetwork_WiFi_Sche_desc;
	}
}
function Get_Component_Profile_Item_AddNew(){
	var $profile_item_container = $("<div>").addClass("profile_item_container addnew");
	$profile_item_container.unbind("click").click(function(e){
		e = e || event;
		e.stopPropagation();
		var rule_num = 0;
		if(rule_num >= sdn_maximum){
			show_customize_alert("<#weekSche_MAX_Num#>".replace("#MAXNUM", sdn_maximum));
			return false;
		}
		selected_sdn_idx = "";
		if($(".profile_setting_container").css("display") == "none"){
			$(".popup_element").css("display", "flex");
			$(".container").addClass("blur_effect");
			$(".popup_container.popup_element").empty().append(show_Get_Start("popup"));
		}
		else{
			$(this).closest("#profile_list_content").find(".profile_item_container").removeClass("selected");
			$(this).closest(".profile_item_container").addClass("selected");
			$(".profile_setting_container").empty().append(show_Get_Start());
			resize_iframe_height();
		}
	});
	var $item_text_container = $("<div>").addClass("item_text_container");
	$item_text_container.appendTo($profile_item_container);

	$("<div>").addClass("main_info").html("<#GuestNetwork_ProfileAdd#>").appendTo($item_text_container);

	return $profile_item_container;
}
function Get_Component_Profile_Item(_profile_data){
	var specific_data = sdn_all_rl_json.filter(function(item, index, array){
			return (item.sdn_rl.idx == _profile_data.sdn_idx);
		})[0];

	var $profile_item_container = $("<div>").addClass("profile_item_container " + _profile_data.type + "").attr("sdn_idx", _profile_data.sdn_idx);

	var $item_tag_cntr = $("<div>").addClass("item_tag_container").appendTo($profile_item_container);
	if(specific_data.sdn_rl.vpnc_idx != "0"){
		$("<div>").addClass("item_tag VPN").html("<#vpnc_title#>").appendTo($item_tag_cntr);
	}
	if(specific_data.sdn_rl.vpns_idx != "0"){
		$("<div>").addClass("item_tag VPN").html("<#BOP_isp_heart_item#>").appendTo($item_tag_cntr);
	}
	if(specific_data.sdn_rl.sdn_enable == "1" && specific_data.apg_rl.timesched == "2"){
		var utctimestamsp = parseInt(httpApi.hookGet("utctimestamp", true));
		var cur_time = (isNaN(utctimestamsp)) ? 0 : utctimestamsp;
		var end_expiretime = specific_data.apg_rl.expiretime.split(",")[1];
		if(end_expiretime != undefined && end_expiretime != "" && end_expiretime.length == 13){
			var end_time = parseInt(end_expiretime.substr(2,10));
			var remaining_time = ((end_time - cur_time) > 0) ? (end_time - cur_time) : 0;
			var HMS = secondsToHMS(remaining_time);
			$("<div>")
				.addClass("item_tag Timer")
				.attr({
					"title": "<#mssid_time_remaining#>",
					"end_time":end_time,"cur_time":cur_time,
					"data-container":"AccTime","access_time":(remaining_time > 0 ? true : false)
				})
				.html(HMS.hours + ":" + HMS.minutes + ":" + HMS.seconds)
				.appendTo($item_tag_cntr);
		}
	}

	if(get_cp_type_support("2") && specific_data.sdn_rl.cp_idx == "2"){
		$("<div>").addClass("item_tag no_icon").html("Free WiFi").appendTo($item_tag_cntr);
	}
	if(get_cp_type_support("4") && specific_data.sdn_rl.cp_idx == "4"){
		$("<div>").addClass("item_tag no_icon").html("Message Board").appendTo($item_tag_cntr);
	}

	var $item_text_container = $("<div>").addClass("item_text_container").appendTo($profile_item_container)
	$item_text_container.unbind("click").click(function(e){
		e = e || event;
		e.stopPropagation();
		selected_sdn_idx = _profile_data.sdn_idx;
		if($(".profile_setting_container").css("display") == "none"){
			$(".popup_element").css("display", "flex");
			$(".container").addClass("blur_effect");
			$(".popup_container.popup_element").empty().append(Get_Component_Full_Setting("popup"));
			Update_Setting_Profile($(".popup_container.popup_element"), specific_data);
			adjust_popup_container_top($(".popup_container.popup_element"), 100);
		}
		else{
			$(this).closest("#profile_list_content").find(".profile_item_container").removeClass("selected");
			$(this).closest(".profile_item_container").addClass("selected");
			$(".profile_setting_container").empty().append(function(){
				return Get_Component_Full_Setting();
			});
			Update_Setting_Profile($(".profile_setting_container"), specific_data);
			resize_iframe_height();
		}
	});

	var $main_info = $("<div>").addClass("main_info").appendTo($item_text_container);
	$("<div>").addClass("main_info_text")
		.attr({"title":"<#QIS_finish_wireless_item1#>: " + _profile_data.name})
		.html(htmlEnDeCode.htmlEncode(_profile_data.name)).appendTo($main_info);
	var $sub_info = $("<div>").addClass("sub_info").appendTo($item_text_container);
	$("<div>").addClass("clients_num").attr({"title":"<#number_of_client#>"}).html(htmlEnDeCode.htmlEncode(_profile_data.client_num)).appendTo($sub_info);/* untranslated */

	if(!($.isEmptyObject(specific_data.vlan_rl))){
		$("<div>").addClass("point_icon").appendTo($sub_info);
		$("<div>").html("VLAN <span>" + specific_data.vlan_rl.vid + "</span>").appendTo($sub_info);
	}
	var conn_type_arr = get_conn_type(specific_data);
	if(conn_type_arr.length > 0){
		$("<div>").addClass("point_icon").appendTo($sub_info);
	}
	$.each(conn_type_arr, function(index, item){
		$("<div>").addClass("conn_type " + item.type).attr({"title":item.text}).appendTo($sub_info);
	});

	var $item_action_container = $("<div>").addClass("item_action_container");
	$item_action_container.appendTo($profile_item_container);
	var $icon_switch = $("<div>").addClass("icon_switch").addClass((function(){
		return ((_profile_data.activate == "1") ? "on" : "off");
	})());

	$icon_switch.appendTo($item_action_container);
	$item_action_container.unbind("click").click(function(e){
		e = e || event;
		e.stopPropagation();
		$this_icon_switch = $(this).find(".icon_switch");
		$this_icon_switch.toggleClass("off on").promise().done(function(){
			var $profile_obj = $(this).closest(".profile_item_container");
			if($(this).hasClass("on")){
				var other_portal_enabled = false;
				if(get_cp_type_support(specific_data.sdn_rl.cp_idx)){
					$.each(sdn_all_rl_json, function(index, item) {
						if(item.sdn_rl.idx == specific_data.sdn_rl.idx)
							return true;
						if(item.sdn_rl.cp_idx != "0"){
							if(item.sdn_rl.sdn_enable == "1"){
								other_portal_enabled = true;
								return false;
							}
						}
					});
				}
				if(other_portal_enabled){
					$(this).toggleClass("off on");
					show_customize_alert("<#vpnc_conn_maxi_general#>".replace(/VPN/g, "<#GuestNetwork_Portal_type#>").replace(/2/g, "1"));
					return;
				}
				specific_data.sdn_rl.sdn_enable = "1";
				specific_data.apg_rl.enable = "1";
			}
			else{
				specific_data.sdn_rl.sdn_enable = "0";
				specific_data.apg_rl.enable = "0";
			}
			selected_sdn_idx = specific_data.sdn_rl.idx;
			var sdn_all_list = parse_JSONToStr_sdn_all_list();
			var nvramSet_obj = {"action_mode": "apply", "rc_service": "restart_wireless;restart_sdn " + selected_sdn_idx + ";"};
			if(httpApi.nvramGet(["qos_enable"]).qos_enable == "1"){
				nvramSet_obj.rc_service += "restart_qos;restart_firewall;";
			}
			if(specific_data.sdn_rl.cp_idx == "2" || specific_data.sdn_rl.cp_idx == "4"){
				nvramSet_obj.rc_service += "restart_chilli;restart_uam_srv;";
			}
			nvramSet_obj["sdn_rl"] = sdn_all_list.sdn_rl;
			nvramSet_obj["apg" + specific_data.apg_rl.apg_idx + "_enable"] = specific_data.apg_rl.enable;
			var showLoading_status = get_showLoading_status(nvramSet_obj.rc_service);
			showLoading();
			if(isWLclient()){
				showLoading(showLoading_status.time);
				setTimeout(function(){
					showWlHintContainer();
				}, showLoading_status.time*1000);
				check_isAlive_and_redirect({"page": "SDN.asp", "time": showLoading_status.time});
			}
			httpApi.nvramSet(nvramSet_obj, function(){
				if(isWLclient()) return;
				showLoading(showLoading_status.time);
				init_sdn_all_list();
				show_sdn_profilelist();
				close_popup_container("all");
				setTimeout(function(){
					if(!window.matchMedia('(max-width: 575px)').matches)
						$this_icon_switch.closest(".profile_item_container").find(".item_text_container").click();
				}, showLoading_status.time*1000);
				if(!isMobile()){
					if(showLoading_status.disconnect){
						check_isAlive_and_redirect({"page": "SDN.asp", "time": showLoading_status.time});
					}
				}
			});
		});
	});

	return $profile_item_container;

	function get_conn_type(_sdn_data){
		var conn_type = [];
		var wifi_band_value = "0";
		var apg_dut_list_array = (_sdn_data.apg_rl.dut_list).split("<");
		$.each(apg_dut_list_array, function(index, dut_info){
			if(dut_info != ""){
				var dut_info_array = dut_info.split(">");
				if(dut_info_array[1] > "0"){
					if(dut_info_array[1] == "1")
						wifi_band_value = "1";
					else if(dut_info_array[1] == "2" || dut_info_array[1] == "4")
						wifi_band_value = "2";
					else if(dut_info_array[1] == "3" || dut_info_array[1] == "5")
						wifi_band_value = "3";
					return false;
				}
			}
		});

		if(wifi_band_value == "0"){
			conn_type = [{"type": "LAN", "text": "<#tm_wired#>"}];
			return conn_type;
		}
		if(_sdn_data.sdn_rl.wifi_sched_on == "0"){
			conn_type = [{"type": "WiFi_offline", "text": "<#tm_wireless#> (<#Clientlist_OffLine#>)"}];
			return conn_type;
		}

		var apg_sec_array = (_sdn_data.apg_rl.security).split("<");
		var cap_wifi_auth = "lock";
		if(apg_sec_array[1] != undefined && apg_sec_array[1] != ""){
			var cap_sec_array = apg_sec_array[1].split(">");
			cap_wifi_auth = ((cap_sec_array[1] == "open") ? "" : "lock");
		}

		switch(wifi_band_value){
			case "1":
				conn_type = [{"type":  ((cap_wifi_auth == "lock") ? "WiFi_2G_lock": "WiFi_2G")  , "text": "2.4 GHz <#tm_wireless#>"}];
			break;
			case "2":
				conn_type = [{"type":  ((cap_wifi_auth == "lock") ? "WiFi_5G_lock": "WiFi_5G")  , "text": "5 GHz <#tm_wireless#>"}];
			break;
			case "3":
				conn_type = [
					{"type":  ((cap_wifi_auth == "lock") ? "WiFi_5G_lock": "WiFi_5G")  , "text": "5 GHz <#tm_wireless#>"},
					{"type":  ((cap_wifi_auth == "lock") ? "WiFi_2G_lock": "WiFi_2G")  , "text": "2.4 GHz <#tm_wireless#>"}
				];
			break;
		}
		return conn_type;
	}
}
function Get_Component_Wizard_Item(_parm){
	var $profile_item_container = $("<div>").addClass("profile_item_container wizard " + _parm.type + "");
	$profile_item_container.unbind("click").click(function(e){
		e = e || event;
		e.stopPropagation();
		show_popup_Wizard_Setting(_parm.type);
	});

	var $item_text_container = $("<div>").addClass("item_text_container");
	$item_text_container.appendTo($profile_item_container);

	$("<div>").addClass("main_info").html(htmlEnDeCode.htmlEncode(_parm.title)).appendTo($item_text_container);
	$("<div>").addClass("sub_info").html(htmlEnDeCode.htmlEncode(_parm.desc)).appendTo($item_text_container);

	var $item_action_container = $("<div>").addClass("item_action_container").appendTo($profile_item_container);
	$("<div>").addClass("icon_add").appendTo($item_action_container);

	return $profile_item_container;
}
function Get_Component_Wizard_Item_Customized(){
	var $profile_item_container = $("<div>").addClass("profile_item_container wizard customized");
	$profile_item_container.unbind("click").click(function(e){
		e = e || event;
		e.stopPropagation();
		show_popup_Wizard_Setting("Customized");
	});
	var $item_text_container = $("<div>").addClass("item_text_container");
	$item_text_container.appendTo($profile_item_container);

	$("<div>").addClass("main_info").html("<#GuestNetwork_Customized#>").appendTo($item_text_container);

	var $item_action_container = $("<div>").addClass("item_action_container");
	$item_action_container.appendTo($profile_item_container);
	$("<div>").addClass("icon_add").appendTo($item_action_container);

	return $profile_item_container;
}
function Get_Component_Wizard_Item_Scenario(){
	var $profile_item_container = $("<div>").addClass("profile_item_container wizard scenario").attr({"data-container": "wizard_scenario"});
	var $item_text_container = $("<div>").addClass("item_text_container");
	$item_text_container.appendTo($profile_item_container);

	$("<div>").addClass("main_info").html("<#GuestNetwork_Scenarios_Exp#>").appendTo($item_text_container);

	var $item_action_container = $("<div>").addClass("item_action_container");
	$item_action_container.appendTo($profile_item_container);
	$("<div>").addClass("icon_add").appendTo($item_action_container);

	return $profile_item_container;
}
function Get_Component_Scenarios_Explorer(view_mode){
	var $container = $("<div>").addClass("setting_content_container");

	if(view_mode == "popup"){
		Get_Component_Popup_Profile_Title("<#GuestNetwork_Scenarios_Exp#>").addClass("Scenarios").appendTo($container)
			.find("#title_close_btn").unbind("click").click(function(e){
				e = e || event;
				e.stopPropagation();
				close_popup_container($container);
			});
	}

	var $content_container = $("<div>").appendTo($container).addClass("profile_setting");
	if(view_mode == "popup")
		$content_container.addClass("popup_content_container");
	else
		$content_container.addClass("no_popup_content_container");

	$content_container.addClass("Scenarios").appendTo($container);
	var $title = $("<div>").addClass("title btn_back_container").appendTo($content_container)
		.unbind("click").click(function(e){
			e = e || event;
			e.stopPropagation();
			var $parent_cntr = $container.parent();
			var $popup_container = $container.closest(".popup_container");
			var hide_title_flag = $popup_container.hasClass("hide_title_cntr");
			$parent_cntr.find(".setting_content_container").replaceWith(show_Get_Start(view_mode));
			if(hide_title_flag){
				$parent_cntr.find(".setting_content_container .popup_title_container").hide();
			}
			if(isMobile()){
				$popup_container.addClass("full_width");
			}
			resize_iframe_height();
		});
	$("<div>").addClass("icon_arrow_left").appendTo($title);
	$("<div>").html(htmlEnDeCode.htmlEncode("<#btn_Back#>")).appendTo($title);

	var $item_scenarios_cntr = $("<div>").addClass("item_scenarios_container").appendTo($content_container);
	$("<div>").addClass("title").html("<#GuestNetwork_SDN_desc#>").appendTo($item_scenarios_cntr);
	$.each(scenarios_list, function(index, scenariosItem){
		Get_Component_Scenarios(scenariosItem).appendTo($item_scenarios_cntr);
	});
	function Get_Component_Scenarios(_scenariosItem){
		var $scenarios_cntr = $("<div>").addClass("scenarios_container");
		var $scenarios_comp = $("<div>").addClass("scenarios_component").appendTo($scenarios_cntr).unbind("click").click(function(e){
			e = e || event;
			e.stopPropagation();
			if(_scenariosItem.type != undefined && _scenariosItem.type != "")
				show_popup_Wizard_Setting(_scenariosItem.type);
		});
		var $scenes = $("<div>").addClass("scenes").appendTo($scenarios_comp);
		if(_scenariosItem.source != undefined && _scenariosItem.source == "Cloud")
			$scenes.css("background-image", "url(" + _scenariosItem.scenes + ")");
		else
			$scenes.addClass(_scenariosItem.scenes);
		var $desc = $("<div>").addClass("desc").appendTo($scenarios_comp);
		$("<div>").html(_scenariosItem.text).appendTo($desc);
		return $scenarios_cntr;
	}

	return $container;
}
function Get_Component_Type_Scenarios(view_mode){
	var $container = $("<div>").addClass("setting_content_container");

	if(view_mode == "popup"){
		Get_Component_Popup_Profile_Title("<#GuestNetwork_Scenarios_Exp#>").addClass("Scenarios").appendTo($container)
			.find("#title_close_btn").unbind("click").click(function(e){
				e = e || event;
				e.stopPropagation();
				close_popup_container($container);
			});
	}

	var $content_container = $("<div>").appendTo($container).addClass("profile_setting");
	if(view_mode == "popup")
		$content_container.addClass("popup_content_container");
	else
		$content_container.addClass("no_popup_content_container");

	$content_container.addClass("Scenarios").appendTo($container);

	var wizard_type_list = [
		{"type":"Employee", "title":"<#GuestNetwork_Employee#>", "desc":str_GuestNetwork_Wizard_type_desc0+" "+str_GuestNetwork_Employee_recommend},
		{"type":"Portal", "title":"<#GuestNetwork_Portal#>", "desc":str_GuestNetwork_Portal_desc2},
		{"type":"Guest", "title":"<#Guest_Network#>", "desc":"<#GuestNetwork_Guests_desc#>."},
		{"type":"Sched", "title":"<#GuestNetwork_Sche_network#>", "desc":str_GuestNetwork_WiFi_Sche_desc},
		{"type":"IoT", "title":"<#GuestNetwork_IoT#>", "desc":"<#GuestNetwork_IoT_desc#>."}
	];

	var $wizard_list_bg = $("<div>").addClass("wizard_list_bg type_scenarios").appendTo($content_container);
	$.each(wizard_type_list, function(index, item){
		var $profile_item_container = $("<div>").addClass("profile_item_container wizard type_scenarios " + item.type + "")
			.appendTo($wizard_list_bg).unbind("click").click(function(e){
				e = e || event;
				e.stopPropagation();
				show_popup_Wizard_Setting(item.type);
			});
		var $item_text_container = $("<div>").addClass("item_text_container").appendTo($profile_item_container);
		$("<div>").addClass("main_info").html(htmlEnDeCode.htmlEncode(item.title)).appendTo($item_text_container);
		$("<div>").addClass("sub_info").html(htmlEnDeCode.htmlEncode(item.desc)).appendTo($item_text_container);

		var $item_scenarios_container = $("<div>").addClass("item_scenarios_container").appendTo($profile_item_container);
		$.each(scenarios_list, function(index, scenariosItem){
			if(item.type == scenariosItem.type)
				Get_Component_Scenarios(scenariosItem).appendTo($item_scenarios_container);
		});
	});

	return $container;

	function Get_Component_Scenarios(_scenariosItem){
		var $scenarios_cntr = $("<div>").addClass("scenarios_container");
		var $scenarios_comp = $("<div>").addClass("scenarios_component").appendTo($scenarios_cntr);
		var $scenes = $("<div>").addClass("scenes").appendTo($scenarios_comp);
		if(_scenariosItem.source != undefined && _scenariosItem.source == "Cloud")
			$scenes.css("background-image", "url(" + _scenariosItem.scenes + ")");
		else
			$scenes.addClass(_scenariosItem.scenes);
		var $desc = $("<div>").addClass("desc").appendTo($scenarios_comp);
		$("<div>").html(_scenariosItem.text).appendTo($desc);
		return $scenarios_cntr;
	}
}
function Get_Component_Bandwidth_Setting(_parm){
	var $container = $("<div>").addClass("profile_setting_item bandwidth_set");

	if(_parm.container_id != undefined)
		$container.attr("id", _parm.container_id);

	if(_parm.openHint != undefined){
		var hint_array = _parm.openHint.split("_");
		$("<a>").addClass("hintstyle").attr({"href":"javascript:void(0);"}).html(htmlEnDeCode.htmlEncode(_parm.title)).unbind("click").click(function(){
			openHint(hint_array[0], hint_array[1], "rwd_vpns");
		}).appendTo($("<div>").addClass("title").appendTo($container));
	}
	else
		$("<div>").addClass("title").html(htmlEnDeCode.htmlEncode(_parm.title)).appendTo($container);

	var set_value_1 = "";
	if(_parm.set_value_1 != undefined)
		set_value_1 = _parm.set_value_1;

	var set_value_2 = "";
	if(_parm.set_value_2 != undefined)
		set_value_2 = _parm.set_value_2;

	var $bandwidth_container = $("<div>").addClass("bandwidth_container").appendTo($container);

	var $download = $("<div>").addClass("bandwidth_item").appendTo($bandwidth_container);
	$("<div>").addClass("BW_icon icon_download").appendTo($download);
	var $input_1 = $("<input/>")
						.addClass("textInput")
						.attr({"id":_parm.id_1, "type":_parm.type_1, "maxlength":_parm.maxlength_1, "autocomplete":"off","autocorrect":"off","autocapitalize":"off","spellcheck":"false"})
						.val(set_value_1)
						.unbind("blur").blur(function(e){
							e = e || event;
							e.stopPropagation();
						}).on('click', function () {
							var target = this;
							setTimeout(function(){
								target.scrollIntoViewIfNeeded();
							},400);
						})
						.appendTo($download);
	if(_parm.need_check_1)
		$input_1.attr("need_check", true);

	var $upload = $("<div>").addClass("bandwidth_item").appendTo($bandwidth_container);
	$("<div>").addClass("BW_icon icon_upload").appendTo($upload);
	var $input_2 = $("<input/>")
						.addClass("textInput")
						.attr({"id":_parm.id_2, "type":_parm.type_2, "maxlength":_parm.maxlength_2, "autocomplete":"off","autocorrect":"off","autocapitalize":"off","spellcheck":"false"})
						.val(set_value_2)
						.unbind("blur").blur(function(e){
							e = e || event;
							e.stopPropagation();
						})
						.on('click', function () {
							var target = this;
							setTimeout(function(){
								target.scrollIntoViewIfNeeded();
							},400);
						})
						.appendTo($upload);

	if(_parm.need_check_2)
		$input_2.attr("need_check", true);

	return $container;
}
function Get_Component_Print_Btn(_parm){
	var $container = $("<div>").addClass("profile_setting_item nowrap switch_item");
	if(_parm.container_id != undefined)
		$container.attr("id", _parm.container_id);

	if(_parm.openHint != undefined){
		var hint_array = _parm.openHint.split("_");
		$("<a>").addClass("hintstyle").attr({"href":"javascript:void(0);"}).html(htmlEnDeCode.htmlEncode(_parm.title)).unbind("click").click(function(){
			openHint(hint_array[0], hint_array[1], "rwd_vpns");
		}).appendTo($("<div>").addClass("title").appendTo($container));
	}
	else
		$("<div>").addClass("title").html(htmlEnDeCode.htmlEncode(_parm.title)).appendTo($container);

	var $input_container = $("<div>").addClass("input_container").appendTo($container);
	var $icon_print = $("<div>").addClass("icon_print").appendTo($input_container);
	if(_parm.id != undefined)
		$icon_print.attr("id", _parm.id);

	return $container;
}
function Get_Container_Assign_DNS(_dns_list, _callback){
	var $container = $("<div>").addClass("setting_content_container");

	Get_Component_Popup_Profile_Title("<#IPConnection_x_DNS_List#>").appendTo($container)
		.find("#title_close_btn").unbind("click").click(function(e){
			e = e || event;
			e.stopPropagation();
			close_popup_container($container);
		});

	var $content_container = $("<div>").addClass("popup_content_container profile_setting").appendTo($container);

	var $DNS_List = Get_Component_DNS_List().appendTo($content_container);
	Get_Component_DNS_Manual().appendTo($DNS_List);

	var $action_container = $("<div>").addClass("action_container").appendTo($content_container);
	$("<div>").addClass("btn_container apply").html("<#CTL_ok#>").unbind("click").click(function(e){
		e = e || event;
		e.stopPropagation();
		if(validate_format($container)){
			var dns_list = {"dns1":"", "dns2":""};
			var $clicked_dns = $content_container.find("[data-container=DNS_List] .icon_radio.clicked");
			if($clicked_dns.attr("data-list-type") == "DB"){
				dns_list.dns1 = $clicked_dns.attr("data-dns1");
				dns_list.dns2 = $clicked_dns.attr("data-dns2");
			}
			else if($clicked_dns.attr("data-list-type") == "Manual"){
				$clicked_dns.attr({"data-dns1":$content_container.find("#dns1").val(), "data-dns2":$content_container.find("#dns2").val()});
				dns_list.dns1 = $clicked_dns.attr("data-dns1");
				dns_list.dns2 = $clicked_dns.attr("data-dns2");
			}
			_callback(dns_list);
			close_popup_container($container);
		}

		function validate_format(_obj){
			jQuery.fn.show_validate_hint_DNS = function(hintStr){
				$(this).closest(".profile_setting").find(".validate_hint").remove();

				$("<div>")
					.html(hintStr)
					.addClass("validate_hint")
					.insertAfter($(this).closest(".dns_list_container"));
					resize_iframe_height();
			}
			var valid_is_IP_format = function(str, type){
				var testResultPass = {
					'isError': false,
					'errReason': ''
				};
				var testResultFail = {
					'isError': true,
					'errReason': str + " <#JS_validip#>"
				};
				var format = new RegExp(ip_RegExp[type], "gi");
				if(format.test(str))
					return testResultPass;
				else
					return testResultFail;
			};
			$(_obj).find(".validate_hint").remove();
			var $clicked_dns = $(_obj).find("[data-container=DNS_List] .icon_radio.clicked");
			if($clicked_dns.attr("data-list-type") == "Manual"){
				var $dns1 = $(_obj).find("#dns1");
				var $dns2 = $(_obj).find("#dns2");
				$dns1.val($dns1.val().replace(/\s+/g, ''));//remove space
				$dns2.val($dns2.val().replace(/\s+/g, ''));//remove space
				if($dns1.val() != ""){
					var isValid_dns1 = valid_is_IP_format($dns1.val(), "IPv4");
					if(isValid_dns1.isError){
						$dns1.show_validate_hint_DNS(isValid_dns1.errReason);
						$dns1.focus();
						return false;
					}
				}
				if($dns2.val() != ""){
					var isValid_dns2 = valid_is_IP_format($dns2.val(), "IPv4");
					if(isValid_dns2.isError){
						$dns2.show_validate_hint_DNS(isValid_dns2.errReason);
						$dns2.focus();
						return false;
					}
				}
			}

			return true;
		}
	}).appendTo($action_container);

	if(_dns_list.dns1 != "" || _dns_list.dns2 != ""){
		var $specific_dns = $content_container.find("[data-dns1='"+_dns_list.dns1+"'][data-dns2='"+_dns_list.dns2+"']");
		if($specific_dns.length > 0){
			$specific_dns.addClass("clicked");
		}
		else{
			var $manual_dns = $content_container.find("[data-list-type='Manual']");
			$manual_dns.attr({"data-dns1":_dns_list.dns1, "data-dns2":_dns_list.dns2}).addClass("clicked");
			$content_container.find("#dns1").val(_dns_list.dns1);
			$content_container.find("#dns2").val(_dns_list.dns2);
		}
	}
	else{
		$content_container.find("[data-list-type=Default]").addClass("clicked");
	}

	return $container;

	function Get_Component_DNS_List(){
		var $container = $("<div>").attr({"data-container":"DNS_List"});
		Object.keys(dns_list_data).forEach(function(FilterMode){
			var FilterMode_arr = dns_list_data[FilterMode];
			if(FilterMode_arr.length > 0){
				var $dns_list_cntr = $("<div>").addClass("dns_list_container").appendTo($container);
				var $cate_title = $("<div>").addClass("category_title").html(htmlEnDeCode.htmlEncode(FilterMode)).appendTo($dns_list_cntr);
				var $item_title_cntr = $("<div>").addClass("item_container").appendTo($dns_list_cntr);
				$("<div>").addClass("radio").appendTo($item_title_cntr);
				$("<div>").addClass("service").html("<#qis_service#>").appendTo($item_title_cntr);
				$("<div>").addClass("server").html("<#HSDPAConfig_DNSServers_itemname#>").appendTo($item_title_cntr);
				$("<div>").addClass("desc").html("<#Description#>").appendTo($item_title_cntr);
				$.each(FilterMode_arr, function(index, dns_item){
					if(dns_item.confirmed == "Yes"){
						var $item_title_cntr = $("<div>").addClass("item_container dns_content").appendTo($dns_list_cntr);
						var $icon_radio = $("<div>").addClass("icon_radio")
							.attr({"data-dns1":dns_item.ServiceIP1, "data-dns2":dns_item.ServiceIP2, "data-list-type":"DB"})
							.unbind("click").click(function(e){
								e = e || event;
								e.stopPropagation();
								$(this).closest("[data-container=DNS_List]").find(".icon_radio").removeClass("clicked");
								$(this).addClass("clicked");
							});
						$("<div>").addClass("radio").append($icon_radio).appendTo($item_title_cntr);
						$("<div>").addClass("service").html(htmlEnDeCode.htmlEncode(dns_item.DNSService)).appendTo($item_title_cntr);
						$("<div>").addClass("server").html(htmlEnDeCode.htmlEncode(dns_item.ServiceIP1) + "<br>" + htmlEnDeCode.htmlEncode(dns_item.ServiceIP2)).appendTo($item_title_cntr);
						var desc = "";
						if(dns_item.Description != undefined){
							desc = htmlEnDeCode.htmlEncode(dns_item.Description);
						}
						if(dns_item.url != undefined){
							desc += "&nbsp;";
							desc += $("<a/>").attr({"href":dns_item.url, "target":"_blank"}).css({"text-decoration":"underline"}).html("<b><#Learn_more#></b>")[0].outerHTML;
						}
						$("<div>").addClass("desc").html(desc).appendTo($item_title_cntr);
					}
				});
			}
		});
		return $container
	}
	function Get_Component_DNS_Manual(){
		var $dns_list_cntr = $("<div>").addClass("dns_list_container manual_set");
		var $cate_title = $("<div>").addClass("category_title").html("<#Manual_Setting_btn#>").appendTo($dns_list_cntr);
		var $item_title_cntr = $("<div>").addClass("item_container").appendTo($dns_list_cntr);
		$("<div>").addClass("radio").appendTo($item_title_cntr);
		$("<div>").addClass("service").html("<#qis_service#>").appendTo($item_title_cntr);
		$("<div>").addClass("server").html("<#HSDPAConfig_DNSServers_itemname#>").appendTo($item_title_cntr);
		var $item_title_cntr = $("<div>").addClass("item_container dns_content").appendTo($dns_list_cntr);
		var $icon_radio = $("<div>").addClass("icon_radio")
			.attr({"data-dns1":"", "data-dns2":"", "data-list-type":"Manual"})
			.unbind("click").click(function(e){
				e = e || event;
				e.stopPropagation();
				$(this).closest("[data-container=DNS_List]").find(".icon_radio").removeClass("clicked");
				$(this).addClass("clicked");
			});
		$("<div>").addClass("radio").append($icon_radio).appendTo($item_title_cntr);
		var $service = $("<div>").addClass("service").appendTo($item_title_cntr);
		$("<div>").html("<#IPConnection_x_DNSServer1_itemname#>").appendTo($service);
		$("<div>").html("<#IPConnection_x_DNSServer2_itemname#>").appendTo($service);
		var $server = $("<div>").addClass("server").appendTo($item_title_cntr);
		$("<input/>")
			.addClass("textInput")
			.attr({"id":"dns1", "type":"text", "maxlength":15, "autocomplete":"off","autocorrect":"off","autocapitalize":"off","spellcheck":"false"})
			.on('click', function () {
				$icon_radio.click();
				var target = this;
				setTimeout(function(){
					target.scrollIntoViewIfNeeded();//for mobile view
				},400);
			})
			.unbind("keypress").keypress(function(){
				return validator.isIPAddr(this, event);
			})
			.appendTo($server);
		$("<input/>")
			.addClass("textInput")
			.attr({"id":"dns2", "type":"text", "maxlength":15, "autocomplete":"off","autocorrect":"off","autocapitalize":"off","spellcheck":"false"})
			.on('click', function () {
				$icon_radio.click();
				var target = this;
				setTimeout(function(){
					target.scrollIntoViewIfNeeded();//for mobile view
				},400);
			})
			.unbind("keypress").keypress(function(){
				return validator.isIPAddr(this, event);
			})
			.appendTo($server);

		var $default_item_title_cntr = $("<div>").addClass("item_container dns_content default").appendTo($dns_list_cntr);
		var $default_icon_radio = $("<div>").addClass("icon_radio")
			.attr({"data-dns1":"", "data-dns2":"", "data-list-type":"Default"})
			.unbind("click").click(function(e){
				e = e || event;
				e.stopPropagation();
				$(this).closest("[data-container=DNS_List]").find(".icon_radio").removeClass("clicked");
				$(this).addClass("clicked");
			});
		$("<div>").addClass("radio").append($default_icon_radio).appendTo($default_item_title_cntr);
		$("<div>").addClass("service").html("<#Setting_factorydefault_value#>").appendTo($default_item_title_cntr);

		return $dns_list_cntr;
	}
}
function Get_FWF_Change_BG_Container(){
	var $container = $("<div>").attr({"id":"FWF_Change_BG_Cntr"});

	var $btn_back_cntr = $("<div>").addClass("btn_back_container").appendTo($container)
		.unbind("click").click(function(e){
			e = e || event;
			e.stopPropagation();
			var $FWF_ui_items = $(this).closest("#FWF_ui_container").find("#FWF_Preview_Cntr, #FWF_Change_BG_Cntr");
			$FWF_ui_items.eq(0).show();
			$FWF_ui_items.eq(1).hide();
			resize_iframe_height();
		});
	$("<div>").addClass("icon_arrow_left").appendTo($btn_back_cntr);
	$("<div>").html(htmlEnDeCode.htmlEncode("<#btn_Back#>")).appendTo($btn_back_cntr);

	var $bg_template_cntr = $("<div>").addClass("bg_template_container").appendTo($container);
	$.each(FreeWiFi_template, function(index, value){
		$("<div>").addClass("bg_template")
			.css('background-image', 'url(' + value.image + ')')
			.unbind("click").click(function(e){
				e = e || event;
				e.stopPropagation();
				/*
				var $this_parent = $(this).parent(".bg_template_cntr");
				$(this).addClass("clicked").parent(".bg_template_cntr").siblings(".bg_template_cntr").children(".bg_template").not($this_parent).removeClass("clicked");
				*/
				var image = $(this).css('background-image').replace('url(','').replace(')','').replace(/\"/gi, "");
				var $FWF_ui_items = $(this).closest("#FWF_ui_container").find("#FWF_Preview_Cntr, #FWF_Change_BG_Cntr");
				$FWF_ui_items.eq(0).show().find("[data-component=FWF_bg]").css({"background-image": 'url(' + image + ')'});
				$FWF_ui_items.eq(1).hide();
			}).appendTo($("<div>").addClass("bg_template_cntr").appendTo($bg_template_cntr));
	});

	Get_Upload_Image_Container(function(_image){
		var $FWF_ui_items = $bg_template_cntr.closest("#FWF_ui_container").find("#FWF_Preview_Cntr, #FWF_Change_BG_Cntr");
		$FWF_ui_items.eq(0).show().find("[data-component=FWF_bg]").css({"background-image": 'url(' + _image + ')'});
		$FWF_ui_items.eq(1).hide();
	}).appendTo($("<div>").addClass("bg_template_cntr").appendTo($bg_template_cntr));

	return $container;
}
function Get_MB_Change_BG_Container(){
	var $container = $("<div>").attr({"id":"MB_Change_BG_Cntr"});

	var $btn_back_cntr = $("<div>").addClass("btn_back_container").appendTo($container)
		.unbind("click").click(function(e){
			e = e || event;
			e.stopPropagation();
			var $MB_ui_items = $(this).closest("#MB_ui_container").find("#MB_Preview_Cntr, #MB_Change_BG_Cntr");
			$MB_ui_items.eq(0).show();
			$MB_ui_items.eq(1).hide();
			resize_iframe_height();
		});
	$("<div>").addClass("icon_arrow_left").appendTo($btn_back_cntr);
	$("<div>").html(htmlEnDeCode.htmlEncode("<#btn_Back#>")).appendTo($btn_back_cntr);

	var $bg_template_cntr = $("<div>").addClass("bg_template_container MB").appendTo($container);
	$.each(MessageBoard_template, function(index, value){
		$("<div>").addClass("bg_template")
			.css('background-image', 'url(' + value.image + ')')
			.unbind("click").click(function(e){
				e = e || event;
				e.stopPropagation();
				var image = $(this).css('background-image').replace('url(','').replace(')','').replace(/\"/gi, "");
				var $MB_ui_items = $(this).closest("#MB_ui_container").find("#MB_Preview_Cntr, #MB_Change_BG_Cntr");
				$MB_ui_items.eq(0).show().find("[data-component=MB_bg]").css({"background-image": 'url(' + image + ')'});
				$MB_ui_items.eq(1).hide();
			}).appendTo($("<div>").addClass("bg_template_cntr").appendTo($bg_template_cntr));
	});

	Get_Upload_Image_Container(function(_image){
		var $MB_ui_items = $bg_template_cntr.closest("#MB_ui_container").find("#MB_Preview_Cntr, #MB_Change_BG_Cntr");
		$MB_ui_items.eq(0).show().find("[data-component=MB_bg]").css({"background-image": 'url(' + _image + ')'});
		$MB_ui_items.eq(1).hide();
	}).appendTo($("<div>").addClass("bg_template_cntr").appendTo($bg_template_cntr));

	return $container;
}
function Get_FWF_Preview_Container(){
	var $container = $("<div>").attr({"id":"FWF_Preview_Cntr"});
	var $FWF_bg = $("<div>").attr({"data-component":"FWF_bg"}).addClass("FWF_bg").appendTo($container);
	$FWF_bg.css('background-image', 'url(' + FreeWiFi_template[4].image + ')');

	var $FWF_change_bg = $("<div>").addClass("FWF_change_bg").attr({"title":"<#Captive_Portal_Click_Image#>"}).appendTo($FWF_bg);
	$FWF_change_bg.unbind("click").click(function(e){
		e = e || event;
		e.stopPropagation();
		var $FWF_ui_items = $(this).closest("#FWF_ui_container").find("#FWF_Preview_Cntr, #FWF_Change_BG_Cntr");
		$FWF_ui_items.eq(0).hide();
		$FWF_ui_items.eq(1).show();
	});
	var $FWF_cntr = $("<div>").addClass("FWF_container").appendTo($FWF_bg);
	var $FWF_portal_cntr = $("<div>").addClass("FWF_portal_cntr").appendTo($FWF_cntr);
	var $FWF_text_info_cntr = $("<div>").addClass("FWF_text_info_cntr").appendTo($FWF_portal_cntr);
	$("<div>").attr({"data-component":"FWF_brand_name"}).addClass("brand_name").html("<#FreeWiFi_BrandName#>").appendTo($FWF_text_info_cntr);
	$("<div>").attr({"data-component":"FWF_terms_service"}).addClass("terms_service").html(htmlEnDeCode.htmlEncode(terms_service_template).replace(/(?:\r\n|\r|\n)/g, '<div style=height:6px;></div>')).appendTo($FWF_portal_cntr);
	var $action_info_cntr = $("<div>").addClass("action_info_cntr").appendTo($FWF_portal_cntr);
	var $cb_text_container = $("<div>").addClass("cb_text_container").appendTo($action_info_cntr);
	$("<div>").addClass("cb_icon clicked").appendTo($cb_text_container);
	$("<div>").addClass("cb_text").html("<#FreeWiFi_Agree_Terms_Service#>").appendTo($cb_text_container);
	$("<div>").attr({"data-component":"FWF_passcode"}).addClass("passcode_container").html("Enter Passcode").appendTo($action_info_cntr);/* untranslated */
	$("<div>").addClass("btn_container").html("<#FreeWiFi_Continue#>").appendTo($("<div>").addClass("action_container").appendTo($action_info_cntr))
	return $container;
}
function Get_MB_Preview_Container(){
	var $container = $("<div>").attr({"id":"MB_Preview_Cntr"});
	var $MB_bg = $("<div>").attr({"data-component":"MB_bg"}).addClass("MB_bg").appendTo($container);
	$MB_bg.css('background-image', 'url(' + MessageBoard_template[0].image + ')');

	var $MB_change_bg = $("<div>").addClass("MB_change_bg").attr({"title":"<#Captive_Portal_Click_Image#>"}).appendTo($MB_bg);
	$MB_change_bg.unbind("click").click(function(e){
		e = e || event;
		e.stopPropagation();
		var $MB_ui_items = $(this).closest("#MB_ui_container").find("#MB_Preview_Cntr, #MB_Change_BG_Cntr");
		$MB_ui_items.eq(0).hide();
		$MB_ui_items.eq(1).show();
	});
	var $MB_cntr = $("<div>").addClass("MB_container").appendTo($MB_bg);
	var $MB_portal_cntr = $("<div>").addClass("MB_portal_cntr").appendTo($MB_cntr);
	var $MB_text_info_cntr = $("<div>").addClass("MB_text_info_cntr").appendTo($MB_portal_cntr);
	$("<div>").attr({"data-component":"MB_desc"}).addClass("MB_desc").html("<#Description#>").appendTo($MB_text_info_cntr);
	var $action_info_cntr = $("<div>").addClass("action_info_cntr").appendTo($MB_portal_cntr);
	$("<div>").addClass("btn_container").html("<#FreeWiFi_Continue#>").appendTo($("<div>").addClass("action_container").appendTo($action_info_cntr))
	return $container;
}
function Get_Upload_Image_Container(_callback){
	var $bg_upload_image = $("<div>").addClass("bg_upload_image").attr({"title":"<#FreeWiFi_Upload_Image#>"});
	$("<input/>").attr({"type":"file", "accept":".jpg,.jpeg,.png,.svg"}).addClass("bg_upload_file")
		.on("click", function(){
			$(this).val("");
		})
		.on("change", function(){
			var $file = $(this);
			if(!checkImageExtension($file.val())){
				show_customize_alert("<#Setting_upload_hint#>");
				return;
			}
			var fileReader = new FileReader();
			fileReader.onload = function(fileReader){
				var source_image_size = 0;
				if((fileReader.total != undefined) && (!isNaN(fileReader.total)))
					source_image_size = fileReader.total;
				if(Math.round(source_image_size / 1024) > 10240){
					show_customize_alert("<#FreeWiFi_Image_Size_Alert#>");
					return;
				}
				var $img = $("<img/>").attr("src", fileReader.target.result);
				var mimeType = $img.attr("src").split(",")[0].split(":")[1].split(";")[0];
				var $canvas = $("<canvas/>").attr({"width":"1152", "height":"864"});
				var ctx = $canvas[0].getContext("2d");
				ctx.clearRect(0, 0, 1152, 864);
				setTimeout(function(){
					ctx.drawImage($img[0], 0, 0, 1152, 864);
					var dataURL = $canvas[0].toDataURL(mimeType);
					if(Math.round(dataURL.length / 1024) > 2048){
						show_customize_alert("<#FreeWiFi_Image_Size_Compressed_Alert#>");
						return;
					}
					else{
						if(typeof _callback == "function")
							_callback(dataURL);
					}
				}, 100);
			}
			fileReader.readAsDataURL($file.prop("files")[0]);
	}).appendTo($bg_upload_image);

	return $bg_upload_image;
}
function show_feature_desc(){
	$(".container").addClass("blur_effect");
	if($(".popup_container.popup_element").css("display") == "flex"){
		$(".popup_container.popup_element").addClass("blur_effect");
	}
	$(".popup_element_second").css("display", "flex");
	$(".popup_container.popup_element_second").empty();

	$(".popup_container.popup_element_second").append(Get_Component_Feature_Desc());
	adjust_popup_container_top($(".popup_container.popup_element_second"), 100);
	resize_iframe_height();

	function Get_Component_Feature_Desc(){
		var wizard_type_list = [
			{"type":"Employee", "title":"<#GuestNetwork_Employee#>", "desc":str_GuestNetwork_Wizard_type_desc0+" "+str_GuestNetwork_Employee_recommend},
			{"type":"Portal", "title":"<#GuestNetwork_Portal#>", "desc":str_GuestNetwork_Portal_desc2+" "+stringSafeGet("<#GuestNetwork_Portal_desc3#>")},
			{"type":"Guest", "title":"<#Guest_Network#>", "desc":"<#GuestNetwork_Guests_desc#> "+stringSafeGet("<#GuestNetwork_Guests_desc2#>")},
			{"type":"Kids", "title":"<#GuestNetwork_Kid#>", "desc":str_GuestNetwork_WiFi_Sche_desc+" "+stringSafeGet("<#GuestNetwork_WiFi_Sche_desc2#>")},
			{"type":"IoT", "title":"<#GuestNetwork_IoT#>", "desc":"<#GuestNetwork_IoT_desc#> "+stringSafeGet("<#GuestNetwork_IoT_desc2#>")+""},
			{"type":"VPN", "title":"<#GuestNetwork_VPN#>", "desc":stringSafeGet("<#GuestNetwork_VPN_desc2#>")}
		];
		if(isSwMode("ap")){
			wizard_type_list = wizard_type_list.filter(function(item, index, array){
				return (item.type == "Employee" || item.type == "Guest" || item.type == "Kids");
			});
		}
		if(isSupport("BUSINESS")){
			var specific_wizard_type = wizard_type_list.filter(function(item, index, array){
				return (item.type == "Kids");
			})[0];
			if(specific_wizard_type != undefined){
				specific_wizard_type.type = "Sched";
				specific_wizard_type.title = "<#GuestNetwork_Sche_network#>";
			}
		}
		else{
			wizard_type_list = wizard_type_list.filter(function(item, index, array){
				return (item.type != "Employee" && item.type != "Portal");
			});
		}

		var $container = $("<div>");

		var $popup_title_container = $("<div>").addClass("popup_title_container");
		$popup_title_container.appendTo($container);
		$("<div>").addClass("title").html("<#NewFeatureAbout#>").appendTo($popup_title_container);
		var $close_btn = $("<div>").addClass("vpn_icon_all_collect close_btn");
		$close_btn.appendTo($popup_title_container);
		$close_btn.unbind("click").click(function(e){
			e = e || event;
			e.stopPropagation();
			close_popup_container("all");
		});

		var $popup_content_container = $("<div>").addClass("popup_content_container");
		$popup_content_container.appendTo($container);

		var $feature_desc_cntr = $("<div>").addClass("feature_desc_container").appendTo($popup_content_container);
		$("<div>").addClass("title").html("<#NewFeatureDesc#>").appendTo($feature_desc_cntr);
		$("<div>").addClass("desc").html(
			stringSafeGet("<#SDN_feature_desc#>")+" "+stringSafeGet("<#SDN_feature_desc2#>")+" "+stringSafeGet("<#SDN_feature_desc3#>")
		).appendTo($feature_desc_cntr);

		var $wizard_list_bg = $("<div>").addClass("wizard_list_bg").appendTo($feature_desc_cntr);
		$.each(wizard_type_list, function(index, item){
			var $profile_item_container = $("<div>").addClass("profile_item_container wizard feature_desc " + item.type + "").appendTo($wizard_list_bg);
			var $item_text_container = $("<div>").addClass("item_text_container").appendTo($profile_item_container);
			$("<div>").addClass("main_info").html(htmlEnDeCode.htmlEncode(item.title)).appendTo($item_text_container);
			$("<div>").addClass("sub_info").html(htmlEnDeCode.htmlEncode(item.desc)).appendTo($item_text_container);
		});

		return $container;
	}
}
function show_Get_Start(view_mode){
	var $container = $("<div>").addClass("setting_content_container");

	if(view_mode == "popup")
		$container.append(Get_Component_Popup_Profile_Title("<#QKSet_all_title#>"));

	var $content_container = $("<div>").addClass("profile_setting");
	if(view_mode == "popup")
		$content_container.addClass("popup_content_container");
	else
		$content_container.addClass("no_popup_content_container");

	$content_container.addClass("Get_Start").appendTo($container);

	var specific_wizard_type = "";
	$("<div>").addClass("title").html(htmlEnDeCode.htmlEncode(str_SDN_choose)).appendTo($content_container);
	if(isSupport("BUSINESS")){
		var $business_list_bg = $("<div>").addClass("wizard_list_bg").appendTo($content_container);
		var Employee_parm = {"type":"Employee", "title":"", "desc":""};
		specific_wizard_type = wizard_type_text.filter(function(item, index, array){
			return (item.type == "Employee");
		})[0];
		if(specific_wizard_type != undefined){
			Employee_parm.title = specific_wizard_type.text
			Employee_parm.desc = specific_wizard_type.desc;
		}
		Get_Component_Wizard_Item(Employee_parm).appendTo($business_list_bg);
	}

	var $home_list_bg = $("<div>").addClass("wizard_list_bg").appendTo($content_container);
	if(isSwMode("rt")){
		if(cp_type_rl_json.length > 0){
			var Portal_parm = {"type":"Portal", "title":"", "desc":""};
			specific_wizard_type = wizard_type_text.filter(function(item, index, array){
				return (item.type == "Portal");
			})[0];
			if(specific_wizard_type != undefined){
				Portal_parm.title = specific_wizard_type.text
				Portal_parm.desc = specific_wizard_type.desc;
			}
			Get_Component_Wizard_Item(Portal_parm).appendTo($home_list_bg);
		}
	}
	var Guest_parm = {"type":"Guest", "title":"", "desc":""};
	specific_wizard_type = wizard_type_text.filter(function(item, index, array){
		return (item.type == "Guest");
	})[0];
	if(specific_wizard_type != undefined){
		Guest_parm.title = specific_wizard_type.text
		Guest_parm.desc = specific_wizard_type.desc;
	}
	Get_Component_Wizard_Item(Guest_parm).appendTo($home_list_bg);
	var Kids_parm = {"type":"Kids", "title":"", "desc":""};
	if(isSupport("BUSINESS")){
		Kids_parm = {"type":"Sched", "title":"", "desc":""};
	}
	specific_wizard_type = wizard_type_text.filter(function(item, index, array){
		return (item.type == "Kids" || item.type == "Sched");
	})[0];
	if(specific_wizard_type != undefined){
		Kids_parm.title = specific_wizard_type.text
		Kids_parm.desc = specific_wizard_type.desc;
	}
	Get_Component_Wizard_Item(Kids_parm).appendTo($home_list_bg);
	if(isSwMode("rt")){
		var IoT_parm = {"type":"IoT", "title":"", "desc":""};
		specific_wizard_type = wizard_type_text.filter(function(item, index, array){
			return (item.type == "IoT");
		})[0];
		if(specific_wizard_type != undefined){
			IoT_parm.title = specific_wizard_type.text
			IoT_parm.desc = specific_wizard_type.desc;
		}
		Get_Component_Wizard_Item(IoT_parm).appendTo($home_list_bg);
		var VPN_parm = {"type":"VPN", "title":"", "desc":""};
		specific_wizard_type = wizard_type_text.filter(function(item, index, array){
			return (item.type == "VPN");
		})[0];
		if(specific_wizard_type != undefined){
			VPN_parm.title = specific_wizard_type.text
			VPN_parm.desc = specific_wizard_type.desc;
		}
		Get_Component_Wizard_Item(VPN_parm).appendTo($home_list_bg);

		if(isSupport("BUSINESS")){
			var $scenario_list_bg = $("<div>").addClass("wizard_list_bg").appendTo($content_container);
			Get_Component_Wizard_Item_Scenario().appendTo($scenario_list_bg)
				.unbind("click").click(function(e){
					e = e || event;
					e.stopPropagation();
					var $parent_cntr = $container.parent();
					var $popup_cntr = $container.closest(".popup_container");
					var hide_title_flag = $popup_cntr.hasClass("hide_title_cntr");
					$parent_cntr.find(".setting_content_container").replaceWith(Get_Component_Scenarios_Explorer(view_mode));
					if(hide_title_flag){
						$parent_cntr.find(".setting_content_container .popup_title_container").hide();
					}
					if(isMobile()){
						$popup_cntr.addClass("full_width");
					}
					resize_iframe_height();
					resize_iframe_height();
					window.scrollTo({top: 0});
				});
		}
	}
	var $customized_list_bg = $("<div>").addClass("wizard_list_bg").appendTo($content_container);
	Get_Component_Wizard_Item_Customized().appendTo($customized_list_bg);
	return $container;
}
function show_popup_Wizard_Setting(_type){
	if(_type == undefined || _type == "")
		return;

	if(sdn_all_rl_json.length >= sdn_maximum){
		show_customize_alert("<#AiMesh_Binding_Rule_Maxi#>");
		return;
	}

	$(".container").addClass("blur_effect");
	$(".qis_container").addClass("blur_effect");
	if($(".popup_container.popup_element").css("display") == "flex"){
		$(".popup_container.popup_element").addClass("blur_effect");
	}
	$(".popup_element_second").css("display", "flex");
	$(".popup_container.popup_element_second").empty();

	switch(_type){
		case "Employee":
			$(".popup_container.popup_element_second").append(Get_Wizard_Employee("popup"));
			break;
		case "Guest":
			$(".popup_container.popup_element_second").append(Get_Wizard_Guest("popup"));
			break;
		case "Kids":
		case "Sched":
			$(".popup_container.popup_element_second").append(Get_Wizard_Kids("popup"));
			break;
		case "IoT":
			$(".popup_container.popup_element_second").append(Get_Wizard_IoT("popup"));
			break;
		case "VPN":
			$(".popup_container.popup_element_second").append(Get_Wizard_VPN("popup"));
			break;
		case "Customized":
			$(".popup_container.popup_element_second").append(Get_Wizard_Customized("popup"));
			break;
		case "Portal":
			$(".popup_container.popup_element_second").append(Get_Wizard_Portal("popup"));
			break;
	}

	var $setting_content_cntr = $(".popup_container.popup_element_second .setting_content_container").attr({"data-wizard-type": _type});
	if(is_cfg_ready){
		update_aimesh_wifi_band_info();
		update_aimesh_wifi_band_full();
		if(aimesh_wifi_band_full.all.band_1 && !(aimesh_wifi_band_full.all.band_2)){
			set_value_Custom_Select($setting_content_cntr, "wifi_band", "2");
		}
		else if(!(aimesh_wifi_band_full.all.band_1) && aimesh_wifi_band_full.all.band_2){
			set_value_Custom_Select($setting_content_cntr, "wifi_band", "1");
		}
		else{
			set_value_Custom_Select($setting_content_cntr, "wifi_band", "3");
		}
		var $AiMesh_List = $setting_content_cntr.find("[data-container=AiMesh_List]");
		var $sel_wifi_band = $setting_content_cntr.find("#select_wifi_band");
		if(_type == "IoT"){
			set_value_Custom_Select($setting_content_cntr, "wifi_band", "1");
		}
		Set_AiMesh_List_CB($AiMesh_List, $sel_wifi_band.children(".selected").attr("value"));
		if(!($AiMesh_List.find(".icon_checkbox").hasClass("clicked"))){
			$setting_content_cntr.find("#more_config").click();
		}
	}
	else{
		$setting_content_cntr.find("#wifi_band").closest(".profile_setting_item").remove();
		$setting_content_cntr.find("[data-container=wizard_aimesh_cntr]").remove();
	}

	var portal_status = {"support":false, "type":"0"};
	if(_type == "Portal"){
		portal_status.support = true;
		portal_status.type = "2";
	}
	else if(_type == "Kids"){
		if(get_cp_type_support("4")){
			portal_status.support = true;
			portal_status.type = "4";
		}
	}
	if(portal_status.support){
		$setting_content_cntr.find("[data-portal-type]").hide(0, function(){
			$(this).find(".profile_setting_item").hide();
		}).filter("[data-portal-type=0]").show(0, function(){
			$(this).find(".profile_setting_item").show();
		});
		var $select_portal_type = $setting_content_cntr.find("#select_portal_type");
		$.each(cp_type_rl_json, function(index, item){
			if(item.sdn_idx != ""){
				$select_portal_type.children("[value='"+item.cp_idx+"']")
					.addClass("disabled")
					.attr({"data-disabled":"true"})
					.html(htmlEnDeCode.htmlEncode(item.cp_text + " (<#Status_Used#>)"))
			}
		});
		if($select_portal_type.children("[value="+portal_status.type+"]").length > 0){
			var default_select_portal = {"type":"0", "show_hint":false}
			if(!$select_portal_type.children("[value="+portal_status.type+"]").hasClass("disabled")){//check same type
				default_select_portal.type = portal_status.type;
			}
			if(default_select_portal.type != "0"){
				$.each(sdn_all_rl_json, function(index, item) {
					if(item.sdn_rl.cp_idx != "0" && item.sdn_rl.cp_idx != default_select_portal.type){//check other type
						if(item.sdn_rl.sdn_enable == "1"){
							default_select_portal.type = "0";
							default_select_portal.show_hint = true;
							return false;
						}
					}
				});
			}
			if(default_select_portal.show_hint){
				$select_portal_type
					.closest(".custom_select_container")
					.attr("temp_disable", "disabled")
					.closest(".profile_setting_item")
					.after($("<div>").html("<#vpnc_conn_maxi_general#>".replace(/VPN/g, "<#GuestNetwork_Portal_type#>").replace(/2/g, "1")).addClass("item_hint"));
			}
			set_value_Custom_Select($setting_content_cntr, "portal_type", default_select_portal.type);
			$setting_content_cntr.find("[data-portal-type]").hide(0, function(){
				$(this).find(".profile_setting_item").hide();
			}).filter("[data-portal-type="+default_select_portal.type+"]").show(0, function(){
				$(this).find(".profile_setting_item").show();
			});
		}
	}

	adjust_popup_container_top($(".popup_container.popup_element_second"), 100);
	resize_iframe_height();

	function Get_Wizard_Employee(view_mode){
		var _set_apply_btn_status = function(_obj){
			var $btn_container_apply = $(_obj).find(".action_container .btn_container.apply");
			var isBlank = validate_isBlank($(_obj));
			if(isBlank){
				$btn_container_apply.removeClass("valid_fail").addClass("valid_fail").unbind("click");
			}
			else{
				$btn_container_apply.removeClass("valid_fail").unbind("click").click(function(e){
					e = e || event;
					e.stopPropagation();
					var wizard_type = $(this).closest(".setting_content_container").attr("data-wizard-type");
					if(validate_format_Wizard_Item($(_obj), wizard_type)){
						var sdn_obj = get_new_sdn_profile();
						var sdn_idx = Object.keys(sdn_obj);
						var sdn_profile = sdn_obj[sdn_idx];
						selected_sdn_idx = sdn_profile.sdn_rl.idx;
						var nvramSet_obj = {"action_mode": "apply", "rc_service": "restart_wireless;restart_sdn " + selected_sdn_idx + ";"};
						var rc_append = "";
						var wifi_band = (!is_cfg_ready) ? 3 : (parseInt($(_obj).find("#select_wifi_band").children(".selected").attr("value")));
						sdn_profile.sdn_rl.sdn_name = wizard_type;
						sdn_profile.apg_rl.ssid = $(_obj).find("#sdn_name").val();
						if(wifi_band > 0){
							var sec_option_id = $(_obj).find("#security_employee .switch_text_container").children(".selected").attr("data-option-id");
							var wifi_pwd = "";
							var wifi_auth = "psk2";
							var wifi_crypto = "aes";
							if(sec_option_id == "radius"){
								sdn_profile.radius_rl = get_new_radius_rl();
								sdn_profile.radius_rl.radius_idx = sdn_profile.apg_rl.apg_idx;
								sdn_profile.radius_rl.auth_server_1 = $(_obj).find("#radius_ipaddr").val();
								sdn_profile.radius_rl.auth_port_1 = $(_obj).find("#radius_port").val();
								sdn_profile.radius_rl.auth_key_1 =$(_obj).find("#radius_key").val();
								wifi_auth = $(_obj).find("#select_wifi_auth_radius").children(".selected").attr("value");
								if(wifi_auth == "suite-b")
									wifi_crypto = "suite-b";
							}
							else if(sec_option_id == "pwd"){
								wifi_auth = $(_obj).find("#select_wifi_auth").children(".selected").attr("value");
								wifi_pwd = $(_obj).find("#sdn_pwd").val();
							}
							var radius_idx = sdn_profile.apg_rl.apg_idx;
							sdn_profile.apg_rl.security = "<3>" + wifi_auth + ">" + wifi_crypto + ">" + wifi_pwd + ">" + radius_idx + "";
							sdn_profile.apg_rl.security += "<13>" + wifi_auth + ">" + wifi_crypto + ">" + wifi_pwd + ">" + radius_idx + "";
							sdn_profile.apg_rl.security += "<16>sae>aes>" + wifi_pwd + ">" + radius_idx + "";
							sdn_profile.apg_rl.sched = schedule_handle_data.json_array_to_string(wizard_schedule.Get_Value());
							if($(_obj).find("#wizard_schedule").hasClass("on")){
								sdn_profile.apg_rl.timesched = ((sdn_profile.apg_rl.sched == "") ? "0" : "1");
							}
							else{
								sdn_profile.apg_rl.timesched = "0";
							}
							if($(_obj).find("#bw_enabled").hasClass("on")){
								sdn_profile.apg_rl.bw_limit = "<1>" + ($(_obj).find("#bw_ul").val())*1024 + ">" + ($(_obj).find("#bw_dl").val())*1024;
								nvramSet_obj.qos_enable = "1";
								nvramSet_obj.qos_type = "2";
								rc_append += "restart_qos;restart_firewall;";
							}
							else
								sdn_profile.apg_rl.bw_limit = "<0>>";
						}
						var dut_list = "";
						if(is_cfg_ready){
							if(wifi_band > 0){
								$(_obj).find("[data-container=AiMesh_List]").find(".node_container").each(function(){
									if($(this).find(".icon_checkbox").hasClass("clicked")){
										var node_mac = $(this).attr("data-node-mac");
										var specific_node = cfg_clientlist.filter(function(item, index, array){
											return (item.mac == node_mac);
										})[0];
										if(specific_node != undefined){
											dut_list += get_specific_dut_list(wifi_band, specific_node);
										}
									}
								});
								dut_list += get_unChecked_dut_list(_obj);
							}
							else{
								dut_list = get_dut_list(wifi_band);
							}
						}
						else{
							dut_list = get_dut_list_by_mac("3", cap_mac);
						}
						sdn_profile.apg_rl.dut_list = dut_list;

						sdn_profile.sdn_access_rl = [];
						if($(_obj).find("#access_intranet").hasClass("on")){
							sdn_profile.sdn_access_rl.push({"access_sdn_idx": "0", "sdn_idx": sdn_profile.sdn_rl.idx});
						}
						sdn_profile.apg_rl.ap_isolate = "0";

						var sdn_all_rl_tmp = JSON.parse(JSON.stringify(sdn_all_rl_json));
						sdn_all_rl_tmp.push(JSON.parse(JSON.stringify(sdn_profile)));
						var vlan_rl_tmp = JSON.parse(JSON.stringify(vlan_rl_json));
						vlan_rl_tmp.push(JSON.parse(JSON.stringify(sdn_profile.vlan_rl)));
						var sdn_all_list = parse_JSONToStr_sdn_all_list({"sdn_all_rl":sdn_all_rl_tmp, "vlan_rl":vlan_rl_tmp});
						if(rc_append != "")
							nvramSet_obj.rc_service = nvramSet_obj.rc_service + rc_append;
						$.extend(nvramSet_obj, sdn_all_list);
						var apgX_rl = parse_apg_rl_to_apgX_rl(sdn_profile.apg_rl);
						$.extend(nvramSet_obj, apgX_rl);
						if(is_QIS_flow){
							nvramSet_obj.sdn_rc_service = nvramSet_obj.rc_service;
							delete nvramSet_obj["rc_service"];
							delete nvramSet_obj["action_mode"];
							$(this).replaceWith($("<img>").attr({"width": "30px", "src": "/images/InternetScan.gif"}));
							apply.SDN_Profile(nvramSet_obj);
							return;
						}
						var showLoading_status = get_showLoading_status(nvramSet_obj.rc_service);
						if(!httpApi.app_dataHandler){
							showLoading();
							close_popup_container("all");
							if(isWLclient()){
								showLoading(showLoading_status.time);
								setTimeout(function(){
									showWlHintContainer();
								}, showLoading_status.time*1000);
								check_isAlive_and_redirect({"page": "SDN.asp", "time": showLoading_status.time});
							}
						}
						httpApi.nvramSet(nvramSet_obj, function(){
							if(isWLclient()) return;
							showLoading(showLoading_status.time);
							setTimeout(function(){
								init_sdn_all_list();
								show_sdn_profilelist();
								if(!window.matchMedia('(max-width: 575px)').matches)
									$("#profile_list_content").find("[sdn_idx=" + selected_sdn_idx + "] .item_text_container").click();
							}, showLoading_status.time*1000);
							if(!isMobile()){
								if(showLoading_status.disconnect){
									check_isAlive_and_redirect({"page": "SDN.asp", "time": showLoading_status.time});
								}
							}
						});
					}
				});
			}
		};

		var $container = $("<div>").addClass("setting_content_container");

		if(view_mode == "popup"){
			Get_Component_Popup_Profile_Title("<#GuestNetwork_Employee#>").appendTo($container)
				.find("#title_close_btn").unbind("click").click(function(e){
					e = e || event;
					e.stopPropagation();
					close_popup_container($container);
				});
		}
		else
			Get_Component_Profile_Title("<#GuestNetwork_Employee#>").appendTo($container);

		var $content_container = $("<div>").addClass("popup_content_container profile_setting").appendTo($container);

		var sdn_name_parm = {"title":"<#QIS_finish_wireless_item1#>", "type":"text", "id":"sdn_name", "need_check":true, "maxlength":32, "openHint":"0_1"};
		Get_Component_Input(sdn_name_parm).appendTo($content_container)
			.find("#" + sdn_name_parm.id + "")
			.unbind("keypress").keypress(function(){
				return validator.isString(this, event);
			});

		var security_options = [{"text":"<#HSDPAConfig_Password_itemname#>","option_id":"pwd"}, {"text":"<#menu5_1_5#>","option_id":"radius"}];
		var security_parm = {"title":"<#Security#>", "options": security_options, "container_id":"security_employee"};
		Get_Component_Switch_Text(security_parm).attr({"data-group":"wifi_settings"}).appendTo($content_container)
			.find(".switch_text_container > .switch_text_item")
			.click(function(e){
				e = e || event;
				e.stopPropagation();
				var option_id = $(this).attr("data-option-id");
				$(this).closest(".profile_setting").find("[data-sec-option-id]").hide().next(".validate_hint").remove();
				$(this).closest(".profile_setting").find("[data-sec-option-id=" + option_id + "]").show();
				if(option_id == "pwd"){
					$(this).closest(".profile_setting").find("#sdn_pwd_strength").hide();
				}
				resize_iframe_height();
				_set_apply_btn_status($(this).closest(".profile_setting"));
				if(is_cfg_ready && isSupport("wpa3-e")){
					Set_AiMesh_List_CB(
						$(this).closest(".profile_setting").find("[data-container=AiMesh_List]"),
						$(this).closest(".profile_setting").find("#select_wifi_band").children(".selected").attr("value")
					);
				}
			});

		var wifi_auth_options = [{"text":"WPA2-Personal","value":"psk2"},/*{"text":"WPA3-Personal","value":"sae"},*/{"text":"WPA/WPA2-Personal","value":"pskpsk2"},{"text":"WPA2/WPA3-Personal","value":"psk2sae"}];
		var wifi_auth_options_parm = {"title": "<#WLANConfig11b_AuthenticationMethod_itemname#>", "id": "wifi_auth", "options": wifi_auth_options, "set_value": "psk2"};
		Get_Component_Custom_Select(wifi_auth_options_parm).attr({"data-sec-option-id":"pwd", "data-group":"wifi_settings"}).appendTo($content_container);

		var wifi_auth_radius_options = [{"text":"WPA2-Enterprise","value":"wpa2"},{"text":"WPA/WPA2-Enterprise","value":"wpawpa2"}];
		if(isSupport("wpa3-e")){
			wifi_auth_radius_options.push({"text":"WPA3-Enterprise","value":"wpa3"});
			wifi_auth_radius_options.push({"text":"WPA2/WPA3-Enterprise","value":"wpa2wpa3"});
			wifi_auth_radius_options.push({"text":"WPA3-Enterprise 192-bit","value":"suite-b"});
		}
		var wifi_auth_radius_options_parm = {"title": "<#WLANConfig11b_AuthenticationMethod_itemname#>", "id": "wifi_auth_radius", "options": wifi_auth_radius_options, "set_value": "wpa2"};
		var $sel_wifi_auth_radius = Get_Component_Custom_Select(wifi_auth_radius_options_parm).attr({"data-sec-option-id":"radius", "data-group":"wifi_settings"}).appendTo($content_container).find("#select_" + wifi_auth_radius_options_parm.id + "");
		if(is_cfg_ready && isSupport("wpa3-e")){
			$sel_wifi_auth_radius.children("div").click(function(e){
				Set_AiMesh_List_CB(
					$(this).closest(".profile_setting").find("[data-container=AiMesh_List]"),
					$(this).closest(".profile_setting").find("#select_wifi_band").children(".selected").attr("value")
				);
			});
		}

		var sdn_pwd_parm = {"title":"<#QIS_finish_wireless_item2#>", "type":"password", "id":"sdn_pwd", "need_check":true, "maxlength":32, "openHint":"0_7"};
		Get_Component_Input(sdn_pwd_parm).attr({"data-sec-option-id":"pwd", "data-group":"wifi_settings"}).appendTo($content_container)
			.find("#" + sdn_pwd_parm.id + "")
			.unbind("keyup").keyup(function(){
				chkPass($(this).val(), "rwd_vpn_pwd", $("#sdn_pwd_strength"));
				resize_iframe_height();
			})
			.unbind("blur").blur(function(){
				if($(this).val() == "")
					$("#" + sdn_pwd_parm.id + "_strength").hide();
			});
		$("<div>").attr({"id":"sdn_pwd_strength", "data-sec-option-id":"pwd", "data-group":"wifi_settings"}).append(Get_Component_PWD_Strength_Meter()).appendTo($content_container).hide();

		var radius_ipaddr_parm = {"title":"<#WLANAuthentication11a_ExAuthDBIPAddr_itemname#>", "type":"text", "id":"radius_ipaddr", "need_check":true, "maxlength":15, "openHint":"2_1"};
		Get_Component_Input(radius_ipaddr_parm).attr({"data-sec-option-id":"radius", "data-group":"wifi_settings"}).appendTo($content_container)
			.find("#" + radius_ipaddr_parm.id + "")
			.unbind("keypress").keypress(function(){
				return validator.isIPAddr(this, event);
			});

		var radius_port_parm = {"title":"<#WLANAuthentication11a_ExAuthDBPortNumber_itemname#>", "type":"text", "id":"radius_port", "need_check":true, "maxlength":5, "openHint":"2_2"};
		Get_Component_Input(radius_port_parm).attr({"data-sec-option-id":"radius", "data-group":"wifi_settings"}).appendTo($content_container)
			.find("#" + radius_port_parm.id + "")
			.unbind("keypress").keypress(function(){
				return validator.isNumber(this,event);
			});

		var radius_key_parm = {"title":"<#WLANAuthentication11a_ExAuthDBPassword_itemname#>", "type":"password", "id":"radius_key", "need_check":true, "maxlength":64, "openHint":"2_3"};
		Get_Component_Input(radius_key_parm).attr({"data-sec-option-id":"radius", "data-group":"wifi_settings"}).appendTo($content_container);

		$content_container.find("[data-sec-option-id=radius]").hide();

		var more_config_parm = {"title":"<#MoreConfig#>", "id":"more_config", "slide_target":"more_config_cntr"};
		Get_Component_Slide_Title(more_config_parm).appendTo($content_container);
		var $more_config_cntr = $("<div>").attr({"data-slide_target": "more_config_cntr"}).hide().appendTo($content_container);

		var wifi_band_options = [{"text":"2.4GHz / 5GHz","value":"3"}, {"text":"2.4GHz","value":"1"}, {"text":"5GHz","value":"2"}, {"text":"<#wl_securitylevel_0#>","value":"0"}];
		var wifi_band_options_parm = {"title": "WiFi <#Interface#>", "id": "wifi_band", "options": wifi_band_options, "set_value": "3"};
		var $sel_wifi_band = Get_Component_Custom_Select(wifi_band_options_parm).appendTo($more_config_cntr).find("#select_" + wifi_band_options_parm.id + "");
		$sel_wifi_band.children("div").click(function(e){
			var options = $(this).attr("value");
			var $profile_setting = $(this).closest(".profile_setting");
			$profile_setting.find("[data-container=AiMesh_List]").next(".validate_hint").remove();
			var $wifi_settings_objs = $profile_setting.find("[data-group=wifi_settings]");
			if(options == "0"){
				$wifi_settings_objs.hide().next(".validate_hint").remove();
				_set_apply_btn_status($profile_setting);
			}
			else{
				$wifi_settings_objs.show().filter("#wizard_aimesh").addClass("arrow_up");
				Set_AiMesh_List_CB($profile_setting.find("[data-container=AiMesh_List]"), options);
				if($wifi_settings_objs.find(".switch_text_container").length > 0)
					$wifi_settings_objs.find(".switch_text_container .switch_text_item.selected").click();
				if($wifi_settings_objs.find("#bw_enabled").length > 0){
					if($wifi_settings_objs.find("#bw_enabled").hasClass("on")){
						$wifi_settings_objs.filter("#container_bw").show();
					}
					else{
						$wifi_settings_objs.filter("#container_bw").hide();
					}
				}
				if($wifi_settings_objs.find("#wizard_schedule").length > 0){
					if($wifi_settings_objs.find("#wizard_schedule").hasClass("on"))
						$wifi_settings_objs.filter("#container_wizard_schedule").show();
					else
						$wifi_settings_objs.filter("#container_wizard_schedule").hide();
				}
				_set_apply_btn_status($profile_setting);
			}
			resize_iframe_height();
		});

		var aimesh_parm = {"title":"<#aimesh_mode#>", "id":"wizard_aimesh", "slide_target":"wizard_aimesh_cntr"};
		var $wizard_slide_aimesh = Get_Component_Slide_Title(aimesh_parm).attr({"data-container":"wizard_aimesh_cntr", "data-group":"wifi_settings"}).addClass("arrow_up").appendTo($more_config_cntr);
		var $wizard_aimesh_cntr = $("<div>").attr({"data-slide_target":"wizard_aimesh_cntr", "data-container":"wizard_aimesh_cntr", "data-group":"wifi_settings"}).show().appendTo($more_config_cntr);
		var $AiMesh_List = $("<div>").attr({"data-container":"AiMesh_List"}).append(Get_AiMesh_List_Container()).appendTo($wizard_aimesh_cntr);
		$AiMesh_List.find(".node_content_container").click(function(e){
			e = e || event;
			e.stopPropagation();
			_set_apply_btn_status($(this).closest(".profile_setting"));
		});

		var wizard_schedule = new schedule({
			show_timeset_viewport_callback: resize_iframe_height,
			icon_trash_callback: resize_iframe_height,
			btn_save_callback: resize_iframe_height,
			btn_cancel_callback: resize_iframe_height
		});
		var wizard_schedule_parm = {"title":str_WiFi_sche, "type":"switch", "id":"wizard_schedule", "set_value":"off"};
		Get_Component_Switch(wizard_schedule_parm).attr({"data-group":"wifi_settings"}).appendTo($more_config_cntr).find("#" + wizard_schedule_parm.id + "").click(function(e){
			e = e || event;
			e.stopPropagation();
			var $control_container = $(this).closest(".profile_setting").find("#container_wizard_schedule");
			if($(this).hasClass("on")){
				$control_container.empty().append(wizard_schedule.Get_UI());
				$control_container.show();
			}
			else{
				$control_container.hide();
			}
			resize_iframe_height();
		});
		$("<div>").addClass("profile_setting_item schedule_ui").attr({"id":"container_wizard_schedule", "data-group":"wifi_settings"}).append(wizard_schedule.Get_UI()).hide().appendTo($more_config_cntr);

		if(isSwMode("rt")){
			var bw_enabled_parm = {"title":"<#Bandwidth_Limiter#>", "type":"switch", "id":"bw_enabled", "set_value":"off"};
			Get_Component_Switch(bw_enabled_parm).attr({"data-group":"wifi_settings"}).appendTo($more_config_cntr)
				.find("#" + bw_enabled_parm.id + "").click(function(e){
					e = e || event;
					e.stopPropagation();
					var $control_container = $(this).closest(".profile_setting").find("#container_bw_dl, #container_bw_ul, #container_bw");
					if($(this).hasClass("on")){
						$control_container.show();
					}
					else{
						$control_container.hide().next(".validate_hint").remove();
					}
					resize_iframe_height();
					_set_apply_btn_status($(this).closest(".profile_setting"));
				});

			var bw_parm = {"title":"<#Bandwidth_Setting#> (Mb/s)", "container_id":"container_bw",
				"type_1":"text", "id_1":"bw_dl", "maxlength_1":12, "need_check_1":true,
				"type_2":"text", "id_2":"bw_ul", "maxlength_2":12, "need_check_2":true};
			Get_Component_Bandwidth_Setting(bw_parm).attr({"data-group":"wifi_settings"}).hide().appendTo($more_config_cntr);

			var access_intranet_parm = {"title":"<#Access_Intranet#>", "type":"switch", "id":"access_intranet", "openHint":"0_26"};
			Get_Component_Switch(access_intranet_parm).appendTo($more_config_cntr);
		}

		var $action_container = $("<div>").addClass("action_container").appendTo($content_container);
		$("<div>").addClass("btn_container apply").html("<#CTL_apply#>").appendTo($action_container);

		$content_container.find("[need_check=true]").keyup(function(){
			_set_apply_btn_status($content_container);
		});
		_set_apply_btn_status($content_container);

		return $container;
	}
	function Get_Wizard_Kids(view_mode){
		var _set_apply_btn_status = function(_obj){
			var $btn_container_apply = $(_obj).find(".action_container .btn_container.apply");
			var isBlank = validate_isBlank($(_obj));
			if(isBlank){
				$btn_container_apply.removeClass("valid_fail").addClass("valid_fail").unbind("click");
			}
			else{
				$btn_container_apply.removeClass("valid_fail").unbind("click").click(function(e){
					e = e || event;
					e.stopPropagation();
					var wizard_type = $(this).closest(".setting_content_container").attr("data-wizard-type");
					if(validate_format_Wizard_Item($(_obj), wizard_type)){
						var sdn_obj = get_new_sdn_profile();
						var sdn_idx = Object.keys(sdn_obj);
						var sdn_profile = sdn_obj[sdn_idx];
						selected_sdn_idx = sdn_profile.sdn_rl.idx;
						var nvramSet_obj = {"action_mode": "apply", "rc_service": "restart_wireless;restart_sdn " + selected_sdn_idx + ";"};
						var rc_append = "";
						var wifi_band = (!is_cfg_ready) ? 3 : (parseInt($(_obj).find("#select_wifi_band").children(".selected").attr("value")));
						sdn_profile.sdn_rl.sdn_name = wizard_type;
						sdn_profile.apg_rl.ssid = $(_obj).find("#sdn_name").val();
						if(wifi_band > 0){
							var $sdn_pwd = $(_obj).find("#sdn_pwd");
							var radius_idx = sdn_profile.apg_rl.apg_idx;
							sdn_profile.apg_rl.security = "<3>psk2>aes>" + $sdn_pwd.val() + ">" + radius_idx + "";
							sdn_profile.apg_rl.security += "<13>psk2>aes>" + $sdn_pwd.val() + ">" + radius_idx + "";
							sdn_profile.apg_rl.security += "<16>sae>aes>" + $sdn_pwd.val() + ">" + radius_idx + "";
							sdn_profile.apg_rl.sched = schedule_handle_data.json_array_to_string(wizard_schedule.Get_Value());
							if($(_obj).find("#wizard_schedule").hasClass("on")){
								sdn_profile.apg_rl.timesched = ((sdn_profile.apg_rl.sched == "") ? "0" : "1");
							}
							else{
								sdn_profile.apg_rl.timesched = "0";
							}
							if($(_obj).find("#bw_enabled").hasClass("on")){
								sdn_profile.apg_rl.bw_limit = "<1>" + ($(_obj).find("#bw_ul").val())*1024 + ">" + ($(_obj).find("#bw_dl").val())*1024;
								nvramSet_obj.qos_enable = "1";
								nvramSet_obj.qos_type = "2";
								rc_append += "restart_qos;restart_firewall;";
							}
							else
								sdn_profile.apg_rl.bw_limit = "<0>>";
						}
						var dut_list = "";
						if(is_cfg_ready){
							if(wifi_band > 0){
								$(_obj).find("[data-container=AiMesh_List]").find(".node_container").each(function(){
									if($(this).find(".icon_checkbox").hasClass("clicked")){
										var node_mac = $(this).attr("data-node-mac");
										var specific_node = cfg_clientlist.filter(function(item, index, array){
											return (item.mac == node_mac);
										})[0];
										if(specific_node != undefined){
											dut_list += get_specific_dut_list(wifi_band, specific_node);
										}
									}
								});
								dut_list += get_unChecked_dut_list(_obj);
							}
							else{
								dut_list = get_dut_list(wifi_band);
							}
						}
						else{
							dut_list = get_dut_list_by_mac("3", cap_mac);
						}
						sdn_profile.apg_rl.dut_list = dut_list;

						sdn_profile.sdn_access_rl = [];
						if($(_obj).find("#access_intranet").hasClass("on")){
							sdn_profile.sdn_access_rl.push({"access_sdn_idx": "0", "sdn_idx": sdn_profile.sdn_rl.idx});
						}
						sdn_profile.apg_rl.ap_isolate = "0";
						if(wizard_type == "Kids"){
							if(get_cp_type_support("4")){
								sdn_profile.sdn_rl.cp_idx = $(_obj).find("#select_portal_type").children(".selected").attr("value");
								if(sdn_profile.sdn_rl.cp_idx == "4"){
									if($.isEmptyObject(sdn_profile.cp_rl)){
										var idx_for_customized_ui = "";
										sdn_profile.cp_rl = JSON.parse(JSON.stringify(new cp_profile_attr()));
										var specific_cp_type_rl = cp_type_rl_json.filter(function(item, index, array){
											return (item.cp_idx == sdn_profile.sdn_rl.cp_idx);
										})[0];
										if(specific_cp_type_rl != undefined){
											if(specific_cp_type_rl.profile[0] != undefined){
												idx_for_customized_ui = specific_cp_type_rl.profile[0].idx_for_customized_ui;
											}
										}
										if(idx_for_customized_ui == ""){
											idx_for_customized_ui = $.now();
										}
										sdn_profile.cp_rl.idx_for_customized_ui = idx_for_customized_ui;
									}
									sdn_profile.cp_rl.cp_idx = sdn_profile.sdn_rl.cp_idx;
									sdn_profile.cp_rl.enable = "1";
									sdn_profile.cp_rl.conntimeout = 60*60;
									sdn_profile.cp_rl.redirecturl = "";
									sdn_profile.cp_rl.auth_type = "0";
									sdn_profile.cp_rl.term_of_service = "0";
									sdn_profile.cp_rl.NAS_ID = "";
									uploadFreeWiFi({
										"cp_idx": "4",
										"id": sdn_profile.cp_rl.idx_for_customized_ui,
										"MB_desc": $("#MB_desc").val(),
										"image": $(_obj).find("#MB_ui_container [data-component=MB_bg]").css('background-image').replace('url(','').replace(')','').replace(/\"/gi, ""),
										"auth_type": sdn_profile.cp_rl.auth_type
									});
									rc_append += "restart_chilli;restart_uam_srv;";
								}
							}
						}
						var sdn_all_rl_tmp = JSON.parse(JSON.stringify(sdn_all_rl_json));
						sdn_all_rl_tmp.push(JSON.parse(JSON.stringify(sdn_profile)));
						var vlan_rl_tmp = JSON.parse(JSON.stringify(vlan_rl_json));
						vlan_rl_tmp.push(JSON.parse(JSON.stringify(sdn_profile.vlan_rl)));
						var sdn_all_list = parse_JSONToStr_sdn_all_list({"sdn_all_rl":sdn_all_rl_tmp, "vlan_rl":vlan_rl_tmp});
						if(rc_append != "")
							nvramSet_obj.rc_service = nvramSet_obj.rc_service + rc_append;
						$.extend(nvramSet_obj, sdn_all_list);
						var apgX_rl = parse_apg_rl_to_apgX_rl(sdn_profile.apg_rl);
						$.extend(nvramSet_obj, apgX_rl);
						if(sdn_profile.sdn_rl.cp_idx == "4"){
							var cpX_rl = parse_cp_rl_to_cpX_rl(sdn_profile.cp_rl);
							$.extend(nvramSet_obj, cpX_rl);
						}
						if(is_QIS_flow){
							nvramSet_obj.sdn_rc_service = nvramSet_obj.rc_service;
							delete nvramSet_obj["rc_service"];
							delete nvramSet_obj["action_mode"];
							$(this).replaceWith($("<img>").attr({"width": "30px", "src": "/images/InternetScan.gif"}));
							apply.SDN_Profile(nvramSet_obj);
							return;
						}
						var showLoading_status = get_showLoading_status(nvramSet_obj.rc_service);
						if(!httpApi.app_dataHandler){
							showLoading();
							close_popup_container("all");
							if(isWLclient()){
								showLoading(showLoading_status.time);
								setTimeout(function(){
									showWlHintContainer();
								}, showLoading_status.time*1000);
								check_isAlive_and_redirect({"page": "SDN.asp", "time": showLoading_status.time});
							}
						}
						httpApi.nvramSet(nvramSet_obj, function(){
							if(isWLclient()) return;
							showLoading(showLoading_status.time);
							setTimeout(function(){
								init_sdn_all_list();
								show_sdn_profilelist();
								if(!window.matchMedia('(max-width: 575px)').matches)
									$("#profile_list_content").find("[sdn_idx=" + selected_sdn_idx + "] .item_text_container").click();
							}, showLoading_status.time*1000);
							if(!isMobile()){
								if(showLoading_status.disconnect){
									check_isAlive_and_redirect({"page": "SDN.asp", "time": showLoading_status.time});
								}
							}
						});
					}
				});
			}
		};

		var $container = $("<div>").addClass("setting_content_container");

		var profile_title = ((isSupport("BUSINESS")) ? "<#GuestNetwork_Sche_network#>" : "<#GuestNetwork_Kid#>");
		if(view_mode == "popup"){
			Get_Component_Popup_Profile_Title(profile_title).appendTo($container)
				.find("#title_close_btn").unbind("click").click(function(e){
					e = e || event;
					e.stopPropagation();
					close_popup_container($container);
				});
		}
		else
			Get_Component_Profile_Title(profile_title).appendTo($container);/* untranslated */

		var $content_container = $("<div>").addClass("popup_content_container profile_setting").appendTo($container);

		var sdn_name_parm = {"title":"<#QIS_finish_wireless_item1#>", "type":"text", "id":"sdn_name", "need_check":true, "maxlength":32, "openHint":"0_1"};
		Get_Component_Input(sdn_name_parm).appendTo($content_container)
			.find("#" + sdn_name_parm.id + "")
			.unbind("keypress").keypress(function(){
				return validator.isString(this, event);
			});

		var sdn_pwd_parm = {"title":"<#QIS_finish_wireless_item2#>", "type":"password", "id":"sdn_pwd", "need_check":true, "maxlength":32, "openHint":"0_7"};
		Get_Component_Input(sdn_pwd_parm).attr({"data-group":"wifi_settings"}).appendTo($content_container)
			.find("#" + sdn_pwd_parm.id + "")
			.unbind("keyup").keyup(function(){
				chkPass($(this).val(), "rwd_vpn_pwd", $("#sdn_pwd_strength"));
				resize_iframe_height();
			})
			.unbind("blur").blur(function(){
				if($(this).val() == "")
					$("#" + sdn_pwd_parm.id + "_strength").hide();
			});
		$("<div>").attr({"id":"sdn_pwd_strength"}).attr({"data-group":"wifi_settings"}).append(Get_Component_PWD_Strength_Meter()).appendTo($content_container).hide();

		var wizard_schedule = new schedule({
			data:schedule_handle_data.string_to_json_array("M13E17002100<M14116002200"),
			show_timeset_viewport_callback: resize_iframe_height,
			icon_trash_callback: resize_iframe_height,
			btn_save_callback: resize_iframe_height,
			btn_cancel_callback: resize_iframe_height
		});
		var wizard_schedule_parm = {"title":str_WiFi_sche, "type":"switch", "id":"wizard_schedule", "set_value":"on"};
		Get_Component_Switch(wizard_schedule_parm).attr({"data-group":"wifi_settings"}).appendTo($content_container).find("#" + wizard_schedule_parm.id + "").click(function(e){
			e = e || event;
			e.stopPropagation();
			var $control_container = $(this).closest(".profile_setting").find("#container_wizard_schedule");
			if($(this).hasClass("on")){
				$control_container.empty().append(wizard_schedule.Get_UI());
				$control_container.show();
			}
			else{
				$control_container.hide();
			}
			resize_iframe_height();
		});
		$("<div>").addClass("profile_setting_item schedule_ui").attr({"id":"container_wizard_schedule", "data-group":"wifi_settings"}).append(wizard_schedule.Get_UI()).appendTo($content_container);

		if(isSwMode("rt")){
			if(get_cp_type_support("4")){
				var portal_options = [{"text":"<#wl_securitylevel_0#>", "value":"0"}];
				cp_type_rl_json.forEach(function(item){
					if(item.cp_idx == "2")
						return;
					portal_options.push({"text":item.cp_text, "value":item.cp_idx});
				});
				var portal_options_parm = {"title": "<#GuestNetwork_Portal_type#>", "id": "portal_type", "options": portal_options};
				Get_Component_Custom_Select(portal_options_parm).appendTo($content_container)
					.find("#select_" + portal_options_parm.id + "").children("div").click(function(e){
						if($(this).attr("data-disabled") == "true")
							return false;
						var options = $(this).attr("value");
						$(this).closest(".profile_setting").find("[data-portal-type]").hide(0, function(){
							$(this).find(".profile_setting_item").hide();
						}).filter("[data-portal-type="+options+"]").show(0, function(){
							$(this).find(".profile_setting_item").show();
						});
						resize_iframe_height();
						_set_apply_btn_status($(this).closest(".profile_setting"));
					});

				var $message_board_cntr = $("<div>").attr({"data-portal-type":"4"}).appendTo($content_container);
				var MB_desc_parm = {"title":"<#Description#>", "type":"text", "id":"MB_desc", "need_check":true, "maxlength":80, "set_value":"<#Description#>"};
				Get_Component_Input(MB_desc_parm).appendTo($message_board_cntr)
						.find("#" + MB_desc_parm.id + "")
						.unbind("keypress").keypress(function(){
							return validator.isString(this, event);
						})
						.unbind("keyup").keyup(function(){
							$(this).closest(".profile_setting").find("#MB_Preview_Cntr [data-component=MB_desc]").html(htmlEnDeCode.htmlEncode($(this).val()));
						})
						.attr({"oninput": "trigger_keyup($(this))"});

				var $MB_ui_cntr = $("<div>").attr({"id":"MB_ui_container"}).addClass("profile_setting_item MB_ui").appendTo($message_board_cntr);
				Get_MB_Preview_Container().appendTo($MB_ui_cntr);
				Get_MB_Change_BG_Container().hide().appendTo($MB_ui_cntr);
			}
		}

		var more_config_parm = {"title":"<#MoreConfig#>", "id":"more_config", "slide_target":"more_config_cntr"};
		Get_Component_Slide_Title(more_config_parm).appendTo($content_container);
		var $more_config_cntr = $("<div>").attr({"data-slide_target": "more_config_cntr"}).hide().appendTo($content_container);

		var wifi_band_options = [{"text":"2.4GHz / 5GHz","value":"3"}, {"text":"2.4GHz","value":"1"}, {"text":"5GHz","value":"2"}, {"text":"<#wl_securitylevel_0#>","value":"0"}];
		var wifi_band_options_parm = {"title": "WiFi <#Interface#>", "id": "wifi_band", "options": wifi_band_options, "set_value": "3"};
		var $sel_wifi_band = Get_Component_Custom_Select(wifi_band_options_parm).appendTo($more_config_cntr).find("#select_" + wifi_band_options_parm.id + "");
		$sel_wifi_band.children("div").click(function(e){
			var options = $(this).attr("value");
			var $profile_setting = $(this).closest(".profile_setting");
			$profile_setting.find("[data-container=AiMesh_List]").next(".validate_hint").remove();
			var $wifi_settings_objs = $profile_setting.find("[data-group=wifi_settings]");
			if(options == "0"){
				$wifi_settings_objs.hide().next(".validate_hint").remove();
				_set_apply_btn_status($profile_setting);
			}
			else{
				$wifi_settings_objs.show().filter("#wizard_aimesh").addClass("arrow_up");
				Set_AiMesh_List_CB($profile_setting.find("[data-container=AiMesh_List]"), options);
				$wifi_settings_objs.find("#sdn_pwd").blur();
				if($wifi_settings_objs.find("#bw_enabled").length > 0){
					if($wifi_settings_objs.find("#bw_enabled").hasClass("on")){
						$wifi_settings_objs.filter("#container_bw").show();
					}
					else{
						$wifi_settings_objs.filter("#container_bw").hide();
					}
				}
				if($wifi_settings_objs.find("#wizard_schedule").length > 0){
					if($wifi_settings_objs.find("#wizard_schedule").hasClass("on"))
						$wifi_settings_objs.filter("#container_wizard_schedule").show();
					else
						$wifi_settings_objs.filter("#container_wizard_schedule").hide();
				}
				_set_apply_btn_status($profile_setting);
			}
			resize_iframe_height();
		});

		var aimesh_parm = {"title":"<#aimesh_mode#>", "id":"wizard_aimesh", "slide_target":"wizard_aimesh_cntr"};
		var $wizard_slide_aimesh = Get_Component_Slide_Title(aimesh_parm).attr({"data-container":"wizard_aimesh_cntr", "data-group":"wifi_settings"}).addClass("arrow_up").appendTo($more_config_cntr);
		var $wizard_aimesh_cntr = $("<div>").attr({"data-slide_target":"wizard_aimesh_cntr", "data-container":"wizard_aimesh_cntr", "data-group":"wifi_settings"}).show().appendTo($more_config_cntr);
		var $AiMesh_List = $("<div>").attr({"data-container":"AiMesh_List"}).append(Get_AiMesh_List_Container()).appendTo($wizard_aimesh_cntr);
		$AiMesh_List.find(".node_content_container").click(function(e){
			e = e || event;
			e.stopPropagation();
			_set_apply_btn_status($(this).closest(".profile_setting"));
		});

		if(isSwMode("rt")){
			var bw_enabled_parm = {"title":"<#Bandwidth_Limiter#>", "type":"switch", "id":"bw_enabled", "set_value":"off"};
			Get_Component_Switch(bw_enabled_parm).attr({"data-group":"wifi_settings"}).appendTo($more_config_cntr)
				.find("#" + bw_enabled_parm.id + "").click(function(e){
					e = e || event;
					e.stopPropagation();
					var $control_container = $(this).closest(".profile_setting").find("#container_bw_dl, #container_bw_ul, #container_bw");
					if($(this).hasClass("on")){
						$control_container.show();
					}
					else{
						$control_container.hide().next(".validate_hint").remove();
					}
					resize_iframe_height();
					_set_apply_btn_status($(this).closest(".profile_setting"));
				});

			var bw_parm = {"title":"<#Bandwidth_Setting#> (Mb/s)", "container_id":"container_bw",
				"type_1":"text", "id_1":"bw_dl", "maxlength_1":12, "need_check_1":true,
				"type_2":"text", "id_2":"bw_ul", "maxlength_2":12, "need_check_2":true};
			Get_Component_Bandwidth_Setting(bw_parm).attr({"data-group":"wifi_settings"}).hide().appendTo($more_config_cntr);

			var access_intranet_parm = {"title":"<#Access_Intranet#>", "type":"switch", "id":"access_intranet", "openHint":"0_26", "set_value":"off"};
			Get_Component_Switch(access_intranet_parm).appendTo($more_config_cntr);
		}

		var $action_container = $("<div>").addClass("action_container").appendTo($content_container);
		$("<div>").addClass("btn_container apply").html("<#CTL_apply#>").appendTo($action_container);

		$content_container.find("[need_check=true]").keyup(function(){
			_set_apply_btn_status($content_container);
		});
		_set_apply_btn_status($content_container);

		return $container;
	}
	function Get_Wizard_VPN(view_mode){
		var _set_apply_btn_status = function(_obj){
			var $btn_container_apply = $(_obj).find(".action_container .btn_container.apply");
			var isBlank = validate_isBlank($(_obj));
			if(isBlank){
				$btn_container_apply.removeClass("valid_fail").addClass("valid_fail").unbind("click");
			}
			else{
				$btn_container_apply.removeClass("valid_fail").unbind("click").click(function(e){
					e = e || event;
					e.stopPropagation();
					var wizard_type = $(this).closest(".setting_content_container").attr("data-wizard-type");
					if(validate_format_Wizard_Item($(_obj), wizard_type)){
						var sdn_obj = get_new_sdn_profile();
						var sdn_idx = Object.keys(sdn_obj);
						var sdn_profile = sdn_obj[sdn_idx];
						selected_sdn_idx = sdn_profile.sdn_rl.idx;
						var nvramSet_obj = {"action_mode": "apply", "rc_service": "restart_wireless;restart_sdn " + selected_sdn_idx + ";"};
						var rc_append = "";
						var wifi_band = parseInt($(_obj).find("#select_wifi_band").children(".selected").attr("value"));
						sdn_profile.sdn_rl.sdn_name = wizard_type;
						sdn_profile.sdn_rl.vpnc_idx = "0";
						sdn_profile.sdn_rl.vpns_idx = "0";
						if($(_obj).find("#vpn_enabled").hasClass("on")){
							var $selected_vpn = $(_obj).find("[data-container=VPN_Profiles] .icon_radio.clicked");
							if($selected_vpn.length){
								var vpn_type = $selected_vpn.attr("data-vpn-type");
								var vpn_idx = $selected_vpn.attr("data-idx");
								if(vpn_type == "vpnc"){
									sdn_profile.sdn_rl.vpnc_idx = vpn_idx;
								}
								else if(vpn_type == "vpns"){
									sdn_profile.sdn_rl.vpns_idx = vpn_idx;
								}
							}
						}
						sdn_profile.apg_rl.ssid = $(_obj).find("#sdn_name").val();
						if(wifi_band > 0){
							var $sdn_pwd = $(_obj).find("#sdn_pwd");
							var radius_idx = sdn_profile.apg_rl.apg_idx;
							sdn_profile.apg_rl.security = "<3>psk2>aes>" + $sdn_pwd.val() + ">" + radius_idx + "";
							sdn_profile.apg_rl.security += "<13>psk2>aes>" + $sdn_pwd.val() + ">" + radius_idx + "";
							sdn_profile.apg_rl.security += "<16>sae>aes>" + $sdn_pwd.val() + ">" + radius_idx + "";
							sdn_profile.apg_rl.sched = schedule_handle_data.json_array_to_string(wizard_schedule.Get_Value());
							if($(_obj).find("#wizard_schedule").hasClass("on")){
								sdn_profile.apg_rl.timesched = ((sdn_profile.apg_rl.sched == "") ? "0" : "1");
							}
							else{
								sdn_profile.apg_rl.timesched = "0";
							}
							if($(_obj).find("#bw_enabled").hasClass("on")){
								sdn_profile.apg_rl.bw_limit = "<1>" + ($(_obj).find("#bw_ul").val())*1024 + ">" + ($(_obj).find("#bw_dl").val())*1024;
								nvramSet_obj.qos_enable = "1";
								nvramSet_obj.qos_type = "2";
								rc_append += "restart_qos;restart_firewall;";
							}
							else
								sdn_profile.apg_rl.bw_limit = "<0>>";
						}
						var dut_list = "";
						if(wifi_band > 0){
							$(_obj).find("[data-container=AiMesh_List]").find(".node_container").each(function(){
								if($(this).find(".icon_checkbox").hasClass("clicked")){
									var node_mac = $(this).attr("data-node-mac");
									var specific_node = cfg_clientlist.filter(function(item, index, array){
										return (item.mac == node_mac);
									})[0];
									if(specific_node != undefined){
										dut_list += get_specific_dut_list(wifi_band, specific_node);
									}
								}
							});
							dut_list += get_unChecked_dut_list(_obj);
						}
						else{
							dut_list = get_dut_list(wifi_band);
						}
						sdn_profile.apg_rl.dut_list = dut_list;

						sdn_profile.sdn_access_rl = [];
						if($(_obj).find("#access_intranet").hasClass("on")){
							sdn_profile.sdn_access_rl.push({"access_sdn_idx": "0", "sdn_idx": sdn_profile.sdn_rl.idx});
						}
						sdn_profile.apg_rl.ap_isolate = "0";

						var sdn_all_rl_tmp = JSON.parse(JSON.stringify(sdn_all_rl_json));
						sdn_all_rl_tmp.push(JSON.parse(JSON.stringify(sdn_profile)));
						var vlan_rl_tmp = JSON.parse(JSON.stringify(vlan_rl_json));
						vlan_rl_tmp.push(JSON.parse(JSON.stringify(sdn_profile.vlan_rl)));
						var sdn_all_list = parse_JSONToStr_sdn_all_list({"sdn_all_rl":sdn_all_rl_tmp, "vlan_rl":vlan_rl_tmp});
						if(rc_append != "")
							nvramSet_obj.rc_service = nvramSet_obj.rc_service + rc_append;
						$.extend(nvramSet_obj, sdn_all_list);
						var apgX_rl = parse_apg_rl_to_apgX_rl(sdn_profile.apg_rl);
						$.extend(nvramSet_obj, apgX_rl);
						var showLoading_status = get_showLoading_status(nvramSet_obj.rc_service);
						if(!httpApi.app_dataHandler){
							showLoading();
							close_popup_container("all");
							if(isWLclient()){
								showLoading(showLoading_status.time);
								setTimeout(function(){
									showWlHintContainer();
								}, showLoading_status.time*1000);
								check_isAlive_and_redirect({"page": "SDN.asp", "time": showLoading_status.time});
							}
						}
						httpApi.nvramSet(nvramSet_obj, function(){
							if(isWLclient()) return;
							showLoading(showLoading_status.time);
							setTimeout(function(){
								init_sdn_all_list();
								show_sdn_profilelist();
								if(!window.matchMedia('(max-width: 575px)').matches)
									$("#profile_list_content").find("[sdn_idx=" + selected_sdn_idx + "] .item_text_container").click();
							}, showLoading_status.time*1000);
							if(!isMobile()){
								if(showLoading_status.disconnect){
									check_isAlive_and_redirect({"page": "SDN.asp", "time": showLoading_status.time});
								}
							}
						});
					}
				});
			}
		};

		var $container = $("<div>").addClass("setting_content_container");

		if(view_mode == "popup"){
			Get_Component_Popup_Profile_Title("VPN <#Network#>").appendTo($container)
				.find("#title_close_btn").unbind("click").click(function(e){
					e = e || event;
					e.stopPropagation();
					close_popup_container($container);
				});
		}
		else
			Get_Component_Profile_Title("VPN <#Network#>").appendTo($container);

		var $content_container = $("<div>").addClass("popup_content_container profile_setting").appendTo($container);

		var sdn_name_parm = {"title":"<#QIS_finish_wireless_item1#>", "type":"text", "id":"sdn_name", "need_check":true, "maxlength":32, "openHint":"0_1"};
		Get_Component_Input(sdn_name_parm).appendTo($content_container)
			.find("#" + sdn_name_parm.id + "")
			.unbind("keypress").keypress(function(){
				return validator.isString(this, event);
			});

		var sdn_pwd_parm = {"title":"<#QIS_finish_wireless_item2#>", "type":"password", "id":"sdn_pwd", "need_check":true, "maxlength":32, "openHint":"0_7"};
		Get_Component_Input(sdn_pwd_parm).attr({"data-group":"wifi_settings"}).appendTo($content_container)
			.find("#" + sdn_pwd_parm.id + "")
			.unbind("keyup").keyup(function(){
				chkPass($(this).val(), "rwd_vpn_pwd", $("#sdn_pwd_strength"));
				resize_iframe_height();
			})
			.unbind("blur").blur(function(){
				if($(this).val() == "")
					$("#" + sdn_pwd_parm.id + "_strength").hide();
			});
		$("<div>").attr({"id":"sdn_pwd_strength", "data-group":"wifi_settings"}).append(Get_Component_PWD_Strength_Meter()).appendTo($content_container).hide();

		var vpn_enabled_parm = {"title":"VPN", "type":"switch", "id":"vpn_enabled"};
		Get_Component_Switch(vpn_enabled_parm).appendTo($content_container)
			.find("#" + vpn_enabled_parm.id + "").click(function(e){
				e = e || event;
				e.stopPropagation();
				var $control_container = $(this).closest(".profile_setting").find("[data-container=VPN_Profiles]");
				if($(this).hasClass("on")){
					$control_container.show();
				}
				else{
					$control_container.hide();
				}
				resize_iframe_height();
			});
		Get_Component_VPN_Profiles().appendTo($content_container);

		$.each(vpnc_profile_list, function(index, item){
			if(item.activate == "0"){
				$content_container.find("[data-container=VPN_Profiles] [data-vpn-type=vpnc][data-idx="+item.vpnc_idx+"]")
					.siblings("[data-component=icon_warning]").show();
			}
		});

		$.each(vpns_rl_json, function(index, item){
			if(item.activate == "0"){
				$content_container.find("[data-container=VPN_Profiles] [data-vpn-type=vpns][data-idx="+item.vpns_idx+"]")
					.siblings("[data-component=icon_warning]").show();
			}
			if(item.sdn_idx != ""){
				$content_container.find("[data-container=VPN_Profiles] [data-vpn-type=vpns][data-idx="+item.vpns_idx+"]").addClass("disabled").attr({"data-disabled":"true"})
					.siblings("[data-component=icon_error]").show();
			}
		});

		var more_config_parm = {"title":"<#MoreConfig#>", "id":"more_config", "slide_target":"more_config_cntr"};
		Get_Component_Slide_Title(more_config_parm).appendTo($content_container);
		var $more_config_cntr = $("<div>").attr({"data-slide_target": "more_config_cntr"}).hide().appendTo($content_container);

		var wifi_band_options = [{"text":"2.4GHz / 5GHz","value":"3"}, {"text":"2.4GHz","value":"1"}, {"text":"5GHz","value":"2"}, {"text":"<#wl_securitylevel_0#>","value":"0"}];
		var wifi_band_options_parm = {"title": "WiFi <#Interface#>", "id": "wifi_band", "options": wifi_band_options, "set_value": "3"};
		var $sel_wifi_band = Get_Component_Custom_Select(wifi_band_options_parm).appendTo($more_config_cntr).find("#select_" + wifi_band_options_parm.id + "");
		$sel_wifi_band.children("div").click(function(e){
			var options = $(this).attr("value");
			var $profile_setting = $(this).closest(".profile_setting");
			$profile_setting.find("[data-container=AiMesh_List]").next(".validate_hint").remove();
			var $wifi_settings_objs = $profile_setting.find("[data-group=wifi_settings]");
			if(options == "0"){
				$wifi_settings_objs.hide().next(".validate_hint").remove();
				_set_apply_btn_status($profile_setting);
			}
			else{
				$wifi_settings_objs.show().filter("#wizard_aimesh").addClass("arrow_up");
				Set_AiMesh_List_CB($profile_setting.find("[data-container=AiMesh_List]"), options);
				$wifi_settings_objs.find("#sdn_pwd").blur();
				if($wifi_settings_objs.find("#bw_enabled").length > 0){
					if($wifi_settings_objs.find("#bw_enabled").hasClass("on")){
						$wifi_settings_objs.filter("#container_bw").show();
					}
					else{
						$wifi_settings_objs.filter("#container_bw").hide();
					}
				}
				if($wifi_settings_objs.find("#wizard_schedule").length > 0){
					if($wifi_settings_objs.find("#wizard_schedule").hasClass("on"))
						$wifi_settings_objs.filter("#container_wizard_schedule").show();
					else
						$wifi_settings_objs.filter("#container_wizard_schedule").hide();
				}
				_set_apply_btn_status($profile_setting);
			}
			resize_iframe_height();
		});

		var aimesh_parm = {"title":"<#aimesh_mode#>", "id":"wizard_aimesh", "slide_target":"wizard_aimesh_cntr"};
		var $wizard_slide_aimesh = Get_Component_Slide_Title(aimesh_parm).attr({"data-container":"wizard_aimesh_cntr", "data-group":"wifi_settings"}).addClass("arrow_up").appendTo($more_config_cntr);
		var $wizard_aimesh_cntr = $("<div>").attr({"data-slide_target":"wizard_aimesh_cntr", "data-container":"wizard_aimesh_cntr", "data-group":"wifi_settings"}).show().appendTo($more_config_cntr);
		var $AiMesh_List = $("<div>").attr({"data-container":"AiMesh_List"}).append(Get_AiMesh_List_Container()).appendTo($wizard_aimesh_cntr);
		$AiMesh_List.find(".node_content_container").click(function(e){
			e = e || event;
			e.stopPropagation();
			_set_apply_btn_status($(this).closest(".profile_setting"));
		});

		var wizard_schedule = new schedule({
			show_timeset_viewport_callback: resize_iframe_height,
			icon_trash_callback: resize_iframe_height,
			btn_save_callback: resize_iframe_height,
			btn_cancel_callback: resize_iframe_height
		});
		var wizard_schedule_parm = {"title":str_WiFi_sche, "type":"switch", "id":"wizard_schedule", "set_value":"off"};
		Get_Component_Switch(wizard_schedule_parm).attr({"data-group":"wifi_settings"}).appendTo($more_config_cntr).find("#" + wizard_schedule_parm.id + "").click(function(e){
			e = e || event;
			e.stopPropagation();
			var $control_container = $(this).closest(".profile_setting").find("#container_wizard_schedule");
			if($(this).hasClass("on")){
				$control_container.empty().append(wizard_schedule.Get_UI());
				$control_container.show();
			}
			else{
				$control_container.hide();
			}
			resize_iframe_height();
		});
		$("<div>").addClass("profile_setting_item schedule_ui").attr({"id":"container_wizard_schedule", "data-group":"wifi_settings"}).append(wizard_schedule.Get_UI()).hide().appendTo($more_config_cntr);

		var bw_enabled_parm = {"title":"<#Bandwidth_Limiter#>", "type":"switch", "id":"bw_enabled", "set_value":"off"};
		Get_Component_Switch(bw_enabled_parm).attr({"data-group":"wifi_settings"}).appendTo($more_config_cntr)
			.find("#" + bw_enabled_parm.id + "").click(function(e){
				e = e || event;
				e.stopPropagation();
				var $control_container = $(this).closest(".profile_setting").find("#container_bw_dl, #container_bw_ul, #container_bw");
				if($(this).hasClass("on")){
					$control_container.show();
				}
				else{
					$control_container.hide().next(".validate_hint").remove();
				}
				resize_iframe_height();
				_set_apply_btn_status($(this).closest(".profile_setting"));
			});

		var bw_parm = {"title":"<#Bandwidth_Setting#> (Mb/s)", "container_id":"container_bw",
			"type_1":"text", "id_1":"bw_dl", "maxlength_1":12, "need_check_1":true,
			"type_2":"text", "id_2":"bw_ul", "maxlength_2":12, "need_check_2":true};
		Get_Component_Bandwidth_Setting(bw_parm).attr({"data-group":"wifi_settings"}).hide().appendTo($more_config_cntr);

		var access_intranet_parm = {"title":"<#Access_Intranet#>", "type":"switch", "id":"access_intranet", "openHint":"0_26"};
		Get_Component_Switch(access_intranet_parm).appendTo($more_config_cntr);

		var $action_container = $("<div>").addClass("action_container").appendTo($content_container);
		$("<div>").addClass("btn_container apply").html("<#CTL_apply#>").appendTo($action_container);

		$content_container.find("[need_check=true]").keyup(function(){
			_set_apply_btn_status($content_container);
		});
		_set_apply_btn_status($content_container);

		return $container;
	}
	function Get_Wizard_IoT(view_mode){
		var _set_apply_btn_status = function(_obj){
			var $btn_container_apply = $(_obj).find(".action_container .btn_container.apply");
			var isBlank = validate_isBlank($(_obj));
			if(isBlank){
				$btn_container_apply.removeClass("valid_fail").addClass("valid_fail").unbind("click");
			}
			else{
				$btn_container_apply.removeClass("valid_fail").unbind("click").click(function(e){
					e = e || event;
					e.stopPropagation();
					var wizard_type = $(this).closest(".setting_content_container").attr("data-wizard-type");
					if(validate_format_Wizard_Item($(_obj), wizard_type)){
						var sdn_obj = get_new_sdn_profile();
						var sdn_idx = Object.keys(sdn_obj);
						var sdn_profile = sdn_obj[sdn_idx];
						selected_sdn_idx = sdn_profile.sdn_rl.idx;
						var nvramSet_obj = {"action_mode": "apply", "rc_service": "restart_wireless;restart_sdn " + selected_sdn_idx + ";"};
						var rc_append = "";
						var wifi_band = (!is_cfg_ready) ? 1 : (parseInt($(_obj).find("#select_wifi_band").children(".selected").attr("value")));
						sdn_profile.sdn_rl.sdn_name = wizard_type;
						sdn_profile.apg_rl.ssid = $(_obj).find("#sdn_name").val();
						if(wifi_band > 0){
							var $sdn_pwd = $(_obj).find("#sdn_pwd");
							var radius_idx = sdn_profile.apg_rl.apg_idx;
							sdn_profile.apg_rl.security = "<3>psk2>aes>" + $sdn_pwd.val() + ">" + radius_idx + "";
							sdn_profile.apg_rl.security += "<13>psk2>aes>" + $sdn_pwd.val() + ">" + radius_idx + "";
							sdn_profile.apg_rl.security += "<16>sae>aes>" + $sdn_pwd.val() + ">" + radius_idx + "";
							sdn_profile.apg_rl.sched = schedule_handle_data.json_array_to_string(wizard_schedule.Get_Value());
							if($(_obj).find("#wizard_schedule").hasClass("on")){
								sdn_profile.apg_rl.timesched = ((sdn_profile.apg_rl.sched == "") ? "0" : "1");
							}
							else{
								sdn_profile.apg_rl.timesched = "0";
							}
						}
						sdn_profile.apg_rl.bw_limit = "<0>>";
						var dut_list = "";
						if(is_cfg_ready){
							if(wifi_band > 0){
								$(_obj).find("[data-container=AiMesh_List]").find(".node_container").each(function(){
									if($(this).find(".icon_checkbox").hasClass("clicked")){
										var node_mac = $(this).attr("data-node-mac");
										var specific_node = cfg_clientlist.filter(function(item, index, array){
											return (item.mac == node_mac);
										})[0];
										if(specific_node != undefined){
											dut_list += get_specific_dut_list(wifi_band, specific_node);
										}
									}
								});
								dut_list += get_unChecked_dut_list(_obj);
							}
							else{
								dut_list = get_dut_list(wifi_band);
							}
						}
						else{
							dut_list = get_dut_list_by_mac("1", cap_mac);
						}
						sdn_profile.apg_rl.dut_list = dut_list;

						sdn_profile.sdn_access_rl = [];
						sdn_profile.apg_rl.ap_isolate = "0";

						sdn_profile.subnet_rl = {};
						vlan_rl_json = vlan_rl_json.filter(function(item, index, array){
							return (item.vlan_idx != sdn_profile.vlan_rl.vlan_idx);
						});
						sdn_profile.vlan_rl = {};
						sdn_profile.sdn_rl.subnet_idx = "0";
						sdn_profile.sdn_rl.vlan_idx = "0";

						var sdn_all_rl_tmp = JSON.parse(JSON.stringify(sdn_all_rl_json));
						sdn_all_rl_tmp.push(JSON.parse(JSON.stringify(sdn_profile)));
						var vlan_rl_tmp = JSON.parse(JSON.stringify(vlan_rl_json));
						vlan_rl_tmp.push(JSON.parse(JSON.stringify(sdn_profile.vlan_rl)));
						var sdn_all_list = parse_JSONToStr_sdn_all_list({"sdn_all_rl":sdn_all_rl_tmp, "vlan_rl":vlan_rl_tmp});
						if(rc_append != "")
							nvramSet_obj.rc_service = nvramSet_obj.rc_service + rc_append;
						$.extend(nvramSet_obj, sdn_all_list);
						var apgX_rl = parse_apg_rl_to_apgX_rl(sdn_profile.apg_rl);
						$.extend(nvramSet_obj, apgX_rl);
						if(is_QIS_flow){
							nvramSet_obj.sdn_rc_service = nvramSet_obj.rc_service;
							delete nvramSet_obj["rc_service"];
							delete nvramSet_obj["action_mode"];
							$(this).replaceWith($("<img>").attr({"width": "30px", "src": "/images/InternetScan.gif"}));
							apply.SDN_Profile(nvramSet_obj);
							return;
						}
						var showLoading_status = get_showLoading_status(nvramSet_obj.rc_service);
						if(!httpApi.app_dataHandler){
							showLoading();
							close_popup_container("all");
							if(isWLclient()){
								showLoading(showLoading_status.time);
								setTimeout(function(){
									showWlHintContainer();
								}, showLoading_status.time*1000);
								check_isAlive_and_redirect({"page": "SDN.asp", "time": showLoading_status.time});
							}
						}
						httpApi.nvramSet(nvramSet_obj, function(){
							if(isWLclient()) return;
							showLoading(showLoading_status.time);
							setTimeout(function(){
								init_sdn_all_list();
								show_sdn_profilelist();
								if(!window.matchMedia('(max-width: 575px)').matches)
									$("#profile_list_content").find("[sdn_idx=" + selected_sdn_idx + "] .item_text_container").click();
							}, showLoading_status.time*1000);
							if(!isMobile()){
								if(showLoading_status.disconnect){
									check_isAlive_and_redirect({"page": "SDN.asp", "time": showLoading_status.time});
								}
							}
						});
					}
				});
			}
		};

		var $container = $("<div>").addClass("setting_content_container");

		if(view_mode == "popup"){
			Get_Component_Popup_Profile_Title("<#GuestNetwork_IoT#>").appendTo($container)
				.find("#title_close_btn").unbind("click").click(function(e){
					e = e || event;
					e.stopPropagation();
					close_popup_container($container);
				});
		}
		else
			Get_Component_Profile_Title("<#GuestNetwork_IoT#>").appendTo($container);

		var $content_container = $("<div>").addClass("popup_content_container profile_setting").appendTo($container);

		var sdn_name_parm = {"title":"<#QIS_finish_wireless_item1#>", "type":"text", "id":"sdn_name", "need_check":true, "maxlength":32, "openHint":"0_1"};
		Get_Component_Input(sdn_name_parm).appendTo($content_container)
			.find("#" + sdn_name_parm.id + "")
			.unbind("keypress").keypress(function(){
				return validator.isString(this, event);
			});

		var sdn_pwd_parm = {"title":"<#QIS_finish_wireless_item2#>", "type":"password", "id":"sdn_pwd", "need_check":true, "maxlength":32, "openHint":"0_7"};
		Get_Component_Input(sdn_pwd_parm).attr({"data-group":"wifi_settings"}).appendTo($content_container)
			.find("#" + sdn_pwd_parm.id + "")
			.unbind("keyup").keyup(function(){
				chkPass($(this).val(), "rwd_vpn_pwd", $("#sdn_pwd_strength"));
				resize_iframe_height();
			})
			.unbind("blur").blur(function(){
				if($(this).val() == "")
					$("#" + sdn_pwd_parm.id + "_strength").hide();
			});
		$("<div>").attr({"id":"sdn_pwd_strength", "data-group":"wifi_settings"}).append(Get_Component_PWD_Strength_Meter()).appendTo($content_container).hide();

		var more_config_parm = {"title":"<#MoreConfig#>", "id":"more_config", "slide_target":"more_config_cntr"};
		Get_Component_Slide_Title(more_config_parm).appendTo($content_container);
		var $more_config_cntr = $("<div>").attr({"data-slide_target": "more_config_cntr"}).hide().appendTo($content_container);

		var wifi_band_options = [{"text":"2.4GHz / 5GHz","value":"3"}, {"text":"2.4GHz","value":"1"}, {"text":"5GHz","value":"2"}, {"text":"<#wl_securitylevel_0#>","value":"0"}];
		var wifi_band_options_parm = {"title": "WiFi <#Interface#>", "id": "wifi_band", "options": wifi_band_options, "set_value": "1"};
		var $sel_wifi_band = Get_Component_Custom_Select(wifi_band_options_parm).appendTo($more_config_cntr).find("#select_" + wifi_band_options_parm.id + "");
		$sel_wifi_band.children("div").click(function(e){
			var options = $(this).attr("value");
			var $profile_setting = $(this).closest(".profile_setting");
			$profile_setting.find("[data-container=AiMesh_List]").next(".validate_hint").remove();
			var $wifi_settings_objs = $profile_setting.find("[data-group=wifi_settings]");
			if(options == "0"){
				$wifi_settings_objs.hide().next(".validate_hint").remove();
				_set_apply_btn_status($profile_setting);
			}
			else{
				$wifi_settings_objs.show().filter("#wizard_aimesh").addClass("arrow_up");
				Set_AiMesh_List_CB($profile_setting.find("[data-container=AiMesh_List]"), options);
				$wifi_settings_objs.find("#sdn_pwd").blur();
				if($wifi_settings_objs.find("#wizard_schedule").length > 0){
					if($wifi_settings_objs.find("#wizard_schedule").hasClass("on"))
						$wifi_settings_objs.filter("#container_wizard_schedule").show();
					else
						$wifi_settings_objs.filter("#container_wizard_schedule").hide();
				}
				_set_apply_btn_status($profile_setting);
			}
			resize_iframe_height();
		});

		var aimesh_parm = {"title":"<#aimesh_mode#>", "id":"wizard_aimesh", "slide_target":"wizard_aimesh_cntr"};
		var $wizard_slide_aimesh = Get_Component_Slide_Title(aimesh_parm).attr({"data-container":"wizard_aimesh_cntr", "data-group":"wifi_settings"}).addClass("arrow_up").appendTo($more_config_cntr);
		var $wizard_aimesh_cntr = $("<div>").attr({"data-slide_target":"wizard_aimesh_cntr", "data-container":"wizard_aimesh_cntr", "data-group":"wifi_settings"}).show().appendTo($more_config_cntr);
		var $AiMesh_List = $("<div>").attr({"data-container":"AiMesh_List"}).append(Get_AiMesh_List_Container()).appendTo($wizard_aimesh_cntr);
		$AiMesh_List.find(".node_content_container").click(function(e){
			e = e || event;
			e.stopPropagation();
			_set_apply_btn_status($(this).closest(".profile_setting"));
		});

		var wizard_schedule = new schedule({
			show_timeset_viewport_callback: resize_iframe_height,
			icon_trash_callback: resize_iframe_height,
			btn_save_callback: resize_iframe_height,
			btn_cancel_callback: resize_iframe_height
		});
		var wizard_schedule_parm = {"title":str_WiFi_sche, "type":"switch", "id":"wizard_schedule", "set_value":"off"};
		Get_Component_Switch(wizard_schedule_parm).attr({"data-group":"wifi_settings"}).appendTo($more_config_cntr).find("#" + wizard_schedule_parm.id + "").click(function(e){
			e = e || event;
			e.stopPropagation();
			var $control_container = $(this).closest(".profile_setting").find("#container_wizard_schedule");
			if($(this).hasClass("on")){
				$control_container.empty().append(wizard_schedule.Get_UI());
				$control_container.show();
			}
			else{
				$control_container.hide();
			}
			resize_iframe_height();
		});
		$("<div>").addClass("profile_setting_item schedule_ui").attr({"id":"container_wizard_schedule", "data-group":"wifi_settings"}).append(wizard_schedule.Get_UI()).hide().appendTo($more_config_cntr);

		var $action_container = $("<div>").addClass("action_container").appendTo($content_container);
		$("<div>").addClass("btn_container apply").html("<#CTL_apply#>").appendTo($action_container);

		$content_container.find("[need_check=true]").keyup(function(){
			_set_apply_btn_status($content_container);
		});
		_set_apply_btn_status($content_container);

		return $container;
	}
	function Get_Wizard_Guest(view_mode){
		var _set_apply_btn_status = function(_obj){
			var $btn_container_apply = $(_obj).find(".action_container .btn_container.apply");
			var isBlank = validate_isBlank($(_obj));
			if(isBlank){
				$btn_container_apply.removeClass("valid_fail").addClass("valid_fail").unbind("click");
			}
			else{
				$btn_container_apply.removeClass("valid_fail").unbind("click").click(function(e){
					e = e || event;
					e.stopPropagation();
					var wizard_type = $(this).closest(".setting_content_container").attr("data-wizard-type");
					if(validate_format_Wizard_Item($(_obj), wizard_type)){
						var sdn_obj = get_new_sdn_profile();
						var sdn_idx = Object.keys(sdn_obj);
						var sdn_profile = sdn_obj[sdn_idx];
						selected_sdn_idx = sdn_profile.sdn_rl.idx;
						selected_vlan_idx = sdn_profile.sdn_rl.vlan_idx;
						var nvramSet_obj = {"action_mode": "apply", "rc_service": "restart_wireless;restart_sdn " + selected_sdn_idx + ";"};
						var rc_append = "";
						var wifi_band = (!is_cfg_ready) ? 3 : (parseInt($(_obj).find("#select_wifi_band").children(".selected").attr("value")));
						sdn_profile.sdn_rl.sdn_name = wizard_type;
						sdn_profile.apg_rl.ssid = $(_obj).find("#sdn_name").val();
						if(wifi_band > 0){
							var sec_option_id = $(_obj).find("#security_guest .switch_text_container").children(".selected").attr("data-option-id");
							var wifi_pwd = "";
							var wifi_auth = "psk2";
							if(sec_option_id == "open"){
								wifi_auth = "open";
								wifi_pwd = "";
							}
							else if(sec_option_id == "pwd"){
								wifi_auth = "psk2";
								wifi_pwd = $(_obj).find("#sdn_pwd").val();
							}
							var radius_idx = sdn_profile.apg_rl.apg_idx;
							sdn_profile.apg_rl.security = "<3>" + wifi_auth + ">aes>" + wifi_pwd + ">" + radius_idx + "";
							sdn_profile.apg_rl.security += "<13>" + wifi_auth + ">aes>" + wifi_pwd + ">" + radius_idx + "";
							sdn_profile.apg_rl.security += "<16>sae>aes>" + wifi_pwd + ">" + radius_idx + "";
							sdn_profile.apg_rl.sched = schedule_handle_data.json_array_to_string(wizard_schedule.Get_Value());
							if($(_obj).find("#wizard_schedule").hasClass("on")){
								sdn_profile.apg_rl.timesched = wizard_schedule.Get_Value_Mode();
								if(sdn_profile.apg_rl.timesched == "1"){
									sdn_profile.apg_rl.timesched = ((sdn_profile.apg_rl.sched == "") ? "0" : "1");
								}
							}
							else{
								sdn_profile.apg_rl.timesched = "0";
							}
							if($(_obj).find("#bw_enabled").hasClass("on")){
								sdn_profile.apg_rl.bw_limit = "<1>" + ($(_obj).find("#bw_ul").val())*1024 + ">" + ($(_obj).find("#bw_dl").val())*1024;
								nvramSet_obj.qos_enable = "1";
								nvramSet_obj.qos_type = "2";
								rc_append += "restart_qos;restart_firewall;";
							}
							else
								sdn_profile.apg_rl.bw_limit = "<0>>";
						}
						var dut_list = "";
						if(is_cfg_ready){
							if(wifi_band > 0){
								$(_obj).find("[data-container=AiMesh_List]").find(".node_container").each(function(){
									if($(this).find(".icon_checkbox").hasClass("clicked")){
										var node_mac = $(this).attr("data-node-mac");
										var specific_node = cfg_clientlist.filter(function(item, index, array){
											return (item.mac == node_mac);
										})[0];
										if(specific_node != undefined){
											dut_list += get_specific_dut_list(wifi_band, specific_node);
										}
									}
								});
								dut_list += get_unChecked_dut_list(_obj);
							}
							else{
								dut_list = get_dut_list(wifi_band);
							}
						}
						else{
							dut_list = get_dut_list_by_mac("3", cap_mac);
						}
						sdn_profile.apg_rl.dut_list = dut_list;

						sdn_profile.sdn_access_rl = [];
						if($(_obj).find("#access_intranet").hasClass("on")){
							sdn_profile.sdn_access_rl.push({"access_sdn_idx": "0", "sdn_idx": sdn_profile.sdn_rl.idx});
						}
						sdn_profile.apg_rl.ap_isolate = "1";

						if(rc_append != ""){
							nvramSet_obj.rc_service = nvramSet_obj.rc_service + rc_append;
						}
						var showLoading_status = get_showLoading_status(nvramSet_obj.rc_service);
						if(wifi_band > 0){
							if(sdn_profile.apg_rl.timesched == "2"){
								sdn_profile.apg_rl.expiretime = accesstime_handle_data.json_array_to_string(wizard_schedule.Get_Value_AccessTime(showLoading_status.time));
							}
						}
						var sdn_all_rl_tmp = JSON.parse(JSON.stringify(sdn_all_rl_json));
						sdn_all_rl_tmp.push(JSON.parse(JSON.stringify(sdn_profile)));
						var vlan_rl_tmp = JSON.parse(JSON.stringify(vlan_rl_json));
						vlan_rl_tmp.push(JSON.parse(JSON.stringify(sdn_profile.vlan_rl)));
						var sdn_all_list = parse_JSONToStr_sdn_all_list({"sdn_all_rl":sdn_all_rl_tmp, "vlan_rl":vlan_rl_tmp});
						$.extend(nvramSet_obj, sdn_all_list);
						var apgX_rl = parse_apg_rl_to_apgX_rl(sdn_profile.apg_rl);
						$.extend(nvramSet_obj, apgX_rl);
						if(is_QIS_flow){
							nvramSet_obj.sdn_rc_service = nvramSet_obj.rc_service;
							delete nvramSet_obj["rc_service"];
							delete nvramSet_obj["action_mode"];
							$(this).replaceWith($("<img>").attr({"width": "30px", "src": "/images/InternetScan.gif"}));
							apply.SDN_Profile(nvramSet_obj);
							return;
						}
						if(!httpApi.app_dataHandler){
							showLoading();
							close_popup_container("all");
							if(isWLclient()){
								showLoading(showLoading_status.time);
								setTimeout(function(){
									showWlHintContainer();
								}, showLoading_status.time*1000);
								check_isAlive_and_redirect({"page": "SDN.asp", "time": showLoading_status.time});
							}
						}
						httpApi.nvramSet(nvramSet_obj, function(){
							if(isWLclient()) return;
							showLoading(showLoading_status.time);
							setTimeout(function(){
								init_sdn_all_list();
								show_sdn_profilelist();
								if(!window.matchMedia('(max-width: 575px)').matches)
									$("#profile_list_content").find("[sdn_idx=" + selected_sdn_idx + "] .item_text_container").click();
							}, showLoading_status.time*1000);
							if(!isMobile()){
								if(showLoading_status.disconnect){
									check_isAlive_and_redirect({"page": "SDN.asp", "time": showLoading_status.time});
								}
							}
						});
					}
				});
			}
		};

		var $container = $("<div>").addClass("setting_content_container");

		if(view_mode == "popup"){
			Get_Component_Popup_Profile_Title("<#Guest_Network#>").appendTo($container)
				.find("#title_close_btn").unbind("click").click(function(e){
					e = e || event;
					e.stopPropagation();
					close_popup_container($container);
				});
		}
		else
			Get_Component_Profile_Title("<#Guest_Network#>").appendTo($container);

		var $content_container = $("<div>").addClass("popup_content_container profile_setting").appendTo($container);

		var sdn_name_parm = {"title":"<#QIS_finish_wireless_item1#>", "type":"text", "id":"sdn_name", "need_check":true, "maxlength":32, "openHint":"0_1"};
		Get_Component_Input(sdn_name_parm).appendTo($content_container)
			.find("#" + sdn_name_parm.id + "")
			.unbind("keypress").keypress(function(){
				return validator.isString(this, event);
			});

		var security_options = [{"text":"Open System","option_id":"open"}, {"text":"<#HSDPAConfig_Password_itemname#>","option_id":"pwd"}];
		var security_parm = {"title":"<#Security#>", "options": security_options, "container_id":"security_guest"};
		Get_Component_Switch_Text(security_parm).attr({"data-group":"wifi_settings"}).appendTo($content_container)
			.find(".switch_text_container > .switch_text_item")
			.click(function(e){
				e = e || event;
				e.stopPropagation();
				var option_id = $(this).attr("data-option-id");
				$(this).closest(".profile_setting").find("[data-sec-option-id]").hide().next(".validate_hint").remove();
				$(this).closest(".profile_setting").find("[data-sec-option-id=" + option_id + "]").show();
				if(option_id == "pwd"){
					$(this).closest(".profile_setting").find("#sdn_pwd_strength").hide();
				}
				resize_iframe_height();
				_set_apply_btn_status($(this).closest(".profile_setting"));
			});

		var sdn_pwd_parm = {"title":"<#QIS_finish_wireless_item2#>", "type":"password", "id":"sdn_pwd", "need_check":true, "maxlength":32, "openHint":"0_7"};
		Get_Component_Input(sdn_pwd_parm).attr({"data-sec-option-id":"pwd", "data-group":"wifi_settings"}).appendTo($content_container)
			.find("#" + sdn_pwd_parm.id + "")
			.unbind("keyup").keyup(function(){
				chkPass($(this).val(), "rwd_vpn_pwd", $("#sdn_pwd_strength"));
				resize_iframe_height();
			})
			.unbind("blur").blur(function(){
				if($(this).val() == "")
					$("#" + sdn_pwd_parm.id + "_strength").hide();
			});
		$("<div>").attr({"id":"sdn_pwd_strength", "data-sec-option-id":"pwd", "data-group":"wifi_settings"}).append(Get_Component_PWD_Strength_Meter()).appendTo($content_container).hide();

		$content_container.find("[data-sec-option-id=pwd]").hide();

		var wizard_schedule = new schedule({
			AccTime_support: true,
			mode:2,
			show_timeset_viewport_callback: resize_iframe_height,
			icon_trash_callback: resize_iframe_height,
			btn_save_callback: resize_iframe_height,
			btn_cancel_callback: resize_iframe_height
		});
		var wizard_schedule_parm = {"title":str_WiFi_sche, "type":"switch", "id":"wizard_schedule", "set_value":"on"};
		Get_Component_Switch(wizard_schedule_parm).attr({"data-group":"wifi_settings"}).appendTo($content_container).find("#" + wizard_schedule_parm.id + "").click(function(e){
			e = e || event;
			e.stopPropagation();
			var $control_container = $(this).closest(".profile_setting").find("#container_wizard_schedule");
			if($(this).hasClass("on")){
				$control_container.empty().append(wizard_schedule.Get_UI());
				$control_container.show();
			}
			else{
				$control_container.hide();
			}
			resize_iframe_height();
		});
		$("<div>").addClass("profile_setting_item schedule_ui").attr({"id":"container_wizard_schedule", "data-group":"wifi_settings"}).append(wizard_schedule.Get_UI()).appendTo($content_container);

		var more_config_parm = {"title":"<#MoreConfig#>", "id":"more_config", "slide_target":"more_config_cntr"};
		Get_Component_Slide_Title(more_config_parm).appendTo($content_container);
		var $more_config_cntr = $("<div>").attr({"data-slide_target": "more_config_cntr"}).hide().appendTo($content_container);

		var wifi_band_options = [{"text":"2.4GHz / 5GHz","value":"3"}, {"text":"2.4GHz","value":"1"}, {"text":"5GHz","value":"2"}, {"text":"<#wl_securitylevel_0#>","value":"0"}];
		var wifi_band_options_parm = {"title": "WiFi <#Interface#>", "id": "wifi_band", "options": wifi_band_options, "set_value": "3"};
		var $sel_wifi_band = Get_Component_Custom_Select(wifi_band_options_parm).appendTo($more_config_cntr).find("#select_" + wifi_band_options_parm.id + "");
		$sel_wifi_band.children("div").click(function(e){
			var options = $(this).attr("value");
			var $profile_setting = $(this).closest(".profile_setting");
			$profile_setting.find("[data-container=AiMesh_List]").next(".validate_hint").remove();
			var $wifi_settings_objs = $profile_setting.find("[data-group=wifi_settings]");
			if(options == "0"){
				$wifi_settings_objs.hide().next(".validate_hint").remove();
				_set_apply_btn_status($profile_setting);
			}
			else{
				$wifi_settings_objs.show().filter("#wizard_aimesh").addClass("arrow_up");
				Set_AiMesh_List_CB($profile_setting.find("[data-container=AiMesh_List]"), options);
				if($wifi_settings_objs.find(".switch_text_container").length > 0)
					$wifi_settings_objs.find(".switch_text_container .switch_text_item.selected").click();
				if($wifi_settings_objs.find("#bw_enabled").length > 0){
					if($wifi_settings_objs.find("#bw_enabled").hasClass("on")){
						$wifi_settings_objs.filter("#container_bw").show();
					}
					else{
						$wifi_settings_objs.filter("#container_bw").hide();
					}
				}
				if($wifi_settings_objs.find("#wizard_schedule").length > 0){
					if($wifi_settings_objs.find("#wizard_schedule").hasClass("on"))
						$wifi_settings_objs.filter("#container_wizard_schedule").show();
					else
						$wifi_settings_objs.filter("#container_wizard_schedule").hide();
				}
				_set_apply_btn_status($profile_setting);
			}
			resize_iframe_height();
		});

		var aimesh_parm = {"title":"<#aimesh_mode#>", "id":"wizard_aimesh", "slide_target":"wizard_aimesh_cntr"};
		var $wizard_slide_aimesh = Get_Component_Slide_Title(aimesh_parm).attr({"data-container":"wizard_aimesh_cntr", "data-group":"wifi_settings"}).addClass("arrow_up").appendTo($more_config_cntr);
		var $wizard_aimesh_cntr = $("<div>").attr({"data-slide_target":"wizard_aimesh_cntr", "data-container":"wizard_aimesh_cntr", "data-group":"wifi_settings"}).show().appendTo($more_config_cntr);
		var $AiMesh_List = $("<div>").attr({"data-container":"AiMesh_List"}).append(Get_AiMesh_List_Container()).appendTo($wizard_aimesh_cntr);
		$AiMesh_List.find(".node_content_container").click(function(e){
			e = e || event;
			e.stopPropagation();
			_set_apply_btn_status($(this).closest(".profile_setting"));
		});

		if(isSwMode("rt")){
			var bw_enabled_parm = {"title":"<#Bandwidth_Limiter#>", "type":"switch", "id":"bw_enabled", "set_value":"off"};
			Get_Component_Switch(bw_enabled_parm).attr({"data-group":"wifi_settings"}).appendTo($more_config_cntr)
				.find("#" + bw_enabled_parm.id + "").click(function(e){
					e = e || event;
					e.stopPropagation();
					var $control_container = $(this).closest(".profile_setting").find("#container_bw_dl, #container_bw_ul, #container_bw");
					if($(this).hasClass("on")){
						$control_container.show();
					}
					else{
						$control_container.hide().next(".validate_hint").remove();
					}
					resize_iframe_height();
					_set_apply_btn_status($(this).closest(".profile_setting"));
				});

			var bw_parm = {"title":"<#Bandwidth_Setting#> (Mb/s)", "container_id":"container_bw",
				"type_1":"text", "id_1":"bw_dl", "maxlength_1":12, "need_check_1":true,
				"type_2":"text", "id_2":"bw_ul", "maxlength_2":12, "need_check_2":true};
			Get_Component_Bandwidth_Setting(bw_parm).attr({"data-group":"wifi_settings"}).hide().appendTo($more_config_cntr);

			var access_intranet_parm = {"title":"<#Access_Intranet#>", "type":"switch", "id":"access_intranet", "openHint":"0_26", "set_value":"off"};
			Get_Component_Switch(access_intranet_parm).appendTo($more_config_cntr);
		}

		var $action_container = $("<div>").addClass("action_container").appendTo($content_container);
		$("<div>").addClass("btn_container apply").html("<#CTL_apply#>").appendTo($action_container);

		$content_container.find("[need_check=true]").keyup(function(){
			_set_apply_btn_status($content_container);
		});
		_set_apply_btn_status($content_container);

		return $container;
	}
	function Get_Wizard_Customized(view_mode){
		var _set_apply_btn_status = function(_obj){
			var $btn_container_apply = $(_obj).find(".action_container .btn_container.apply");
			var isBlank = validate_isBlank($(_obj));
			if(isBlank){
				$btn_container_apply.removeClass("valid_fail").addClass("valid_fail").unbind("click");
			}
			else{
				$btn_container_apply.removeClass("valid_fail").unbind("click").click(function(e){
					e = e || event;
					e.stopPropagation();
					var wizard_type = $(this).closest(".setting_content_container").attr("data-wizard-type");
					if(validate_format_Wizard_Item($(_obj), wizard_type)){
						var sdn_obj = get_new_sdn_profile();
						var sdn_idx = Object.keys(sdn_obj);
						var sdn_profile = sdn_obj[sdn_idx];
						selected_sdn_idx = sdn_profile.sdn_rl.idx;
						var nvramSet_obj = {"action_mode": "apply", "rc_service": "restart_wireless;restart_sdn " + selected_sdn_idx + ";"};
						var rc_append = "";
						var wifi_band = parseInt($(_obj).find("#select_wifi_band").children(".selected").attr("value"));
						sdn_profile.sdn_rl.sdn_name = wizard_type;
						sdn_profile.apg_rl.ssid = $(_obj).find("#sdn_name").val();
						var dut_list = "";
						if(wifi_band > 0){
							var $sdn_pwd = $(_obj).find("#sdn_pwd");
							var radius_idx = sdn_profile.apg_rl.apg_idx;
							sdn_profile.apg_rl.security = "<3>psk2>aes>" + $sdn_pwd.val() + ">" + radius_idx + "";
							sdn_profile.apg_rl.security += "<13>psk2>aes>" + $sdn_pwd.val() + ">" + radius_idx + "";
							sdn_profile.apg_rl.security += "<16>sae>aes>" + $sdn_pwd.val() + ">" + radius_idx + "";
							sdn_profile.apg_rl.sched = schedule_handle_data.json_array_to_string(wizard_schedule.Get_Value());
							if($(_obj).find("#wizard_schedule").hasClass("on")){
								sdn_profile.apg_rl.timesched = ((sdn_profile.apg_rl.sched == "") ? "0" : "1");
							}
							else{
								sdn_profile.apg_rl.timesched = "0";
							}
							if($(_obj).find("#bw_enabled").hasClass("on")){
								sdn_profile.apg_rl.bw_limit = "<1>" + ($(_obj).find("#bw_ul").val())*1024 + ">" + ($(_obj).find("#bw_dl").val())*1024;
								nvramSet_obj.qos_enable = "1";
								nvramSet_obj.qos_type = "2";
								rc_append += "restart_qos;restart_firewall;";
							}
							else
								sdn_profile.apg_rl.bw_limit = "<0>>";

							$(_obj).find("[data-container=AiMesh_List]").find(".node_container").each(function(){
								if($(this).find(".icon_checkbox").hasClass("clicked")){
									var node_mac = $(this).attr("data-node-mac");
									var specific_node = cfg_clientlist.filter(function(item, index, array){
										return (item.mac == node_mac);
									})[0];
									if(specific_node != undefined){
										dut_list += get_specific_dut_list(wifi_band, specific_node);
									}
								}
							});
							dut_list += get_unChecked_dut_list(_obj);
						}
						else{
							dut_list = get_dut_list(wifi_band);
						}
						sdn_profile.apg_rl.dut_list = dut_list;

						sdn_profile.sdn_access_rl = [];
						if($(_obj).find("#access_intranet").hasClass("on")){
							sdn_profile.sdn_access_rl.push({"access_sdn_idx": "0", "sdn_idx": sdn_profile.sdn_rl.idx});
						}
						sdn_profile.apg_rl.ap_isolate = "0";

						var sdn_all_rl_tmp = JSON.parse(JSON.stringify(sdn_all_rl_json));
						sdn_all_rl_tmp.push(JSON.parse(JSON.stringify(sdn_profile)));
						var vlan_rl_tmp = JSON.parse(JSON.stringify(vlan_rl_json));
						vlan_rl_tmp.push(JSON.parse(JSON.stringify(sdn_profile.vlan_rl)));
						var sdn_all_list = parse_JSONToStr_sdn_all_list({"sdn_all_rl":sdn_all_rl_tmp, "vlan_rl":vlan_rl_tmp});
						if(rc_append != "")
							nvramSet_obj.rc_service = nvramSet_obj.rc_service + rc_append;
						$.extend(nvramSet_obj, sdn_all_list);
						var apgX_rl = parse_apg_rl_to_apgX_rl(sdn_profile.apg_rl);
						$.extend(nvramSet_obj, apgX_rl);
						var showLoading_status = get_showLoading_status(nvramSet_obj.rc_service);
						if(!httpApi.app_dataHandler){
							showLoading();
							close_popup_container("all");
							if(isWLclient()){
								showLoading(showLoading_status.time);
								setTimeout(function(){
									showWlHintContainer();
								}, showLoading_status.time*1000);
								check_isAlive_and_redirect({"page": "SDN.asp", "time": showLoading_status.time});
							}
						}
						httpApi.nvramSet(nvramSet_obj, function(){
							if(isWLclient()) return;
							showLoading(showLoading_status.time);
							setTimeout(function(){
								init_sdn_all_list();
								show_sdn_profilelist();
								if(!window.matchMedia('(max-width: 575px)').matches)
									$("#profile_list_content").find("[sdn_idx=" + selected_sdn_idx + "] .item_text_container").click();
							}, showLoading_status.time*1000);
							if(!isMobile()){
								if(showLoading_status.disconnect){
									check_isAlive_and_redirect({"page": "SDN.asp", "time": showLoading_status.time});
								}
							}
						});
					}
				});
			}
		};

		var $container = $("<div>").addClass("setting_content_container");

		if(view_mode == "popup"){
			Get_Component_Popup_Profile_Title("<#GuestNetwork_Customized#>").appendTo($container)
				.find("#title_close_btn").unbind("click").click(function(e){
					e = e || event;
					e.stopPropagation();
					close_popup_container($container);
				});
		}
		else
			Get_Component_Profile_Title("<#GuestNetwork_Customized#>").appendTo($container);

		var $content_container = $("<div>").addClass("popup_content_container profile_setting").appendTo($container);

		var sdn_name_parm = {"title":"<#QIS_finish_wireless_item1#>", "type":"text", "id":"sdn_name", "need_check":true, "maxlength":32, "openHint":"0_1"};
		Get_Component_Input(sdn_name_parm).appendTo($content_container)
			.find("#" + sdn_name_parm.id + "")
			.unbind("keypress").keypress(function(){
				return validator.isString(this, event);
			});

		var sdn_pwd_parm = {"title":"<#QIS_finish_wireless_item2#>", "type":"password", "id":"sdn_pwd", "need_check":true, "maxlength":32, "openHint":"0_7"};
		Get_Component_Input(sdn_pwd_parm).attr({"data-group":"wifi_settings"}).appendTo($content_container)
			.find("#" + sdn_pwd_parm.id + "")
			.unbind("keyup").keyup(function(){
				chkPass($(this).val(), "rwd_vpn_pwd", $("#sdn_pwd_strength"));
				resize_iframe_height();
			})
			.unbind("blur").blur(function(){
				if($(this).val() == "")
					$("#" + sdn_pwd_parm.id + "_strength").hide();
			});
		$("<div>").attr({"id":"sdn_pwd_strength", "data-group":"wifi_settings"}).append(Get_Component_PWD_Strength_Meter()).appendTo($content_container).hide();

		var more_config_parm = {"title":"<#MoreConfig#>", "id":"more_config", "slide_target":"more_config_cntr"};
		Get_Component_Slide_Title(more_config_parm).appendTo($content_container);
		var $more_config_cntr = $("<div>").attr({"data-slide_target": "more_config_cntr"}).hide().appendTo($content_container);

		var wifi_band_options = [{"text":"2.4GHz / 5GHz","value":"3"}, {"text":"2.4GHz","value":"1"}, {"text":"5GHz","value":"2"}, {"text":"<#wl_securitylevel_0#>","value":"0"}];
		var wifi_band_options_parm = {"title": "WiFi <#Interface#>", "id": "wifi_band", "options": wifi_band_options, "set_value": "3"};
		var $sel_wifi_band = Get_Component_Custom_Select(wifi_band_options_parm).appendTo($more_config_cntr).find("#select_" + wifi_band_options_parm.id + "");
		$sel_wifi_band.children("div").click(function(e){
			var options = $(this).attr("value");
			var $profile_setting = $(this).closest(".profile_setting");
			$profile_setting.find("[data-container=AiMesh_List]").next(".validate_hint").remove();
			var $wifi_settings_objs = $profile_setting.find("[data-group=wifi_settings]");
			if(options == "0"){
				$wifi_settings_objs.hide().next(".validate_hint").remove();
				_set_apply_btn_status($profile_setting);
			}
			else{
				$wifi_settings_objs.show().filter("#wizard_aimesh").addClass("arrow_up");
				Set_AiMesh_List_CB($profile_setting.find("[data-container=AiMesh_List]"), options);
				$wifi_settings_objs.find("#sdn_pwd").blur();
				if($wifi_settings_objs.find("#bw_enabled").length > 0){
					if($wifi_settings_objs.find("#bw_enabled").hasClass("on")){
						$wifi_settings_objs.filter("#container_bw").show();
					}
					else{
						$wifi_settings_objs.filter("#container_bw").hide();
					}
				}
				if($wifi_settings_objs.find("#wizard_schedule").length > 0){
					if($wifi_settings_objs.find("#wizard_schedule").hasClass("on"))
						$wifi_settings_objs.filter("#container_wizard_schedule").show();
					else
						$wifi_settings_objs.filter("#container_wizard_schedule").hide();
				}
				_set_apply_btn_status($profile_setting);
			}
			resize_iframe_height();
		});

		var aimesh_parm = {"title":"<#aimesh_mode#>", "id":"wizard_aimesh", "slide_target":"wizard_aimesh_cntr"};
		var $wizard_slide_aimesh = Get_Component_Slide_Title(aimesh_parm).attr({"data-container":"wizard_aimesh_cntr", "data-group":"wifi_settings"}).addClass("arrow_up").appendTo($more_config_cntr);
		var $wizard_aimesh_cntr = $("<div>").attr({"data-slide_target":"wizard_aimesh_cntr", "data-container":"wizard_aimesh_cntr", "data-group":"wifi_settings"}).show().appendTo($more_config_cntr);
		var $AiMesh_List = $("<div>").attr({"data-container":"AiMesh_List"}).append(Get_AiMesh_List_Container()).appendTo($wizard_aimesh_cntr);
		$AiMesh_List.find(".node_content_container").click(function(e){
			e = e || event;
			e.stopPropagation();
			_set_apply_btn_status($(this).closest(".profile_setting"));
		});

		var wizard_schedule = new schedule({
			show_timeset_viewport_callback: resize_iframe_height,
			icon_trash_callback: resize_iframe_height,
			btn_save_callback: resize_iframe_height,
			btn_cancel_callback: resize_iframe_height
		});
		var wizard_schedule_parm = {"title":str_WiFi_sche, "type":"switch", "id":"wizard_schedule", "set_value":"off"};
		Get_Component_Switch(wizard_schedule_parm).attr({"data-group":"wifi_settings"}).appendTo($more_config_cntr).find("#" + wizard_schedule_parm.id + "").click(function(e){
			e = e || event;
			e.stopPropagation();
			var $control_container = $(this).closest(".profile_setting").find("#container_wizard_schedule");
			if($(this).hasClass("on")){
				$control_container.empty().append(wizard_schedule.Get_UI());
				$control_container.show();
			}
			else{
				$control_container.hide();
			}
			resize_iframe_height();
		});
		$("<div>").addClass("profile_setting_item schedule_ui").attr({"id":"container_wizard_schedule", "data-group":"wifi_settings"}).append(wizard_schedule.Get_UI()).hide().appendTo($more_config_cntr);

		if(isSwMode("rt")){
			var bw_enabled_parm = {"title":"<#Bandwidth_Limiter#>", "type":"switch", "id":"bw_enabled", "set_value":"off"};
			Get_Component_Switch(bw_enabled_parm).attr({"data-group":"wifi_settings"}).appendTo($more_config_cntr)
				.find("#" + bw_enabled_parm.id + "").click(function(e){
					e = e || event;
					e.stopPropagation();
					var $control_container = $(this).closest(".profile_setting").find("#container_bw_dl, #container_bw_ul, #container_bw");
					if($(this).hasClass("on")){
						$control_container.show();
					}
					else{
						$control_container.hide().next(".validate_hint").remove();
					}
					resize_iframe_height();
					_set_apply_btn_status($(this).closest(".profile_setting"));
				});

			var bw_parm = {"title":"<#Bandwidth_Setting#> (Mb/s)", "container_id":"container_bw",
				"type_1":"text", "id_1":"bw_dl", "maxlength_1":12, "need_check_1":true,
				"type_2":"text", "id_2":"bw_ul", "maxlength_2":12, "need_check_2":true};
			Get_Component_Bandwidth_Setting(bw_parm).attr({"data-group":"wifi_settings"}).hide().appendTo($more_config_cntr);

			var access_intranet_parm = {"title":"<#Access_Intranet#>", "type":"switch", "id":"access_intranet", "openHint":"0_26", "set_value":"off"};
			Get_Component_Switch(access_intranet_parm).appendTo($more_config_cntr);
		}

		var $action_container = $("<div>").addClass("action_container").appendTo($content_container);
		$("<div>").addClass("btn_container apply").html("<#CTL_apply#>").appendTo($action_container);

		$content_container.find("[need_check=true]").keyup(function(){
			_set_apply_btn_status($content_container);
		});
		_set_apply_btn_status($content_container);

		return $container;
	}
	function Get_Wizard_Portal(view_mode){
		var _set_apply_btn_status = function(_obj){
			var $btn_container_apply = $(_obj).find(".action_container .btn_container.apply");
			var isBlank = validate_isBlank($(_obj));
			if(isBlank){
				$btn_container_apply.removeClass("valid_fail").addClass("valid_fail").unbind("click");
			}
			else{
				$btn_container_apply.removeClass("valid_fail").unbind("click").click(function(e){
					e = e || event;
					e.stopPropagation();
					var wizard_type = $(this).closest(".setting_content_container").attr("data-wizard-type");
					if(validate_format_Wizard_Item($(_obj), wizard_type)){
						var rc_append = "";
						var sdn_obj = get_new_sdn_profile();
						var sdn_idx = Object.keys(sdn_obj);
						var sdn_profile = sdn_obj[sdn_idx];
						selected_sdn_idx = sdn_profile.sdn_rl.idx;
						var nvramSet_obj = {"action_mode": "apply", "rc_service": "restart_wireless;restart_sdn " + selected_sdn_idx + ";"};
						var rc_append = "";
						sdn_profile.sdn_rl.sdn_name = wizard_type;
						sdn_profile.apg_rl.ssid = $(_obj).find("#sdn_name").val();
						var wifi_pwd = "";
						var wifi_auth = "open";
						var radius_idx = sdn_profile.apg_rl.apg_idx;
						sdn_profile.apg_rl.security = "<3>" + wifi_auth + ">aes>" + wifi_pwd + ">" + radius_idx + "";
						sdn_profile.apg_rl.security += "<13>" + wifi_auth + ">aes>" + wifi_pwd + ">" + radius_idx + "";
						sdn_profile.apg_rl.security += "<16>sae>aes>" + wifi_pwd + ">" + radius_idx + "";
						var dut_list = "";
						if(is_cfg_ready){
							var wifi_band = $(_obj).find("#select_wifi_band").children(".selected").attr("value");
							if(wifi_band != "0"){
								$(_obj).find("[data-container=AiMesh_List]").find(".node_container").each(function(){
									if($(this).find(".icon_checkbox").hasClass("clicked")){
										var node_mac = $(this).attr("data-node-mac");
										var specific_node = cfg_clientlist.filter(function(item, index, array){
											return (item.mac == node_mac);
										})[0];
										if(specific_node != undefined){
											dut_list += get_specific_dut_list(wifi_band, specific_node);
										}
									}
								});
								dut_list += get_unChecked_dut_list(_obj);
							}
							else{
								dut_list = get_dut_list(wifi_band);
							}
						}
						else{
							dut_list = get_dut_list_by_mac("3", cap_mac);
						}
						sdn_profile.apg_rl.dut_list = dut_list;
						sdn_profile.apg_rl.sched = schedule_handle_data.json_array_to_string(wizard_schedule.Get_Value());
						if($(_obj).find("#wizard_schedule").hasClass("on")){
							sdn_profile.apg_rl.timesched = ((sdn_profile.apg_rl.sched == "") ? "0" : "1");
						}
						else{
							sdn_profile.apg_rl.timesched = "0";
						}
						if($(_obj).find("#bw_enabled").hasClass("on")){
							sdn_profile.apg_rl.bw_limit = "<1>" + ($(_obj).find("#bw_ul").val())*1024 + ">" + ($(_obj).find("#bw_dl").val())*1024;
							nvramSet_obj.qos_enable = "1";
							nvramSet_obj.qos_type = "2";
							rc_append += "restart_qos;restart_firewall;";
						}
						else
							sdn_profile.apg_rl.bw_limit = "<0>>";

						sdn_profile.sdn_access_rl = [];
						if($(_obj).find("#access_intranet").hasClass("on")){
							sdn_profile.sdn_access_rl.push({"access_sdn_idx": "0", "sdn_idx": sdn_profile.sdn_rl.idx});
						}
						sdn_profile.apg_rl.ap_isolate = "1";

						sdn_profile.sdn_rl.cp_idx = $(_obj).find("#select_portal_type").children(".selected").attr("value");
						if(sdn_profile.sdn_rl.cp_idx == "2"){
							if($.isEmptyObject(sdn_profile.cp_rl)){
								var idx_for_customized_ui = "";
								sdn_profile.cp_rl = JSON.parse(JSON.stringify(new cp_profile_attr()));
								var specific_cp_type_rl = cp_type_rl_json.filter(function(item, index, array){
									return (item.cp_idx == sdn_profile.sdn_rl.cp_idx);
								})[0];
								if(specific_cp_type_rl != undefined){
									if(specific_cp_type_rl.profile[0] != undefined){
										idx_for_customized_ui = specific_cp_type_rl.profile[0].idx_for_customized_ui;
									}
								}
								if(idx_for_customized_ui == ""){
									idx_for_customized_ui = $.now();
								}
								sdn_profile.cp_rl.idx_for_customized_ui = idx_for_customized_ui;
							}
							sdn_profile.cp_rl.cp_idx = sdn_profile.sdn_rl.cp_idx;
							sdn_profile.cp_rl.enable = "1";
							sdn_profile.cp_rl.conntimeout = 60*60;
							sdn_profile.cp_rl.redirecturl = "";
							sdn_profile.cp_rl.auth_type = "0";
							sdn_profile.cp_rl.term_of_service = "1";
							sdn_profile.cp_rl.NAS_ID = "";
							uploadFreeWiFi({
								"cp_idx": "2",
								"id": sdn_profile.cp_rl.idx_for_customized_ui,
								"brand_name": $(_obj).find("#FWF_brand_name").val(),
								"image": $(_obj).find("#FWF_ui_container [data-component=FWF_bg]").css('background-image').replace('url(','').replace(')','').replace(/\"/gi, ""),
								"terms_service": terms_service_template,
								"auth_type": sdn_profile.cp_rl.auth_type
							});
							rc_append += "restart_chilli;restart_uam_srv;";
						}
						var sdn_all_rl_tmp = JSON.parse(JSON.stringify(sdn_all_rl_json));
						sdn_all_rl_tmp.push(JSON.parse(JSON.stringify(sdn_profile)));
						var vlan_rl_tmp = JSON.parse(JSON.stringify(vlan_rl_json));
						vlan_rl_tmp.push(JSON.parse(JSON.stringify(sdn_profile.vlan_rl)));
						var sdn_all_list = parse_JSONToStr_sdn_all_list({"sdn_all_rl":sdn_all_rl_tmp, "vlan_rl":vlan_rl_tmp});
						if(rc_append != "")
							nvramSet_obj.rc_service = nvramSet_obj.rc_service + rc_append;
						$.extend(nvramSet_obj, sdn_all_list);
						var apgX_rl = parse_apg_rl_to_apgX_rl(sdn_profile.apg_rl);
						$.extend(nvramSet_obj, apgX_rl);
						if(sdn_profile.sdn_rl.cp_idx == "2"){
							var cpX_rl = parse_cp_rl_to_cpX_rl(sdn_profile.cp_rl);
							$.extend(nvramSet_obj, cpX_rl);
						}
						if(is_QIS_flow){
							nvramSet_obj.sdn_rc_service = nvramSet_obj.rc_service;
							delete nvramSet_obj["rc_service"];
							delete nvramSet_obj["action_mode"];
							$(this).replaceWith($("<img>").attr({"width": "30px", "src": "/images/InternetScan.gif"}));
							apply.SDN_Profile(nvramSet_obj);
							return;
						}
						var showLoading_status = get_showLoading_status(nvramSet_obj.rc_service);
						if(!httpApi.app_dataHandler){
							showLoading();
							close_popup_container("all");
							if(isWLclient()){
								showLoading(showLoading_status.time);
								setTimeout(function(){
									showWlHintContainer();
								}, showLoading_status.time*1000);
								check_isAlive_and_redirect({"page": "SDN.asp", "time": showLoading_status.time});
							}
						}
						httpApi.nvramSet(nvramSet_obj, function(){
							if(isWLclient()) return;
							showLoading(showLoading_status.time);
							setTimeout(function(){
								init_sdn_all_list();
								show_sdn_profilelist();
								if(!window.matchMedia('(max-width: 575px)').matches)
									$("#profile_list_content").find("[sdn_idx=" + selected_sdn_idx + "] .item_text_container").click();
							}, showLoading_status.time*1000);
							if(!isMobile()){
								if(showLoading_status.disconnect){
									check_isAlive_and_redirect({"page": "SDN.asp", "time": showLoading_status.time});
								}
							}
						});
					}
				});
			}
		};

		var $container = $("<div>").addClass("setting_content_container");

		if(view_mode == "popup"){
			Get_Component_Popup_Profile_Title("<#GuestNetwork_Portal#>").appendTo($container)
				.find("#title_close_btn").unbind("click").click(function(e){
					e = e || event;
					e.stopPropagation();
					close_popup_container($container);
				});
		}
		else
			Get_Component_Profile_Title("<#GuestNetwork_Portal#>").appendTo($container);

		var $content_container = $("<div>").addClass("popup_content_container profile_setting").appendTo($container);

		var sdn_name_parm = {"title":"<#QIS_finish_wireless_item1#>", "type":"text", "id":"sdn_name", "need_check":true, "maxlength":32, "openHint":"0_1"};
		Get_Component_Input(sdn_name_parm).appendTo($content_container)
			.find("#" + sdn_name_parm.id + "")
			.unbind("keypress").keypress(function(){
				return validator.isString(this, event);
			})
			.unbind("keyup").keyup(function(){
				if(get_cp_type_support("2")){
					var $FWF_brand_name_items = $content_container.find("#FWF_brand_name, #FWF_Preview_Cntr [data-component=FWF_brand_name]");
					if($FWF_brand_name_items.eq(0).attr("data-user-defined") != "true"){
						if($FWF_brand_name_items.eq(0).length > 0){
							$FWF_brand_name_items.eq(0).val($(this).val());
							$FWF_brand_name_items.eq(1).html(htmlEnDeCode.htmlEncode($(this).val()));
						}
					}
				}
			})
			.attr({"oninput": "trigger_keyup($(this))"});

		var portal_options = [{"text":"<#wl_securitylevel_0#>", "value":"0"}];
		cp_type_rl_json.forEach(function(item){
			if(item.cp_idx == "4")
				return;
			portal_options.push({"text":item.cp_text, "value":item.cp_idx});
		});
		var portal_options_parm = {"title": "<#GuestNetwork_Portal_type#>", "id": "portal_type", "options": portal_options};
		Get_Component_Custom_Select(portal_options_parm).appendTo($content_container)
			.find("#select_" + portal_options_parm.id + "").children("div").click(function(e){
				if($(this).attr("data-disabled") == "true")
					return false;
				var options = $(this).attr("value");
				$(this).closest(".profile_setting").find("[data-portal-type]").hide(0, function(){
					$(this).find(".profile_setting_item").hide();
				}).filter("[data-portal-type="+options+"]").show(0, function(){
					$(this).find(".profile_setting_item").show();
				});
				resize_iframe_height();
				_set_apply_btn_status($(this).closest(".profile_setting"));
			});

		if(get_cp_type_support("2")){
			var $freewifi_cntr = $("<div>").attr({"data-portal-type":"2"}).appendTo($content_container);
			var FWF_brand_name_parm = {"title":"<#FreeWiFi_BrandName#>", "type":"text", "id":"FWF_brand_name", "need_check":true, "maxlength":40};
			Get_Component_Input(FWF_brand_name_parm).appendTo($freewifi_cntr)
				.find("#" + FWF_brand_name_parm.id + "")
				.unbind("keypress").keypress(function(){
					return validator.isString(this, event);
				})
				.unbind("keyup").keyup(function(){
					$(this).attr({"data-user-defined":"true"});
					$(this).closest(".profile_setting").find("#FWF_Preview_Cntr [data-component=FWF_brand_name]").html(htmlEnDeCode.htmlEncode($(this).val()));
				})
				.attr({"oninput": "trigger_keyup($(this))"});

			var $FWF_ui_cntr = $("<div>").attr({"id":"FWF_ui_container"}).addClass("profile_setting_item FWF_ui").appendTo($freewifi_cntr);
			Get_FWF_Preview_Container().appendTo($FWF_ui_cntr).find("[data-component=FWF_passcode]").remove();
			Get_FWF_Change_BG_Container().hide().appendTo($FWF_ui_cntr);
		}

		if(isSupport("fbwifi_sdn")){
			var $fb_cntr = $("<div>").attr({"data-portal-type":"3"}).appendTo($content_container);
			var fb_btn_parm = {"id":"fb_btn", "text":"<#btn_goSetting#>"};
			Get_Component_Btn(fb_btn_parm).appendTo($fb_cntr)
				.find("#" + fb_btn_parm.id + "").click(function(e){
					e = e || event;
					e.stopPropagation();
					if(!httpApi.fbwifi.isAvailable()){
						show_customize_alert("<#Facebook_WiFi_disconnected#>");
						return false;
					}
					else{
						var url = httpApi.nvramGet(["fbwifi_cp_config_url"]).fbwifi_cp_config_url;
						window.open(url, '_blank', 'toolbar=no, location=no, menubar=no, top=0, left=0, width=700, height=600, scrollbars=1');
					}
				});
		}

		var more_config_parm = {"title":"<#MoreConfig#>", "id":"more_config", "slide_target":"more_config_cntr"};
		Get_Component_Slide_Title(more_config_parm).appendTo($content_container);
		var $more_config_cntr = $("<div>").attr({"data-slide_target": "more_config_cntr"}).hide().appendTo($content_container);

		var wifi_band_options = [{"text":"2.4GHz / 5GHz","value":"3"}, {"text":"2.4GHz","value":"1"}, {"text":"5GHz","value":"2"}, {"text":"<#wl_securitylevel_0#>","value":"0"}];
		var wifi_band_options_parm = {"title": "WiFi <#Interface#>", "id": "wifi_band", "options": wifi_band_options, "set_value": "3"};
		var $sel_wifi_band = Get_Component_Custom_Select(wifi_band_options_parm).appendTo($more_config_cntr).find("#select_" + wifi_band_options_parm.id + "");
		$sel_wifi_band.children("div").click(function(e){
			var options = $(this).attr("value");
			var $profile_setting = $(this).closest(".profile_setting");
			$profile_setting.find("[data-container=AiMesh_List]").next(".validate_hint").remove();
			var $wifi_settings_objs = $profile_setting.find("[data-group=wifi_settings]");
			if(options == "0"){
				$wifi_settings_objs.hide().next(".validate_hint").remove();
				_set_apply_btn_status($profile_setting);
			}
			else{
				$wifi_settings_objs.show().filter("#wizard_aimesh").addClass("arrow_up");
				Set_AiMesh_List_CB($profile_setting.find("[data-container=AiMesh_List]"), options);
				if($wifi_settings_objs.find("#bw_enabled").length > 0){
					if($wifi_settings_objs.find("#bw_enabled").hasClass("on")){
						$wifi_settings_objs.filter("#container_bw").show();
					}
					else{
						$wifi_settings_objs.filter("#container_bw").hide();
					}
				}
				if($wifi_settings_objs.find("#wizard_schedule").length > 0){
					if($wifi_settings_objs.find("#wizard_schedule").hasClass("on"))
						$wifi_settings_objs.filter("#container_wizard_schedule").show();
					else
						$wifi_settings_objs.filter("#container_wizard_schedule").hide();
				}
				_set_apply_btn_status($profile_setting);
			}
			resize_iframe_height();
		});

		var aimesh_parm = {"title":"<#aimesh_mode#>", "id":"wizard_aimesh", "slide_target":"wizard_aimesh_cntr"};
		var $wizard_slide_aimesh = Get_Component_Slide_Title(aimesh_parm).attr({"data-container":"wizard_aimesh_cntr", "data-group":"wifi_settings"}).addClass("arrow_up").appendTo($more_config_cntr);
		var $wizard_aimesh_cntr = $("<div>").attr({"data-slide_target":"wizard_aimesh_cntr", "data-container":"wizard_aimesh_cntr", "data-group":"wifi_settings"}).show().appendTo($more_config_cntr);
		var $AiMesh_List = $("<div>").attr({"data-container":"AiMesh_List"}).append(Get_AiMesh_List_Container()).appendTo($wizard_aimesh_cntr);
		$AiMesh_List.find(".node_content_container").click(function(e){
			e = e || event;
			e.stopPropagation();
			_set_apply_btn_status($(this).closest(".profile_setting"));
		});

		var wizard_schedule = new schedule({
			show_timeset_viewport_callback: resize_iframe_height,
			icon_trash_callback: resize_iframe_height,
			btn_save_callback: resize_iframe_height,
			btn_cancel_callback: resize_iframe_height
		});
		var wizard_schedule_parm = {"title":str_WiFi_sche, "type":"switch", "id":"wizard_schedule", "set_value":"off"};
		Get_Component_Switch(wizard_schedule_parm).attr({"data-group":"wifi_settings"}).appendTo($more_config_cntr).find("#" + wizard_schedule_parm.id + "").click(function(e){
			e = e || event;
			e.stopPropagation();
			var $control_container = $(this).closest(".profile_setting").find("#container_wizard_schedule");
			if($(this).hasClass("on")){
				$control_container.empty().append(wizard_schedule.Get_UI());
				$control_container.show();
			}
			else{
				$control_container.hide();
			}
			resize_iframe_height();
		});
		$("<div>").addClass("profile_setting_item schedule_ui").attr({"id":"container_wizard_schedule", "data-group":"wifi_settings"}).append(wizard_schedule.Get_UI()).hide().appendTo($more_config_cntr);

		var bw_enabled_parm = {"title":"<#Bandwidth_Limiter#>", "type":"switch", "id":"bw_enabled", "set_value":"off"};
		Get_Component_Switch(bw_enabled_parm).attr({"data-group":"wifi_settings"}).appendTo($more_config_cntr)
			.find("#" + bw_enabled_parm.id + "").click(function(e){
				e = e || event;
				e.stopPropagation();
				var $control_container = $(this).closest(".profile_setting").find("#container_bw_dl, #container_bw_ul, #container_bw");
				if($(this).hasClass("on")){
					$control_container.show();
				}
				else{
					$control_container.hide().next(".validate_hint").remove();
				}
				resize_iframe_height();
				_set_apply_btn_status($(this).closest(".profile_setting"));
			});

		var bw_parm = {"title":"<#Bandwidth_Setting#> (Mb/s)", "container_id":"container_bw",
			"type_1":"text", "id_1":"bw_dl", "maxlength_1":12, "need_check_1":true,
			"type_2":"text", "id_2":"bw_ul", "maxlength_2":12, "need_check_2":true};
		Get_Component_Bandwidth_Setting(bw_parm).attr({"data-group":"wifi_settings"}).hide().appendTo($more_config_cntr);

		var access_intranet_parm = {"title":"<#Access_Intranet#>", "type":"switch", "id":"access_intranet", "openHint":"0_26", "set_value":"off"};
		Get_Component_Switch(access_intranet_parm).appendTo($more_config_cntr);

		var $action_container = $("<div>").addClass("action_container").appendTo($content_container);
		$("<div>").addClass("btn_container apply").html("<#CTL_apply#>").appendTo($action_container);

		$content_container.find("[need_check=true]").keyup(function(){
			_set_apply_btn_status($content_container);
		});
		_set_apply_btn_status($content_container);

		return $container;
	}
}
function Get_Component_Full_Setting(view_mode){
	var $container = $("<div>").addClass("setting_content_container");

	if(view_mode == "popup"){
		var $popup_title_cntr = $("<div>").addClass("popup_title_container edit_mode").appendTo($container);
		var $title_cntr = $("<div>").addClass("title_container").appendTo($popup_title_cntr);
		$("<div>").addClass("title").attr({"data-container":"profile_title"}).html("<#DSL_profile#>").appendTo($title_cntr);
		$("<div>").attr({"id":"title_close_btn"}).addClass("close_btn").unbind("click").click(function(e){
			e = e || event;
			e.stopPropagation();
			close_popup_container($container);
		}).appendTo($popup_title_cntr);
	}
	else{
		var $title_cntr = $("<div>").addClass("profile_setting_title edit_mode").appendTo($container)
		$("<div>").addClass("title").attr({"data-container":"profile_title"}).html("<#DSL_profile#>").appendTo($title_cntr);
		var $profile_title_action = $("<div>").attr({"data-container":"profile_title_action"}).addClass("profile_title_action").appendTo($title_cntr);
		$("<div>").attr({"id":"title_del_btn"}).addClass("del_btn").appendTo($profile_title_action);
	}

	var $content_container = $("<div>").addClass("popup_content_container profile_setting").appendTo($container);

	var $subtab_cntr = Get_Setting_SubTab_Container().appendTo($content_container);

	var $subtab_container_net = $("<div>").attr({"subtab_container": "net"}).appendTo($content_container);
	var category_general = $("<div>").addClass("category_container").attr({"data-category-cntr":"general"}).appendTo($subtab_container_net);
	var general_parm = {"title":"<#menu5_1_1#>", "id":"category_general", "slide_target":"category_general_cntr"};
	Get_Component_Category_Slide_Title(general_parm).appendTo(category_general)
		.find(".title_cntr").unbind("click").click(function(e){
			e = e || event;
			e.stopPropagation();
			handle_category_container($(this), general_parm);
		});
	Get_Net_General_Container().hide().appendTo(category_general);

	var category_adv = $("<div>").addClass("category_container").attr({"data-category-cntr":"adv"}).appendTo($subtab_container_net);
	var adv_parm = {"title":"<#menu5#>", "id":"category_adv", "slide_target":"category_adv_cntr"};
	Get_Component_Category_Slide_Title(adv_parm).appendTo(category_adv)
		.find(".title_cntr").unbind("click").click(function(e){
			e = e || event;
			e.stopPropagation();
			handle_category_container($(this), adv_parm);
		});
	Get_Net_Adv_Container().hide().appendTo(category_adv);

	var category_aimesh = $("<div>").addClass("category_container").attr({"data-category-cntr":"aimesh","data-group":"wifi_settings"}).appendTo($subtab_container_net);
	var aimesh_parm = {"title":"<#aimesh_mode#>", "id":"category_aimesh", "slide_target":"category_aimesh_cntr"};/* untranslated */
	Get_Component_Category_Slide_Title(aimesh_parm).appendTo(category_aimesh)
		.find(".title_cntr").unbind("click").click(function(e){
			e = e || event;
			e.stopPropagation();
			handle_category_container($(this), aimesh_parm);
		});
	Get_Net_AiMesh_Container().hide().appendTo(category_aimesh);

	var $action_container = $("<div>").attr({"data-container":"action_container"}).addClass("action_container").appendTo($subtab_container_net);
	$("<div>").attr({"data-component":"del_btn"}).addClass("btn_container delete mobile").appendTo($action_container).html("<#CTL_del#>");
	$("<div>").attr({"data-component":"apply_btn"}).addClass("btn_container apply").appendTo($action_container).html("<#CTL_apply#>");

	//var $subtab_container_sec = $("<div>").attr({"subtab_container": "sec"}).appendTo($content_container);

	if(isSwMode("rt")){
		var $subtab_container_vpn = $("<div>").attr({"subtab_container": "vpn"}).appendTo($content_container);
		var vpn_enabled_parm = {"title":"VPN", "type":"switch", "id":"vpn_enabled"};
		Get_Component_Switch(vpn_enabled_parm).appendTo($subtab_container_vpn)
			.find("#" + vpn_enabled_parm.id + "").click(function(e){
				e = e || event;
				e.stopPropagation();
				var $control_container = $(this).closest(".profile_setting").find("[data-container=VPN_Profiles]");
				if($(this).hasClass("on")){
					$control_container.show();
				}
				else{
					$control_container.hide();
				}
				resize_iframe_height();
			});
		Get_Component_VPN_Profiles().appendTo($subtab_container_vpn);
	}

	var $subtab_container_client = $("<div>").attr({"subtab_container": "client"}).appendTo($content_container);
	var $client_info_cntr =  $("<div>").attr({"data-container":"client_info_container"}).addClass("client_info_container").appendTo($subtab_container_client);
	var client_tab_options = [{"text":"<#Clientlist_All_List#>","option_id":"all"}, {"text":"<#tm_wired#>","option_id":"wired"}, {"text":"<#tm_wireless#>","option_id":"wireless"}];
	var client_tab_parm = {"title":"<#ConnectedClient#> : 0 <#Clientlist_Online#>", "options": client_tab_options, "container_id":"client_tab"};
	Get_Component_Switch_Text(client_tab_parm).appendTo($client_info_cntr)
		.find(".switch_text_container > .switch_text_item")
		.click(function(e){
			e = e || event;
			e.stopPropagation();
			var sdn_idx = $client_info_cntr.closest(".setting_content_container").attr("sdn_idx");
			var option_id = $(this).attr("data-option-id");
			var $clientlist_container = $client_info_cntr.find("[data-container=clientlist_container]").empty();
			var $client_num = $client_info_cntr.find("#client_tab > .title");
			var sdn_clientlist_data = [];
			var get_clientlist = httpApi.hookGet("get_clientlist");
			$.each(get_clientlist, function(index, client){
				if(client.sdn_idx != undefined && client.sdn_idx == sdn_idx){
					if(client.isOnline == "1"){
						switch(option_id){
							case "all":
								sdn_clientlist_data.push(client);
								break;
							case "wired":
								if(client.isWL == "0"){
									sdn_clientlist_data.push(client);
								}
								break;
							case "wireless":
								if(client.isWL >= "1"){
									sdn_clientlist_data.push(client);
								}
								break;
						}
					}
				}
			});
			$client_num.html("<#ConnectedClient#> : " + sdn_clientlist_data.length + " <#Clientlist_Online#>");
			Get_Component_ClientList(sdn_clientlist_data).appendTo($clientlist_container);
			resize_iframe_height();
		});
	$("<div>").attr({"data-container":"clientlist_container"}).addClass("clientlist_container").appendTo($client_info_cntr);

	if(isSwMode("rt")){
		var $subtab_container_portal = $("<div>").attr({"subtab_container": "portal"}).appendTo($content_container);
		var portal_options = [{"text":"<#wl_securitylevel_0#>", "value":"0"}];
		cp_type_rl_json.forEach(function(item){
			portal_options.push({"text":item.cp_text, "value":item.cp_idx});
		});
		var portal_options_parm = {"title": "<#GuestNetwork_Portal_type#>", "id": "portal_type", "options": portal_options};
		Get_Component_Custom_Select(portal_options_parm).appendTo($subtab_container_portal)
			.find("#select_" + portal_options_parm.id + "").children("div").click(function(e){
				if($(this).attr("data-disabled") == "true")
					return false;
				var options = $(this).attr("value");
				$(this).closest(".profile_setting").find("[data-portal-type]").hide(0, function(){
					$(this).find(".profile_setting_item").hide();
				}).filter("[data-portal-type="+options+"]").show(0, function(){
					$(this).find(".profile_setting_item").show();
				});
				resize_iframe_height();
				set_apply_btn_status($(this).closest(".profile_setting"));
			});

		if(get_cp_type_support("2")){
			var $freewifi_cntr = $("<div>").attr({"data-portal-type":"2"}).appendTo($subtab_container_portal);
			var FWF_brand_name_parm = {"title":"<#FreeWiFi_BrandName#>", "type":"text", "id":"FWF_brand_name", "need_check":true, "maxlength":40};
				Get_Component_Input(FWF_brand_name_parm).appendTo($freewifi_cntr)
					.find("#" + FWF_brand_name_parm.id + "")
					.unbind("keypress").keypress(function(){
						return validator.isString(this, event);
					})
					.unbind("keyup").keyup(function(){
						$(this).closest(".profile_setting").find("#FWF_Preview_Cntr [data-component=FWF_brand_name]").html(htmlEnDeCode.htmlEncode($(this).val()));
					})
					.attr({"oninput": "trigger_keyup($(this))"});

			var $FWF_ui_cntr = $("<div>").attr({"id":"FWF_ui_container"}).addClass("profile_setting_item FWF_ui").appendTo($freewifi_cntr);
			Get_FWF_Preview_Container().appendTo($FWF_ui_cntr);
			Get_FWF_Change_BG_Container().hide().appendTo($FWF_ui_cntr);

			var FWF_conntimeout_parm = {"title":"<#FreeWiFi_timeout#> (<#Minute#>)", "type":"text", "id":"FWF_conntimeout", "need_check":true, "maxlength":3, "set_value":"60", "openHint":"31_3"};
			Get_Component_Input(FWF_conntimeout_parm).appendTo($freewifi_cntr)
				.find("#" + FWF_conntimeout_parm.id + "")
				.unbind("keypress").keypress(function(){
					return validator.isNumber(this,event);
				});
			var FWF_redirecturl_parm = {"title":"<#FreeWiFi_LandingPage#> (<#FreeWiFi_RedirectPage#>)", "type":"text", "id":"FWF_redirecturl", "maxlength":256};
			Get_Component_Input(FWF_redirecturl_parm).appendTo($freewifi_cntr);
			var FWF_passcode_enabled_parm = {"title":"<#FreeWiFi_Option_Add_Passcode#>", "type":"switch", "id":"FWF_passcode_enabled", "set_value":"off"};
			Get_Component_Switch(FWF_passcode_enabled_parm).appendTo($freewifi_cntr)
				.find("#" + FWF_passcode_enabled_parm.id + "").click(function(e){
					e = e || event;
					e.stopPropagation();
					var $control_container = $(this).closest(".profile_setting").find("#container_FWF_passcode, #FWF_Preview_Cntr [data-component=FWF_passcode]");
					if($(this).hasClass("on")){
						$control_container.show();
					}
					else{
						$control_container.hide().next(".validate_hint").remove();
					}
					resize_iframe_height();
					set_apply_btn_status($(this).closest(".profile_setting"));
				});
			var FWF_passcode_parm = {"title":"<#FreeWiFi_Passcode#>", "type":"text", "id":"FWF_passcode", "need_check":true, "maxlength":64, "container_id":"container_FWF_passcode"};
			Get_Component_Input(FWF_passcode_parm).hide().appendTo($freewifi_cntr);

			var parm_terms_service = terms_service_template;
			var FWF_termsservice_parm = {"title":"<#Terms_of_Service#>", "id":"FWF_termsservice", "rows":"8", "cols":"55", "need_check":true, "maxlength":"5120", "set_value":parm_terms_service};
			Get_Component_Textarea(FWF_termsservice_parm).appendTo($freewifi_cntr)
				.find("#" + FWF_termsservice_parm.id + "")
				.unbind("keypress").keypress(function(){
					return validator.isString(this, event);
				})
				.unbind("keyup").keyup(function(){
					$(this).closest(".profile_setting").find("#FWF_Preview_Cntr [data-component=FWF_terms_service]").html(htmlEnDeCode.htmlEncode($(this).val()).replace(/(?:\r\n|\r|\n)/g, '<div style=height:6px;></div>'));
				})
				.attr({"oninput": "trigger_keyup($(this))"});
		}
		if(get_cp_type_support("4")){
			var $message_board_cntr = $("<div>").attr({"data-portal-type":"4"}).appendTo($subtab_container_portal);
			var MB_desc_parm = {"title":"<#Description#>", "type":"text", "id":"MB_desc", "need_check":true, "maxlength":80};
				Get_Component_Input(MB_desc_parm).appendTo($message_board_cntr)
					.find("#" + MB_desc_parm.id + "")
					.unbind("keypress").keypress(function(){
						return validator.isString(this, event);
					})
					.unbind("keyup").keyup(function(){
						$(this).closest(".profile_setting").find("#MB_Preview_Cntr [data-component=MB_desc]").html(htmlEnDeCode.htmlEncode($(this).val()));
					})
					.attr({"oninput": "trigger_keyup($(this))"});

			var $MB_ui_cntr = $("<div>").attr({"id":"MB_ui_container"}).addClass("profile_setting_item MB_ui").appendTo($message_board_cntr);
			Get_MB_Preview_Container().appendTo($MB_ui_cntr);
			Get_MB_Change_BG_Container().hide().appendTo($MB_ui_cntr);
		}

		if(isSupport("fbwifi_sdn")){
			var $fb_cntr = $("<div>").attr({"data-portal-type":"3"}).appendTo($subtab_container_portal);
			var fb_btn_parm = {"id":"fb_btn", "text":"<#btn_goSetting#>"};
			Get_Component_Btn(fb_btn_parm).appendTo($fb_cntr)
				.find("#" + fb_btn_parm.id + "").click(function(e){
					e = e || event;
					e.stopPropagation();

					if(!httpApi.fbwifi.isAvailable()){
						return false;
					}
					else{
						var url = httpApi.nvramGet(["fbwifi_cp_config_url"]).fbwifi_cp_config_url;
						window.open(url, '_blank', 'toolbar=no, location=no, menubar=no, top=0, left=0, width=700, height=600, scrollbars=1');
					}
				});
		}
	}
	var $action_container = $("<div>").attr({"id":"action_container"}).addClass("action_container").appendTo($content_container);
	var $btn_container_delete_mobile = $("<div>").attr({"id":"del_btn"}).addClass("btn_container delete mobile").appendTo($action_container).html("<#CTL_del#>");
	var $btn_container_apply = $("<div>").attr({"id":"apply_btn"}).addClass("btn_container apply").appendTo($action_container).html("<#CTL_apply#>");

	$content_container.find("[need_check=true]").keyup(function(){
		set_apply_btn_status($content_container);
	});

	$subtab_cntr.find("[subtab=net]").click();
	$subtab_container_net.find("[data-category-cntr=general] .title_cntr").click();
	$action_container.hide();

	return $container;

	function Get_Setting_SubTab_Container(){
		var $container = $("<div>").addClass("setting_subtab_container");
		$("<div>").addClass("subtab_item").attr({"subtab":"net"}).html("<#Network#>").unbind("click").click(function(e){
			e = e || event;
			e.stopPropagation();
			$(this).closest(".Setting_SubTab_Container").find(".subtab_item").removeClass("selected");
			$(this).addClass("selected");
			var subtab = $(this).attr("subtab");
			$(this).closest(".profile_setting").find("[subtab_container]").hide();
			$(this).closest(".profile_setting").find("[subtab_container='" + subtab + "']").show();
			$(this).closest(".profile_setting").find("#action_container").hide();
			resize_iframe_height();
		}).appendTo($container);
		/*
		$("<div>").addClass("subtab_item").attr({"subtab":"sec"}).html("Security").unbind("click").click(function(e){
			e = e || event;
			e.stopPropagation();
			$(this).closest(".Setting_SubTab_Container").find(".subtab_item").removeClass("selected");
			$(this).addClass("selected");
			var subtab = $(this).attr("subtab");
			$(this).closest(".profile_setting").find("[subtab_container]").hide();
			$(this).closest(".profile_setting").find("[subtab_container='" + subtab + "']").show();
			$(this).closest(".profile_setting").find("#action_container").show();
			resize_iframe_height();
		}).appendTo($container);
		*/
		if(isSwMode("rt")){
			$("<div>").addClass("subtab_item").attr({"subtab":"vpn"}).html("VPN").unbind("click").click(function(e){
				e = e || event;
				e.stopPropagation();
				$(this).closest(".Setting_SubTab_Container").find(".subtab_item").removeClass("selected");
				$(this).addClass("selected");
				var subtab = $(this).attr("subtab");
				$(this).closest(".profile_setting").find("[subtab_container]").hide();
				$(this).closest(".profile_setting").find("[subtab_container='" + subtab + "']").show();
				$(this).closest(".profile_setting").find("#action_container").show();
				resize_iframe_height();
			}).appendTo($container);
		}
		$("<div>").addClass("subtab_item").attr({"subtab":"client"}).html("<#Full_Clients#>").unbind("click").click(function(e){
			e = e || event;
			e.stopPropagation();
			$(this).closest(".Setting_SubTab_Container").find(".subtab_item").removeClass("selected");
			$(this).addClass("selected");
			var subtab = $(this).attr("subtab");
			$(this).closest(".profile_setting").find("[subtab_container]").hide();
			$(this).closest(".profile_setting").find("[subtab_container='" + subtab + "']").show();
			$(this).closest(".profile_setting").find("#action_container").hide();
			resize_iframe_height();
		}).appendTo($container);
		if(isSwMode("rt")){
			if(cp_type_rl_json.length > 0){
				$("<div>").addClass("subtab_item").attr({"subtab":"portal"}).html("Guest Portal").unbind("click").click(function(e){
					e = e || event;
					e.stopPropagation();
					$(this).closest(".Setting_SubTab_Container").find(".subtab_item").removeClass("selected");
					$(this).addClass("selected");
					var subtab = $(this).attr("subtab");
					$(this).closest(".profile_setting").find("[subtab_container]").hide();
					$(this).closest(".profile_setting").find("[subtab_container='" + subtab + "']").show();
					$(this).closest(".profile_setting").find("#action_container").show();
					resize_iframe_height();
				}).appendTo($container);
			}
		}

		return $container;
	}
	function Get_Net_General_Container(){
		var $net_mode_container = $("<div>").attr({"data-slide_target": "category_general_cntr"});

		var sdn_name_parm = {"title":"<#QIS_finish_wireless_item1#>", "type":"text", "id":"sdn_name", "need_check":true, "maxlength":32, "openHint":"0_1"};
		var $sdn_name_cntr = Get_Component_Input(sdn_name_parm).appendTo($net_mode_container)
		$sdn_name_cntr.find("#" + sdn_name_parm.id + "")
			.unbind("keypress").keypress(function(){
				return validator.isString(this, event);
			});
		$sdn_name_cntr.addClass("qrcode_item").find(".input_container").append($("<div>").attr({"id":"qrcode_btn"}).addClass("qrcode_btn"));

		var wifi_band_options = [{"text":"2.4GHz / 5GHz","value":"3"}, {"text":"2.4GHz","value":"1"}, {"text":"5GHz","value":"2"}, {"text":"<#wl_securitylevel_0#>","value":"0"}];
		var wifi_band_options_parm = {"title": "WiFi <#Interface#>", "id": "wifi_band", "options": wifi_band_options, "set_value": "3"};
		var $sel_wifi_band = Get_Component_Custom_Select(wifi_band_options_parm).appendTo($net_mode_container).find("#select_" + wifi_band_options_parm.id + "");
		$sel_wifi_band.children("div").click(function(e){
			var options = $(this).attr("value");
			var $profile_setting = $(this).closest(".profile_setting");
			$profile_setting.find("[data-container=AiMesh_List]").next(".validate_hint").remove();
			var $wifi_settings_objs = $profile_setting.find("[data-group=wifi_settings]");
			if(options == "0"){
				$wifi_settings_objs.hide().next(".validate_hint").remove();
				set_apply_btn_status($profile_setting);
			}
			else{
				$wifi_settings_objs.show().filter("#sdn_pwd_strength").hide();
				Set_AiMesh_List_CB($profile_setting.find("[data-container=AiMesh_List]"), options);
				if($wifi_settings_objs.find(".switch_text_container").length > 0)
					$wifi_settings_objs.find(".switch_text_container .switch_text_item.selected").click();
				if($wifi_settings_objs.find("#bw_enabled").length > 0){
					if($wifi_settings_objs.find("#bw_enabled").hasClass("on")){
						$wifi_settings_objs.filter("#container_bw").show();
					}
					else{
						$wifi_settings_objs.filter("#container_bw").hide();
					}
				}
				if($wifi_settings_objs.find("#schedule").length > 0){
					if($wifi_settings_objs.find("#schedule").hasClass("on"))
						$wifi_settings_objs.filter("#container_schedule").show();
					else
						$wifi_settings_objs.filter("#container_schedule").hide();
				}

				set_apply_btn_status($profile_setting);
			}
			resize_iframe_height();
		});

		var security_options = [{"text":"Open System","option_id":"open"}, {"text":"<#HSDPAConfig_Password_itemname#>","option_id":"pwd"}];
		var security_parm = {"title":"<#Security#>", "options": security_options, "container_id":"security_guest"};
		Get_Component_Switch_Text(security_parm).attr({"data-group":"wifi_settings"}).appendTo($net_mode_container)
			.find(".switch_text_container > .switch_text_item")
			.click(function(e){
				e = e || event;
				e.stopPropagation();
				var option_id = $(this).attr("data-option-id");
				$(this).closest(".profile_setting").find("[data-sec-option-id]").hide();
				$(this).closest(".profile_setting").find("[data-sec-option-id=" + option_id + "]").show();
				if(option_id == "pwd"){
					$(this).closest(".profile_setting").find("#sdn_pwd_strength").hide();
				}
				resize_iframe_height();
				set_apply_btn_status($net_mode_container.closest(".profile_setting"));
			});

		var security_radius_options = [{"text":"<#HSDPAConfig_Password_itemname#>","option_id":"pwd"}, {"text":"<#menu5_1_5#>","option_id":"radius"}];
		var security_radius_parm = {"title":"<#Security#>", "options": security_radius_options, "container_id":"security_employee"};
		Get_Component_Switch_Text(security_radius_parm).attr({"data-group":"wifi_settings"}).appendTo($net_mode_container)
			.find(".switch_text_container > .switch_text_item")
			.click(function(e){
				e = e || event;
				e.stopPropagation();
				var option_id = $(this).attr("data-option-id");
				$(this).closest(".profile_setting").find("[data-sec-option-id]").hide().next(".validate_hint").remove();
				$(this).closest(".profile_setting").find("[data-sec-option-id=" + option_id + "]").show();
				if(option_id == "pwd"){
					$(this).closest(".profile_setting").find("#sdn_pwd_strength").hide();
				}

				resize_iframe_height();
				set_apply_btn_status($net_mode_container.closest(".profile_setting"));
				if(isSupport("wpa3-e")){
					Set_AiMesh_List_CB(
						$(this).closest(".profile_setting").find("[data-container=AiMesh_List]"),
						$(this).closest(".profile_setting").find("#select_wifi_band").children(".selected").attr("value")
					);
				}
			});

		var wifi_auth_options = [{"text":"WPA2-Personal","value":"psk2"},/*{"text":"WPA3-Personal","value":"sae"},*/{"text":"WPA/WPA2-Personal","value":"pskpsk2"},{"text":"WPA2/WPA3-Personal","value":"psk2sae"}];
		var wifi_auth_options_parm = {"title": "<#WLANConfig11b_AuthenticationMethod_itemname#>", "id": "wifi_auth", "options": wifi_auth_options, "set_value": "psk2"};
		Get_Component_Custom_Select(wifi_auth_options_parm).attr({"data-sec-option-id":"pwd","data-group":"wifi_settings"}).appendTo($net_mode_container);

		var wifi_auth_radius_options = [{"text":"WPA2-Enterprise","value":"wpa2"},{"text":"WPA/WPA2-Enterprise","value":"wpawpa2"}];
		if(isSupport("wpa3-e")){
			wifi_auth_radius_options.push({"text":"WPA3-Enterprise","value":"wpa3"});
			wifi_auth_radius_options.push({"text":"WPA2/WPA3-Enterprise","value":"wpa2wpa3"});
			wifi_auth_radius_options.push({"text":"WPA3-Enterprise 192-bit","value":"suite-b"});
		}
		var wifi_auth_radius_options_parm = {"title": "<#WLANConfig11b_AuthenticationMethod_itemname#>", "id": "wifi_auth_radius", "options": wifi_auth_radius_options, "set_value": "wpa2"};
		var $sel_wifi_auth_radius = Get_Component_Custom_Select(wifi_auth_radius_options_parm).attr({"data-sec-option-id":"radius","data-group":"wifi_settings"}).appendTo($net_mode_container).find("#select_" + wifi_auth_radius_options_parm.id + "");
		if(isSupport("wpa3-e")){
			$sel_wifi_auth_radius.children("div").click(function(e){
				Set_AiMesh_List_CB(
					$(this).closest(".profile_setting").find("[data-container=AiMesh_List]"),
					$(this).closest(".profile_setting").find("#select_wifi_band").children(".selected").attr("value")
				);
			});
		}

		var sdn_pwd_parm = {"title":"<#QIS_finish_wireless_item2#>", "type":"password", "id":"sdn_pwd", "need_check":true, "maxlength":32, "openHint":"0_7"};
		Get_Component_Input(sdn_pwd_parm).attr({"data-sec-option-id":"pwd","data-group":"wifi_settings"}).appendTo($net_mode_container)
			.find("#" + sdn_pwd_parm.id + "")
			.unbind("keyup").keyup(function(){
				chkPass($(this).val(), "rwd_vpn_pwd", $("#sdn_pwd_strength"));
				resize_iframe_height();
			})
			.unbind("blur").blur(function(){
				if($(this).val() == ""){
					$("#" + sdn_pwd_parm.id + "_strength").hide();
				}
			});
		$("<div>").attr({"id":"sdn_pwd_strength", "data-sec-option-id":"pwd", "data-group":"wifi_settings"}).append(Get_Component_PWD_Strength_Meter()).appendTo($net_mode_container).hide();

		var radius_ipaddr_parm = {"title":"<#WLANAuthentication11a_ExAuthDBIPAddr_itemname#>", "type":"text", "id":"radius_ipaddr", "need_check":true, "maxlength":15, "openHint":"2_1"};
		Get_Component_Input(radius_ipaddr_parm).attr({"data-sec-option-id":"radius","data-group":"wifi_settings"}).appendTo($net_mode_container)
			.find("#" + radius_ipaddr_parm.id + "")
			.unbind("keypress").keypress(function(){
				return validator.isIPAddr(this, event);
			});

		var radius_port_parm = {"title":"<#WLANAuthentication11a_ExAuthDBPortNumber_itemname#>", "type":"text", "id":"radius_port", "need_check":true, "maxlength":5, "openHint":"2_2"};
		Get_Component_Input(radius_port_parm).attr({"data-sec-option-id":"radius","data-group":"wifi_settings"}).appendTo($net_mode_container)
			.find("#" + radius_port_parm.id + "")
			.unbind("keypress").keypress(function(){
				return validator.isNumber(this,event);
			});

		var radius_key_parm = {"title":"<#WLANAuthentication11a_ExAuthDBPassword_itemname#>", "type":"password", "id":"radius_key", "need_check":true, "maxlength":64, "openHint":"2_3"};
		Get_Component_Input(radius_key_parm).attr({"data-sec-option-id":"radius","data-group":"wifi_settings"}).appendTo($net_mode_container);

		if(!isMobile()){
			var qrcode_btn_parm = {"title":"Print the Wi-Fi quick connect sheet", "id":"qrcode_print_btn"};
			Get_Component_Print_Btn(qrcode_btn_parm).appendTo($net_mode_container);
		}
		var bw_enabled_parm = {"title":"<#Bandwidth_Limiter#>", "type":"switch", "id":"bw_enabled"};
		Get_Component_Switch(bw_enabled_parm).attr({"data-group":"wifi_settings"}).appendTo($net_mode_container)
			.find("#" + bw_enabled_parm.id + "").click(function(e){
				e = e || event;
				e.stopPropagation();
				var $control_container = $(this).closest(".profile_setting").find("#container_bw_dl, #container_bw_ul, #container_bw");
				if($(this).hasClass("on")){
					$control_container.show();
				}
				else{
					$control_container.hide().next(".validate_hint").remove();
				}
				resize_iframe_height();
				set_apply_btn_status($net_mode_container.closest(".profile_setting"));
			});

		var bw_parm = {"title":"<#Bandwidth_Setting#> (Mb/s)", "container_id":"container_bw",
			"type_1":"text", "id_1":"bw_dl", "maxlength_1":12, "need_check_1":true,
			"type_2":"text", "id_2":"bw_ul", "maxlength_2":12, "need_check_2":true};
		Get_Component_Bandwidth_Setting(bw_parm).attr({"data-group":"wifi_settings"}).appendTo($net_mode_container);

		var access_intranet_parm = {"title":"<#Access_Intranet#>", "type":"switch", "id":"access_intranet", "openHint":"0_26"};
		Get_Component_Switch(access_intranet_parm).appendTo($net_mode_container);

		var schedule_parm = {"title":str_WiFi_sche, "type":"switch", "id":"schedule"};
		Get_Component_Switch(schedule_parm).attr({"data-group":"wifi_settings"}).appendTo($net_mode_container).find("#" + schedule_parm.id + "").click(function(e){
			e = e || event;
			e.stopPropagation();
			var $control_container = $(this).closest(".profile_setting").find("#container_schedule");
			if($(this).hasClass("on")){
				$control_container.empty().append(sdn_schedule.Get_UI());
				$control_container.show();
			}
			else{
				$control_container.hide();
			}
			resize_iframe_height();
		});

		$("<div>").addClass("profile_setting_item schedule_ui").attr({"id":"container_schedule","data-group":"wifi_settings"}).hide().appendTo($net_mode_container);

		 return $net_mode_container;
	}
	function Get_Net_Adv_Container(){
		var $net_mode_container = $("<div>").attr({"data-slide_target": "category_adv_cntr"});

		if(isSwMode("rt")){
			var dhcp_enable_parm = {"title":"<#LANHostConfig_DHCPServerConfigurable_itemname#>", "type":"switch", "id":"dhcp_enable", "openHint":"5_1"};
			Get_Component_Switch(dhcp_enable_parm).appendTo($net_mode_container);

			var ipaddr_parm = {"title":"<#LAN_IP#>", "type":"text", "id":"ipaddr", "need_check":true, "maxlength":15};
			Get_Component_Input(ipaddr_parm).appendTo($net_mode_container)
				.find("#" + ipaddr_parm.id + "")
				.unbind("keypress").keypress(function(){
					return validator.isIPAddr(this, event);
				});

			var netmask_options = [
				{"text":"255.255.255.0 (253 <#Full_Clients#>)","value":"255.255.255.0"},
				{"text":"255.255.255.128 (125 <#Full_Clients#>)","value":"255.255.255.128"},
				{"text":"255.255.255.192 (61 <#Full_Clients#>)","value":"255.255.255.192"},
				{"text":"255.255.255.224 (29 <#Full_Clients#>)","value":"255.255.255.224"},
				{"text":"255.255.255.240 (13 <#Full_Clients#>)","value":"255.255.255.240"},
			];
			var netmask_parm = {"title": "<#IPConnection_x_ExternalSubnetMask_itemname#>", "id": "netmask", "options": netmask_options, "set_value": "255.255.255.0", "openHint":"4_2"};
			Get_Component_Custom_Select(netmask_parm).appendTo($net_mode_container);
		}

		var vid_parm = {"title":"<#WANVLANIDText#>", "type":"text", "id":"vlan_id", "need_check":true, "maxlength":4};//1-4094
		if(is_Web_iframe){
			vid_parm["btn_text"] = "<#CTL_assign#>";
			Get_Component_Input_And_Btn(vid_parm).appendTo($net_mode_container)
				.find("#" + vid_parm.id + ", .profile_btn_container").each(function(){
					if($(this).filter("#" + vid_parm.id + "").length){
						$(this).filter("#" + vid_parm.id + "").unbind("keypress").keypress(function(){
							return validator.isNumber(this,event);
						})
					}
					if($(this).filter(".profile_btn_container").length){
						$(this).filter(".profile_btn_container").unbind("click").click(function(e){
							e = e || event;
							e.stopPropagation();
							top.location.href = "/Advanced_VLAN_Profile_Content.asp";
						});
					}
				});
		}
		else{
			Get_Component_Input(vid_parm).appendTo($net_mode_container)
				.find("#" + vid_parm.id + "")
				.unbind("keypress").keypress(function(){
					return validator.isNumber(this,event);
				});
		}

		if(isSwMode("rt")){
			var hide_ssid_parm = {"title":"<#WLANConfig11b_x_BlockBCSSID_itemname#>", "type":"switch", "id":"hide_ssid"};
			Get_Component_Switch(hide_ssid_parm).attr({"data-group":"wifi_settings"}).appendTo($net_mode_container);

			var dns_parm = {
				"title":"<#HSDPAConfig_DNSServers_itemname#>",
				"id":"dns",
				"openHint":"5_7",
				"text":"<#Setting_factorydefault_value#>",
				"container_id":"container_dns",
				"btn_text":"<#CTL_assign#>"
			}
			var $dns_obj = Get_Component_Pure_Text_And_Btn(dns_parm).appendTo($net_mode_container);
			$dns_obj.find("#" + dns_parm.id + "").attr({"data-dns1":"", "data-dns2":""});
			$dns_obj.find(".profile_btn_container").unbind("click").click(function(e){
				e = e || event;
				e.stopPropagation();
				$(".container").addClass("blur_effect");
				if($(".popup_container.popup_element").css("display") == "flex"){
					$(".popup_container.popup_element").addClass("blur_effect");
				}
				$(".popup_element_second").css("display", "flex");
				$(".popup_container.popup_element_second").empty();
				var dns_list = {"dns1":$content_container.find("#dns").attr("data-dns1"), "dns2":$content_container.find("#dns").attr("data-dns2")};
				$(".popup_container.popup_element_second").append(Get_Container_Assign_DNS(dns_list, callback_assign_dns));
				$(".popup_container.popup_element_second").addClass("fit_width");
				adjust_popup_container_top($(".popup_container.popup_element_second"), 100);
				resize_iframe_height();
				function callback_assign_dns(_dns_list){
					var $dns = $content_container.find("#dns");
					$dns.attr({"data-dns1":_dns_list.dns1, "data-dns2":_dns_list.dns2});
					if(_dns_list.dns1 == "" && _dns_list.dns2 == ""){
						$dns.html("<#Setting_factorydefault_value#>");
					}
					else{
						var dns_text = "";
						if(_dns_list.dns1 != ""){
							dns_text += _dns_list.dns1;
						}
						if(_dns_list.dns2 != ""){
							if(dns_text != "")
								dns_text += ", ";
							dns_text += _dns_list.dns2;
						}
						$dns.html(htmlEnDeCode.htmlEncode(dns_text));
					}
				}
			});

			var ap_iso_parm = {"title":"<#WLANConfig11b_x_IsolateAP_itemname#>", "type":"switch", "id":"ap_isolate", "openHint":"3_5"};
			Get_Component_Switch(ap_iso_parm).attr({"data-group":"wifi_settings"}).appendTo($net_mode_container);
		}
		return $net_mode_container;
	}
	function Get_Net_AiMesh_Container(){
		var $net_mode_container = $("<div>").attr({"data-slide_target":"category_aimesh_cntr"});

		var $AiMesh_List = $("<div>").attr({"data-container":"AiMesh_List"}).append(Get_AiMesh_List_Container()).appendTo($net_mode_container);
		$AiMesh_List.find(".node_content_container").click(function(e){
			e = e || event;
			e.stopPropagation();
			set_apply_btn_status($(this).closest(".profile_setting"));
		});

		return $net_mode_container;
	}
	function handle_category_container(_obj, _parm){
		$subtab_container_net.find(".category_slide_title").filter(":not(#" + _parm.id + ")").removeClass("expand");
		$subtab_container_net.find("[data-slide_target]").hide();
		$parent_cntr = $(_obj).parent(".category_slide_title").toggleClass("expand");
		if($parent_cntr.hasClass("expand")){
			$subtab_container_net.find("[data-slide_target='" + _parm.slide_target + "']").show();
		}
		resize_iframe_height();
	}
}
function Update_Setting_Profile(_obj, _profile_data){
	var support_portal = (function(){
		if(isSwMode("rt")){
			if(_profile_data.sdn_rl.sdn_name == "Portal")
				return true;
			else if(_profile_data.sdn_rl.sdn_name == "Kids"){
				if(get_cp_type_support("4")){
					return true;
				}
			}
		}
		return false;
	})();
	if(support_portal){
		if(_profile_data.sdn_rl.sdn_name == "Portal"){
			$(_obj).find("[data-portal-type=4], #select_portal_type > div[value=4]").remove();
		}
		else if(_profile_data.sdn_rl.sdn_name == "Kids"){
			$(_obj).find("[subtab=portal]").html("Kid's Portal");/* untranslated */
			$(_obj).find("[data-portal-type=2], #select_portal_type > div[value=2]").remove();
		}
	}
	else{
		$(_obj).find("[subtab=portal], [subtab_container=portal]").remove();
	}
	$(_obj).find("[data-container=profile_title]").html("<#DSL_profile#>");
	var specific_wizard_type = wizard_type_text.filter(function(item, index, array){
		return (item.type == _profile_data.sdn_rl.sdn_name);
	})[0];
	if(specific_wizard_type != undefined){
		$(_obj).find("[data-container=profile_title]").html(htmlEnDeCode.htmlEncode(specific_wizard_type.text));
	}
	$(_obj).find(".setting_content_container, .profile_setting").attr("sdn_idx", _profile_data.sdn_rl.idx);
	$(_obj).find("#sdn_name").val(_profile_data.apg_rl.ssid);
	var apg_sec_array = (_profile_data.apg_rl.security).split("<");
	var cap_wifi_auth = (_profile_data.sdn_rl.sdn_name == "Portal" || _profile_data.sdn_rl.sdn_name == "Guest") ? "open" : "psk2";
	var cap_wifi_pwd = "";
	if(apg_sec_array[1] != undefined && apg_sec_array[1] != ""){
		var cap_sec_array = apg_sec_array[1].split(">");
		cap_wifi_auth = cap_sec_array[1];
		cap_wifi_pwd = cap_sec_array[3];
	}
	$(_obj).find("#sdn_pwd").val(cap_wifi_pwd);

	var wifi_band_value = "0";
	var apg_dut_list_array = (_profile_data.apg_rl.dut_list).split("<");
	$.each(apg_dut_list_array, function(index, dut_info){
		if(dut_info != ""){
			var dut_info_array = dut_info.split(">");
			if(dut_info_array[1] > "0"){
				if(dut_info_array[1] == "1")
					wifi_band_value = "1";
				else if(dut_info_array[1] == "2" || dut_info_array[1] == "4")
					wifi_band_value = "2";
				else if(dut_info_array[1] == "3" || dut_info_array[1] == "5")
					wifi_band_value = "3";
				return false;
			}
		}
	});
	set_value_Custom_Select(_obj, "wifi_band", wifi_band_value);
	update_aimesh_wifi_band_info(_profile_data);
	update_aimesh_wifi_band_full();

	if(_profile_data.sdn_rl.sdn_name == "Guest"){
		var wifi_auth = "psk2";
		if(cap_wifi_auth == "open"){
			$(_obj).find("#security_guest .switch_text_container").children("[data-option-id=open]").click();
			wifi_auth = "psk2";
		}
		else{
			$(_obj).find("#security_guest .switch_text_container").children("[data-option-id=pwd]").click();
			wifi_auth = cap_wifi_auth;
		}
		set_value_Custom_Select(_obj, "wifi_auth", wifi_auth);
	}
	else{
		$(_obj).find("#security_guest").remove();
	}

	if(_profile_data.sdn_rl.sdn_name == "Employee"){
		var wifi_auth = "psk2";
		var wifi_auth_radius = "wpa2";
		if(cap_wifi_auth == "wpa2" || cap_wifi_auth == "wpawpa2" ||
			cap_wifi_auth == "wpa3" || cap_wifi_auth == "wpa2wpa3" || cap_wifi_auth == "suite-b"
		){
			$(_obj).find("#radius_ipaddr").val(_profile_data.radius_rl.auth_server_1);
			$(_obj).find("#radius_port").val(_profile_data.radius_rl.auth_port_1);
			$(_obj).find("#radius_key").val(_profile_data.radius_rl.auth_key_1);
			$(_obj).find("#security_employee .switch_text_container").children("[data-option-id=radius]").click();
			wifi_auth = "psk2";
			wifi_auth_radius = cap_wifi_auth;
		}
		else{
			$(_obj).find("#security_employee .switch_text_container").children("[data-option-id=pwd]").click();
			wifi_auth = cap_wifi_auth;
			wifi_auth_radius = "wpa2";
		}
		set_value_Custom_Select(_obj, "wifi_auth", wifi_auth);
		set_value_Custom_Select(_obj, "wifi_auth_radius", wifi_auth_radius);
	}
	else{
		$(_obj).find("#security_employee, [data-sec-option-id=radius]").remove();
	}

	if(_profile_data.sdn_rl.sdn_name != "Guest" && _profile_data.sdn_rl.sdn_name != "Employee"){
		set_value_Custom_Select(_obj, "wifi_auth", cap_wifi_auth);
	}

	if(_profile_data.sdn_rl.sdn_name == "Portal"){
		$(_obj).find("[data-sec-option-id=pwd]").remove();
	}

	var is_radius_server = (cap_wifi_auth == "wpa2" || cap_wifi_auth == "wpawpa2" || cap_wifi_auth == "wpa3" || cap_wifi_auth == "wpa2wpa3" || cap_wifi_auth == "suite-b");
	var is_eth_only = (wifi_band_value == "0") ? true : false;
	var is_wifi_off = (_profile_data.sdn_rl.wifi_sched_on == "0") ? true : false;
	if(is_radius_server || is_eth_only || is_wifi_off){
		$(_obj).find("#qrcode_btn").remove();
		$(_obj).find("#qrcode_print_btn").closest(".profile_setting_item").remove();
	}
	else{
		var qrstring = "WIFI:";
		qrstring += "S:" + encode_utf8(escape_string(_profile_data.apg_rl.ssid)) + ";";
		qrstring += "T:" + ((cap_wifi_auth == "open") ? "nopass" : "WPA") + ";";
		qrstring += "P:" + ((cap_wifi_auth == "open") ? "" : escape_string(cap_wifi_pwd)) + ";";
		if(_profile_data.apg_rl.hide_ssid == "1"){
			qrstring += "H:true;"
		}
		qrstring += ';';
		$(_obj).find("#qrcode_btn").unbind("click").click(function(e){
			e = e || event;
			e.stopPropagation();
			$(".container").addClass("blur_effect");
			if($(".popup_container.popup_element").css("display") == "flex"){
				$(".popup_container.popup_element").addClass("blur_effect");
			}
			$(".popup_element_second").css("display", "flex");
			$(".popup_container.popup_element_second").empty();
			$(".popup_container.popup_element_second").append(Get_Component_QRCode({"text": qrstring}));
			adjust_popup_container_top($(".popup_container.popup_element_second"), 100);

			function Get_Component_QRCode(_parm){
				var text = "https://www.asus.com/";
				if(_parm != undefined){
					if(_parm.text != undefined){
						text = _parm.text;
					}
				}
				var $container = $("<div>");
				var $popup_title_container = $("<div>").addClass("popup_title_container").appendTo($container);
				$("<div>").addClass("title").html("<#WiFi_Share#>").appendTo($popup_title_container);
				$("<div>").addClass("vpn_icon_all_collect close_btn").unbind("click").click(function(e){
					e = e || event;
					e.stopPropagation();
					close_popup_container($container);
				}).appendTo($popup_title_container);
				var $popup_content_container = $("<div>").addClass("popup_content_container").appendTo($container)
				var $wifi_share_cntr = $("<div>").addClass("wifi_share_container").appendTo($popup_content_container);
				var $qrcode_cntr = $("<div>").addClass("qrcode_container").appendTo($wifi_share_cntr);
				$("<div>").addClass("qr_code")
					.qrcode({text: text}).appendTo($qrcode_cntr)
					.find("canvas").css({"width":"100%", "height":"100%"});
				$("<div>").addClass("qr_text").html("<#Scan_QR_Code_To_Connect#>").appendTo($qrcode_cntr);

				var download_qrcode = {"support":false, "mode":""};
				if(isMobile()){
					if(typeof appWrapper != "undefined"){//APP RWD
						if(appWrapper === true){
							if(typeof app_action_support == "object"){
								download_qrcode.support = (app_action_support.download_qrcode == "1") ? true : false;
								download_qrcode.mode = ((download_qrcode.support) ? "postToApp" : "");
							}
						}
					}
				}
				else{
					download_qrcode.support = true;
					download_qrcode.mode = "download";
				}
				if(download_qrcode.support){
					var $export_btn_cntr = $("<div>").addClass("export_btn_container").appendTo($wifi_share_cntr);
					var $export_btn = $("<div>").addClass("export_btn").unbind("click").click(function(e){
						e = e || event;
						e.stopPropagation();
						var qrcode_image = "";
						var canvas = $qrcode_cntr.find("canvas");
						if(canvas.length == 1) {
							qrcode_image = canvas[0].toDataURL("image/png");
						}
						if(download_qrcode.mode == "download"){
							if(qrcode_image != ""){
								downloadURI("data:" + qrcode_image, "SSID_QRCode.png");
							}
							else{
								alert("<#weekSche_format_incorrect#>");
							}
						}
						else if(download_qrcode.mode == "postToApp"){
							postMessageToApp(
								{
									"app_action": "download_qrcode",
									"qrcode_text": qrstring,
									"qrcode_image": qrcode_image
								}
							);
						}

						function downloadURI(uri, name) {
							var link = document.createElement("a");
							link.download = name;
							link.href = uri;
							link.click();
						}
					}).appendTo($export_btn_cntr);
					$("<div>").addClass("btn_text").html(htmlEnDeCode.htmlEncode("QR Code")).appendTo($export_btn);/* untranslated */
				}
				return $container;
			}
		});

		if(!isMobile()){
			var is_portal_type = false;
			if(!($.isEmptyObject(_profile_data.cp_rl))){
				if(_profile_data.cp_rl.cp_idx == "2"){
					is_portal_type = true;
				}
			}
			if(is_portal_type){
				$(_obj).find("#qrcode_print_btn").unbind("click").click(function(e){
					e = e || event;
					e.stopPropagation();
					var $canvas = $("<div>").qrcode({text: qrstring}).find("canvas").css({"width":"100%", "height":"100%"});
					var canvas = $canvas;
					if(canvas.length == 1) {
						var data = canvas[0].toDataURL("image/png");
						var args = {
							"qrcode_title": $(_obj).find("#FWF_brand_name").val(),
							"qrcode": data,
							"ssid": _profile_data.apg_rl.ssid,
							"pwd": (cap_wifi_auth != "open") ? cap_wifi_pwd : ""
						};
						if(args.qrcode_title != ""){
							$("#print_html #print_wifi_info_desc").html(args.qrcode_title);
						}
						$("#print_html #print_qrcode_img").css("background-image", "url(" + args.qrcode + ")");
						$("#print_html #print_wifi_ssid").html(args.ssid);
						if(args.pwd != ""){
							$("#print_html #print_wifi_pwd_cntr").show().find("#print_wifi_pwd").html(args.pwd);
							$("#print_html [data-group=wifi_manually_desc]").hide().filter("[data-component=wifi_pwd]").show();
						}
						else{
							$("#print_html #print_wifi_pwd_cntr").hide()
							$("#print_html [data-group=wifi_manually_desc]").hide().filter("[data-component=wifi_open]").show();
						}
						$("#print_html #print_wifi_info_desc").html(args.title);

						var printWin = window.open('', '_blank', '');
						var $html_cntr = $("<html>");
						$("<meta>").attr({"charset":"UTF-8"}).appendTo($html_cntr);
						$("<meta>").attr({"name":"viewport", "content":"width=device-width, initial-scale=1.0, user-scalable=no"}).appendTo($html_cntr);
						$("<meta>").attr({"http-equiv":"X-UA-Compatible", "content":"ie=edge"}).appendTo($html_cntr);
						var $head_cntr = $("<head>").appendTo($html_cntr);
						$("<title>").html("Print").appendTo($head_cntr);
						$head_cntr.append($("#print_css").html());
						var $body_cntr = $("<body>").addClass("print_body").appendTo($html_cntr);
						$body_cntr.append($("#print_html").html());
						printWin.document.write($html_cntr[0].outerHTML);
						setTimeout(function(){
							printWin.print();
							printWin.close();
						}, 50);
					}
				});
			}
			else{
				$(_obj).find("#qrcode_print_btn").closest(".profile_setting_item").remove();
			}
		}

		function escape_string(_str){
			_str = _str.replace(/\\/g, "\\\\");
			_str = _str.replace(/\"/g, "\\\"");
			_str = _str.replace(/;/g, "\\;");
			_str = _str.replace(/:/g, "\\:");
			_str = _str.replace(/,/g, "\\,");
			return _str;
		}
		function encode_utf8(s){
			return unescape(encodeURIComponent(s));
		}
	}

	var support_bw = (function(){
		return (isSwMode("rt") && _profile_data.sdn_rl.sdn_name != "IoT") ? true : false;
	})();
	if(support_bw){
		var bw_limit = _profile_data.apg_rl.bw_limit;
		$(_obj).find("#container_bw_dl, #container_bw_ul, #container_bw").hide();
		$(_obj).find("#bw_enabled").removeClass("off on").addClass((function(){
			var bw_enabled = bw_limit.substr(1,1);
			if(bw_enabled == "1"){
				var bw_list = bw_limit.substr(3).split(">");
				$(_obj).find("#bw_dl").val((bw_list[1]/1024));
				$(_obj).find("#bw_ul").val((bw_list[0]/1024));
				$(_obj).find("#container_bw_dl, #container_bw_ul,  #container_bw").show();
				return "on";
			}
			else{
				return "off";
			}
		})());
	}
	else{
		$(_obj).find("#bw_enabled").closest(".profile_setting_item").remove();
		$(_obj).find("#container_bw").remove();
	}

	var support_access_intranet = (function(){
		return (isSwMode("rt") && _profile_data.sdn_rl.sdn_name != "IoT") ? true : false;
	})();
	if(support_access_intranet){
		$(_obj).find("#access_intranet").removeClass("off on").addClass((function(){
			var result = "off";
			if(!($.isEmptyObject(_profile_data.sdn_access_rl))){
				var specific_data = _profile_data.sdn_access_rl.filter(function(item, index, array){
					return (item.sdn_idx == _profile_data.sdn_rl.idx && item.access_sdn_idx == "0");
				})[0];
				if(specific_data != undefined){
					result = "on";
				}
			}
			return result;
		})());
	}
	else{
		$(_obj).find("#access_intranet").closest(".profile_setting_item").remove();
	}

	$(_obj).find("#ap_isolate").removeClass("off on").addClass((function(){
		return ((_profile_data.apg_rl.ap_isolate == "1") ? "on" : "off");
	})());

	$(_obj).find("#schedule").removeClass("off on").addClass((function(){
		var schedule_config = {data:schedule_handle_data.string_to_json_array(_profile_data.apg_rl.sched)};
		if(_profile_data.sdn_rl.sdn_name == "Guest"){
			schedule_config["AccTime_support"] = true;
			schedule_config["AccTime_data"] = accesstime_handle_data.string_to_json_array(_profile_data.apg_rl.expiretime);
			schedule_config["mode"] = parseInt(_profile_data.apg_rl.timesched);
			schedule_config["AccTime_quickset_callback"] = resize_iframe_height;
			schedule_config["show_timeset_viewport_callback"] = resize_iframe_height;
			schedule_config["icon_trash_callback"] = resize_iframe_height;
			schedule_config["btn_save_callback"] = resize_iframe_height;
		}
		sdn_schedule = new schedule(schedule_config);
		$(_obj).find("#container_schedule").append(sdn_schedule.Get_UI());
		if(parseInt(_profile_data.apg_rl.timesched) > 0){
			$(_obj).find("#container_schedule").show();
			return "on";
		}
		else{
			$(_obj).find("#container_schedule").hide();
			return "off";
		}
	})());

	if(!($.isEmptyObject(_profile_data.subnet_rl))){
		var dns_arr = _profile_data.subnet_rl.dns.split(",");
		var dns_list = {"dns1":dns_arr[0], "dns2":dns_arr[1]};
		$(_obj).find("#dns").attr({"data-dns1":dns_list.dns1, "data-dns2":dns_list.dns2});
		var dns_text = "";
		if(dns_list.dns1 != ""){
			dns_text += dns_list.dns1;
		}
		if(dns_list.dns2 != ""){
			if(dns_text != "")
				dns_text += ", ";
			dns_text += dns_list.dns2;
		}
		if(dns_text != "")
			$(_obj).find("#dns").html(htmlEnDeCode.htmlEncode(dns_text));

		$(_obj).find("#dhcp_enable").removeClass("off on").addClass((function(){
			return ((_profile_data.subnet_rl.dhcp_enable == "1") ? "on" : "off");
		})());
		$(_obj).find("#ipaddr").val(_profile_data.subnet_rl.addr);
		set_value_Custom_Select(_obj, "netmask", _profile_data.subnet_rl.netmask);
	}
	else{
		$(_obj).find("#dhcp_enable").closest(".profile_setting_item").remove();
		$(_obj).find("#ipaddr").closest(".profile_setting_item").remove();
		$(_obj).find("#netmask").closest(".profile_setting_item").remove();
		$(_obj).find("#container_dns").remove();
	}
	if(!($.isEmptyObject(_profile_data.vlan_rl))){
		$(_obj).find("#vlan_id").val(_profile_data.vlan_rl.vid);
	}
	else{
		$(_obj).find("#vlan_id").closest(".profile_setting_item, .profile_setting_two_item").remove();
	}
	$(_obj).find("#hide_ssid").removeClass("off on").addClass((function(){
		return ((_profile_data.apg_rl.hide_ssid == "1") ? "on" : "off");
	})());

	var vpn_type = "";
	var vpn_idx = "";
	if(_profile_data.sdn_rl.vpnc_idx > "0"){
		vpn_type = "vpnc";
		vpn_idx = _profile_data.sdn_rl.vpnc_idx;
	}
	else if(_profile_data.sdn_rl.vpns_idx > "0"){
		vpn_type = "vpns";
		vpn_idx = _profile_data.sdn_rl.vpns_idx;
	}
	if($(_obj).find("[data-vpn-type="+vpn_type+"][data-idx="+vpn_idx+"]").length == 0){
		vpn_type = "";
		vpn_idx = "";
	}
	$(_obj).find("[data-container=VPN_Profiles]").hide();
	$(_obj).find("#vpn_enabled").removeClass("off on").addClass((function(){
		if(vpn_type == "")
			return "off";
		else{
			$(_obj).find("[data-container=VPN_Profiles]").show()
				.find(".icon_radio").removeClass("clicked").filter("[data-vpn-type="+vpn_type+"][data-idx="+vpn_idx+"]").addClass("clicked");
			return "on";
		}
	})());

	$.each(vpnc_profile_list, function(index, item){
		if(item.activate == "0"){
			$(_obj).find("[data-container=VPN_Profiles] [data-vpn-type=vpnc][data-idx="+item.vpnc_idx+"]").siblings("[data-component=icon_warning]").show();
		}
	});

	$.each(vpns_rl_json, function(index, item){
		if(item.activate == "0"){
			$(_obj).find("[data-container=VPN_Profiles] [data-vpn-type=vpns][data-idx="+item.vpns_idx+"]")
				.siblings("[data-component=icon_warning]").show();
		}
		if(item.sdn_idx != "" && item.vpns_idx != vpn_idx){
			$(_obj).find("[data-container=VPN_Profiles] [data-vpn-type=vpns][data-idx="+item.vpns_idx+"]").addClass("disabled").attr({"data-disabled":"true"})
				.siblings("[data-component=icon_error]").show();
		}
	});

	if(support_portal){
		set_value_Custom_Select(_obj, "portal_type", _profile_data.sdn_rl.cp_idx);
		var $select_portal_type = $(_obj).find("#select_portal_type");
		$.each(cp_type_rl_json, function(index, item){
			if(item.sdn_idx != "" && item.sdn_idx != _profile_data.sdn_rl.idx){
				$select_portal_type.children("[value='"+item.cp_idx+"']")
					.addClass("disabled")
					.attr({"data-disabled":"true"})
					.html(htmlEnDeCode.htmlEncode(item.cp_text + " (<#Status_Used#>)"))
			}
		});
		$select_portal_type.children("div").click(function(e){
			e = e || event;
			e.stopPropagation();
			var options = $(this).attr("value");
			if(options > "0"){
				var specific_cp_type_rl = cp_type_rl_json.filter(function(item, index, array){
					return (item.cp_idx == options);
				})[0];
				if(specific_cp_type_rl != undefined){
					if(specific_cp_type_rl.profile[0] != undefined){
						update_freewifi_settings(specific_cp_type_rl.profile[0]);
					}
				}
			}
		});
		if(_profile_data.sdn_rl.sdn_enable == "1"){
			if(_profile_data.sdn_rl.cp_idx == "0"){
				$.each(sdn_all_rl_json, function(index, item) {
					if(item.sdn_rl.idx == _profile_data.sdn_rl.idx)
						return true;
					if(item.sdn_rl.cp_idx != "0"){
						if(item.sdn_rl.sdn_enable == "1"){
							$select_portal_type
								.closest(".custom_select_container").attr("temp_disable", "disabled")
								.closest(".profile_setting_item")
								.after($("<div>").html("<#vpnc_conn_maxi_general#>".replace(/VPN/g, "<#GuestNetwork_Portal_type#>").replace(/2/g, "1")).addClass("item_hint"));
							return false;
						}
					}
				});
			}
		}

		function update_freewifi_settings(_cp_rl_data){
			if(_cp_rl_data.cp_idx == "2"){
				$(_obj).find("#FWF_conntimeout").val((parseInt(_cp_rl_data.conntimeout))/60);
				$(_obj).find("#FWF_redirecturl").val(_cp_rl_data.redirecturl);
				var response = httpApi.hookGet("get_customized_attribute-" + _cp_rl_data.idx_for_customized_ui + "");
				if(response != undefined && response != "NoData"){
					var settings = response.splash_page_setting;
					$(_obj).find("#FWF_brand_name").val(encode_decode_text(settings.brand_name, "decode"));
					$(_obj).find("#FWF_ui_container [data-component=FWF_brand_name]").html(htmlEnDeCode.htmlEncode($(_obj).find("#FWF_brand_name").val()));
					$(_obj).find("#FWF_ui_container [data-component=FWF_bg]").css('background-image', 'url(' + settings.image + ')');
					var terms_service_content = encode_decode_text(settings.terms_service_content, "decode");
					$(_obj).find("#FWF_termsservice").val(terms_service_content);
					terms_service_content = htmlEnDeCode.htmlEncode(terms_service_content).replace().replace(/(?:\r\n|\r|\n)/g, '<div style=height:6px;></div>');
					$(_obj).find("#FWF_ui_container [data-component=FWF_terms_service]").html(terms_service_content);
				}
				$(_obj).find("#FWF_passcode_enabled").removeClass("off on").addClass((function(){
					if(_cp_rl_data.auth_type == "1"){
						$(_obj).find("#container_FWF_passcode, #FWF_Preview_Cntr [data-component=FWF_passcode]").show();
						return "on";
					}
					else{
						$(_obj).find("#container_FWF_passcode, #FWF_Preview_Cntr [data-component=FWF_passcode]").hide();
						return "off";
					}
				})());
				$(_obj).find("#FWF_passcode").val(_cp_rl_data.local_auth_profile);
			}
			else if(_cp_rl_data.cp_idx == "4"){
				var response = httpApi.hookGet("get_customized_attribute-" + _cp_rl_data.idx_for_customized_ui + "");
				if(response != undefined && response != "NoData"){
					var settings = response.splash_page_setting;
					$(_obj).find("#MB_desc").val(encode_decode_text(settings.MB_desc, "decode"));
					$(_obj).find("#MB_ui_container [data-component=MB_desc]").html(htmlEnDeCode.htmlEncode($(_obj).find("#MB_desc").val()));
					$(_obj).find("#MB_ui_container [data-component=MB_bg]").css('background-image', 'url(' + settings.image + ')');
				}
			}
			set_apply_btn_status($(_obj).find(".profile_setting"));
		}

		$(_obj).find("[data-portal-type]").hide(0, function(){
			$(this).find(".profile_setting_item").hide();
		}).filter("[data-portal-type="+_profile_data.sdn_rl.cp_idx+"]").show(0, function(){
			$(this).find(".profile_setting_item").show();
		});
		if(_profile_data.sdn_rl.cp_idx == "2"){
			$(_obj).find("#FWF_brand_name").val(_profile_data.apg_rl.ssid);
			$(_obj).find("#FWF_ui_container [data-component=FWF_brand_name]").html(htmlEnDeCode.htmlEncode(_profile_data.apg_rl.ssid));
			if(!($.isEmptyObject(_profile_data.cp_rl))){
				if(_profile_data.cp_rl.cp_idx == "2"){
					update_freewifi_settings(_profile_data.cp_rl);
				}
			}
		}
		else if(_profile_data.sdn_rl.cp_idx == "4"){
			$(_obj).find("#MB_desc").val("<#Description#>");
			if(!($.isEmptyObject(_profile_data.cp_rl))){
				if(_profile_data.cp_rl.cp_idx == "4"){
					update_freewifi_settings(_profile_data.cp_rl);
				}
			}
		}
	}

	var $AiMesh_List = $(_obj).find("[data-container=AiMesh_List]");
	var $sel_wifi_band = $(_obj).find("#select_wifi_band");
	$sel_wifi_band.closest(".custom_select_container").click();
	$sel_wifi_band.find("div.selected").click();
	Set_AiMesh_List_CB($AiMesh_List, $sel_wifi_band.children(".selected").attr("value"));
	$AiMesh_List.find(".icon_checkbox").removeClass("clicked");
	$.each(apg_dut_list_array, function(index, dut_info){
		if(dut_info != ""){
			var dut_info_array = dut_info.split(">");
			var node_mac = dut_info_array[0];
			var wifi_band = dut_info_array[1];
			var specific_node = cfg_clientlist.filter(function(item, index, array){
				return (item.mac == node_mac);
			})[0];
			var node_online = false;
			if(specific_node != undefined){
				node_online = (specific_node.online == "1") ? true : false;
			}
			var $node_container = $AiMesh_List.find("[data-node-mac='" + node_mac + "']");
			if($node_container.length > 0){
				if(wifi_band > "0"){
					if(aimesh_wifi_band_info[node_mac] != undefined && aimesh_wifi_band_info[node_mac].length > 0){
						$node_container.attr({"data-profile-setting":"true"}).find(".icon_checkbox").toggleClass("clicked").removeClass("closed");
					}
				}
			}
			else{
				Get_AiMesh_Offline_Container(dut_info_array).appendTo($AiMesh_List);
				$AiMesh_List.find("[data-node-mac='" + node_mac + "'] .icon_checkbox").toggleClass("clicked");
			}
		}
	});

	set_apply_btn_status($(_obj).find(".profile_setting"));

	$(_obj).find("#title_del_btn, #action_container #del_btn, [data-container=action_container] > [data-component=del_btn]").click(function(){
		$(".container").addClass("blur_effect");
		if($(".popup_container.popup_element").css("display") == "flex"){
			$(".popup_container.popup_element").addClass("blur_effect");
		}
		$(".popup_element_second").css("display", "flex");
		$(".popup_container.popup_element_second").empty();
		Get_Component_Del_Profile().appendTo($(".popup_container.popup_element_second")).find("[data-btn=del]").click(function(e){
			e = e || event;
			e.stopPropagation();
			var del_idx = "";
			$.each(sdn_all_rl_json, function(index, item) {
				if(item.sdn_rl.idx == _profile_data.sdn_rl.idx){
					del_idx = index;
					return false;
				}
			});

			if(del_idx !== ""){
				selected_sdn_idx = "";
				var sdn_all_rl_tmp = JSON.parse(JSON.stringify(sdn_all_rl_json));
				var del_sdn_rl = sdn_all_rl_tmp.splice(del_idx, 1);
				var del_sdn_all_rl = parse_JSONToStr_del_sdn_all_rl(del_sdn_rl);
				vlan_rl_json = vlan_rl_json.filter(function(item, index, array){
					return (item.vlan_idx != del_sdn_rl[0].vlan_rl.vlan_idx);
				});
				var vlan_rl_tmp = JSON.parse(JSON.stringify(vlan_rl_json));
				var sdn_all_list = parse_JSONToStr_sdn_all_list({"sdn_all_rl":sdn_all_rl_tmp, "vlan_rl":vlan_rl_tmp});

				//check apgX_dut_list with port binding
				//console.log( _profile_data.apg_rl.dut_list);
				var do_restart_net_and_phy = false;	// do restart_net_and_phy or not
				if(_profile_data.apg_rl.dut_list != undefined){
					var apgX_dut_list_arr = _profile_data.apg_rl.dut_list.split("<");

					$.each(apgX_dut_list_arr, function(index, dut_info){
						if(dut_info != ""){
							var del_idx_dut_list_array_by_mac = dut_info.split(">");
							if(del_idx_dut_list_array_by_mac[2] != ""){	//port binding
								do_restart_net_and_phy = true;
							}
						}
					});
				}

				//check vlan_trunklist existed
				if( vlan_trunklist_orig != "" && _profile_data.vlan_rl.vid > 1){	//vid :2~4094
					do_restart_net_and_phy = true;
				}

				if( do_restart_net_and_phy ){
					var nvramSet_obj = {"action_mode": "apply", "rc_service": "restart_net_and_phy;"};
				}
				else{
					var nvramSet_obj = {"action_mode": "apply", "rc_service": "start_sdn_del;restart_wireless;"};
				}

				//update vlan_trunklist
				if( vlan_trunklist_orig != "" && _profile_data.vlan_rl.vid > 1){	//vid :2~4094
					var updated_vlan_trunklist = rm_vid_from_vlan_trunklist( _profile_data.vlan_rl.vid );
					nvramSet_obj.vlan_trunklist = updated_vlan_trunklist;
					if(vlan_rl_tmp==""){	//without vid for vlan_trunklist
						nvramSet_obj.vlan_trunklist = "";
					}
				}

				nvramSet_obj["apg" + del_sdn_rl[0].apg_rl.apg_idx + "_enable"] = "0";
				if(httpApi.nvramGet(["qos_enable"]).qos_enable == "1"){
					nvramSet_obj.rc_service += "restart_qos;restart_firewall;";
				}
				$.extend(nvramSet_obj, sdn_all_list);
				$.extend(nvramSet_obj, del_sdn_all_rl);
				close_popup_container($(this));
				var showLoading_status = get_showLoading_status(nvramSet_obj.rc_service);
				if(!httpApi.app_dataHandler){
					showLoading();
					close_popup_container("all");
					if(isWLclient()){
						showLoading(showLoading_status.time);
						setTimeout(function(){
							showWlHintContainer();
						}, showLoading_status.time*1000);
						check_isAlive_and_redirect({"page": "SDN.asp", "time": showLoading_status.time});
					}
				}
				httpApi.nvramSet(nvramSet_obj, function(){
					if(isWLclient()) return;
					showLoading(showLoading_status.time);
					init_sdn_all_list();
					show_sdn_profilelist();
					setTimeout(function(){
						if(!window.matchMedia('(max-width: 575px)').matches){
							$("#profile_list_content .profile_item_container.addnew").click();
							if(sdn_all_rl_json.length <= 0){
								$(".popup_element").css("display", "flex");
								$(".container").addClass("blur_effect");
								$(".popup_container.popup_element").empty()
									.append(show_Get_Start("popup"))
									.addClass(function(){
										return "hide_title_cntr" + ((isMobile()) ? " full_width" : "");
									})
									.find(".popup_title_container").hide();
								resize_iframe_height();
							}
						}
						else{
							if(sdn_all_rl_json.length <= 0){
								$("#profile_list_content .profile_item_container.addnew").click();
								$(".popup_container.popup_element")
									.addClass("hide_title_cntr")
									.find(".popup_title_container").hide();
							}
						}
					}, showLoading_status.time*1000);
					if(!isMobile()){
						if(showLoading_status.disconnect){
							check_isAlive_and_redirect({"page": "SDN.asp", "time": showLoading_status.time});
						}
					}
				});
			}
		});
		adjust_popup_container_top($(".popup_container.popup_element_second"), 100);
		resize_iframe_height();
	});

	var $clientlist_container = $(_obj).find("[data-container=client_info_container] [data-container=clientlist_container]");
	var $client_num = $(_obj).find("[data-container=client_info_container] #client_tab > .title");
	var sdn_clientlist_data = [];
	var get_clientlist = httpApi.hookGet("get_clientlist");
	$.each(get_clientlist, function(index, client){
		if(client.sdn_idx != undefined && client.sdn_idx == _profile_data.sdn_rl.idx){
			if(client.isOnline == "1"){
				sdn_clientlist_data.push(client);
			}
		}
	});
	$client_num.html("<#ConnectedClient#> : " + sdn_clientlist_data.length + " <#Clientlist_Online#>");
	Get_Component_ClientList(sdn_clientlist_data).appendTo($clientlist_container);
}
function set_apply_btn_status(_obj){
	var $btn_container_apply = $(_obj).find(".action_container .btn_container.apply");
	var isBlank = validate_isBlank($(_obj));
	if(isBlank){
		$btn_container_apply.removeClass("valid_fail").addClass("valid_fail").unbind("click");
	}
	else{
		$btn_container_apply.removeClass("valid_fail").unbind("click").click(function(e){
			e = e || event;
			e.stopPropagation();
			var sdn_idx = $(_obj).closest(".setting_content_container").attr("sdn_idx");
			if(validate_format_Wizard_Item($(_obj), "ALL")){
				var specific_data = sdn_all_rl_json.filter(function(item, index, array){
					return (item.sdn_rl.idx == sdn_idx);
				})[0];
				var sdn_profile = {};
				var isNewProfile = false;
				if(specific_data == undefined){
					var sdn_obj = get_new_sdn_profile();
					sdn_idx = Object.keys(sdn_obj);
					sdn_profile = sdn_obj[sdn_idx];
					sdn_profile.sdn_rl.sdn_name = "Customized";
					isNewProfile = true;
				}
				else{
					sdn_profile = specific_data;
				}

				selected_sdn_idx = sdn_profile.sdn_rl.idx;

				//check apgX_dut_list with port binding	
				var do_restart_net_and_phy = false;	// do restart_net_and_phy or not
				if(sdn_profile.apg_rl.dut_list != undefined){
					var update_idx_dut_list_array = sdn_profile.apg_rl.dut_list.split("<");

					$.each(update_idx_dut_list_array, function(index, dut_info){
						if(dut_info != ""){
							var update_idx_dut_list_array_by_mac = dut_info.split(">");
							if(update_idx_dut_list_array_by_mac[2] != ""){	//port binding
								do_restart_net_and_phy = true;
							}
						}
					});
				}

				//check vlan_trunklist with port binding
				if( vlan_trunklist_orig != "" && sdn_profile.vlan_rl.vid > 1 ){	//vid :2~4094
					$.each(vlan_trunklist_json_tmp, function(index, item){
						if(item.profile == sdn_profile.vlan_rl.vid){
							do_restart_net_and_phy = true;
						}
					});
				}

				if( do_restart_net_and_phy ){
					var nvramSet_obj = {"action_mode": "apply", "rc_service": "restart_net_and_phy;"};
				}
				else{
					var nvramSet_obj = {"action_mode": "apply", "rc_service": "restart_wireless;restart_sdn " + selected_sdn_idx + ";"};
				}

				var rc_append = "";
				var wifi_band = parseInt($(_obj).find("#select_wifi_band").children(".selected").attr("value"));
				sdn_profile.apg_rl.ssid = $(_obj).find("#sdn_name").val();
				var ori_dut_list_json = convertRulelistToJson(["mac","wifiband","lanport"], sdn_profile.apg_rl.dut_list);
				var dut_list = "";
				if(wifi_band > 0){
					var sec_option_id = "pwd";
					if(sdn_profile.sdn_rl.sdn_name == "Guest")
						sec_option_id = $(_obj).find("#security_guest .switch_text_container").children(".selected").attr("data-option-id");
					else if(sdn_profile.sdn_rl.sdn_name == "Employee")
						sec_option_id = $(_obj).find("#security_employee .switch_text_container").children(".selected").attr("data-option-id");
					else if(sdn_profile.sdn_rl.sdn_name == "Portal")
						sec_option_id = "open";

					var wifi_pwd = "";
					var wifi_auth = "psk2";
					var wifi_crypto = "aes";
					sdn_profile.radius_rl = {};
					if(sec_option_id == "open"){
						wifi_auth = "open";
						wifi_pwd = "";
					}
					else if(sec_option_id == "radius"){
						sdn_profile.radius_rl = get_new_radius_rl();
						sdn_profile.radius_rl.radius_idx = sdn_profile.apg_rl.apg_idx;
						sdn_profile.radius_rl.auth_server_1 = $(_obj).find("#radius_ipaddr").val();
						sdn_profile.radius_rl.auth_port_1 = $(_obj).find("#radius_port").val();
						sdn_profile.radius_rl.auth_key_1 = $(_obj).find("#radius_key").val();
						wifi_auth = $(_obj).find("#select_wifi_auth_radius").children(".selected").attr("value");
						if(wifi_auth == "suite-b")
							wifi_crypto = "suite-b";
					}
					else if(sec_option_id == "pwd"){
						wifi_auth = $(_obj).find("#select_wifi_auth").children(".selected").attr("value");
						wifi_pwd = $(_obj).find("#sdn_pwd").val();
					}
					var radius_idx = sdn_profile.apg_rl.apg_idx
					sdn_profile.apg_rl.security = "<3>" + wifi_auth + ">" + wifi_crypto + ">" + wifi_pwd + ">" + radius_idx + "";
					sdn_profile.apg_rl.security += "<13>" + wifi_auth + ">" + wifi_crypto + ">" + wifi_pwd + ">" + radius_idx + "";
					sdn_profile.apg_rl.security += "<16>sae>aes>" + wifi_pwd + ">" + radius_idx + "";

					sdn_profile.apg_rl.sched = schedule_handle_data.json_array_to_string(sdn_schedule.Get_Value());
					sdn_profile.apg_rl.expiretime = "";
					if($(_obj).find("#schedule").hasClass("on")){
						sdn_profile.apg_rl.timesched = sdn_schedule.Get_Value_Mode();
						if(sdn_profile.apg_rl.timesched == "1"){
							sdn_profile.apg_rl.timesched = ((sdn_profile.apg_rl.sched == "") ? "0" : "1");
						}
					}
					else{
						sdn_profile.apg_rl.timesched = "0";
					}
					if($(_obj).find("#bw_enabled").hasClass("on")){
						sdn_profile.apg_rl.bw_limit = "<1>" + ($(_obj).find("#bw_ul").val())*1024 + ">" + ($(_obj).find("#bw_dl").val())*1024;
						nvramSet_obj.qos_enable = "1";
						nvramSet_obj.qos_type = "2";
						rc_append += "restart_qos;restart_firewall;";
					}
					else{
						if(httpApi.nvramGet(["qos_enable"]).qos_enable == "1"){
							rc_append += "restart_qos;restart_firewall;";
						}
						sdn_profile.apg_rl.bw_limit = "<0>>";
					}
					if($(_obj).find("#ap_isolate").hasClass("on"))
						sdn_profile.apg_rl.ap_isolate = "1";
					else
						sdn_profile.apg_rl.ap_isolate = "0";

					if($(_obj).find("#hide_ssid").hasClass("on"))
						sdn_profile.apg_rl.hide_ssid = "1";
					else
						sdn_profile.apg_rl.hide_ssid = "0";

					$(_obj).find("[data-container=AiMesh_List]").find(".node_container").each(function(){
						if($(this).find(".icon_checkbox").hasClass("clicked")){
							var node_mac = $(this).attr("data-node-mac");
							var specific_node = cfg_clientlist.filter(function(item, index, array){
								return (item.mac == node_mac);
							})[0];
							if(specific_node != undefined){
								dut_list += get_specific_dut_list(wifi_band, specific_node, ori_dut_list_json);
							}
						}
					});
					var offline_dut_list = get_Offline_dut_list(_obj, wifi_band, ori_dut_list_json);
					if(offline_dut_list != ""){
						dut_list += offline_dut_list;
					}
					dut_list += get_unChecked_dut_list(_obj, ori_dut_list_json);
				}
				else{
					dut_list = get_dut_list(wifi_band, ori_dut_list_json);
				}
				sdn_profile.apg_rl.dut_list = dut_list;

				sdn_profile.sdn_access_rl = [];
				if($(_obj).find("#access_intranet").hasClass("on")){
					sdn_profile.sdn_access_rl.push({"access_sdn_idx": "0", "sdn_idx": sdn_profile.sdn_rl.idx});
				}

				sdn_profile.sdn_rl.vpnc_idx = "0";
				sdn_profile.sdn_rl.vpns_idx = "0";
				if($(_obj).find("#vpn_enabled").hasClass("on")){
					var $selected_vpn = $(_obj).find("[data-container=VPN_Profiles] .icon_radio.clicked");
					if($selected_vpn.length){
						var vpn_type = $selected_vpn.attr("data-vpn-type");
						var vpn_idx = $selected_vpn.attr("data-idx");
						if(vpn_type == "vpnc"){
							sdn_profile.sdn_rl.vpnc_idx = vpn_idx;
						}
						else if(vpn_type == "vpns"){
							sdn_profile.sdn_rl.vpns_idx = vpn_idx;
						}
					}
				}
				sdn_profile.sdn_rl.cp_idx = "0";
				var portal_type = $(_obj).find("#select_portal_type").children(".selected").attr("value");
				if(portal_type != undefined)
					sdn_profile.sdn_rl.cp_idx = portal_type;
				if(sdn_profile.sdn_rl.cp_idx == "2" || sdn_profile.sdn_rl.cp_idx == "4"){
					if($.isEmptyObject(sdn_profile.cp_rl)){
						var idx_for_customized_ui = "";
						sdn_profile.cp_rl = JSON.parse(JSON.stringify(new cp_profile_attr()));
						var specific_cp_type_rl = cp_type_rl_json.filter(function(item, index, array){
							return (item.cp_idx == sdn_profile.sdn_rl.cp_idx);
						})[0];
						if(specific_cp_type_rl != undefined){
							if(specific_cp_type_rl.profile[0] != undefined){
								idx_for_customized_ui = specific_cp_type_rl.profile[0].idx_for_customized_ui;
							}
						}
						if(idx_for_customized_ui == ""){
							idx_for_customized_ui = $.now();
						}
						sdn_profile.cp_rl.idx_for_customized_ui = idx_for_customized_ui;
					}
					sdn_profile.cp_rl.cp_idx = sdn_profile.sdn_rl.cp_idx;
					var cp_idx = sdn_profile.cp_rl.cp_idx;
					sdn_profile.cp_rl.enable = "1";
					sdn_profile.cp_rl.conntimeout = ((cp_idx == "2") ? (($(_obj).find("#FWF_conntimeout").val())*60).toString() : 60*60);
					sdn_profile.cp_rl.redirecturl = ((cp_idx == "2") ? $(_obj).find("#FWF_redirecturl").val() : "");
					sdn_profile.cp_rl.auth_type = ((cp_idx == "2") ? (($(_obj).find("#FWF_passcode_enabled").hasClass("on")) ? "1" : "0") : "0");
					sdn_profile.cp_rl.term_of_service = ((cp_idx == "2") ? "1" : "0");
					sdn_profile.cp_rl.NAS_ID = "";
					sdn_profile.cp_rl.local_auth_profile = ((cp_idx == "2") ? (($(_obj).find("#FWF_passcode_enabled").hasClass("on")) ? $(_obj).find("#FWF_passcode").val() : "") : "");
					var upload_data = {"id": sdn_profile.cp_rl.idx_for_customized_ui, "cp_idx": cp_idx, "auth_type": sdn_profile.cp_rl.auth_type};
					if(cp_idx == "2"){
						upload_data["brand_name"] = $("#FWF_brand_name").val();
						upload_data["image"] = $(_obj).find("#FWF_ui_container [data-component=FWF_bg]").css('background-image').replace('url(','').replace(')','').replace(/\"/gi, "");
						upload_data["terms_service"] = $("#FWF_termsservice").val();
					}
					else if(cp_idx == "4"){
						upload_data["MB_desc"] = $("#MB_desc").val();
						upload_data["image"] = $(_obj).find("#MB_ui_container [data-component=MB_bg]").css('background-image').replace('url(','').replace(')','').replace(/\"/gi, "");
					}
					uploadFreeWiFi(upload_data);
					rc_append += "restart_chilli;restart_uam_srv;";
				}
				if($(_obj).find("#container_dns").length > 0){
					sdn_profile.subnet_rl.dns = $(_obj).find("#dns").attr("data-dns1") + "," + $(_obj).find("#dns").attr("data-dns2");
				}

				if($(_obj).find("#dhcp_enable").length > 0){
					if($(_obj).find("#dhcp_enable").hasClass("on"))
						sdn_profile.subnet_rl.dhcp_enable = "1";
					else
						sdn_profile.subnet_rl.dhcp_enable = "0";
				}
				var $ipaddr = $(_obj).find("#ipaddr");
				if($ipaddr.length > 0){
					sdn_profile.subnet_rl.addr = $ipaddr.val();
					var ipaddr_substr = $ipaddr.val().substr(0, $ipaddr.val().lastIndexOf("."));
					sdn_profile.subnet_rl.dhcp_min = ipaddr_substr + "." + "2";
					sdn_profile.subnet_rl.dhcp_max = ipaddr_substr + "." + "254";
					sdn_profile.subnet_rl.netmask = $(_obj).find("#select_netmask").children(".selected").attr("value");
				}
				if($(_obj).find("#vlan_id").length > 0){
					
					//update vlan_trunklist
					if( sdn_profile.vlan_rl.vid > 1){	//vid :2~4094
						var updated_vlan_trunklist = update_vlan_trunklist( sdn_profile.vlan_rl, $(_obj).find("#vlan_id").val());
						nvramSet_obj.vlan_trunklist = updated_vlan_trunklist;
					}

					//update vlan_rl
					sdn_profile.vlan_rl.vid = $(_obj).find("#vlan_id").val();
				}

				if(rc_append != ""){
					nvramSet_obj.rc_service = nvramSet_obj.rc_service + rc_append;
				}
				var showLoading_status = get_showLoading_status(nvramSet_obj.rc_service);
				if(wifi_band > 0){
					if(sdn_profile.apg_rl.timesched == "2"){
						sdn_profile.apg_rl.expiretime = accesstime_handle_data.json_array_to_string(sdn_schedule.Get_Value_AccessTime(showLoading_status.time));
					}
				}
				if(isNewProfile)
					sdn_all_rl_json.push(JSON.parse(JSON.stringify(sdn_profile)));
				var sdn_all_list = parse_JSONToStr_sdn_all_list();
				$.extend(nvramSet_obj, sdn_all_list);
				var apgX_rl = parse_apg_rl_to_apgX_rl(sdn_profile.apg_rl);
				$.extend(nvramSet_obj, apgX_rl);
				if(sdn_profile.sdn_rl.cp_idx == "2" || sdn_profile.sdn_rl.cp_idx == "4"){
					var cpX_rl = parse_cp_rl_to_cpX_rl(sdn_profile.cp_rl);
					$.extend(nvramSet_obj, cpX_rl);
				}
				if(!httpApi.app_dataHandler){
					showLoading();
					close_popup_container("all");
					if(isWLclient()){
						showLoading(showLoading_status.time);
						setTimeout(function(){
							showWlHintContainer();
						}, showLoading_status.time*1000);
						check_isAlive_and_redirect({"page": "SDN.asp", "time": showLoading_status.time});
					}
				}
				httpApi.nvramSet(nvramSet_obj, function(){
					if(isWLclient()) return;
					showLoading(showLoading_status.time);
					setTimeout(function(){
						init_sdn_all_list();
						show_sdn_profilelist();
						if(!window.matchMedia('(max-width: 575px)').matches)
							$("#profile_list_content").find("[sdn_idx=" + selected_sdn_idx + "] .item_text_container").click();
					}, showLoading_status.time*1000);
					if(!isMobile()){
						if(showLoading_status.disconnect){
							check_isAlive_and_redirect({"page": "SDN.asp", "time": showLoading_status.time});
						}
					}
				});
			}
		});
	}
}
function validate_isBlank(_obj){
	var isBlank = false;
	$(_obj).find("[need_check=true]").each(function(index){
		if($(this).closest(".profile_setting_item").css("display") == "none")
			return true;

		var value = $(this).val().replace(/\s+/g, '');//remove space
		if(value == ""){
			isBlank = true;
			return false;
		}
	});

	if($(_obj).find("#select_wifi_band").children(".selected").attr("value") != "0"){
		if($(_obj).find("[data-container=AiMesh_List] .node_container .icon_checkbox").length > 0){
			if(!$(_obj).find("[data-container=AiMesh_List] .node_container .icon_checkbox:not(.disabled)").hasClass("clicked")){
				isBlank = true;
			}
		}
	}

	if(isBlank)
		return true;
	else
		return false;
}
function validate_format_Wizard_Item(_obj, _type){
	$(_obj).find(".validate_hint").remove();
	var valid_block_chars = function(str, keywordArray){
		var testResult = {
			'isError': false,
			'errReason': ''
		};

		// bolck ascii code 32~126 first
		var invalid_char = "";
		for(var i = 0; i < str.length; ++i){
			if(str.charCodeAt(i) < '32' || str.charCodeAt(i) > '126'){
				invalid_char += str.charAt(i);
			}
		}
		if(invalid_char != ""){
			testResult.isError = true;
			testResult.errReason = '<#JS_validstr2#>" '+ invalid_char +'" !';
			return testResult;
		}

		// check if char in the specified array
		if(str){
			for(var i=0; i<keywordArray.length; i++){
				if(str.indexOf(keywordArray[i]) >= 0){
					testResult.isError = true;
					testResult.errReason = keywordArray + " <#JS_invalid_chars#>";
					return testResult;
				}
			}
		}

		return testResult;
	};
	var valid_psk = function(str){
		var testResult = {
			'isError': false,
			'errReason': '',
			'set_value': ''
		};
		var psk_length = str.length;
		var psk_length_trim = str.trim().length;
		if(psk_length < 8){
			testResult.isError = true;
			testResult.errReason = "<#JS_passzero#>";
			testResult.set_value = "00000000";
			return testResult;
		}
		if(psk_length > 31){
			testResult.isError = true;
			testResult.errReason = "<#JS_PSK64Hex#>";
			return testResult;
		}
		if(psk_length != psk_length_trim){
			testResult.isError = true;
			testResult.errReason = stringSafeGet("<#JS_PSK64Hex_whiteSpace#>");
			return testResult;
		}
		if(psk_length == 64 && !check_is_hex(str)){
			testResult.isError = true;
			testResult.errReason = "<#JS_PSK64Hex#>";
			return testResult;
		}
		return testResult;
	};
	var valid_psk_KR = function(str){
		var testResult = {
			'isError': false,
			'errReason': ''
		};
		var psk_length = str.length;
		if(!/[A-Za-z]/.test(str) || !/[0-9]/.test(str) || psk_length < 8 || psk_length > 63 
				|| !/[\!\"\#\$\%\&\'\(\)\*\+\,\-\.\/\:\;\<\=\>\?\@\[\\\]\^\_\`\{\|\}\~]/.test(str)){
			testResult.isError = true;
			testResult.errReason = "<#JS_PSK64Hex_kr#> <#JS_validPWD#>";
			return testResult;
		}

		return testResult;
	};
	var check_is_hex = function(str){
		var re = new RegExp("[^a-fA-F0-9]+","gi");
		if(re.test(str))
			return false;
		else
			return true;
	};
	var valid_SSID = function(str){
		var testResult = {
			'isError': false,
			'errReason': '',
			'set_value': ''
		};
		var c;
		var ssid = str;

		len = validator.lengthInUtf8(ssid);
		if(len > 32){
			testResult.isError = true;
			testResult.errReason = "<#JS_max_ssid#>";
			return testResult;
		}

		for(var i = 0; i < len; ++i){
			c = ssid.charCodeAt(i);
			if(!isSupport("utf8_ssid")){
				if(validator.ssidChar(c)){
					testResult.isError = true;
					testResult.errReason = '<#JS_validSSID1#> ' + ssid.charAt(i) + ' <#JS_validSSID2#>';
					return testResult;
				}
			}
		}

		return testResult;
	};
	var valid_isLegalIP = function(str){
		var testResult = {
			'isError': false,
			'errReason': ''
		};
		var A_class_start = inet_network("1.0.0.0");
		var A_class_end = inet_network("126.255.255.255");
		var B_class_start = inet_network("127.0.0.0");
		var B_class_end = inet_network("127.255.255.255");
		var C_class_start = inet_network("128.0.0.0");
		var C_class_end = inet_network("255.255.255.255");
		var ip_num = inet_network(str);
		if(ip_num > A_class_start && ip_num < A_class_end){
			return testResult;
		}
		else if(ip_num > B_class_start && ip_num < B_class_end){
			testResult.isError = true;
			testResult.errReason = str + " <#JS_validip#>";
			return testResult;
		}
		else if(ip_num > C_class_start && ip_num < C_class_end){
			return testResult;
		}
		else{
			testResult.isError = true;
			testResult.errReason = str + " <#JS_validip#>";
			return testResult;
		}
	};
	var valid_IP_CIDR = function(addr, type, mode){
		//mode, 0:IP, 1:IP/CIDR, 2:IP or IP/CIDR
		var testResultPass = {
			'isError': false,
			'errReason': ''
		};
		var testResultFail = {
			'isError': true,
			'errReason': addr + " <#JS_validip#>"
		};
		var IP = new RegExp(ip_RegExp[type],"gi");
		var IP_CIDR = new RegExp(ip_RegExp[type + "_CIDR"], "gi");
		if(mode == "0"){
			if(IP.test(addr))
				return testResultPass;
			else{
				testResultFail.errReason = testResultFail.errReason + ", IP Address without CIDR."
				return testResultFail;
			}
		}
		else if(mode == "1"){
			if(IP_CIDR.test(addr))
				return testResultPass;
			else{
				testResultFail.errReason = testResultFail.errReason + ", IP Address/CIDR"
				return testResultFail;
			}
		}
		else if(mode == "2"){
			if(IP_CIDR.test(addr) || IP.test(addr))
				return testResultPass;
			else{
				testResultFail.errReason = testResultFail.errReason + ", IP Address without CIDR or IP Address/CIDR."
				return testResultFail;
			}
		}
		else
			return testResultFail;
	};
	var valid_is_IP_format = function(str, type){
		var testResultPass = {
			'isError': false,
			'errReason': ''
		};
		var testResultFail = {
			'isError': true,
			'errReason': str + " <#JS_validip#>"
		};
		var format = new RegExp(ip_RegExp[type], "gi");
		if(format.test(str))
			return testResultPass;
		else
			return testResultFail;
	};
	var valid_isLegalMask = function(str){
		var testResult = {
			'isError': false,
			'errReason': ''
		};
		var wrong_netmask = 0;
		var netmask_num = inet_network(str);
		var netmask_reverse_num = 0;
		var test_num = 0;
		if(netmask_num != -1) {
			if(netmask_num == 0) {
				netmask_reverse_num = 0; //Viz 2011.07 : Let netmask 0.0.0.0 pass
			}
			else {
				netmask_reverse_num = ~netmask_num;
			}

			if(netmask_num < 0) {
				wrong_netmask = 1;
			}

			test_num = netmask_reverse_num;
			while(test_num != 0){
				if((test_num + 1) % 2 == 0) {
					test_num = (test_num + 1) / 2 - 1;
				}
				else{
					wrong_netmask = 1;
					break;
				}
			}
			if(wrong_netmask == 1){
				testResult.isError = true;
				testResult.errReason = str + " is not a valid Mask address!";
				return testResult;
			}
			else {
				return testResult;
			}
		}
		else { //null
			testResult.isError = true;
			testResult.errReason = "This is not a valid Mask address!";
			return testResult;
		}
	};
	var valid_bandwidth = function(str){
		var testResult = {
			'isError': false,
			'errReason': ''
		};
		if(!$.isNumeric(str)){
			testResult.isError = true;
			testResult.errReason = "<#QoS_invalid_period#>";
			return testResult;
		}
		if(str.split(".").length > 2 || parseFloat(str) < 0.1){
			testResult.isError = true;
			testResult.errReason = "<#min_bound#> : 0.1 Mb/s";
			return testResult;
		}
		return testResult;
	};
	var validate_dhcp_range = function(str, ipaddr, netmask){
		var testResult = {
			'isError': false,
			'errReason': ''
		};
		var ip_num = inet_network(str);
		var subnet_head, subnet_end;
		if(ip_num <= 0){
			testResult.isError = true;
			testResult.errReason = str + " <#JS_validip#>";
			return testResult;
		}
		subnet_head = getSubnet(ipaddr, netmask, "head");
		subnet_end = getSubnet(ipaddr, netmask, "end");
		if(ip_num <= subnet_head || ip_num >= subnet_end){
			testResult.isError = true;
			testResult.errReason = str + " <#JS_validip#>";
			return testResult;
		}
		return testResult;
	};
	var valid_num_range = function(str, mini, maxi){
		var testResult = {
			'isError': true,
			'errReason': '<#JS_validrange#> ' + mini + ' <#JS_validrange_to#> ' + maxi
		};
		if(isNaN(str))
			return testResult;
		else{
			var input_num = parseInt(str);
			var mini_num = parseInt(mini);
			var maxi_num = parseInt(maxi);
			if(input_num < mini_num || input_num > maxi_num)
				return testResult;
			else{
				testResult.isError = false;
				testResult.errReason = "";
				return testResult;
			}
		}
	};
	var valid_url = function(str){
		var testResult = {
			'isError': false,
			'errReason': ''
		};

		var urlregex = new RegExp("^(http|https|ftp)\://([a-zA-Z0-9\.\-]+(\:[a-zA-Z0-9\.&amp;%\$\-]+)*@)*((25[0-5]|2[0-4][0-9]|[0-1]{1}[0-9]{2}|[1-9]{1}[0-9]{1}|[1-9])\.(25[0-5]|2[0-4][0-9]|[0-1]{1}[0-9]{2}|[1-9]{1}[0-9]{1}|[1-9]|0)\.(25[0-5]|2[0-4][0-9]|[0-1]{1}[0-9]{2}|[1-9]{1}[0-9]{1}|[1-9]|0)\.(25[0-5]|2[0-4][0-9]|[0-1]{1}[0-9]{2}|[1-9]{1}[0-9]{1}|[0-9])|([a-zA-Z0-9\-]+\.)*[a-zA-Z0-9\-]+\.(com|edu|gov|int|mil|net|org|biz|arpa|info|name|pro|aero|coop|museum|[a-zA-Z]{2}))(\:[0-9]+)*(/($|[a-zA-Z0-9\.\,\?\'\\\+&amp;%\$#\=~_\-]+))*$");
		if(urlregex.test(str)){
			return testResult;
		}
		else{
			testResult.isError = true;
			testResult.errReason = "<#feedback_format_alert#> ex. http or https ://www.asus.com";
			return testResult;
		}
	};
	var isSku = function(_ptn){
		var ttc = httpApi.nvramGet(["territory_code"]).territory_code;
		return (ttc.search(_ptn) == -1) ? false : true;
	}

	var sdn_idx = $(_obj).attr("sdn_idx");
	var $sdn_name = $(_obj).find("#sdn_name");
	if($sdn_name.val() == ""){
		$sdn_name.show_validate_hint("<#JS_fieldblank#>");
		$sdn_name.focus();
		return false;
	}
	var isValid_sdn_name = valid_SSID($sdn_name.val());
	if(isValid_sdn_name.isError){
		$sdn_name.show_validate_hint(isValid_sdn_name.errReason);
		$sdn_name.focus();
		return false;
	}
	var specific_sdn_name = "";
	if(sdn_idx != undefined){
		specific_sdn_name = sdn_all_rl_json.filter(function(item, index, array){
			return (item.sdn_rl.idx != sdn_idx && item.apg_rl.ssid == $sdn_name.val());
		});
	}
	else{
		specific_sdn_name = sdn_all_rl_json.filter(function(item, index, array){
			return (item.apg_rl.ssid == $sdn_name.val());
		});
	}
	if(specific_sdn_name.length > 0){
		$sdn_name.show_validate_hint("<#JS_duplicate#>");
		$sdn_name.focus();
		return false;
	}

	var $sdn_pwd = $(_obj).find("#sdn_pwd");
	var $sdn_pwd_cntr = $sdn_pwd.closest(".profile_setting_item");
	if($sdn_pwd_cntr.length > 0 && $sdn_pwd_cntr.css("display") != "none"){
		$sdn_pwd.val($sdn_pwd.val().replace(/\s+/g, ''));//remove space
		if($sdn_pwd.val() == ""){
			$sdn_pwd.show_validate_hint("<#JS_fieldblank#>");
			$sdn_pwd.focus();
			return false;
		}
		var isValid_pwd = valid_block_chars($sdn_pwd.val(), ["<", ">"]);
		if(isValid_pwd.isError){
			$sdn_pwd.show_validate_hint(isValid_pwd.errReason);
			$sdn_pwd.focus();
			return false;
		}
		if(isSku("KR")){
			isValid_pwd = valid_psk_KR($sdn_pwd.val());
			if(isValid_pwd.isError){
				$sdn_pwd.show_validate_hint(isValid_pwd.errReason);
				$sdn_pwd.focus();
				return false;
			}
		}
		else{
			isValid_pwd = valid_psk($sdn_pwd.val());
			if(isValid_pwd.isError){
				$sdn_pwd.show_validate_hint(isValid_pwd.errReason);
				$sdn_pwd.focus();
				if(isValid_pwd.set_value)
					$sdn_pwd.val(isValid_pwd.set_value);
				return false;
			}
		}
		//confirm common string combination	#JS_common_passwd#
		var is_common_string = check_common_string($sdn_pwd.val(), "wpa_key");
		if(is_common_string){
			if(!confirm("<#JS_common_passwd#>")){
				$sdn_pwd.focus();
				return false;
			}
		}
	}

	var $radius_ipaddr = $(_obj).find("#radius_ipaddr");
	if($radius_ipaddr.length != 0){
		if($radius_ipaddr.closest(".profile_setting_item").css("display") != "none"){
			$radius_ipaddr.val($radius_ipaddr.val().replace(/\s+/g, ''));//remove space
			if(isSwMode("rt") && isSupport("ipv6")){
				var isValid_ipv4 = valid_is_IP_format($radius_ipaddr.val(), "IPv4");
				var isValid_ipv6 = valid_is_IP_format($radius_ipaddr.val(), "IPv6");
				if(isValid_ipv4.isError && isValid_ipv6.isError){
					$radius_ipaddr.show_validate_hint(($radius_ipaddr.val() + " <#JS_validip#>"));
					$radius_ipaddr.focus();
					return false;
				}
			}
			else{
				var isValid_ipv4 = valid_is_IP_format($radius_ipaddr.val(), "IPv4");
				if(isValid_ipv4.isError){
					$radius_ipaddr.show_validate_hint(($radius_ipaddr.val() + " <#JS_validip#>"));
					$radius_ipaddr.focus();
					return false;
				}
			}
		}
	}

	var $radius_port = $(_obj).find("#radius_port");
	if($radius_port.length != 0){
		if($radius_port.closest(".profile_setting_item").css("display") != "none"){
			$radius_port.val($radius_port.val().replace(/\s+/g, ''));//remove space
			if($radius_port.val() == ""){
				$radius_port.show_validate_hint("<#JS_fieldblank#>");
				$radius_port.focus();
				return false;
			}
			var isValid_radius_port = valid_num_range($radius_port.val(), 0, 65535);
			if(isValid_radius_port.isError){
				$radius_port.show_validate_hint(isValid_radius_port.errReason);
				$radius_port.focus();
				return false;
			}
		}
	}

	var $radius_key = $(_obj).find("#radius_key");
	if($radius_key.length != 0){
		if($radius_key.closest(".profile_setting_item").css("display") != "none"){
			var isValid_radius_key = valid_block_chars($radius_key.val(), ["\""]);
			if(isValid_radius_key.isError){
				$radius_key.show_validate_hint(isValid_radius_key.errReason);
				$radius_key.focus();
				return false;
			}
		}
	}

	var $bw_enabled = $(_obj).find("#bw_enabled");
	if($bw_enabled.length != 0){
		if($bw_enabled.closest(".profile_setting_item").css("display") != "none"){
			var $bw_dl = $(_obj).find("#bw_dl");
			var $bw_ul = $(_obj).find("#bw_ul");
			$bw_dl.val($bw_dl.val().replace(/\s+/g, ''));//remove space
			$bw_ul.val($bw_ul.val().replace(/\s+/g, ''));//remove space
			if($bw_enabled.hasClass("on")){
				if($bw_dl.val() == ""){
					$bw_dl.show_validate_hint("<#JS_fieldblank#>");
					$bw_dl.focus();
					return false;
				}
				else{
					var isValid_bw_dl = valid_bandwidth($bw_dl.val());
					if(isValid_bw_dl.isError){
						$bw_dl.show_validate_hint(isValid_bw_dl.errReason);
						$bw_dl.focus();
						return false;
					}
				}
				if($bw_ul.val() == ""){
					$bw_ul.show_validate_hint("<#JS_fieldblank#>");
					$bw_ul.focus();
					return false;
				}
				else{
					var isValid_bw_ul = valid_bandwidth($bw_ul.val());
					if(isValid_bw_ul.isError){
						$bw_ul.show_validate_hint(isValid_bw_ul.errReason);
						$bw_ul.focus();
						return false;
					}
				}
			}
		}
	}
	if(_type == "ALL"){
		if(sdn_idx != undefined){
			var $ipaddr = $(_obj).find("#ipaddr");
			if($ipaddr.length != 0){
				$ipaddr.val($ipaddr.val().replace(/\s+/g, ''));//remove space
				if($ipaddr.val() == ""){
					$ipaddr.show_validate_hint("<#JS_fieldblank#>");
					$ipaddr.focus();
					return false;
				}
				var isValid_ipaddr = valid_isLegalIP($ipaddr.val());
				if(isValid_ipaddr.isError){
					$ipaddr.show_validate_hint(isValid_ipaddr.errReason);
					$ipaddr.focus();
					return false;
				}
				var lan_ipaddr = httpApi.nvramGet(["lan_ipaddr"]).lan_ipaddr;
				if($ipaddr.val() == lan_ipaddr){
					$ipaddr.show_validate_hint("<#JS_conflict_LANIP#>");
					$ipaddr.focus();
					return false;
				}
				var ipaddr_substr = $ipaddr.val().substr(0, $ipaddr.val().lastIndexOf("."));
				var wan0_ipaddr_substr = httpApi.nvramGet(["wan0_ipaddr"]).wan0_ipaddr.substr(0, httpApi.nvramGet(["wan0_ipaddr"]).wan0_ipaddr.lastIndexOf("."));
				var wan1_ipaddr_substr = httpApi.nvramGet(["wan1_ipaddr"]).wan1_ipaddr.substr(0, httpApi.nvramGet(["wan1_ipaddr"]).wan1_ipaddr.lastIndexOf("."));
				if((wan0_ipaddr_substr == ipaddr_substr) || (wan1_ipaddr_substr == ipaddr_substr)){
					$ipaddr.show_validate_hint("<#JS_conflict_LANIP#>".replace("LAN", "WAN"));
					$ipaddr.focus();
					return false;
				}
				var specific_data = sdn_all_rl_json.filter(function(item, index, array){
					if(item.sdn_rl.idx != sdn_idx && !$.isEmptyObject(item.subnet_rl)){
						var compare_ipaddr = item.subnet_rl.addr.substr(0, item.subnet_rl.addr.lastIndexOf("."));
						return (compare_ipaddr == ipaddr_substr);
					}
				})[0];
				var lan_ipaddr_substr = lan_ipaddr.substr(0, lan_ipaddr.lastIndexOf("."));
				if(specific_data != undefined || (lan_ipaddr_substr == ipaddr_substr)){
					$ipaddr.show_validate_hint("<#vpn_openvpn_conflict#>");
					$ipaddr.focus();
					return false;
				}
			}

			var $vlan_id = $(_obj).find("#vlan_id");
			if($vlan_id.length != 0){
				$vlan_id.val($vlan_id.val().replace(/\s+/g, ''));//remove space
				if($vlan_id.val() == ""){
					$vlan_id.show_validate_hint("<#JS_fieldblank#>");
					$vlan_id.focus();
					return false;
				}
				var isValid_vlan_id = valid_num_range($vlan_id.val(), 1, 4094);
				if(isValid_vlan_id.isError){
					$vlan_id.show_validate_hint(isValid_vlan_id.errReason);
					$vlan_id.focus();
					return false;
				}
				var specific_data = sdn_all_rl_json.filter(function(item, index, array){
					return (item.sdn_rl.idx != sdn_idx && item.vlan_rl.vid == $vlan_id.val());
				})[0];
				var specific_sdn_rl = sdn_all_rl_json.filter(function(item, index, array){
					return (item.sdn_rl.idx == sdn_idx);
				})[0];
				var specific_vlan_rl = vlan_rl_json.filter(function(item, index, array){
					return (item.vlan_idx != specific_sdn_rl.sdn_rl.vlan_idx && item.vid == $vlan_id.val());
				})[0];
				if(specific_data != undefined || specific_vlan_rl != undefined){
					$vlan_id.show_validate_hint("<#JS_duplicate#>");
					$vlan_id.focus();
					return false;
				}
			}

			var portal_type = $(_obj).find("#select_portal_type").children(".selected").attr("value");
			if(portal_type == "2"){
				var $FWF_conntimeout = $(_obj).find("#FWF_conntimeout");
				$FWF_conntimeout.val($FWF_conntimeout.val().replace(/\s+/g, ''));//remove space
				if($FWF_conntimeout.val() == ""){
					$FWF_conntimeout.show_validate_hint("<#JS_fieldblank#>");
					$FWF_conntimeout.focus();
					return false;
				}
				var isValid_FWF_conntimeout = valid_num_range($FWF_conntimeout.val(), 1, 999);
				if(isValid_FWF_conntimeout.isError){
					$FWF_conntimeout.show_validate_hint(isValid_FWF_conntimeout.errReason);
					$FWF_conntimeout.focus();
					return false;
				}

				var $FWF_redirecturl = $(_obj).find("#FWF_redirecturl");
				$FWF_redirecturl.val($FWF_redirecturl.val().replace(/\s+/g, ''));//remove space
				if($FWF_redirecturl.val() != ""){
					var isValid_FWF_redirecturl = valid_url($FWF_redirecturl.val());
					if(isValid_FWF_redirecturl.isError){
						$FWF_redirecturl.show_validate_hint(isValid_FWF_redirecturl.errReason);
						$FWF_redirecturl.focus();
						return false;
					}
				}

				var $FWF_passcode_enabled = $(_obj).find("#FWF_passcode_enabled");
				if($FWF_passcode_enabled.hasClass("on")){
					var $FWF_passcode = $(_obj).find("#FWF_passcode");
					$FWF_passcode.val($FWF_passcode.val().replace(/\s+/g, ''));//remove space
					if($FWF_passcode.val() == ""){
						$FWF_passcode.show_validate_hint("<#JS_fieldblank#>");
						$FWF_passcode.focus();
						return false;
					}
					var isValid_passcode = valid_block_chars($FWF_passcode.val(), ["<", ">"]);
					if(isValid_passcode.isError){
						$FWF_passcode.show_validate_hint(isValid_passcode.errReason);
						$FWF_passcode.focus();
						return false;
					}
					//confirm common string combination	#JS_common_passwd#
					var is_common_string = check_common_string($FWF_passcode.val(), "httpd_password");
					if(is_common_string){
						if(!confirm("<#JS_common_passwd#>")){
							$FWF_passcode.focus();
							return false;
						}
					}
				}
			}
		}
	}
	return true;
}
function show_sdn_profilelist(){
	//$("#profile_list_num").html(htmlEnDeCode.htmlEncode("" + (Object.keys(sdn_all_rl_json).length - 1) + "/" + sdn_maximum));
	$("#profile_list_content").empty();
	$.each(sdn_all_rl_json, function(index, sdn_all_rl){
		if(sdn_all_rl.sdn_rl.idx == "0")
			return true;
		var item = {
			"sdn_idx": sdn_all_rl.sdn_rl.idx,
			"type": sdn_all_rl.sdn_rl.sdn_name,
			"name": sdn_all_rl.apg_rl.ssid,
			"activate": sdn_all_rl.sdn_rl.sdn_enable,
			"client_num": sdn_all_rl.client_num
		};
		$("#profile_list_content").append(Get_Component_Profile_Item(item));
	});
	if(sdn_all_rl_json.length > 0)
		$("#profile_list_content").append($("<div>").addClass("horizontal_line"));

	var $AddNew_item = Get_Component_Profile_Item_AddNew().appendTo($("#profile_list_content"));
	if($(".profile_setting_container").css("display") != "none"){
		if(selected_sdn_idx == ""){
			$AddNew_item.click();
			if(sdn_all_rl_json.length <= 0){
				$(".popup_element").css("display", "flex");
				$(".container").addClass("blur_effect");
				$(".popup_container.popup_element").empty()
					.append(show_Get_Start("popup"))
					.addClass(function(){
						return "hide_title_cntr" + ((isMobile()) ? " full_width" : "");
					})
					.find(".popup_title_container").hide();
				resize_iframe_height();
			}
		}
		else{
			$("#profile_list_content").find("[sdn_idx="+selected_sdn_idx+"] .item_text_container").click();
		}
	}
	else{
		$(".profile_setting_container").empty().append(show_Get_Start());
		if((window.matchMedia('(max-width: 575px)').matches)){//for mobile
			if(sdn_all_rl_json.length <= 0){
				$AddNew_item.click();
				$(".popup_container.popup_element")
					.addClass("hide_title_cntr")
					.find(".popup_title_container").hide();
			}
		}
	}
	clearInterval(interval_AccTime);
	var $AccTime_list = $("#profile_list_content").find("[data-container=AccTime][access_time=true]");
	if($AccTime_list.length > 0){
		interval_AccTime = setInterval(function(){
			$.each($AccTime_list, function(index, item){
				var end_time = parseInt($(item).attr("end_time"));
				var cur_time = parseInt($(item).attr("cur_time")) + 1;
				var remaining_time = ((end_time - cur_time) > 0) ? (end_time - cur_time) : 0;
				var HMS = secondsToHMS(remaining_time);
				$(item).attr({"cur_time": cur_time}).html(HMS.hours + ":" + HMS.minutes + ":" + HMS.seconds);
				if(remaining_time <= 0){
					init_sdn_all_list();
					show_sdn_profilelist();
				}
			});
		},1000);
	}
}
var vpnc_profile_list = [];
function Get_VPNC_Profile(){
	var vpnc_profile_attr = function(){
		this.desc = "";
		this.proto = "";
		this.vpnc_idx = "0";
		this.activate = "0";
	};
	var vpnc_clientlist = decodeURIComponent(httpApi.nvramCharToAscii(["vpnc_clientlist"]).vpnc_clientlist);
	var each_profile = vpnc_clientlist.split("<");
	$.each(each_profile, function(index, value){
		if(value != ""){
			var profile_data = value.split(">");
			var vpnc_profile = new vpnc_profile_attr();
			vpnc_profile.desc = profile_data[0];
			vpnc_profile.proto = profile_data[1];
			if(isSupport("vpn_fusion")){
				vpnc_profile.activate = profile_data[5];
				vpnc_profile.vpnc_idx = profile_data[6];
			}
			vpnc_profile_list.push(JSON.parse(JSON.stringify(vpnc_profile)));
		}
	});
}
var dns_list_data = [];
function Get_DNS_List_DB(){
	var dns_list = [];
	$.getJSON("/ajax/DNS_List.json", function(local_data){
		dns_list = Object.keys(local_data).map(function(e){
			return local_data[e];
		});
		category_DNS();

		setTimeout(function(){
			$.getJSON("https://nw-dlcdnet.asus.com/plugin/js/DNS_List.json",
				function(cloud_data){
					if(JSON.stringify(local_data) != JSON.stringify(cloud_data)){
						if(Object.keys(cloud_data).length > 0){
							dns_list = Object.keys(cloud_data).map(function(e){
								return cloud_data[e];
							});
							category_DNS();
						}
					}
				}
			);
		}, 1000);
	});

	function category_DNS(){
		dns_list_data = [];
		$.each(dns_list, function(index, item){
			if(item.FilterMode != undefined){
				if(dns_list_data[item.FilterMode] == undefined){
					dns_list_data[item.FilterMode] = [];
				}
				dns_list_data[item.FilterMode].push(item);
			}
		});
	}
}
var scenarios_list = [
	{
		"type":"Employee",
		"text":stringSafeGet("<#GuestNetwork_Employee#>"),
		"scenes":"office",
		"idx": 1
	},
	{
		"type":"Portal",
		"text":str_Scenario_Hotel,
		"scenes":"hotel",
		"idx": 4
	},
	{
		"type":"Portal",
		"text":str_Scenario_coffee,
		"scenes":"coffee_shop",
		"idx": 5
	},
	{
		"type":"Guest",
		"text":str_Scenario_Mall,
		"scenes":"shopping_mall",
		"idx": 6
	},
	{
		"type":(isSupport("BUSINESS") ? "Sched" : "Kids"),
		"text":str_Scenario_Study,
		"scenes":"remote_study",
		"idx": 9
	},
	{
		"type":(isSupport("BUSINESS") ? "Sched" : "Kids"),
		"text":str_Scenario_Classroom,
		"scenes":"classroom",
		"idx": 10
	},
	{
		"type":"IoT",
		"text":str_Scenario_smartHome,
		"scenes":"smart_home",
		"idx": 11
	},
	{
		"type":"IoT",
		"text":str_Scenario_EV_charging,
		"scenes":"ev_station",
		"idx": 13
	},
	{
		"type":"Guest",
		"text":str_Scenario_Friends,
		"scenes":"friends",
		"idx": 16
	}
];
function Get_Scenarios_List(){
	var sdn_scenarios_db_translation_mapping = [
		{tag:"#RESTAURANT",text:stringSafeGet("<#Scenario_Restaurant#>")},
		{tag:"#GYM",text:stringSafeGet("<#AiMesh_NodeLocation28#>")},
		{tag:"#CHILDREN",text:stringSafeGet("<#Scenario_Children#>")},
		{tag:"#VOICE_ASSISTANT",text:stringSafeGet("<#Scenario_Voice_assistance#>")},
		{tag:"#SURVEILLANCE_DEVICES",text:stringSafeGet("<#Scenario_Surveillance_devices#>")},
		{tag:"#WFH",text:stringSafeGet("<#Scenario_WFH#>")},
		{tag:"#BRANCH_OFFICE",text:stringSafeGet("<#Scenario_branchOffice#>")}
	];
	setTimeout(function(){
		$.getJSON("https://nw-dlcdnet.asus.com/plugin/js/extend_sdn_scenarios.json", function(data){
			$.each(data, function(index, item){
				if(item["type"] == "Kids" && isSupport("BUSINESS")){
					item["type"] = "Sched";
				}
				item["source"] = "Cloud";

				if(item.translation != "") {
					var specific_translation = sdn_scenarios_db_translation_mapping.filter(function(map_item){
						return (map_item.tag == item.translation);
					})[0];
					if(specific_translation != undefined){
						item.text = specific_translation.text;
					}
				}
			});
			if(data.length > 0){
				$.merge(scenarios_list, data);
				scenarios_list.sort(function(a, b){return parseInt(a.idx) - parseInt(b.idx);});
			}
		});
	},1000);
}

var sdn_all_rl_attr = function(){
	this.sdn_rl = {};
	this.vlan_rl = {};
	this.subnet_rl = {};
	this.radius_rl = {};
	this.apg_rl = {};
	this.cp_rl = {};
	this.sdn_access_rl = [];
};
var sdn_rl_attr = function(){
	this.idx = "0";
	this.sdn_name = "";
	this.sdn_enable = "1";
	this.vlan_idx = "0";
	this.subnet_idx = "0";
	this.apg_idx = "0";
	this.vpnc_idx = "0";
	this.vpns_idx = "0";
	this.dns_filter_idx = "0";
	this.urlf_idx = "0";
	this.nwf_idx = "0";
	this.cp_idx = "0";
	this.gre_idx = "0";
	this.firewall_idx = "0";
	this.kill_switch = "0";
	this.access_host_service = "0";
	this.wan_unit = "0";
	this.pppoe_relay = "0";
	this.wan6_unit = "0";
	this.createby = "WEB";
	this.mtwan_idx = "0";
	this.mswan_idx = "0";
};
var vlan_rl_attr = function(){
	this.vlan_idx = "";
	this.vid = "";
	this.port_isolation = "0";
};
var subnet_rl_attr = function(){
	this.subnet_idx = "";
	this.ifname = "";
	this.addr = "";
	this.netmask = "";
	this.dhcp_enable = "";
	this.dhcp_min = "";
	this.dhcp_max = "";
	this.dhcp_lease = "";
	this.domain_name = "";
	this.dns = "";
	this.wins = "";
	this.dhcp_static = "";
	this.dhcp_unit = "";
};
var apg_rl_attr = function(){
	this.apg_idx = "";
	this.enable = "";
	this.ssid = "";
	this.hide_ssid = "";
	this.security = "";
	this.bw_limit = "";
	this.timesched = "";
	this.sched = "";
	this.expiretime = "";
	this.ap_isolate = "";
	this.macmode = "";
	this.maclist = "";
	this.iot_max_cmpt = "";
	this.dut_list = "";
};
var radius_rl_attr = function(){
	this.radius_idx = "";
	this.auth_server_1 = "";
	this.auth_port_1 = "";
	this.auth_key_1 = "";
	this.acct_server_1 = "";
	this.acct_port_1 = "";
	this.acct_key_1 = "";
	this.auth_server_2 = "";
	this.auth_port_2 = "";
	this.auth_key_2 = "";
	this.acct_server_2 = "";
	this.acct_port_2 = "";
	this.acct_key_2 = "";
};
var cp_profile_attr = function(){
	this.cp_idx = "";
	this.enable = "0";
	this.auth_type = "0";
	this.conntimeout = "0";
	this.idle_timeout = "0";
	this.auth_timeout = "0";
	this.redirecturl = "0";
	this.term_of_service = "1";
	this.bw_limit_ul = "0";
	this.bw_limit_dl = "0";
	this.NAS_ID = "0";
	this.idx_for_customized_ui = "";
	this.local_auth_profile  = "";
	this.radius_profile = "";
};
var sdn_access_rl_attr = function(){
	this.access_sdn_idx = "0";//default 0, is br0.
	this.sdn_idx = "";
};
function init_sdn_all_list(){
	if(isSwMode("rt")){
		httpApi.nvramGet(["qos_enable"], true);
	}
	var apg_wifi_sched_on = httpApi.hookGet("apg_wifi_sched_on", true);
	sdn_all_rl_json = [];
	cp_type_rl_json.forEach(function(item){
		item.sdn_idx = "";
		item.profile = [];
	});
	vpns_rl_json.forEach(function(item){
		item.sdn_idx = "";
	});
	var sdn_all_rl_info = httpApi.nvramCharToAscii(["sdn_rl", "vlan_rl", "subnet_rl", "radius_list", "sdn_access_rl"], true);
	var sdn_rl = decodeURIComponent(sdn_all_rl_info.sdn_rl);
	var vlan_rl = decodeURIComponent(sdn_all_rl_info.vlan_rl);
	var subnet_rl = decodeURIComponent(sdn_all_rl_info.subnet_rl);
	var radius_rl = decodeURIComponent(sdn_all_rl_info.radius_list);
	var sdn_access_rl = parse_StrToJSON_sdn_access_rl_list(decodeURIComponent(sdn_all_rl_info.sdn_access_rl));
	vlan_rl_json = parse_StrToJSON_vlan_rl_list(vlan_rl);
	var each_sdn_rl = sdn_rl.split("<");
	$.each(each_sdn_rl, function(index, value){
		if(value != ""){
			var sdn_all_rl = JSON.parse(JSON.stringify(new sdn_all_rl_attr()));
			var profile_data = value.split(">");
			var sdn_rl_profile = set_sdn_profile(profile_data);
			if(sdn_rl_profile.idx == "0")
				return;
			sdn_all_rl.sdn_rl = sdn_rl_profile;

			var specific_cp_type_rl = cp_type_rl_json.filter(function(item, index, array){
				return (item.cp_idx == sdn_all_rl.sdn_rl.cp_idx);
			})[0];
			if(specific_cp_type_rl != undefined){
				specific_cp_type_rl.sdn_idx = sdn_all_rl.sdn_rl.idx;
			}

			var specific_vpns_rl = vpns_rl_json.filter(function(item, index, array){
				return (item.vpns_idx == sdn_all_rl.sdn_rl.vpns_idx);
			})[0];
			if(specific_vpns_rl != undefined){
				specific_vpns_rl.sdn_idx = sdn_all_rl.sdn_rl.idx;
			}

			var specific_vlan = vlan_rl_json.filter(function(item, index, array){
				return (item.vlan_idx == sdn_rl_profile.vlan_idx);
			})[0];
			if(specific_vlan != undefined){
				sdn_all_rl.vlan_rl = specific_vlan;
			}

			var subnet_rl_list = parse_StrToJSON_subnet_rl_list(subnet_rl);
			var specific_subnet = subnet_rl_list.filter(function(item, index, array){
				return (item.subnet_idx == sdn_rl_profile.subnet_idx);
			})[0];
			if(specific_subnet != undefined){
				sdn_all_rl.subnet_rl = specific_subnet;
			}
			var apg_rl_list = get_apg_rl_list(sdn_rl_profile.apg_idx);
			var specific_apg = apg_rl_list.filter(function(item, index, array){
				return (item.apg_idx == sdn_rl_profile.apg_idx);
			})[0];
			if(specific_apg != undefined){
				sdn_all_rl.apg_rl = specific_apg;
				var radius_rl_list = parse_StrToJSON_radius_rl_list(radius_rl);
				var specific_radius = radius_rl_list.filter(function(item, index, array){
					return (item.radius_idx == specific_apg.apg_idx);
				})[0];
				if(specific_radius != undefined){
					sdn_all_rl.radius_rl = specific_radius;
				}
			}
			var cp_rl_list = get_cp_rl_list(sdn_rl_profile.cp_idx);
			var specific_cp = cp_rl_list.filter(function(item, index, array){
				return (item.cp_idx == sdn_rl_profile.cp_idx);
			})[0];
			if(specific_cp != undefined){
				sdn_all_rl.cp_rl = specific_cp;
				httpApi.hookGet("get_customized_attribute-" + specific_cp.idx_for_customized_ui + "", true);
			}
			var specific_acc_rl = sdn_access_rl.filter(function(item, index, array){
				return (item.sdn_idx == sdn_all_rl.sdn_rl.idx);
			});
			if(specific_acc_rl.length > 0){
				sdn_all_rl.sdn_access_rl = specific_acc_rl;
			}
			sdn_all_rl.client_num = 0;
			sdn_all_rl_json.push(sdn_all_rl);
		}
	});
	update_SDN_ClientList();
	init_aimesh_wifi_band_info();
	$.each(cp_type_rl_json, function(index, item){
		var cp_rl_list = get_cp_rl_list(item.cp_idx);
		$.each(cp_rl_list, function(index, value){
			item.profile.push(value);
		});
	});

	function set_sdn_profile(profile_data){
		var sdn_profile = JSON.parse(JSON.stringify(new sdn_rl_attr()));
		sdn_profile.idx = profile_data[0];
		sdn_profile.sdn_name = profile_data[1];
		sdn_profile.sdn_enable = profile_data[2];
		sdn_profile.vlan_idx = profile_data[3];
		sdn_profile.subnet_idx = profile_data[4];
		sdn_profile.apg_idx = profile_data[5];
		sdn_profile.wifi_sched_on = ((apg_wifi_sched_on["apg" + profile_data[5] + ""] == "undefined") ? "1" : apg_wifi_sched_on["apg" + profile_data[5] + ""]);
		sdn_profile.vpnc_idx = profile_data[6];
		sdn_profile.vpns_idx = profile_data[7];
		sdn_profile.dns_filter_idx = profile_data[8];
		sdn_profile.urlf_idx = profile_data[9];
		sdn_profile.nwf_idx = profile_data[10];
		sdn_profile.cp_idx = profile_data[11];
		sdn_profile.gre_idx = profile_data[12];
		sdn_profile.firewall_idx = profile_data[13];
		sdn_profile.kill_switch = profile_data[14];
		sdn_profile.access_host_service = profile_data[15];
		sdn_profile.wan_unit = ((profile_data[16] == "undefined" || profile_data[16] == "") ? "0" : profile_data[16]);
		sdn_profile.pppoe_relay = ((profile_data[17] == "undefined" || profile_data[17] == "") ? "0" : profile_data[17]);
		sdn_profile.wan6_unit = ((profile_data[18] == "undefined" || profile_data[18] == "") ? "0" : profile_data[18]);
		sdn_profile.createby = ((profile_data[19] == "undefined" || profile_data[19] == "") ? "WEB" : profile_data[19]);
		sdn_profile.mtwan_idx = ((profile_data[20] == "undefined" || profile_data[20] == "") ? "0" : profile_data[20]);
		sdn_profile.mswan_idx = ((profile_data[21] == "undefined" || profile_data[21] == "") ? "0" : profile_data[21]);
		return sdn_profile;
	}
	function parse_StrToJSON_vlan_rl_list(vlan_rl){
		var vlan_rl_list = [];
		var each_vlan_rl = vlan_rl.split("<");
		$.each(each_vlan_rl, function(index, value){
			if(value != ""){
				var profile_data = value.split(">");
				var vlan_profile = new vlan_rl_attr();
				vlan_profile.vlan_idx = profile_data[0];
				vlan_profile.vid = profile_data[1];
				vlan_profile.port_isolation = ((profile_data[2] == "undefined" || profile_data[2] == "") ? "0" : profile_data[2]);
				vlan_rl_list.push(JSON.parse(JSON.stringify(vlan_profile)));
			}
		});
		vlan_rl_list.sort(function(a, b) {
			return parseInt(a.vlan_idx) - parseInt(b.vlan_idx);
		});
		return vlan_rl_list;
	}
	function parse_StrToJSON_subnet_rl_list(subnet_rl){
		var subnet_rl_list = [];
		var each_subnet_rl = subnet_rl.split("<");
		$.each(each_subnet_rl, function(index, value){
			if(value != ""){
				var profile_data = value.split(">");
				var subnet_profile = new subnet_rl_attr();
				subnet_profile.subnet_idx = profile_data[0];
				subnet_profile.ifname = profile_data[1];
				subnet_profile.addr = profile_data[2];
				subnet_profile.netmask = profile_data[3];
				subnet_profile.dhcp_enable = profile_data[4];
				subnet_profile.dhcp_min = profile_data[5];
				subnet_profile.dhcp_max = profile_data[6];
				subnet_profile.dhcp_lease = profile_data[7];
				subnet_profile.domain_name = profile_data[8];
				subnet_profile.dns = profile_data[9];
				subnet_profile.wins = profile_data[10];
				subnet_profile.dhcp_static = profile_data[11];
				subnet_profile.dhcp_unit = profile_data[12];
				subnet_rl_list.push(JSON.parse(JSON.stringify(subnet_profile)));
			}
		});
		subnet_rl_list.sort(function(a, b) {
		    return parseInt(a.subnet_idx) - parseInt(b.subnet_idx);
		});
		return subnet_rl_list;
	}
	function parse_StrToJSON_radius_rl_list(radius_rl){
		var radius_rl_list = [];
		var each_radius_rl = radius_rl.split("<");
		$.each(each_radius_rl, function(index, value){
			if(value != ""){
				var profile_data = value.split(">");
				var radius_profile = new radius_rl_attr();
				radius_profile.radius_idx = profile_data[0];
				radius_profile.auth_server_1 = profile_data[1];
				radius_profile.auth_port_1 = profile_data[2];
				radius_profile.auth_key_1 = profile_data[3];
				radius_profile.acct_server_1 = profile_data[4];
				radius_profile.acct_port_1 = profile_data[5];
				radius_profile.acct_key_1 = profile_data[6];
				radius_profile.auth_server_2 = profile_data[7];
				radius_profile.auth_port_2 = profile_data[8];
				radius_profile.auth_key_2 = profile_data[9];
				radius_profile.acct_server_2 = profile_data[10];
				radius_profile.acct_port_2 = profile_data[11];
				radius_profile.acct_key_2 = profile_data[12];
				radius_rl_list.push(JSON.parse(JSON.stringify(radius_profile)));
			}
		});
		radius_rl_list.sort(function(a, b) {
			return parseInt(a.subnet_idx) - parseInt(b.subnet_idx);
		});
		return radius_rl_list;
	}
	function parse_StrToJSON_sdn_access_rl_list(sdn_access_rl){
		var sdn_access_rl_list = [];
		var each_sdn_access_rl = sdn_access_rl.split("<");
		$.each(each_sdn_access_rl, function(index, value){
			if(value != ""){
				var profile_data = value.split(">");
				var sdn_access_rl_profile = new sdn_access_rl_attr();
				sdn_access_rl_profile.access_sdn_idx = profile_data[0];
				sdn_access_rl_profile.sdn_idx = profile_data[1];
				sdn_access_rl_list.push(JSON.parse(JSON.stringify(sdn_access_rl_profile)));
			}
		});
		return sdn_access_rl_list;
	}
	function get_apg_rl_list(_apg_idx){
		var apg_rl_list = [];
		if(parseInt(_apg_idx) > 0){
			var apg_profile = new apg_rl_attr();
			var apg_info = httpApi.nvramCharToAscii(["apg" + _apg_idx + "_enable", "apg" + _apg_idx + "_ssid", "apg" + _apg_idx + "_hide_ssid", "apg" + _apg_idx + "_security", "apg" + _apg_idx + "_bw_limit",
				"apg" + _apg_idx + "_timesched", "apg" + _apg_idx + "_sched", "apg" + _apg_idx + "_expiretime","apg" + _apg_idx + "_ap_isolate", "apg" + _apg_idx + "_macmode", "apg" + _apg_idx + "_maclist", "apg" + _apg_idx + "_iot_max_cmpt", "apg" + _apg_idx + "_dut_list"], true);
			apg_profile.apg_idx = _apg_idx.toString();
			apg_profile.enable = apg_info["apg" + _apg_idx + "_enable"];
			apg_profile.ssid = decodeURIComponent(apg_info["apg" + _apg_idx + "_ssid"]);
			apg_profile.hide_ssid = apg_info["apg" + _apg_idx + "_hide_ssid"];
			apg_profile.security = decodeURIComponent(apg_info["apg" + _apg_idx + "_security"]);
			apg_profile.bw_limit = decodeURIComponent(apg_info["apg" + _apg_idx + "_bw_limit"]);
			apg_profile.timesched = apg_info["apg" + _apg_idx + "_timesched"];
			apg_profile.sched = decodeURIComponent(apg_info["apg" + _apg_idx + "_sched"]);
			apg_profile.expiretime = decodeURIComponent(apg_info["apg" + _apg_idx + "_expiretime"]);
			apg_profile.ap_isolate = apg_info["apg" + _apg_idx + "_ap_isolate"];
			apg_profile.macmode = decodeURIComponent(apg_info["apg" + _apg_idx + "_macmode"]);
			apg_profile.maclist = decodeURIComponent(apg_info["apg" + _apg_idx + "_maclist"]);
			apg_profile.iot_max_cmpt = apg_info["apg" + _apg_idx + "_iot_max_cmpt"];
			apg_profile.dut_list = decodeURIComponent(apg_info["apg" + _apg_idx + "_dut_list"]);
			apg_rl_list.push(JSON.parse(JSON.stringify(apg_profile)));
		}
		return apg_rl_list;
	}
	function get_cp_rl_list(_cp_idx){
		var cp_rl_list = [];
		if(parseInt(_cp_idx) > 0){
			var cp_info = httpApi.nvramCharToAscii(["cp" + _cp_idx + "_profile", "cp" + _cp_idx + "_local_auth_profile", "cp" + _cp_idx + "_radius_profile"], true);
			var cp_profile = decodeURIComponent(cp_info["cp" + _cp_idx + "_profile"]);
			var each_cp_profile = cp_profile.split("<");
			$.each(each_cp_profile, function(index, value){
				if(value != ""){
					var profile_data = value.split(">");
					var cp_profile = new cp_profile_attr();
					cp_profile.cp_idx = _cp_idx.toString();
					cp_profile.enable = profile_data[0];
					cp_profile.auth_type = profile_data[1];
					cp_profile.conntimeout = profile_data[2];
					cp_profile.idle_timeout = profile_data[3];
					cp_profile.auth_timeout = profile_data[4];
					cp_profile.redirecturl = profile_data[5];
					cp_profile.term_of_service = profile_data[6];
					cp_profile.bw_limit_ul = profile_data[7];
					cp_profile.bw_limit_dl = profile_data[8];
					cp_profile.NAS_ID = profile_data[9];
					cp_profile.idx_for_customized_ui = profile_data[10];
					cp_profile.local_auth_profile = decodeURIComponent(cp_info["cp" + _cp_idx + "_local_auth_profile"]).substring(1);
					cp_profile.radius_profile = decodeURIComponent(cp_info["cp" + _cp_idx + "_radius_profile"]);
					cp_rl_list.push(JSON.parse(JSON.stringify(cp_profile)));
				}
			});
		}
		return cp_rl_list;
	}
	function update_SDN_ClientList(){
		httpApi.updateClientList();
		var get_clientlist = httpApi.hookGet("get_clientlist", true);
		$.each(get_clientlist, function(index, value){
			if(value.sdn_idx != undefined){
				if(value.isOnline == "1"){
					var specific_data = sdn_all_rl_json.filter(function(item, index, array){
						return (item.sdn_rl.idx == value.sdn_idx);
					})[0];
					if(specific_data != undefined){
						specific_data.client_num += 1;
					}
				}
			}
		});
	}
}
var vlan_vid_add = "";
function get_new_sdn_profile(){
	var sdn_rl_profile = get_new_sdn_rl();
	var vlan_rl_profile = get_new_vlan_rl();
	var subnet_rl_profile = get_new_subnet_rl(sdn_rl_profile);
	var apg_rl_profile = get_new_apg_rl();
	sdn_rl_profile.vlan_idx = vlan_rl_profile.vlan_idx;
	sdn_rl_profile.subnet_idx = subnet_rl_profile.subnet_idx;
	sdn_rl_profile.apg_idx = apg_rl_profile.apg_idx;

	var dut_list = "";
	$.each(cfg_clientlist, function(index, node_info){
		var wifi_band_set = {"mac":"", "wifi_band":0};
		var wifi_band_info = httpApi.aimesh_get_node_wifi_band(node_info);
		$.each(wifi_band_info, function(index, band_info){
			if(band_info.no_used > 0){
				wifi_band_set.mac = node_info.mac;
				wifi_band_set.wifi_band += band_info.band;
			}
		});
		if(wifi_band_set.wifi_band != 0){
			dut_list += "<" + wifi_band_set.mac + ">" + wifi_band_set.wifi_band + ">";
		}
	});
	apg_rl_profile.dut_list = dut_list;
	var sdn_obj = {};
	sdn_obj["sdn_" + sdn_rl_profile.idx + ""] = JSON.parse(JSON.stringify(new sdn_all_rl_attr()));
	sdn_obj["sdn_" + sdn_rl_profile.idx + ""]["sdn_rl"] = JSON.parse(JSON.stringify(sdn_rl_profile));
	sdn_obj["sdn_" + sdn_rl_profile.idx + ""]["vlan_rl"] = vlan_rl_profile;
	sdn_obj["sdn_" + sdn_rl_profile.idx + ""]["subnet_rl"] = subnet_rl_profile;
	sdn_obj["sdn_" + sdn_rl_profile.idx + ""]["apg_rl"] = apg_rl_profile;
	return sdn_obj;

	function get_new_sdn_rl(){
		var idx = 1;
		idx = get_rl_new_idx(idx, "sdn");
		var sdn_profile = new sdn_rl_attr();
		sdn_profile.idx = idx.toString();
		return (JSON.parse(JSON.stringify(sdn_profile)));
	}
	function get_new_vlan_rl(){
		var vlan_idx = 1;
		vlan_idx = get_vlan_rl_new_idx(vlan_idx, "vlan");

		var vid = 52;
		vid = get_vlan_rl_new_vid(vid);

		vlan_vid_add = vid;

		var vlan_profile = new vlan_rl_attr();
		vlan_profile.vlan_idx = vlan_idx.toString();
		vlan_profile.vid = vid.toString();
		return (JSON.parse(JSON.stringify(vlan_profile)));

		function get_vlan_rl_new_idx(start_idx, category){
			var new_idx = parseInt(start_idx);
			for(new_idx; new_idx < (new_idx + sdn_maximum); new_idx+=1){
				var specific_data = vlan_rl_json.filter(function(item, index, array){
					return (item.vlan_idx == new_idx);
				})[0];
				if(specific_data == undefined){
					break;
				}
			}
			return new_idx;
		}
		function get_vlan_rl_new_vid(start_vid){
			var new_vid = start_vid;
			for(start_vid; start_vid < (start_vid + sdn_maximum); start_vid+=1){
				var specific_data = vlan_rl_json.filter(function(item, index, array){
					return (item.vid == start_vid);
				})[0];
				if(specific_data == undefined){
					new_vid = start_vid;
					break;
				}
			}
			return new_vid;
		}
	}
	function get_new_subnet_rl(sdn_profile){
		var subnet_idx = 1;
		subnet_idx = get_rl_new_idx(subnet_idx, "subnet");
		var dhcp_info = httpApi.nvramDefaultGet(["lan_ipaddr", "lan_netmask", "dhcp_enable_x", "dhcp_start", "dhcp_end", "dhcp_lease",
			"lan_domain", "dhcp_dns1_x", "dhcp_dns2_x", "dhcp_wins_x", "dhcp_static_x"]);
		var ipaddr = get_subnet_rl_new_ipaddr();
		var ipaddr_substr = ipaddr.substr(0,ipaddr.lastIndexOf("."));
		var ipaddr_min = ipaddr_substr + "." + "2";
		var ipaddr_max = ipaddr_substr + "." + "254";

		var subnet_profile = new subnet_rl_attr();
		subnet_profile.subnet_idx = subnet_idx.toString();
		subnet_profile.ifname = "br" + (parseInt(sdn_profile.idx) + 51);
		subnet_profile.addr = ipaddr;
		subnet_profile.netmask = dhcp_info.lan_netmask;
		subnet_profile.dhcp_enable = dhcp_info.dhcp_enable_x;
		subnet_profile.dhcp_min = ipaddr_min;
		subnet_profile.dhcp_max = ipaddr_max;
		subnet_profile.dhcp_lease = dhcp_info.dhcp_lease;
		subnet_profile.domain_name = dhcp_info.lan_domain;
		subnet_profile.dns = dhcp_info.dhcp_dns1_x + "," + dhcp_info.dhcp_dns2_x;
		subnet_profile.wins = dhcp_info.dhcp_wins_x;
		subnet_profile.dhcp_static = dhcp_info.dhcp_static_x;
		subnet_profile.dhcp_unit = "";
		return (JSON.parse(JSON.stringify(subnet_profile)));

		function get_subnet_rl_new_ipaddr(){
			var exist_ipaddr_arr = [];
			exist_ipaddr_arr.push(get_network_num(httpApi.nvramGet(["lan_ipaddr"]).lan_ipaddr));
			exist_ipaddr_arr.push(get_network_num(httpApi.nvramGet(["wan0_ipaddr"]).wan0_ipaddr));
			exist_ipaddr_arr.push(get_network_num(httpApi.nvramGet(["wan1_ipaddr"]).wan1_ipaddr));
			$.each(sdn_all_rl_json, function(index, sdn_all_rl){
				if(!$.isEmptyObject(sdn_all_rl.subnet_rl)){
					exist_ipaddr_arr.push(get_network_num(sdn_all_rl.subnet_rl.addr));
				}
			});
			return (function(){
				var init_subnet = 52;
				var ipaddr_arr = httpApi.nvramDefaultGet(["lan_ipaddr"]).lan_ipaddr.split(".");
				var new_ipaddr = "";
				for(init_subnet; init_subnet < 255; init_subnet += 1){
					new_ipaddr = ipaddr_arr[0] + "." + ipaddr_arr[1] + "." + init_subnet;
					if($.inArray(new_ipaddr, exist_ipaddr_arr) == "-1") break;
				}
				return new_ipaddr + "." + ipaddr_arr[3];;
			})();
			function get_network_num(_ipaddr){
				return _ipaddr.substr(0, _ipaddr.lastIndexOf("."));
			}
		}
	}
	function get_new_apg_rl(){
		var apg_idx = 1;
		apg_idx = get_rl_new_idx(apg_idx, "apg");
		var apg_profile = new apg_rl_attr();
		apg_profile.apg_idx = apg_idx.toString();
		apg_profile.enable = "1";
		apg_profile.ssid = "ASUSTEST";
		apg_profile.security = "";
		apg_profile.hide_ssid = "0";
		apg_profile.bw_limit = "<0>>";
		apg_profile.timesched = "0";
		apg_profile.sched = "";
		apg_profile.ap_isolate = "0";
		apg_profile.macmode = "0";
		apg_profile.maclist = "";
		apg_profile.iot_max_cmpt = "";
		apg_profile.dut_list = "";
		return (JSON.parse(JSON.stringify(apg_profile)));
	}
	function get_rl_new_idx(start_idx, category){
		var new_idx = parseInt(start_idx);
		var compare_key = "idx";
		switch(category){
			case "sdn":
				compare_key = "idx";
				break;
			case "vlan":
				compare_key = "vlan_idx";
				break;
			case "subnet":
				compare_key = "subnet_idx";
				break;
			case "apg":
				compare_key = "apg_idx";
				break;
		}
		for(new_idx; new_idx < (new_idx + sdn_maximum); new_idx+=1){
			var specific_data = sdn_all_rl_json.filter(function(item, index, array){
				return (item.sdn_rl[compare_key] == new_idx);
			})[0];
			if(specific_data == undefined){
				break;
			}
		}
		return new_idx;
	}
}
function get_new_radius_rl(){
	var radius_profile = new radius_rl_attr();
	return (JSON.parse(JSON.stringify(radius_profile)));
}
function get_dut_list(_wifiband, _ori_dut_list_json){
	var dut_list = "";
	$.each(cfg_clientlist, function(index, node_info){
		var wifi_band_set = {"mac":"", "wifi_band":"", "lanport":""};
		var wifi_band_info = httpApi.aimesh_get_node_wifi_band(node_info);
		$.each(wifi_band_info, function(index, band_info){
			if(band_info.no_used > 0){
				wifi_band_set.mac = node_info.mac;
				if(_wifiband == "1"){//2.4G
					if(band_info.band == "1")
						wifi_band_set.wifi_band += band_info.band;
				}
				else if(_wifiband == "2"){//5G
					if(band_info.band == "2" || band_info.band == "4" || band_info.band == "8")
						wifi_band_set.wifi_band += band_info.band;
				}
				else if(_wifiband == "3"){//2.4G&5G
					if(band_info.band == "1" || band_info.band == "2" || band_info.band == "4" || band_info.band == "8")
						wifi_band_set.wifi_band += band_info.band;
				}
				else if(_wifiband == "0"){
					wifi_band_set.wifi_band = "0";
				}
			}
		});
		if(wifi_band_set.wifi_band != ""){
			if(Array.isArray(_ori_dut_list_json)){
				var specific_node = _ori_dut_list_json.filter(function(item, index, array){
					return (item.mac == wifi_band_set.mac);
				})[0];
				if(specific_node != undefined)
					wifi_band_set.lanport = specific_node.lanport;
			}
			dut_list += "<" + wifi_band_set.mac + ">" + wifi_band_set.wifi_band + ">" + wifi_band_set.lanport;
		}
	});
	return dut_list;
}
function get_specific_dut_list(_wifiband, _node_info, _ori_dut_list_json){
	var dut_list = "";
	var wifi_band_set = {"mac":"", "wifi_band":0, "lanport":""};
	var wifi_band_info = httpApi.aimesh_get_node_wifi_band(_node_info);
	$.each(wifi_band_info, function(index, band_info){
		if(band_info.no_used > 0){
			wifi_band_set.mac = _node_info.mac;
			if(_wifiband == "1"){//2.4G
				if(band_info.band == "1")
					wifi_band_set.wifi_band += band_info.band;
			}
			else if(_wifiband == "2"){//5G
				if(band_info.band == "2" || band_info.band == "4")
					wifi_band_set.wifi_band += band_info.band;
			}
			else if(_wifiband == "3"){//2.4G&5G
				if(band_info.band == "1" || band_info.band == "2" || band_info.band == "4")
					wifi_band_set.wifi_band += band_info.band;
			}
		}
	});
	if(Array.isArray(_ori_dut_list_json)){
		var specific_node = _ori_dut_list_json.filter(function(item, index, array){
			return (item.mac == wifi_band_set.mac);
		})[0];
		if(specific_node != undefined)
			wifi_band_set.lanport = specific_node.lanport;
	}
	if(wifi_band_set.wifi_band != "0"){
		dut_list = "<" + wifi_band_set.mac + ">" + wifi_band_set.wifi_band + ">" + wifi_band_set.lanport;
	}

	return dut_list;
}
function get_Offline_dut_list(_obj, _wifiband, _ori_dut_list_json){
	var dut_list = "";
	if(_wifiband != "0"){
		var $node_list = $(_obj).find("[data-container=AiMesh_List] [data-node-status=offline]");
		$node_list.each(function(i, node_cntr){
			var wifi_band_match = false;
			var node_mac = $(node_cntr).attr("data-node-mac");
			var node_wifi_band = $(node_cntr).attr("data-wifi-band");
			var node_lanport = "";
			if(_wifiband == "1"){
				if(node_wifi_band == "1"){
					wifi_band_match = true;
				}
			}
			else if(_wifiband == "2"){//5G
				if(node_wifi_band == "2" || node_wifi_band == "4"){
					wifi_band_match = true;
				}
			}
			else if(_wifiband == "3"){//2.4G&5G
				if(node_wifi_band == "3" || node_wifi_band == "5"){
					wifi_band_match = true;
				}
			}
			if(Array.isArray(_ori_dut_list_json)){
				var specific_node = _ori_dut_list_json.filter(function(item, index, array){
					return (item.mac == node_mac);
				})[0];
				if(specific_node != undefined)
					node_lanport = specific_node.lanport;
			}
			if(wifi_band_match)
				dut_list += "<" + node_mac + ">" + node_wifi_band + ">" + node_lanport;
		});
	}
	return dut_list;
}
function get_unChecked_dut_list(_obj, _ori_dut_list_json){
	var dut_list = "";
	$(_obj).find("[data-container=AiMesh_List]").find(".node_container[data-support-dutlist=true]").each(function(){
		if(!$(this).find(".icon_checkbox").hasClass("clicked")){
			var node_mac = $(this).attr("data-node-mac");
			var node_lanport = "";
			if(Array.isArray(_ori_dut_list_json)){
				var specific_node = _ori_dut_list_json.filter(function(item, index, array){
					return (item.mac == node_mac);
				})[0];
				if(specific_node != undefined)
					node_lanport = specific_node.lanport;
			}
			dut_list += "<" + node_mac + ">>" + node_lanport + "";
		}
	});
	return dut_list;
}
function get_dut_list_by_mac(_wifiband, _node_mac){
	var dut_list = "";
	_wifiband = (_wifiband != undefined && _wifiband != "") ? _wifiband : "3";
	_node_mac = (_node_mac != undefined && _node_mac != "") ? _node_mac : ((httpApi.hookGet('get_lan_hwaddr')) ? httpApi.hookGet('get_lan_hwaddr') : '');
	var wifi_band_set = {"mac":"", "wifi_band":0, "lanport":""};
	var wifi_band_info = [];
	var get_node_wifi_band = httpApi.hookGet("get_node_wifi_band");
	if(get_node_wifi_band != undefined && get_node_wifi_band != ""){
		if(get_node_wifi_band[_node_mac] != undefined && get_node_wifi_band[_node_mac]["wifi_band"] != undefined){
			$.each(get_node_wifi_band[_node_mac]["wifi_band"], function(index, value){
				var band_data = value;
				var band_info = {};
				band_info.band = parseInt(value.band);
				band_info.count = parseInt(value.count);
				band_info.no_used = 0;
				$.each(band_data.vif, function(vif_if, vif_data){
					if(vif_data.type == "1")
						band_info.no_used += 1;
				});
				wifi_band_info.push(band_info);
			});
		}
	}
	$.each(wifi_band_info, function(index, band_info){
		if(band_info.no_used > 0){
			wifi_band_set.mac = _node_mac;
			if(_wifiband == "1"){//2.4G
				if(band_info.band == "1")
					wifi_band_set.wifi_band += band_info.band;
			}
			else if(_wifiband == "2"){//5G
				if(band_info.band == "2" || band_info.band == "4")
					wifi_band_set.wifi_band += band_info.band;
			}
			else if(_wifiband == "3"){//2.4G&5G
				if(band_info.band == "1" || band_info.band == "2" || band_info.band == "4")
					wifi_band_set.wifi_band += band_info.band;
			}
		}
	});
	if(wifi_band_set.wifi_band != "0"){
		dut_list = "<" + wifi_band_set.mac + ">" + wifi_band_set.wifi_band + ">" + wifi_band_set.lanport;
	}

	return dut_list;
}
function get_cp_type_support(_type){
	return ((cp_type_support["type_"+_type+""] == true) ? true : false);
}
function sortObject(obj){
    return Object.keys(obj).sort().reduce(function (result, key) {
        result[key] = obj[key];
        return result;
    }, {});
}
function parse_JSONToStr_sdn_all_list(_profile_data){
	var _sdn_all_rl_json = JSON.parse(JSON.stringify(sdn_all_rl_json));
	var _vlan_rl_json = JSON.parse(JSON.stringify(vlan_rl_json));
	var _vlan_rl_at_sdn_all_rl = [];
	if(_profile_data != undefined){
		if(_profile_data.sdn_all_rl != undefined && (typeof _profile_data.sdn_all_rl == "object")){
			_sdn_all_rl_json = ((_profile_data.sdn_all_rl.length > 0) ? JSON.parse(JSON.stringify(_profile_data.sdn_all_rl)) : []);
		}
		if(_profile_data.vlan_rl != undefined && _profile_data.vlan_rl != "" && (typeof _profile_data.vlan_rl == "object") && (_profile_data.vlan_rl.length > 0)){
			_vlan_rl_json = JSON.parse(JSON.stringify(_profile_data.vlan_rl));
		}
	}
	var result = {"sdn_rl":"", "vlan_rl":"", "subnet_rl":"", "radius_list":"", "sdn_access_rl":""};
	var sdn_rl = "", vlan_rl = "", subnet_rl = "", radius_rl = "", sdn_access_rl = "";
	sdn_rl = (httpApi.nvramDefaultGet(["sdn_rl"]).sdn_rl).replace(/&#60/g, "<").replace(/&#62/g, ">");
	_sdn_all_rl_json.sort(function(a, b) {
		return parseInt(a.sdn_rl.idx) - parseInt(b.sdn_rl.idx);
	});
	$.each(_sdn_all_rl_json, function(index, sdn_all_rl){
		var sdn_profile = sdn_all_rl.sdn_rl;
		if(!$.isEmptyObject(sdn_profile)){
			if(sdn_profile.idx == "0")
				return;
			sdn_rl += "<";
			sdn_rl += sdn_profile.idx;
			sdn_rl += ">";
			sdn_rl += sdn_profile.sdn_name;
			sdn_rl += ">";
			sdn_rl += sdn_profile.sdn_enable;
			sdn_rl += ">";
			sdn_rl += sdn_profile.vlan_idx;
			sdn_rl += ">";
			sdn_rl += sdn_profile.subnet_idx;
			sdn_rl += ">";
			sdn_rl += sdn_profile.apg_idx;
			sdn_rl += ">";
			sdn_rl += sdn_profile.vpnc_idx;
			sdn_rl += ">";
			sdn_rl += sdn_profile.vpns_idx;
			sdn_rl += ">";
			sdn_rl += sdn_profile.dns_filter_idx;
			sdn_rl += ">";
			sdn_rl += sdn_profile.urlf_idx;
			sdn_rl += ">";
			sdn_rl += sdn_profile.nwf_idx;
			sdn_rl += ">";
			sdn_rl += sdn_profile.cp_idx;
			sdn_rl += ">";
			sdn_rl += sdn_profile.gre_idx;
			sdn_rl += ">";
			sdn_rl += sdn_profile.firewall_idx;
			sdn_rl += ">";
			sdn_rl += sdn_profile.kill_switch;
			sdn_rl += ">";
			sdn_rl += sdn_profile.access_host_service;
			sdn_rl += ">";
			sdn_rl += sdn_profile.wan_unit;
			sdn_rl += ">";
			sdn_rl += sdn_profile.pppoe_relay;
			sdn_rl += ">";
			sdn_rl += sdn_profile.wan6_unit;
			sdn_rl += ">";
			sdn_rl += sdn_profile.createby;
			sdn_rl += ">";
			sdn_rl += sdn_profile.mtwan_idx;
			sdn_rl += ">";
			sdn_rl += sdn_profile.mswan_idx;
		}

		var subnet_profile = sdn_all_rl.subnet_rl;
		if(!$.isEmptyObject(subnet_profile)){
			subnet_rl += "<";
			subnet_rl += subnet_profile.subnet_idx;
			subnet_rl += ">";
			subnet_rl += subnet_profile.ifname;
			subnet_rl += ">";
			subnet_rl += subnet_profile.addr;
			subnet_rl += ">";
			subnet_rl += subnet_profile.netmask;
			subnet_rl += ">";
			subnet_rl += subnet_profile.dhcp_enable;
			subnet_rl += ">";
			subnet_rl += subnet_profile.dhcp_min;
			subnet_rl += ">";
			subnet_rl += subnet_profile.dhcp_max;
			subnet_rl += ">";
			subnet_rl += subnet_profile.dhcp_lease;
			subnet_rl += ">";
			subnet_rl += subnet_profile.domain_name;
			subnet_rl += ">";
			subnet_rl += subnet_profile.dns;
			subnet_rl += ">";
			subnet_rl += subnet_profile.wins;
			subnet_rl += ">";
			subnet_rl += subnet_profile.dhcp_static;
			subnet_rl += ">";
			subnet_rl += subnet_profile.dhcp_unit;
			subnet_rl += ">";
		}

		var radius_profile = sdn_all_rl.radius_rl;
		if(!$.isEmptyObject(radius_profile)){
			radius_rl += "<";
			radius_rl += radius_profile.radius_idx;
			radius_rl += ">";
			radius_rl += radius_profile.auth_server_1;
			radius_rl += ">";
			radius_rl += radius_profile.auth_port_1;
			radius_rl += ">";
			radius_rl += radius_profile.auth_key_1;
			radius_rl += ">";
			radius_rl += radius_profile.acct_server_1;
			radius_rl += ">";
			radius_rl += radius_profile.acct_port_1;
			radius_rl += ">";
			radius_rl += radius_profile.acct_key_1;
			radius_rl += ">";
			radius_rl += radius_profile.auth_server_2;
			radius_rl += ">";
			radius_rl += radius_profile.auth_port_2;
			radius_rl += ">";
			radius_rl += radius_profile.auth_key_2;
			radius_rl += ">";
			radius_rl += radius_profile.acct_server_2;
			radius_rl += ">";
			radius_rl += radius_profile.acct_port_2;
			radius_rl += ">";
			radius_rl += radius_profile.acct_key_2;
			radius_rl += ">";
		}

		if(!$.isEmptyObject(sdn_all_rl.vlan_rl)){
			_vlan_rl_at_sdn_all_rl.push(sdn_all_rl.vlan_rl);
		}

		var sdn_access_json = sdn_all_rl.sdn_access_rl;
		if(sdn_access_json.length > 0){
			$.each(sdn_access_json, function(index, sdn_access_profile){
				if(!$.isEmptyObject(sdn_access_profile)){
					sdn_access_rl += "<";
					sdn_access_rl += sdn_access_profile.access_sdn_idx;
					sdn_access_rl += ">";
					sdn_access_rl += sdn_access_profile.sdn_idx;
				}
			});
		}
	});

	$.each(_vlan_rl_at_sdn_all_rl, function(index, vlan_profile){
		if(!$.isEmptyObject(vlan_profile)){
			var specific_vlan = _vlan_rl_json.filter(function(item, index, array){
				return (item.vlan_idx == vlan_profile.vlan_idx);
			})[0];

			if(specific_vlan == undefined){
				_vlan_rl_json.push(vlan_profile);
			}
		}
	})
	_vlan_rl_json.sort(function(a, b) {
		return parseInt(a.vlan_idx) - parseInt(b.vlan_idx);
	});

	$.each(_vlan_rl_json, function(index, vlan_profile){
		if(!$.isEmptyObject(vlan_profile)){
			vlan_rl += "<";
			vlan_rl += vlan_profile.vlan_idx;
			vlan_rl += ">";
			vlan_rl += vlan_profile.vid;
			vlan_rl += ">";
			vlan_rl += vlan_profile.port_isolation;
			vlan_rl += ">";
		}
	});

	result.sdn_rl = sdn_rl;
	result.vlan_rl = vlan_rl;
	result.subnet_rl = subnet_rl;
	result.radius_list = radius_rl;
	result.sdn_access_rl = sdn_access_rl;
	return result;
}
function parse_JSONToStr_del_sdn_all_rl(_json_data){
	var json_data_tmp = JSON.parse(JSON.stringify(_json_data));
	var result = {"sdn_rl_x":"", "vlan_rl_x":"", "subnet_rl_x":"", "radius_list_x":""};
	var sdn_rl = "", vlan_rl = "", subnet_rl = "", radius_rl = "";
	json_data_tmp.sort(function(a, b) {
		return parseInt(a.sdn_rl.idx) - parseInt(b.sdn_rl.idx);
	});
	$.each(json_data_tmp, function(index, sdn_all_rl){
		var sdn_profile = sdn_all_rl.sdn_rl;
		if(!$.isEmptyObject(sdn_profile)){
			if(sdn_profile.idx == "0")
				return;
			sdn_rl += "<";
			sdn_rl += sdn_profile.idx;
			sdn_rl += ">";
			sdn_rl += sdn_profile.sdn_name;
			sdn_rl += ">";
			sdn_rl += sdn_profile.sdn_enable;
			sdn_rl += ">";
			sdn_rl += sdn_profile.vlan_idx;
			sdn_rl += ">";
			sdn_rl += sdn_profile.subnet_idx;
			sdn_rl += ">";
			sdn_rl += sdn_profile.apg_idx;
			sdn_rl += ">";
			sdn_rl += sdn_profile.vpnc_idx;
			sdn_rl += ">";
			sdn_rl += sdn_profile.vpns_idx;
			sdn_rl += ">";
			sdn_rl += sdn_profile.dns_filter_idx;
			sdn_rl += ">";
			sdn_rl += sdn_profile.urlf_idx;
			sdn_rl += ">";
			sdn_rl += sdn_profile.nwf_idx;
			sdn_rl += ">";
			sdn_rl += sdn_profile.cp_idx;
			sdn_rl += ">";
			sdn_rl += sdn_profile.gre_idx;
			sdn_rl += ">";
			sdn_rl += sdn_profile.firewall_idx;
			sdn_rl += ">";
			sdn_rl += sdn_profile.kill_switch;
			sdn_rl += ">";
			sdn_rl += sdn_profile.access_host_service;
			sdn_rl += ">";
			sdn_rl += sdn_profile.wan_unit;
			sdn_rl += ">";
			sdn_rl += sdn_profile.pppoe_relay;
			sdn_rl += ">";
			sdn_rl += sdn_profile.wan6_unit;
			sdn_rl += ">";
			sdn_rl += sdn_profile.createby;
			sdn_rl += ">";
			sdn_rl += sdn_profile.mtwan_idx;
			sdn_rl += ">";
			sdn_rl += sdn_profile.mswan_idx;
		}

		var vlan_profile = sdn_all_rl.vlan_rl;
		if(!$.isEmptyObject(vlan_profile)){
			vlan_rl += "<";
			vlan_rl += vlan_profile.vlan_idx;
			vlan_rl += ">";
			vlan_rl += vlan_profile.vid;
			vlan_rl += ">";
			vlan_rl += vlan_profile.port_isolation;
			vlan_rl += ">";
		}

		var subnet_profile = sdn_all_rl.subnet_rl;
		if(!$.isEmptyObject(subnet_profile)){
			subnet_rl += "<";
			subnet_rl += subnet_profile.subnet_idx;
			subnet_rl += ">";
			subnet_rl += subnet_profile.ifname;
			subnet_rl += ">";
			subnet_rl += subnet_profile.addr;
			subnet_rl += ">";
			subnet_rl += subnet_profile.netmask;
			subnet_rl += ">";
			subnet_rl += subnet_profile.dhcp_enable;
			subnet_rl += ">";
			subnet_rl += subnet_profile.dhcp_min;
			subnet_rl += ">";
			subnet_rl += subnet_profile.dhcp_max;
			subnet_rl += ">";
			subnet_rl += subnet_profile.dhcp_lease;
			subnet_rl += ">";
			subnet_rl += subnet_profile.domain_name;
			subnet_rl += ">";
			subnet_rl += subnet_profile.dns;
			subnet_rl += ">";
			subnet_rl += subnet_profile.wins;
			subnet_rl += ">";
			subnet_rl += subnet_profile.dhcp_static;
			subnet_rl += ">";
			subnet_rl += subnet_profile.dhcp_unit;
			subnet_rl += ">";
		}

		var radius_profile = sdn_all_rl.radius_rl;
		if(!$.isEmptyObject(radius_profile)){
			radius_rl += "<";
			radius_rl += radius_profile.radius_idx;
			radius_rl += ">";
			radius_rl += radius_profile.auth_server_1;
			radius_rl += ">";
			radius_rl += radius_profile.auth_port_1;
			radius_rl += ">";
			radius_rl += radius_profile.auth_key_1;
			radius_rl += ">";
			radius_rl += radius_profile.acct_server_1;
			radius_rl += ">";
			radius_rl += radius_profile.acct_port_1;
			radius_rl += ">";
			radius_rl += radius_profile.acct_key_1;
			radius_rl += ">";
			radius_rl += radius_profile.auth_server_2;
			radius_rl += ">";
			radius_rl += radius_profile.auth_port_2;
			radius_rl += ">";
			radius_rl += radius_profile.auth_key_2;
			radius_rl += ">";
			radius_rl += radius_profile.acct_server_2;
			radius_rl += ">";
			radius_rl += radius_profile.acct_port_2;
			radius_rl += ">";
			radius_rl += radius_profile.acct_key_2;
			radius_rl += ">";
		}
	});
	result.sdn_rl_x = sdn_rl;
	result.vlan_rl_x = vlan_rl;
	result.subnet_rl_x = subnet_rl;
	result.radius_list_x = radius_rl;
	return result;
}
function parse_apg_rl_to_apgX_rl(_apg_rl){
	var result = {};
	if(!$.isEmptyObject(_apg_rl)){
		var apg_idx = _apg_rl.apg_idx;
		Object.keys(_apg_rl).forEach(function(key){
			if(key != "apg_idx"){
				result["apg" + apg_idx + "_" + key] = _apg_rl[key];
			}
		});
	}
	return result;
}
function parse_cp_rl_to_cpX_rl(_cp_rl){
	var result = {};
	if(!$.isEmptyObject(_cp_rl)){
		var cp_profile = "";
		cp_profile += "<";
		cp_profile += _cp_rl.enable;
		cp_profile += ">";
		cp_profile += _cp_rl.auth_type;
		cp_profile += ">";
		cp_profile += _cp_rl.conntimeout;
		cp_profile += ">";
		cp_profile += _cp_rl.idle_timeout;
		cp_profile += ">";
		cp_profile += _cp_rl.auth_timeout;
		cp_profile += ">";
		cp_profile += _cp_rl.redirecturl;
		cp_profile += ">";
		cp_profile += _cp_rl.term_of_service;
		cp_profile += ">";
		cp_profile += _cp_rl.bw_limit_ul;
		cp_profile += ">";
		cp_profile += _cp_rl.bw_limit_dl;
		cp_profile += ">";
		cp_profile += _cp_rl.NAS_ID;
		cp_profile += ">";
		cp_profile += _cp_rl.idx_for_customized_ui;
		result["cp" + _cp_rl.cp_idx + "_profile"] = cp_profile;
		result["cp" + _cp_rl.cp_idx + "_local_auth_profile"] =  (_cp_rl.auth_type == "1") ? ("<" + _cp_rl.local_auth_profile) : "";
		result["cp" + _cp_rl.cp_idx + "_radius_profile"] = _cp_rl.radius_profile;
	}
	return result;
}
var aimesh_wifi_band_info = [];
function init_aimesh_wifi_band_info(){
	aimesh_wifi_band_info = [];
	$.each(cfg_clientlist, function(index, node_info){
		var wifi_band_set = {"mac":"", "wifi_band":0};
		var wifi_band_info = httpApi.aimesh_get_node_wifi_band(node_info);
		$.each(wifi_band_info, function(index, item){
			item["used"] = 0;
		});
		wifi_band_info.sort(function(a, b){
			return parseInt(a.band) - parseInt(b.band);
		});
		aimesh_wifi_band_info[node_info.mac] = JSON.parse(JSON.stringify(wifi_band_info));
	});
}
function update_aimesh_wifi_band_info(_except_sdn){
	init_aimesh_wifi_band_info();
	$.each(sdn_all_rl_json, function(index, sdn_all_rl){
		if(_except_sdn != undefined){
			if(sdn_all_rl.sdn_rl.idx == _except_sdn.sdn_rl.idx){
				return;
			}
		}
		if(sdn_all_rl.apg_rl.dut_list != undefined){
			var dut_list_arr = sdn_all_rl.apg_rl.dut_list.split("<");
			$.each(dut_list_arr, function(index, dut_info){
				if(dut_info != ""){
					var dut_info_arr = dut_info.split(">");
					var dut_mac = dut_info_arr[0];
					var dut_wifi_band = parseInt(dut_info_arr[1]);
					//1:2.4G, 2:5G, 4:5GL, 8:5GH, 16:6G
					if(aimesh_wifi_band_info[dut_mac] != undefined){
						$.each(aimesh_wifi_band_info[dut_mac], function(index, wifi_band){
							if(wifi_band.band & dut_wifi_band){
								wifi_band.used += 1;
							}
						});
					}
				}
			});
		}
	});
}
var aimesh_wifi_band_full = {"all":{"band_1": true, "band_2": true}, "node":[]};
function update_aimesh_wifi_band_full(){
	//band_1: 2.4G, band_2: 5G;
	aimesh_wifi_band_full = {"all":{"band_1": true, "band_2": true}, "node":[]};
	Object.keys(aimesh_wifi_band_info).forEach(function(dut_mac){
		aimesh_wifi_band_full.node[dut_mac] = {"band_1": true, "band_2": true, "support_WPA3E": false};
		$.each(aimesh_wifi_band_info[dut_mac], function(index, wifi_band){
			if(wifi_band.band == 1 || wifi_band.band == 2 || wifi_band.band == 4){
				if(wifi_band.used < wifi_band.no_used && wifi_band.used < wifi_band_maximum){
					switch(wifi_band.band){
						case 1:
							aimesh_wifi_band_full.node[dut_mac].band_1 = false;
							aimesh_wifi_band_full.all.band_1 = false;
							break;
						case 2:
						case 4:
							aimesh_wifi_band_full.node[dut_mac].band_2 = false;
							aimesh_wifi_band_full.all.band_2 = false;
							break;
					}
				}
			}
		});
		var specific_node_data = cfg_clientlist.filter(function(item, index, array){
			return (item.mac == dut_mac);
		})[0];
		if(specific_node_data != undefined){
			var node_capability = httpApi.aimesh_get_node_capability(specific_node_data);
			if(node_capability.wpa3_enterprise)
				aimesh_wifi_band_full.node[dut_mac].support_WPA3E = true;
		}
	});
}
function Get_AiMesh_List_Container(){
	var $container = $("<div>");
	$.each(cfg_clientlist, function(index, node_info){
		var $node_cntr = $("<div>").addClass("node_container").attr({"data-node-mac":node_info.mac, "data-support-dutlist":"true"}).appendTo($container);
		var $node_content_cntr = $("<div>").addClass("node_content_container").unbind("click").click(function(e){
			e = e || event;
			e.stopPropagation();
			if($(this).children(".icon_checkbox").hasClass("closed"))
				return;
			$(this).children(".icon_checkbox").toggleClass("clicked");
			var $AiMesh_List_cntr = $(this).closest("[data-container=AiMesh_List]");
			$AiMesh_List_cntr.next(".validate_hint").remove();
			if(!$AiMesh_List_cntr.find(".node_container .icon_checkbox:not(.disabled)").hasClass("clicked")){
				$("<div>").html("<#JS_fieldblank#>").addClass("validate_hint").insertAfter($AiMesh_List_cntr);
				resize_iframe_height();
			}
		}).appendTo($node_cntr);
		var $icon_cb = $("<div>").addClass("icon_checkbox").appendTo($node_content_cntr);
		var $node_info = $("<div>").addClass("node_info").appendTo($node_content_cntr);
		var isReNode = ((index != 0) ? true : false);
		var loc_info = get_location_info(node_info, isReNode);
		var node_name_loc = get_model_name(node_info) + " (" + loc_info.loc_text + ")";
		var node_ip_mac = node_info.mac + " | " + ( (node_info.online == "1") ? node_info.ip : "<#Clientlist_OffLine#>");
		$("<div>").addClass("node_main_info").attr({"title":node_name_loc}).html($("<span>").html(htmlEnDeCode.htmlEncode(node_name_loc))).appendTo($node_info);
		$("<div>").addClass("node_sub_info").html(htmlEnDeCode.htmlEncode(node_ip_mac)).appendTo($node_info);
		$("<div>").addClass("icon_location " + loc_info.loc_css + "").appendTo($node_content_cntr);
		var wifi_band_info = httpApi.aimesh_get_node_wifi_band(node_info);
		if(wifi_band_info.length == 0){
			var hint = "* <#AiMesh_Modellist_Not_Support_Feature01#> <#AiMesh_Modellist_Not_Support_Feature03#>".replace("#MODELLIST", get_model_name(node_info));
			$("<div>").addClass("node_hint").html(hint).appendTo($node_cntr);
			$icon_cb.addClass("closed");
			$node_cntr.attr({"data-support-dutlist":"false"})
		}
	});
	return $container;

	function get_location_info(_node_info, _isReNode){
		if(typeof aimesh_location_arr != "object"){
			var aimesh_location_arr = [
				{value:"Home",text:"<#AiMesh_NodeLocation01#>"}, {value:"Living Room",text:"<#AiMesh_NodeLocation02#>"}, {value:"Dining Room",text:"<#AiMesh_NodeLocation03#>"},
				{value:"Bedroom",text:"<#AiMesh_NodeLocation04#>"}, {value:"Office",text:"<#AiMesh_NodeLocation05#>"}, {value:"Stairwell",text:"<#AiMesh_NodeLocation06#>"},
				{value:"Hall",text:"<#AiMesh_NodeLocation07#>"}, {value:"Kitchen",text:"<#AiMesh_NodeLocation08#>"}, {value:"Attic",text:"<#AiMesh_NodeLocation09#>"},
				{value:"Basement",text:"<#AiMesh_NodeLocation10#>"}, {value:"Yard",text:"<#AiMesh_NodeLocation11#>"}, {value:"Master Bedroom",text:"<#AiMesh_NodeLocation12#>"},
				{value:"Guest Room",text:"<#AiMesh_NodeLocation13#>"}, {value:"Kids Room",text:"<#AiMesh_NodeLocation14#>"}, {value:"Study Room",text:"<#AiMesh_NodeLocation15#>"},
				{value:"Hallway",text:"<#AiMesh_NodeLocation16#>"}, {value:"Walk-in Closet",text:"<#AiMesh_NodeLocation17#>"}, {value:"Bathroom",text:"<#AiMesh_NodeLocation18#>"},
				{value:"First Floor",text:"<#AiMesh_NodeLocation26#>"}, {value:"Second Floor",text:"<#AiMesh_NodeLocation19#>"}, {value:"Third Floor",text:"<#AiMesh_NodeLocation20#>"},
				{value:"Storage",text:"<#AiMesh_NodeLocation21#>"}, {value:"Balcony",text:"<#AiMesh_NodeLocation22#>"}, {value:"Meeting Room",text:"<#AiMesh_NodeLocation23#>"},
				{value:"Garage",text:"<#AiMesh_NodeLocation25#>"}, {value:"Gaming Room",text:"<#AiMesh_NodeLocation27#>"}, {value:"Gym",text:"<#AiMesh_NodeLocation28#>"},
				{value:"Custom",text:"<#AiMesh_NodeLocation24#>"}
			];
		}
		var result = {"loc_text":"<#AiMesh_NodeLocation01#>", "loc_css":"Home"};
		var loc_text = "Home";
		if(_isReNode){
			if("config" in _node_info) {
				if("misc" in _node_info.config) {
					if("cfg_alias" in _node_info.config.misc) {
						if(_node_info.config.misc.cfg_alias != "")
							loc_text = _node_info.config.misc.cfg_alias;
					}
				}
			}
		}
		else{
			var alias = _node_info.alias;
			if(alias != _node_info.mac)
				loc_text = alias;
		}
		var specific_loc = aimesh_location_arr.filter(function(item, index, _array){
			return (item.value == loc_text);
		})[0];
		if(specific_loc != undefined){
			result.loc_css = specific_loc.value.replace(/\s/g,'_');
			result.loc_text = specific_loc.text;
		}
		else{
			result.loc_css = "Custom";
			result.loc_text = loc_text;
		}
		return result;
	}
	function get_model_name(_node_info){
		var result = "";
		if(_node_info.ui_model_name == undefined || _node_info.ui_model_name == "")
			result = _node_info.model_name;
		else
			result = _node_info.ui_model_name;
		return result;
	}
}
function Get_AiMesh_Offline_Container(_dut_info_arr){
	var node_mac = _dut_info_arr[0];
	var wifi_band = _dut_info_arr[1];
	var $container = $("<div>");

	var $node_cntr = $("<div>").addClass("node_container").attr({"data-node-mac":node_mac, "data-wifi-band":wifi_band, "data-node-status":"offline"}).appendTo($container);
	var $node_content_cntr = $("<div>").addClass("node_content_container").appendTo($node_cntr);
	var $icon_cb = $("<div>").addClass("icon_checkbox disabled").appendTo($node_content_cntr);
	var $node_info = $("<div>").addClass("node_info").appendTo($node_content_cntr);
	$("<div>").addClass("node_main_info").attr({"title":node_mac}).html($("<span>").html(htmlEnDeCode.htmlEncode("Offline AiMesh nodes"))).appendTo($node_info);
	$("<div>").addClass("node_sub_info").html(htmlEnDeCode.htmlEncode(node_mac)).appendTo($node_info);
	$("<div>").addClass("icon_location Home").appendTo($node_content_cntr);

	var hint = "* <#FW_note_AiMesh_offline#>";
	$("<div>").addClass("node_hint").html(hint).appendTo($node_cntr);

	return $container;
}
function Set_AiMesh_List_CB(_obj, _sel_wifi_bnad){
	var wifi_radius_is_WPA3E = (function(){
		var result = false;
		if(isSupport("wpa3-e")){
			var sec_option_id = $(_obj).closest(".profile_setting").find("#security_employee .switch_text_container").children(".selected").attr("data-option-id");
			if(sec_option_id == "radius"){
				var wifi_auth_radius = $(_obj).closest(".profile_setting").find("#select_wifi_auth_radius").children(".selected").attr("value");
				if(wifi_auth_radius == "wpa3" || wifi_auth_radius == "wpa2wpa3" || wifi_auth_radius == "suite-b")
					result = true;
			}
		}
		return result;
	})();
	Object.keys(aimesh_wifi_band_info).forEach(function(dut_mac){
		var band_1 = aimesh_wifi_band_full.node[dut_mac].band_1;
		var band_2 = aimesh_wifi_band_full.node[dut_mac].band_2;
		var support_WPA3E = aimesh_wifi_band_full.node[dut_mac].support_WPA3E;
		var $node_cntr = $(_obj).find("[data-node-mac='" + dut_mac + "']");
		if(aimesh_wifi_band_info[dut_mac].length > 0){//node support capability wifi_band
			$node_cntr.find(".node_hint").remove();
			$node_cntr.find(".icon_checkbox").removeClass().addClass("icon_checkbox");
			if(_sel_wifi_bnad == "3"){
				if(!band_1 && !band_2){
					if(wifi_radius_is_WPA3E){
						if(support_WPA3E)
							$node_cntr.find(".icon_checkbox").addClass("clicked");
						else
							set_node_cb_closed_WPA3E();
					}
					else
						$node_cntr.find(".icon_checkbox").addClass("clicked");
				}
				else{
					set_node_cb_closed();
				}
			}
			else if(_sel_wifi_bnad == "1"){
				if(!band_1){
					if(wifi_radius_is_WPA3E){
						if(support_WPA3E)
							$node_cntr.find(".icon_checkbox").addClass("clicked");
						else
							set_node_cb_closed_WPA3E();
					}
					else
						$node_cntr.find(".icon_checkbox").addClass("clicked");
				}
				else{
					set_node_cb_closed();
				}
			}
			else if(_sel_wifi_bnad == "2"){
				if(!band_2){
					if(wifi_radius_is_WPA3E){
						if(support_WPA3E)
							$node_cntr.find(".icon_checkbox").addClass("clicked");
						else
							set_node_cb_closed_WPA3E();
					}
					else
						$node_cntr.find(".icon_checkbox").addClass("clicked");
				}
				else{
					set_node_cb_closed();
				}
			}
			var specific_node = cfg_clientlist.filter(function(item, index, array){
				return (item.mac == dut_mac);
			})[0];
			if(specific_node != undefined){
				if(specific_node.online != "1")
					set_node_cb_closed_offline();
			}
		}
		function set_node_cb_closed(){
			$node_cntr.find(".icon_checkbox").addClass("closed");
			var wifi_text = "";
			if(band_1)
				wifi_text += "2.4GHz";
			if(band_2){
				if(wifi_text != "")
					wifi_text += " / "
				wifi_text += "5GHz";
			}
			$("<div>").addClass("node_hint")
				.html(htmlEnDeCode.htmlEncode("* The WiFi interface has reached the maximum. (" + wifi_text + ")"))/* untranslated */
				.appendTo($node_cntr);
		}
		function set_node_cb_closed_WPA3E(){
			$node_cntr.find(".icon_checkbox").addClass("closed");
			$("<div>").addClass("node_hint")
				.html(htmlEnDeCode.htmlEncode("* The Router do not support WPA3-Enterprise."))/* untranslated */
				.appendTo($node_cntr);
		}
		function set_node_cb_closed_offline(){
			var node_is_setted = $node_cntr.attr("data-profile-setting");
			if(typeof node_is_setted == "undefined" && node_is_setted != "true") {
				$node_cntr.find(".icon_checkbox").removeClass("clicked").addClass("closed");
			}
			$("<div>").addClass("node_hint")
				.html(htmlEnDeCode.htmlEncode("* <#AiMesh_Features_Disabled_When_Offline#>"))
				.appendTo($node_cntr);
		}
	});
}
function Get_Component_VPN_Profiles(){
	var $container = $("<div>").addClass("VPN_list_container").attr({"data-container": "VPN_Profiles"});

	var $vpnc_group = $("<div>").addClass("profile_group").appendTo($container);
	$("<div>").addClass("title").html("<#vpnc_title#>").appendTo($vpnc_group);
	var vpnc_options = [];
	vpnc_profile_list.forEach(function(item){
		vpnc_options.push({"type":"vpnc", "text":item.desc, "value":item.vpnc_idx, "proto":item.proto, "activate":item.activate});
	});
	if(vpnc_options.length > 0){
		var $content_cntr = $("<div>").addClass("profile_container").appendTo($vpnc_group);
		$.each(vpnc_options, function(index, value){
			Get_Component_VPN(value).appendTo($content_cntr);
		});
	}
	else{
		$("<div>").addClass("profile_hint").html(str_noProfile_hint).appendTo($vpnc_group);
	}
	var $btn_cntr = $("<div>").addClass("profile_btn_container").unbind("click").click(function(e){
		e = e || event;
		e.stopPropagation();
		if(is_Web_iframe){
			top.location.href = "/Advanced_VPNClient_Content.asp";
		}
		else if(parent.businessWrapper){
			top.location.href = "/index.html?url=vpnc&current_theme=business";
		}
		else{
			top.location.href = "/VPN/vpnc.html" + ((typeof theme == "string" && theme != "") ? "?current_theme=" + theme + "" : "");
		}
	}).appendTo($vpnc_group);
	$("<div>").html("<#btn_goSetting#>").appendTo($btn_cntr);
	$("<div>").addClass("icon_arrow_right").appendTo($btn_cntr);

	if(vpns_rl_json.length > 0){
		var $vpns_group = $("<div>").addClass("profile_group").appendTo($container);
		$("<div>").addClass("title").html("<#BOP_isp_heart_item#>").appendTo($vpns_group);
		var $profile_cntr = $("<div>").addClass("profile_container").appendTo($vpns_group);
		$.each(vpns_rl_json, function(index, vpn_type){
			Get_Component_VPN({"type":"vpns", "text":vpn_type.text, "value":vpn_type.vpns_idx, "activate":vpn_type.activate}).appendTo($profile_cntr);
		});
		var $btn_cntr = $("<div>").addClass("profile_btn_container").unbind("click").click(function(e){
			e = e || event;
			e.stopPropagation();
			if(is_Web_iframe){
				top.location.href = "/Advanced_VPNServer_Content.asp";
			}
			else if(parent.businessWrapper){
				top.location.href = "/index.html?url=vpns&current_theme=business";
			}
			else{
				top.location.href = "/VPN/vpns.html" + ((typeof theme == "string" && theme != "") ? "?current_theme=" + theme + "" : "");
			}
		}).appendTo($vpns_group);
		$("<div>").html("<#btn_goSetting#>").appendTo($btn_cntr);
		$("<div>").addClass("icon_arrow_right").appendTo($btn_cntr);
	}
	if($container.find(".icon_radio.clicked").length == 0){
		$container.find(".icon_radio[data-vpn-type=vpnc]").first().addClass("clicked");
	}
	return $container;

	function Get_Component_VPN(_profile){
		var $profile_cntr = $("<div>").addClass("profile_item")
			.unbind("click").click(function(e){
				e = e || event;
				e.stopPropagation();
				if($(this).find(".icon_radio").attr("data-disabled") == "true")
					return false;
				$(this).closest("[data-container=VPN_Profiles]").find(".icon_radio").removeClass("clicked");
				$(this).find(".icon_radio").addClass("clicked");
			});
		var $text_container = $("<div>").addClass("text_container").appendTo($profile_cntr)
		$("<div>").html(htmlEnDeCode.htmlEncode(_profile.text)).appendTo($text_container);

		var $select_container = $("<div>").addClass("select_container").appendTo($profile_cntr);
		$("<div>").addClass("icon_warning_amber").attr({"data-component":"icon_warning"}).unbind("click").click(function(e){
			e = e || event;
			e.stopPropagation();
			$(this).closest("[data-container=VPN_Profiles]").find(".error_hint_container").remove();
			var error_hint = "<#GuestNetwork_VPN_errHint0#> <#GuestNetwork_VPN_errHint1#>";
			$(this).closest(".profile_item").append(Get_Component_Error_Hint({"text":error_hint}).show());
		}).hide().appendTo($select_container);

		$("<div>").addClass("icon_error_outline").attr({"data-component":"icon_error"}).unbind("click").click(function(e){
			e = e || event;
			e.stopPropagation();
			$(this).closest("[data-container=VPN_Profiles]").find(".error_hint_container").remove();
			var error_hint = "<#GuestNetwork_VPN_errHint2#>";
			$(this).closest(".profile_item").append(Get_Component_Error_Hint({"text":error_hint}).show());
		}).hide().appendTo($select_container);

		$("<div>").addClass("icon_radio").attr({"data-idx":_profile.value, "data-vpn-type":_profile.type}).appendTo($select_container);
		return $profile_cntr;
	}
}
var client_upload_icon_cache = [];
function Get_Component_ClientList(_dataArray){
	if(_dataArray.length == 0){
		return $("<div>").addClass("no_client_hint").html("<#AiMesh_No_Device_Connected#>");
	}
	else {
		var uploadIconMacList = getUploadIconList().replace(/\.log/g, "");
		var $clientlist_cntr = $("<div>");
		for(var i = 0; i < _dataArray.length; i += 1){
			var clientObj = _dataArray[i];
			var $client_cntr = $("<div>").addClass("client_container").appendTo($clientlist_cntr);
			var $client_icon_cntr = $("<div>").addClass("client_icon_container").appendTo($client_cntr);
			var userIconBase64 = "NoIcon";
			var deviceTitle = (clientObj.dpiDevice == "") ? clientObj.vendor : clientObj.dpiDevice;
			var isUserUpload = (uploadIconMacList.indexOf(clientObj.mac.toUpperCase().replace(/\:/g, "")) >= 0) ? true : false;
			if(isSupport("usericon")){
				if(client_upload_icon_cache[clientObj.mac] == undefined){
					var clientMac = clientObj.mac.replace(/\:/g, "");
					var queryString = "get_upload_icon()&clientmac=" + clientMac;
					$.ajax({
						url: '/appGet.cgi?hook=' + queryString,
						dataType: 'json',
						async: false,
						success: function(response){
							userIconBase64 = response.get_upload_icon;
						}
					});
					client_upload_icon_cache[clientObj.mac] = userIconBase64;
				}
				else
					userIconBase64 = client_upload_icon_cache[clientObj.mac];
			}
			if(userIconBase64 != "NoIcon"){
				if(isUserUpload) {
					$client_icon_cntr.addClass("user_icon").css("background-image", "url(" + userIconBase64 + ")");
				}else{
					$client_icon_cntr.addClass("user_icon").append($('<i>').addClass('type').attr('style','--svg:url(' + userIconBase64 + ')'));
				}
			}
			else if(clientObj.type != "0" || clientObj.vendor == ""){
				var icon_type = "type" + clientObj.type;
				$client_icon_cntr.addClass("default_icon");
				if($client_icon_cntr.find('i').length==0){
					$client_icon_cntr.append($('<i>').addClass(icon_type));
				}
				if(clientObj.type == "36"){
					$("<div>").addClass("flash").appendTo($client_icon_cntr);
				}
			}
			else if(clientObj.vendor != ""){
				var vendorIconClassName = getVendorIconClassName(clientObj.vendor.toLowerCase());
				if(vendorIconClassName != "" && !downsize_4m_support) {
					$client_icon_cntr.addClass("default_icon");
					$client_icon_cntr.append($('<i>').addClass("vendor-icon " + vendorIconClassName));
				}
				else {
					var icon_type = "type" + clientObj.type;
					$client_icon_cntr.addClass("default_icon");
					$client_icon_cntr.append($('<i>').addClass(icon_type));
				}
			}
			var $client_content_cntr = $("<div>").addClass("client_content_container").appendTo($client_cntr);
			var $main_info = $("<div>").addClass("main_info").appendTo($client_content_cntr);
			var client_name = (clientObj.nickName == "") ? clientObj.name : clientObj.nickName;
			if(client_name == "")
				client_name = clientObj.mac;
			$("<div>").attr("title", client_name).addClass("client_name").html(htmlEnDeCode.htmlEncode(client_name)).appendTo($main_info);
			var $sub_info = $("<div>").addClass("sub_info").appendTo($client_content_cntr);
			var $conn_type = $("<div>").addClass("conn_type").appendTo($sub_info);
			switch(parseInt(clientObj.isWL)){
				case 0:
					$conn_type.addClass("LAN");
					break;
				case 1:
					$conn_type.addClass("WiFi_2G");
					break;
				case 2:
				case 3:
					$conn_type.addClass("WiFi_5G");
					break;
				case 4:
					$conn_type.addClass("WiFi_6G");
					break;
			}
			$("<div>").html(htmlEnDeCode.htmlEncode(clientObj.mac)).appendTo($sub_info);
			$("<div>").html(htmlEnDeCode.htmlEncode(clientObj.ip)).appendTo($sub_info);
		}
		return $clientlist_cntr;
	}
}
function checkImageExtension(imageFileObj){
	var picExtension = /\.(jpg|jpeg|png|svg)$/i;
	return ((picExtension.test(imageFileObj)) ? true : false);
}
function uploadFreeWiFi(_setting){
	var splash_page_setting = {
		'splash_page_setting':{
			'image': _setting.image,
			"continue_btn": encode_decode_text('<#FreeWiFi_Continue#>', 'encode'),
			"auth_type": encode_decode_text(_setting.auth_type, 'encode')
		}
	};
	if(_setting.cp_idx == "2"){
		splash_page_setting.splash_page_setting["portal_type"] = "2";
		splash_page_setting.splash_page_setting["brand_name"] = encode_decode_text(_setting.brand_name, "encode");
		splash_page_setting.splash_page_setting["terms_service_content"] = encode_decode_text(_setting.terms_service, "encode");
		splash_page_setting.splash_page_setting["terms_service_cb_text"] = encode_decode_text("<#FreeWiFi_Agree_Terms_Service#>", "encode");
		splash_page_setting.splash_page_setting["terms_service_title"] = encode_decode_text("<#Captive_Portal_Terms_Service#>", 'encode');
		splash_page_setting.splash_page_setting["agree_btn"] = encode_decode_text("<#CTL_Agree#>", 'encode');
		if(_setting.auth_type == "1"){
			splash_page_setting.splash_page_setting["passcode_blank"] = encode_decode_text('<#FreeWiFi_Passcode#> : <#JS_fieldblank#>', 'encode');
			splash_page_setting.splash_page_setting["passcode_placeholder"] = encode_decode_text('Enter Passcode', 'encode');
		}
	}
	else if(_setting.cp_idx == "4"){
		splash_page_setting.splash_page_setting["portal_type"] = "4";
		splash_page_setting.splash_page_setting["MB_desc"] = encode_decode_text(_setting.MB_desc, "encode");
	}
	var postData = {
		"splash_page_id": _setting.id,
		"splash_page_attribute": JSON.stringify(splash_page_setting)
	};
	httpApi.uploadFreeWiFi(postData);
}
function encode_decode_text(_string, _type) {
	var _string_temp = _string;
	switch(_type) {
		case "encode" :
			//escaped character
			 _string_temp = _string_temp.replace(/"/g, '\"').replace(/\\/g, "\\");
			//replace new line
			_string_temp = _string_temp.replace(/(?:\r\n|\r|\n)/g, '<br>');
			//encode ASCII
			_string_temp = encodeURIComponent(_string_temp).replace(/[!'()*]/g, escape);
			break;
		case "decode" :
			_string_temp = decodeURIComponent(_string_temp);
			_string_temp = _string_temp.replace(/\<br\>/g, "\n");
			break
	}
	return _string_temp;
}
function convertRulelistToJson(attrArray, rulelist){
	var rulelist_json = [];
	var each_rule = rulelist.split("<");
	var convertAtoJ = function(rule_array){
		var rule_json = {}
		$.each(attrArray, function(index, value){
			rule_json[value] = rule_array[index];
		});
		return rule_json;
	}
	$.each(each_rule, function(index, value){
		if(value != ""){
			var one_rule_array = value.split(">");
			var one_rule_json = convertAtoJ(one_rule_array);
			if(!one_rule_json.error) rulelist_json.push(one_rule_json);
		}
	});
	return rulelist_json;
}

//vlan_trunklist=<0C:9D:92:47:06:50>1#51,91>4#53
function update_vlan_trunklist(update_profile, _new_vid){
	
	var trunklist = "";
	var target_term11 = "#"+update_profile.vid+",";
	var target_term12 = "#"+update_profile.vid+">";
	var target_term13 = "#"+update_profile.vid+"<";
	var target_term14 = "#"+update_profile.vid+"_";
	var target_term21 = ","+update_profile.vid+",";
	var target_term22 = ","+update_profile.vid+">";
	var target_term23 = ","+update_profile.vid+"<";
	var target_term24 = ","+update_profile.vid+"_";
	var replace_term11 = "#"+_new_vid+",";
	var replace_term12 = "#"+_new_vid+">";
	var replace_term13 = "#"+_new_vid+"<";
	var replace_term14 = "#"+_new_vid+"_";
	var replace_term21 = ","+_new_vid+",";
	var replace_term22 = ","+_new_vid+">";
	var replace_term23 = ","+_new_vid+"<";
	var replace_term24 = ","+_new_vid+"_";

	trunklist = vlan_trunklist_tmp.replaceAll(target_term11, replace_term11);
	trunklist = trunklist.replaceAll(target_term12, replace_term12);
	trunklist = trunklist.replaceAll(target_term13, replace_term13);
	trunklist = trunklist.replaceAll(target_term14, replace_term14);
	trunklist = trunklist.replaceAll(target_term21, replace_term21);
	trunklist = trunklist.replaceAll(target_term22, replace_term22);
	trunklist = trunklist.replaceAll(target_term23, replace_term23);
	trunklist = trunklist.replaceAll(target_term24, replace_term24);
	if(trunklist[trunklist.length - 1] == "_"){	//remove last "_" char
		trunklist = trunklist.substring(0, trunklist.length - 1);
	}

	vlan_trunklist_tmp = trunklist+"_";	//eazy to replace
	return trunklist;
}

function rm_vid_from_vlan_trunklist(_rm_vid){

	var trunklist = "";
	//case: remove choosed vid
	$.each(vlan_trunklist_json, function(idx, items) {

		if(vlan_trunklist_json[idx].profile == _rm_vid){
			//alert(vlan_trunklist_json[idx].mac+"/"+vlan_trunklist_json[idx].port);		//matched mac & port

			for(var z=vlan_trunklist_port_array_one_mac.length-1; z>0; z--){	//by mac
				if(vlan_trunklist_json[idx].mac == vlan_trunklist_port_array_one_mac[z][0]){

					for(var x=vlan_trunklist_port_array_one_mac[z].length-1; x>0; x--){
						if(vlan_trunklist_port_array_one_mac[z][x].indexOf(vlan_trunklist_json[idx].port+"#") >= 0){
							vlan_trunklist_port_array_one_mac[z].splice(x, 1);
						}
					}
				}
			}

			vlan_trunklist_json_tmp.splice(idx, 1);
		}
	});
	vlan_trunklist_json = vlan_trunklist_json_tmp;	//sync

	vlan_trunklist_tmp = "";	//clean vlan_trunklist_tmp
	for(var t=1; t<vlan_trunklist_port_array_one_mac.length; t++){

		if(vlan_trunklist_port_array_one_mac[t].length >= 2){	//remove mac if no lan port binded
			for(var s=0; s<vlan_trunklist_port_array_one_mac[t].length; s++){
				vlan_trunklist_tmp += (s==0)?"<":">";
				vlan_trunklist_tmp += vlan_trunklist_port_array_one_mac[t][s];
			}
		}
	}

	vlan_trunklist_tmp += "_";	//eazy to replace

	var target_term11 = "#"+_rm_vid+",";
	var target_term12 = "#"+_rm_vid+">";
	var target_term13 = "#"+_rm_vid+"<";
	var target_term14 = "#"+_rm_vid+"_";
	var target_term21 = ","+_rm_vid+",";
	var target_term22 = ","+_rm_vid+">";
	var target_term23 = ","+_rm_vid+"<";
	var target_term24 = ","+_rm_vid+"_";	
	var replace_term11 = "#";
	var replace_term12 = ">";
	var replace_term13 = "<";
	var replace_term14 = "_";
	var replace_term21 = ",";
	var replace_term22 = ">";
	var replace_term23 = "<";
	var replace_term24 = "_";
	
	//case: remove only one vid 
	trunklist = vlan_trunklist_tmp.replaceAll(target_term11, replace_term11);
	for(var x=1; x<=8; x++){
		trunklist = trunklist.replaceAll(x+target_term12, replace_term12);
		trunklist = trunklist.replaceAll(x+target_term13, replace_term13);
		trunklist = trunklist.replaceAll(x+target_term14, replace_term14);
	}
	trunklist = trunklist.replaceAll(target_term21, replace_term21);
	trunklist = trunklist.replaceAll(target_term22, replace_term22);
	trunklist = trunklist.replaceAll(target_term23, replace_term23);
	trunklist = trunklist.replaceAll(target_term24, replace_term24);
	if(trunklist[trunklist.length - 1] == "_"){	//remove last "_" char
		trunklist = trunklist.substring(0, trunklist.length - 1);
	}

	vlan_trunklist_tmp = trunklist+"_";	//eazy to replace
	return trunklist;
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
	var profile_sec = (((sdn_all_rl_json.length - 2) > 0) ? ((sdn_all_rl_json.length - 2) * 5) : 0);
	result.time += profile_sec;//profile
	return result;
}
function check_isAlive_and_redirect(_parm){
	var page = "";
	var time = (isWLclient()) ? 35 : 30;
	var interval_time = 2;
	if(_parm != undefined){
		if(_parm.page != undefined && _parm.page != "") page = _parm.page;
		if(_parm.time != undefined && _parm.time != "" && !isNaN(_parm.time)) time = parseInt(_parm.time);
	}
	if(parent.businessWrapper){
		if(page == "SDN.asp")
			page = "sdn";
	}
	var lan_ipaddr = httpApi.nvramGet(["lan_ipaddr"]).lan_ipaddr;
	setTimeout(function(){
		var interval_isAlive = setInterval(function(){
			httpApi.isAlive("", lan_ipaddr,
				function(){
					clearInterval(interval_isAlive);
					if(parent.businessWrapper){
						top.pageRedirect(page);
					}
					else{
						top.location.href = "/" + page + "";
					}
				});
		}, 1000*interval_time);
	}, 1000*(time - interval_time));
}
function secondsToHMS(sec){
	var sec = Number(sec);
	var h = Math.floor(sec / (60 * 60));
	var m = Math.floor(sec % (60 * 60) / 60);
	var s = Math.floor(sec % (60 * 60) % 60);
	var result = {"hours": 0, "minutes": 0, "seconds": 0};
	result.hours = h > 0 ? h : 0;
	result.minutes = m > 0 ? m : 0;
	result.seconds = s > 0 ? s : 0;
	result.hours = num_add_left_pad(result.hours, 2);
	result.minutes = num_add_left_pad(result.minutes, 2);
	result.seconds = num_add_left_pad(result.seconds, 2);
	return result;

	function num_add_left_pad(_num, _len){
		if(_len == undefined) _len = 2;
		return ("0".repeat(_len) + _num).slice(-_len);
	}
}
function trigger_keyup(inputObj){
	$(inputObj).keyup();
}
