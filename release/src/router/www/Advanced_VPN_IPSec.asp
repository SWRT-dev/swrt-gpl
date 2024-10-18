<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml">
<html xmlns:v>
<head>
<meta http-equiv="X-UA-Compatible" content="IE=Edge"/>
<meta http-equiv="Content-Type" content="text/html; charset=utf-8" />
<meta HTTP-EQUIV="Pragma" CONTENT="no-cache">
<meta HTTP-EQUIV="Expires" CONTENT="-1">
<link rel="shortcut icon" href="images/favicon.png">
<link rel="icon" href="images/favicon.png">
<title><#Web_Title#> - IP Sec</title>
<link rel="stylesheet" type="text/css" href="index_style.css"> 
<link rel="stylesheet" type="text/css" href="form_style.css">
<link rel="stylesheet" type="text/css" href="pwdmeter.css">
<script type="text/javascript" src="/js/jquery.js"></script>
<script type="text/javascript" src="state.js"></script>
<script type="text/javascript" src="general.js"></script>
<script type="text/javascript" src="popup.js"></script>
<script type="text/javascript" src="help.js"></script>
<script language="JavaScript" type="text/javascript" src="form.js"></script>
<script language="JavaScript" type="text/javascript" src="validator.js"></script>
<script language="JavaScript" type="text/javascript" src="switcherplugin/jquery.iphone-switch.js"></script>
<script type="text/javascript" src="/js/httpApi.js"></script>
<style type="text/css">
.ipsec_view_log_panel {
	width: 720px;
	height: auto;
	margin-top: 65px;
	margin-left: 5px;
	position: absolute;
	-webkit-border-radius: 5px;
	-moz-border-radius: 5px;
	border-radius: 5px;
	z-index: 200;
	background-color: #2B373B;
	box-shadow: 3px 3px 10px #000;
	display: none;
	overflow: auto;
}
.ipsec_connect_status_title_bg {
	background: #C1C1C1;
	height: 25px;
	border-radius: 5px 5px 0 0;
	font-weight: bold;
	font-family: Verdana, Arial, Helvetica;
	color: rgb(119, 119, 119);
	font-size: x-small;
}
.ipsec_connect_status_title {
	float: left;
	width: 120px;
	text-align: center;
	margin: 5px 0;
}
.ipsec_connect_status_close {
	float: right;
	cursor: pointer;
}
.ipsec_connect_status_content_bg {
	height: 25px;
	color: #000000;
}
.ipsec_connect_status_content {
	font-family: Verdana, Arial, Helvetica;
	color: rgb(0, 0, 0);
	font-size: small;
	float: left;
	width: 120px;
	text-align: center;
	margin: 3px 0;
}
.ipsec_connect_status_panel {
	display: none;
	background: #E4E5E6;
	opacity:.95;
	border-radius: 5px;
	-webkit-border-radius: 5px;
	-moz-border-radius: 5px;
	border-radius: 5px;
	-moz-box-shadow: 3px 3px 4px #333;
	-webkit-box-shadow: 3px 3px 4px #333;
	box-shadow: 3px 3px 4px #000;
	-ms-filter: "progid:DXImageTransform.Microsoft.Shadow(Strength=4, Direction=135, Color='#333333')";
	filter: progid:DXImageTransform.Microsoft.Shadow(Strength=4, Direction=135, Color='#333333');
}
.renewLoadingIcon {
	background-image: url(/images/InternetScan.gif);
	width: 125px;
	height: 33px;
	background-repeat: no-repeat;
	background-position: 50%;
	display: none;
}
.faq_link {
	margin-right: 10px;
}
#preshared_key_strength{
	margin-left: 254px;
	margin-top: -25px;
	position: absolute;
}
#client_pwd_strength{
	margin-top: 6px;
	display: flex;
	justify-content: center;
}
</style>
<script>
<% wanlink(); %>
var ipsec_profile_1 = decodeURIComponent('<% nvram_char_to_ascii("","ipsec_profile_1"); %>');
var ipsec_client_list_json = "";
var ipsec_server_enable = '<% nvram_get("ipsec_server_enable"); %>';
var ipsec_connect_status_array = new Array();
var ddns_enable_x = '<% nvram_get("ddns_enable_x"); %>';
var ddns_hostname_x = '<% nvram_get("ddns_hostname_x"); %>';
var conn_name_array = new Array();

var faq_href_windows = "https://nw-dlcdnet.asus.com/support/forward.html?model=&type=Faq&lang="+ui_lang+"&kw=&num=114";
var faq_href_macOS = "https://nw-dlcdnet.asus.com/support/forward.html?model=&type=Faq&lang="+ui_lang+"&kw=&num=115";
var faq_href_iPhone = "https://nw-dlcdnet.asus.com/support/forward.html?model=&type=Faq&lang="+ui_lang+"&kw=&num=116";
var faq_href_android = "https://nw-dlcdnet.asus.com/support/forward.html?model=&type=Faq&lang="+ui_lang+"&kw=&num=117";
var faq_href_port_forwarding = "https://nw-dlcdnet.asus.com/support/forward.html?model=&type=Faq&lang="+ui_lang+"&kw=&num=118";

function initial(){
	show_menu();

	document.getElementById("ipsec_profile_1").value = ipsec_profile_1;

	var vpn_server_array = { "PPTP" : ["PPTP", "Advanced_VPN_PPTP.asp"], "OpenVPN" : ["OpenVPN", "Advanced_VPN_OpenVPN.asp"], "IPSEC" : ["IPSec VPN", "Advanced_VPN_IPSec.asp"]};
	if(!pptpd_support) {
		delete vpn_server_array.PPTP;
	}
	if(!openvpnd_support) {
		delete vpn_server_array.OpenVPN;
	}
	if(!ipsec_srv_support) {
		delete vpn_server_array.IPSEC;
	}
	$('#divSwitchMenu').html(gen_switch_menu(vpn_server_array, "IPSEC"));

	ipsecShowAndHide(ipsec_server_enable);

	switchSettingsMode("1");

	while(document.form.ipsec_local_public_interface.options.length > 0){
		document.form.ipsec_local_public_interface.remove(0);
	}
	var wan_type_list = [];
	var option = ["wan", "<#dualwan_primary#>"];
	wan_type_list.push(option);
	if(dualWAN_support) {
		option = ["wan2", "<#dualwan_secondary#>"];
		wan_type_list.push(option);
	}

	for(var i = 0; i < wan_type_list.length; i += 1) {
		var option = document.createElement("option");
		option.value = wan_type_list[i][0];
		option.text = wan_type_list[i][1];
		document.form.ipsec_local_public_interface.add(option);
	}
	
	if(ipsec_profile_1 != "") {
		var editProfileArray = [];
		editProfileArray = ipsec_profile_1.split(">");
		editProfileArray.unshift("ipsec_profile_1");
		document.form.ipsec_local_public_interface.value = editProfileArray[5];
		document.form.ipsec_preshared_key.value = editProfileArray[8];
		document.form.ipsec_clients_start.value = editProfileArray[15];
		document.form.ipsec_dpd.value = editProfileArray[31];
		settingRadioItemCheck(document.form.ipsec_dead_peer_detection, editProfileArray[32]);
		var ipsec_samba_array = editProfileArray[37].split("<");
		document.form.ipsec_dns1.value = ipsec_samba_array[1];
		document.form.ipsec_dns2.value = ipsec_samba_array[2];
		document.form.ipsec_wins1.value = ipsec_samba_array[3];
		document.form.ipsec_wins2.value = ipsec_samba_array[4];
	}
	else {
		var ipsecLanIPAddr = "10.10.10.1";
		var ipsecLanNetMask = "255.255.255.0";
		var ipConflict;
		//1.check LAN IP
		ipConflict = checkIPConflict("LAN", ipsecLanIPAddr, ipsecLanNetMask);
		if(ipConflict.state) {
			document.form.ipsec_clients_start.value = "10.10.11";
		}
	}
	var ipsec_block_intranet = httpApi.nvramGet(["ipsec_block_intranet"]).ipsec_block_intranet;
	if(ipsec_block_intranet == "")
		ipsec_block_intranet = "0";
	setRadioValue(document.form.vpn_server_client_access, ipsec_block_intranet);

	changeAdvDeadPeerDetection(document.form.ipsec_dead_peer_detection);
	setClientsEnd();

	httpApi.get_ipsec_clientlist(function(response){
		ipsec_client_list_json = response;
		showipsec_clientlist();
	});

	$("#cert_status").html("<#Authenticated_non#>");
	httpApi.get_ipsec_cert_info(function(response){
		if(response != "") {
			if(response.issueTo != "" && response.issueTo != undefined)
				$("#cert_issueTo").html(response.issueTo);
			if(response.issueBy != "" && response.issueBy != undefined)
				$("#cert_issueBy").html(response.issueBy);
			if(response.expire != "" && response.expire != undefined)
				$("#cert_expire").html(response.expire);
			if($("#cert_issueTo").html() != "" && $("#cert_issueBy").html() != "" && $("#cert_expire").html() != ""){
				$("#cert_status").html("<#Authenticated#>");
				$(".ipsec_inactive").hide();
				$(".button_gen_dis.ipsec_active").removeClass("button_gen_dis").addClass("button_gen");
				$(".button_gen.ipsec_active.export").unbind("click");
				$(".button_gen.ipsec_active.export").click(function(e){export_cert("0");});
				$(".button_gen.ipsec_active.renew").unbind("click");
				$(".button_gen.ipsec_active.renew").click(function(e){export_cert("1");});
				if(response.update_state == "1")
					$(".ipsec_inactive.renew_hint").show().html("<br><#vpn_ipsec_need_re_cert_hint#>");
			}
		}
	});
	if('<% nvram_get("ipsec_server_enable"); %>' == "1")
		update_connect_status();

	//check DUT is belong to private IP.
	setTimeout("show_warning_message();", 1000);

	//set FAQ URL
	document.getElementById("faq_windows").href=faq_href_windows;
	document.getElementById("faq_macOS").href=faq_href_macOS;
	document.getElementById("faq_iPhone").href=faq_href_iPhone;
	document.getElementById("faq_android").href=faq_href_android;

	$("#preshared_key_strength").append(Get_Component_PWD_Strength_Meter());
	if($("#ipsec_preshared_key").val() == "")
		$("#preshared_key_strength").css("display", "none");
	else
		chkPass($("#ipsec_preshared_key").val(), "", $("#preshared_key_strength"));
	$("#ipsec_preshared_key").keyup(function(){
		chkPass($(this).val(), "", $("#preshared_key_strength"));
	});
	$("#ipsec_preshared_key").blur(function(){
		if($(this).val() == "")
			$("#preshared_key_strength").css("display", "none");
	});

	$("#client_pwd_strength").append(Get_Component_PWD_Strength_Meter());
	if($("[name='ipsec_client_list_password']").val() == "")
		$("#client_pwd_strength").css("display", "none");
	else
		chkPass($("[name='ipsec_client_list_password']").val(), "", $("#client_pwd_strength"));
	$("[name='ipsec_client_list_password']").keyup(function(){
		chkPass($(this).val(), "", $("#client_pwd_strength"));
	});
	$("[name='ipsec_client_list_password']").blur(function(){
		if($(this).val() == "")
			$("#client_pwd_strength").css("display", "none");
	});
}

var MAX_RETRY_NUM = 5;
var external_ip_retry_cnt = MAX_RETRY_NUM;
function show_warning_message(){
	var set_ddns_text = '<a href="../Advanced_ASUSDDNS_Content.asp" target="_blank" style="text-decoration: underline; font-family:Lucida Console;"><#vpn_ipsec_set_DDNS#></a>';
	var set_ip_and_ddns_text = wanlink_ipaddr() + ', ' + set_ddns_text;
	if(realip_support && (based_modelid == "BRT-AC828"|| wans_mode != "lb")){
		if(realip_state != "2" && external_ip_retry_cnt > 0){
			if( external_ip_retry_cnt == MAX_RETRY_NUM )
				get_real_ip();
			else
				setTimeout("get_real_ip();", 3000);
		}
		else if(realip_state != "2"){
			if(validator.isPrivateIP(wanlink_ipaddr())){
				document.getElementById("privateIP_notes").innerHTML = "<#vpn_privateIP_hint#>";
				document.getElementById("privateIP_notes").style.display = "";
				$(".general_server_addr").html("-");
				document.getElementById("faq_port_forwarding").href=faq_href_port_forwarding;	//this id is include in string : #vpn_privateIP_hint#
			}
			else {
				if(ddns_enable_x == "1" && ddns_hostname_x != "") {
					$(".general_server_addr").html(ddns_hostname_x);
					if(!check_ddns_status())
						$(".general_server_addr").append(', ' + set_ddns_text);
				}
				else {
					$(".general_server_addr").html(set_ip_and_ddns_text);
				}
			}
		}
		else{
			if(!external_ip){
				document.getElementById("privateIP_notes").innerHTML = "<#vpn_privateIP_hint#>";
				document.getElementById("privateIP_notes").style.display = "";
				$(".general_server_addr").html("-");
				document.getElementById("faq_port_forwarding").href=faq_href_port_forwarding;	//this id is include in string : #vpn_privateIP_hint#
			}
			else {
				if(ddns_enable_x == "1" && ddns_hostname_x != "") {
					$(".general_server_addr").html(ddns_hostname_x);
					if(!check_ddns_status())
						$(".general_server_addr").append(', ' + set_ddns_text);
				}
				else {
					$(".general_server_addr").html(set_ip_and_ddns_text);
				}
			}
		}
	}
	else if(validator.isPrivateIP(wanlink_ipaddr())){
		document.getElementById("privateIP_notes").innerHTML = "<#vpn_privateIP_hint#>";
		document.getElementById("privateIP_notes").style.display = "";
		$(".general_server_addr").html("-");
		document.getElementById("faq_port_forwarding").href=faq_href_port_forwarding;	//this id is include in string : #vpn_privateIP_hint#
	}
	else {
		if(ddns_enable_x == "1" && ddns_hostname_x != "") {
			$(".general_server_addr").html(ddns_hostname_x);
			if(!check_ddns_status())
				$(".general_server_addr").append(', ' + set_ddns_text);
		}
		else {
			$(".general_server_addr").html(set_ip_and_ddns_text);
		}
	}
}

function get_real_ip(){
	$.ajax({
		url: 'get_real_ip.asp',
		dataType: 'script',
		error: function(xhr){
			get_real_ip();
		},
		success: function(response){
			external_ip_retry_cnt--;
			show_warning_message();
		}
	});
}

function ipsecShowAndHide(server_enable) {
	if(server_enable == "1") {
		ipsec_server_enable = 1;
	}
	else {
		ipsec_server_enable = 0;
	}

	document.form.ipsec_server_enable.value = ipsec_server_enable;
	if(ipsec_server_enable == "1")
		$(".ipsec_setting_content").show();
	else
		$(".ipsec_setting_content").hide();
}
function switchSettingsMode(mode){
	if(mode == "1") {
		$(".setting_general").css("display", "");
		$(".setting_adv").css("display", "none");
	}	
	else {
		$(".setting_general").css("display", "none");
		$(".setting_adv").css("display", "");
	}
}
function showipsec_clientlist() {
	var code = "";
	var ipsec_user_name = "";
	var ipsec_user_pwd = "";

	code +='<table width="100%" cellspacing="0" cellpadding="4" align="center" class="list_table" id="ipsec_client_list_table">';
	if(Object.keys(ipsec_client_list_json).length == 0)
		code +='<tr><td style="color:#FFCC00;" colspan="5"><#IPConnection_VSList_Norule#></td></tr>';
	else{
		for(var username in ipsec_client_list_json) {
			if(ipsec_client_list_json.hasOwnProperty(username)) {
				var passwd = ipsec_client_list_json[username]["passwd"];
				var ver = ipsec_client_list_json[username]["ver"];
				if(ver == "2")
					ver = "3";
				if(ver != "3")
					ver = "V" + ver;
				else
					ver = "V1&V2";
				code +='<tr>';
				if(username.length > 28) {
					ipsec_user_name = username.substring(0, 26) + "...";
					code +='<td width="30%" title="' + htmlEnDeCode.htmlEncode(username) + '">'+ htmlEnDeCode.htmlEncode(ipsec_user_name) +'</td>';
				}
				else
					code +='<td width="30%" title="' + htmlEnDeCode.htmlEncode(username) + '">' + htmlEnDeCode.htmlEncode(username) + '</td>';
				code +='<td width="30%" title="' + passwd + '" style="pointer-events:none;">-</td>';
				code +='<td width="30%">' + ver + '</td>';
				code +='<td width="10%">';
				code +='<input class="remove_btn" onclick="del_Row(this);" value=""/></td>';
				code +='</tr>';
			}
		}
	}
	
	code +='</table>';
	document.getElementById("ipsec_client_list_Block").innerHTML = code;
}
function addRow_Group(upper) {
	var username_obj = document.form.ipsec_client_list_username;
	var	password_obj = document.form.ipsec_client_list_password;
	var	ike_obj = document.form.ipsec_client_list_ike;

	var rule_num = document.getElementById("ipsec_client_list_table").rows.length;
	var item_num = document.getElementById("ipsec_client_list_table").rows[0].cells.length;		
	if(rule_num >= upper) {
		alert("<#JS_itemlimit1#> " + upper + " <#JS_itemlimit2#>");
		return false;	
	}

	var validAccount = function() {
		var valid_username = document.form.ipsec_client_list_username;
		var valid_password = document.form.ipsec_client_list_password;

		if(valid_username.value == "") {
			alert("<#JS_fieldblank#>");
			valid_username.focus();
			return false;
		}
		else if(!Block_chars(valid_username, [" ", "@", "*", "+", "|", ":", "?", "<", ">", ",", ".", "/", ";", "[", "]", "\\", "=", "\"", "&" ])) {
			return false;
		}

		if(valid_password.value == "") {
			alert("<#JS_fieldblank#>");
			valid_password.focus();
			return false;
		}
		else if(!Block_chars(valid_password, ["<", ">", "&", "\""])) {
			return false;
		}
		else if(valid_password.value.length > 0 && valid_password.value.length < 5) {
			alert("Password should be 5 to 32 characters.");/* untranslated */
			valid_password.focus();
			return false;
		}

		//confirm common string combination	#JS_common_passwd#
		var is_common_string = check_common_string(valid_password.value, "httpd_password");
		if(valid_password.value.length > 0 && is_common_string){
			if(!confirm("<#HSDPAConfig_Password_itemname#> : <#JS_common_passwd#>")){
				valid_password.focus();
				valid_password.select();
				return false;
			}
		}
	
		return true;
	};


	if(validAccount()) {
		if(item_num >= 2) {
			for(var i = 0; i < rule_num; i +=1 ) {
				if(ipsec_client_list_json.hasOwnProperty(username_obj.value)) {
					alert("<#JS_duplicate#>");
					username_obj.focus();
					username_obj.select();
					return false;
				}	
			}
		}
		
		ipsec_client_list_json[username_obj.value] = {"passwd":password_obj.value, "ver":ike_obj.value};
		var username_obj = document.form.ipsec_client_list_username;
		username_obj.value = "";
		password_obj.value = "";
		ike_obj.value = "3";
		$("#client_pwd_strength").css("display", "none");
		showipsec_clientlist();
	}
}
function del_Row(rowdata) {
	var del_username = $(rowdata).parent().siblings(":first").attr("title");
	var i = rowdata.parentNode.parentNode.rowIndex;
	document.getElementById("ipsec_client_list_table").deleteRow(i);
	delete ipsec_client_list_json[del_username];
	if(Object.keys(ipsec_client_list_json).length == 0)
		showipsec_clientlist();
}
function changeAdvDeadPeerDetection (obj) {
	if(obj.value == "0") {
		showhide("tr_adv_dpd_interval", 0);
	}
	else {
		showhide("tr_adv_dpd_interval", 1);
	}
}
function settingRadioItemCheck(obj, checkValue) {
	var radioLength = obj.length;
	for(var i = 0; i < radioLength; i += 1) {
		if(obj[i].value == checkValue) {
			obj[i].checked = true;
		}
	}
}
function getRadioItemCheck(obj) {
	var checkValue = "";
	var radioLength = obj.length;
	for(var i = 0; i < radioLength; i += 1) {
		if(obj[i].checked) {
			checkValue = obj[i].value;
			break;
		}
	}
	return 	checkValue;
}
function validForm() {
	if(ipsec_server_enable == "1") {
		if(!validator.isEmpty(document.form.ipsec_preshared_key))
			return false;
		if(!Block_chars(document.form.ipsec_preshared_key, [">", "<", "&", "\"", "null"]))
			return false;
		if(is_KR_sku){
			if(!validator.psk_KR(document.form.ipsec_preshared_key))
				return false;
		}
		else{
			if(!validator.psk(document.form.ipsec_preshared_key))
				return false;
		}
		//confirm common string combination	#JS_common_passwd#
		var is_common_string = check_common_string(document.form.ipsec_preshared_key.value, "wpa_key");
		if(is_common_string){
			if(!confirm("<#vpn_ipsec_PreShared_Key#> : <#JS_common_passwd#>")){
				document.form.ipsec_preshared_key.focus();
				document.form.ipsec_preshared_key.select();
				return false;
			}
		}

		var ipAddr = document.form.ipsec_clients_start.value.trim() + ".1";
		var ipformat  = /^(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\.(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\.(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\.(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)$/;
		if(!(ipformat.test(ipAddr))) {
			alert(document.form.ipsec_clients_start.value+" <#JS_validip#>");
			document.form.ipsec_clients_start.focus();
			document.form.ipsec_clients_start.select();
			return false;
		}

		var ipsecLanIPAddr = ipAddr;
		var ipsecLanNetMask = "255.255.255.0";
		var ipConflict;
		//1.check LAN IP
		ipConflict = checkIPConflict("LAN", ipsecLanIPAddr, ipsecLanNetMask);
		if(ipConflict.state) {
			alert("<#JS_conflict_LANIP#>: " + ipConflict.ipAddr + ",\n" + "<#Network_segment#>: " + ipConflict.netLegalRangeStart + " ~ " + ipConflict.netLegalRangeEnd);
			document.form.ipsec_clients_start.focus();
			document.form.ipsec_clients_start.select();
			return false;
		}

		if(getRadioItemCheck(document.form.ipsec_dead_peer_detection) == "1") {
			if(!validator.numberRange(document.form.ipsec_dpd, 10, 900)) {
				return false;
			}
		}

		if(document.form.ipsec_dns1.value != "") {
			if(!Block_chars(document.form.ipsec_dns1, [">", "<"]))
				return false;
			if(!validator.isLegalIP(document.form.ipsec_dns1))
				return false
		}
		if(document.form.ipsec_dns2.value != "") {
			if(!Block_chars(document.form.ipsec_dns2, [">", "<"]))
				return false;
			if(!validator.isLegalIP(document.form.ipsec_dns2))
				return false
		}
		if(document.form.ipsec_wins1.value != "") {
			if(!Block_chars(document.form.ipsec_wins1, [">", "<"]))
				return false;
			if(!validator.isLegalIP(document.form.ipsec_wins1))
				return false
		}
		if(document.form.ipsec_wins2.value != "") {
			if(!Block_chars(document.form.ipsec_wins2, [">", "<"]))
				return false;
			if(!validator.isLegalIP(document.form.ipsec_wins2))
				return false
		}
	}

	return true;
}
function applyRule() {
	if(validForm()) {
		var confirmFlag = confirm('<#vpn_ipsec_clients_reconnect_hint#>');
		if(!confirmFlag)
			return false;

		/* data structure
		1 vpn_type, profilename, remote_gateway_method, remote_gateway, 
		5 local_public_interface, local_public_ip, auth_method, auth_method_value, 
		9 local_subnet, local_port, remote_subnet, remote_port, 
		13 transport/tunnel type, virtual_ip, virtual_subnet, accessible_networks, 
		17 ike, encryption_p1, hash_p1, exchange,
		21 local id, remote id, keylife_p1, xauth,
		25 xauth_account, xauth_password, xauth_server_type, traversal,
		29 ike_isakmp, ike_isakmp_nat, ipsec_dpd, dead_peer_detection,
		33 encryption_p2, hash_p2, keylife_p2, keyingtries
		37 samba list, default Activate is 0;
		*/
		var oriProfileArray = [];
		oriProfileArray = ipsec_profile_1.split(">");
		oriProfileArray.unshift("ipsec_profile_1");
		var ipsec_dpd = oriProfileArray[31];
		var ipsec_dead_peer_detection = getRadioItemCheck(document.form.ipsec_dead_peer_detection);
		if(ipsec_dead_peer_detection == "1")
			ipsec_dpd = document.form.ipsec_dpd.value;
		var samba_list = "<" + document.form.ipsec_dns1.value + "<" + document.form.ipsec_dns2.value + "<" + document.form.ipsec_wins1.value + "<" + document.form.ipsec_wins2.value;
		var profile_array = [ "",
			"4", "Host-to-Net", "null", "null",
			document.form.ipsec_local_public_interface.value, "", "1", document.form.ipsec_preshared_key.value,
			"null", "null", "null", "null",
			"null", "1", document.form.ipsec_clients_start.value, "null",
			"1", "null", "null", "0",
			"null", "null", "null", "1",
			"", "", "eap-md5", "1",
			"500", "4500", ipsec_dpd, ipsec_dead_peer_detection,
			"null", "null", "null", "null",
			samba_list, "1"
		];

		var result = "";
		for(var i = 1; i < profile_array.length; i += 1)
			result += profile_array[i] + ">";
		result = result.slice(0, -1);

		if(ipsec_server_enable == "1"){
			document.form.ipsec_profile_1.value = result;
			var cert_address = "";
			var ddns_hostname = '<% nvram_get("ddns_hostname_x"); %>';
			var wan0_ipaddr = "<% nvram_get("wan0_ipaddr"); %>";
			if(ddns_hostname != "")
				cert_address = "@" + ddns_hostname;
			else
				cert_address = wan0_ipaddr;

			document.form.ipsec_profile_2.disabled = false;
			document.form.ipsec_client_list_1.disabled = false;
			document.form.ipsec_client_list_2.disabled = false;
			document.form.ipsec_profile_2.value = "4>Host-to-Netv2>null>null>wan>>0>null>null>null>null>null>null>1>"+document.form.ipsec_clients_start.value+">null>2>null>null>0>"+cert_address+">null>null>0>>>eap-mschapv2>1>500>4500>10>1>null>null>null>null><<<<>1>pubkey>svrCert.pem>always>svrKey.pem>%identity";

			var ipsec_client_list_1 = "";
			var ipsec_client_list_2 = "";
			for(var username in ipsec_client_list_json) {
				if(ipsec_client_list_json.hasOwnProperty(username)) {
					var passwd = ipsec_client_list_json[username]["passwd"];
					var ver = parseInt(ipsec_client_list_json[username]["ver"]);
					if(ver & 1)
						ipsec_client_list_1 += "<" + username + ">" + passwd;
					if(ver & 2)
						ipsec_client_list_2 += "<" + username + ">" + passwd;
				}
			}
			document.form.ipsec_client_list_1.value = ipsec_client_list_1;
			document.form.ipsec_client_list_2.value = ipsec_client_list_2;

			document.form.ipsec_block_intranet.disabled = false;
			document.form.ipsec_block_intranet.value = getRadioValue(document.form.vpn_server_client_access);
		}
		else {
			if(document.form.ipsec_profile_1.value != "") {
				document.form.ipsec_profile_1.disabled = false;
			}
			else {
				document.form.ipsec_profile_1.disabled = true;
			}
			document.form.ipsec_profile_2.disabled = true;
			document.form.ipsec_client_list_1.disabled = true;
			document.form.ipsec_client_list_2.disabled = true;
			document.form.ipsec_block_intranet.disabled = true;
		}

		document.form.ipsec_server_enable.value = ipsec_server_enable;
		var actionScript = "ipsec_stop";
		if(ipsec_server_enable == "1")
			actionScript = "ipsec_start";

		document.form.action_script.value = actionScript;
		showLoading();
		document.form.submit();
	}
}
function viewLog() {
	update_ipsec_log();
	$("#ipsec_view_log_panel").fadeIn(300);
}
function cancel_viewLog() {
	$("#ipsec_view_log_panel").fadeOut(300);
}
function clear_viewLog() {
	httpApi.clean_ipsec_log(function(){
		$("textarea#textarea").html("");
	});
}
function save_viewLog() {
	location.href = "ipsec.log";
}
function refresh_viewLog() {
	update_ipsec_log();
}
function update_ipsec_log() {
	$.ajax({
		url: '/ajax_ipsec_log.xml',
		dataType: 'xml',

		error: function(xml) {
			setTimeout("update_ipsec_log();", 1000);
		},

		success: function(xml) {
			var ipsecXML = xml.getElementsByTagName("ipsec");
			var ipsec_log = ipsecXML[0].firstChild.nodeValue.trim();
			$("textarea#textarea").html(ipsec_log);
		}
	});	
}
function setClientsEnd() {
	var ipAddrEnd = "";
	var ipAddr = document.form.ipsec_clients_start.value.trim() + ".1";
	var ipformat  = /^(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\.(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\.(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\.(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)$/;
	if((ipformat.test(ipAddr))) {
		ipAddrEnd = document.form.ipsec_clients_start.value.trim() + ".254";
		$("#ipsec_clients_end").html(ipAddrEnd);
	}
}

function update_connect_status() {
	var get_ipsec_conn = httpApi.hookGet("get_ipsec_conn", true);
	var totalcnt = 0;

	conn_name_array = [];
	$(".general_connection_status").html("-");

	get_ipsec_conn.forEach(function(item, index, array){
		var parseArray = [];

		ipsec_connect_status_array[get_ipsec_conn[index][0]] = [];
		if(item[0] != undefined && item[0] == get_ipsec_conn[index][0] && item[1] != undefined){
			var itemRow = item[1].split('<');
			for(var i = 0; i < itemRow.length; i += 1) {
				if(itemRow[i] != "") {
					var itemCol = itemRow[i].split('>');
					var eachRuleArray = new Array();
					if(itemCol[6] == "IPSEC"){
						eachRuleArray.push(itemCol[0]);//ipaddr
						eachRuleArray.push(itemCol[1]);//conn_status
						eachRuleArray.push(itemCol[2]);//conn_period
						eachRuleArray.push(itemCol[3]);//account or device name
						eachRuleArray.push(itemCol[4]);//psk_reauth_time
						parseArray.push(eachRuleArray);
					}
				}
			}
			ipsec_connect_status_array[get_ipsec_conn[index][0]] = parseArray;
			conn_name_array.push(get_ipsec_conn[index][0]);
			totalcnt += ipsec_connect_status_array[get_ipsec_conn[index][0]].length;
		}
	});


	if(totalcnt > 0) {
		var code = "";
		code +='<a class="hintstyle2" href="javascript:void(0);" onClick="showIPSecClients(conn_name_array, event);">';
		code +='<#btn_Enabled#>(' + totalcnt + ')</a>';
		$(".general_connection_status").html(code);
	}

	setTimeout("update_connect_status();",3000);
}

function close_connect_status() {
	$("#connection_ipsec_profile_panel").fadeOut(300);
}

function showIPSecClients(ipsec_conn_name_array, e) {
	var html = "";

	$("#connection_ipsec_profile_panel").fadeIn(300);
	$("#connection_ipsec_profile_panel").css("position", "absolute");
	$("#connection_ipsec_profile_panel").css("top", "440px");
	$("#connection_ipsec_profile_panel").css("left", "225px");
	
	html += "<div class='ipsec_connect_status_title_bg'>";
	html += "<div class='ipsec_connect_status_title' style='width:240px;'>Remote IP</div>";/*untranslated*/
	html += "<div class='ipsec_connect_status_title'><#statusTitle_Client#></div>";
	html += "<div class='ipsec_connect_status_title'><#Access_Time#></div>";
	html += "<div class='ipsec_connect_status_title'><#vpn_ipsec_XAUTH#> <#Permission_Management_Users#></div>";
	html += "<div class='ipsec_connect_status_title'>PSKRAUTHTIME</div>";/*untranslated*/
	html += "<div class='ipsec_connect_status_close'><a onclick='close_connect_status();'><img width='18px' height='18px' src=\"/images/button-close.png\" onmouseover='this.src=\"/images/button-close2.png\"' onmouseout='this.src=\"/images/button-close.png\"' border='0'></a></div>";
	html += "</div>";
	html += "<div style='clear:both;'></div>";
	$("#connection_ipsec_profile_panel").html(html);
	html = "";

	//for loop to go through all connection names, while the forEach is to go through all connections under the same connection name.
	var statusText = [[""], ["<#Connected#>"], ["<#Connecting_str#>"], ["<#Connecting_str#>"]];
	for(var i = 0; i < ipsec_conn_name_array.length; i += 1) {
		ipsec_connect_status_array[ipsec_conn_name_array[i]].forEach(function(item, index, array){
			if(item != "") {
				html += "<div class='ipsec_connect_status_content_bg'>";
				html += "<div class='ipsec_connect_status_content' style='width:240px;word-wrap:break-word;word-break:break-all;'>" + item[0] + "</div>";
				html += "<div class='ipsec_connect_status_content'>" + statusText[item[1]] + "</div>";
				html += "<div class='ipsec_connect_status_content'>" + item[2] + "</div>";
				html += "<div class='ipsec_connect_status_content'>" + item[3] + "</div>";
				html += "<div class='ipsec_connect_status_content'>" + item[4] + "</div>";
				html += "</div>";
				html += "<div style='clear:both;'></div>";
			}
		});
	}

	$("#connection_ipsec_profile_panel").append(html);
}

function export_cert(_mode) {
	var type = $("input[name=ipsec_export_cert]:checked").val();//0:Windows, 1:Mobile
	if(_mode == "0") {//download
		if(type == "0")
			location.href = "ikev2_cert_windows.der";
		else if(type == "1")
			location.href = "ikev2_cert_mobile.pem";
	}
	else if(_mode == "1") {//renew
		var ddns_enable_x = httpApi.nvramGet(["ddns_enable_x"]).ddns_enable_x;
		if(ddns_enable_x == "1"){
			if(!check_ddns_status()){
				alert("Update failed, please check your DDNS setting.");/* untranslated */
				window.location.href = "Advanced_ASUSDDNS_Content.asp";
				return false;
			}
		}
		$(".button_gen.ipsec_active.renew").hide();
		$(".ipsec_inactive.renew_hint").hide();
		$(".renewLoadingIcon").show();
		httpApi.renew_ikev2_cert_key(function(){
			var count = 0;
			var timer = 15;
			var interval_check = setInterval(function(){
				var ikev2_cert_state = httpApi.nvramGet(["ikev2_cert_state"], true).ikev2_cert_state;
				if(ikev2_cert_state == "3"){
					clearInterval(interval_check);
					$(".button_gen.ipsec_active.renew").show();
					$(".renewLoadingIcon").hide();
					alert("<#vpn_ipsec_update_cert_success#>");
				}
				else{
					count++;
					if(count >= timer){
						clearInterval(interval_check);
						$(".button_gen.ipsec_active.renew").show();
						$(".renewLoadingIcon").hide();
						alert("<#vpn_ipsec_update_cert_fail#>");
					}
				}
			}, 1000);
		});
	}
}
</script>
</head>

<body onload="initial();" class="bg">
<div id="TopBanner"></div>
<div id="Loading" class="popup_bg"></div>

<iframe name="hidden_frame" id="hidden_frame" src="" width="0" height="0" frameborder="0"></iframe>
<form method="post" name="form" action="/start_apply.htm" target="hidden_frame">
<input type="hidden" name="productid" value="<% nvram_get("productid"); %>">
<input type="hidden" name="current_page" value="Advanced_VPN_IPSec.asp">
<input type="hidden" name="next_page" value="Advanced_VPN_IPSec.asp">
<input type="hidden" name="modified" value="0">
<input type="hidden" name="action_mode" value="apply">
<input type="hidden" name="action_script" value="saveNvram">
<input type="hidden" name="action_wait" value="5">
<input type="hidden" name="preferred_lang" id="preferred_lang" value="<% nvram_get("preferred_lang"); %>">
<input type="hidden" name="ipsec_server_enable" value="<% nvram_get("ipsec_server_enable"); %>">
<input type="hidden" name="ipsec_profile_1" id="ipsec_profile_1" value="">
<input type="hidden" name="ipsec_profile_2" id="ipsec_profile_2" value="">
<input type="hidden" name="ipsec_client_list_1" id="ipsec_client_list_1" value="">
<input type="hidden" name="ipsec_client_list_2" id="ipsec_client_list_2" value="">
<input type="hidden" name="ipsec_block_intranet" value="<% nvram_get("ipsec_block_intranet"); %>" disabled>

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
					<td valign="top" >
						<div id="ipsec_view_log_panel" class="ipsec_view_log_panel" style="padding:15px;">
							<textarea cols="63" rows="44" wrap="off" readonly="readonly" id="textarea" style="width:99%; font-family:'Courier New', Courier, mono; font-size:11px;background:#475A5F;color:#FFFFFF;resize:none;"><% nvram_dump("ipsec.log",""); %></textarea>
							<div style='text-align:center;margin-top:15px;'>
								<input class="button_gen" onclick="cancel_viewLog();" type="button" value="<#CTL_Cancel#>"/>
								<input class="button_gen" onclick="clear_viewLog();" type="button" value="<#CTL_clear#>"/>
								<input class="button_gen" onclick="save_viewLog();" type="button" value="<#CTL_onlysave#>"/>
								<input class="button_gen" onclick="refresh_viewLog();" type="button" value="<#CTL_refresh#>"/>
							</div>
						</div>
						<table width="760px" border="0" cellpadding="5" cellspacing="0" class="FormTitle" id="FormTitle">
						<tbody>
							<tr>
								<td bgcolor="#4D595D" valign="top">
									<div>&nbsp;</div>
									<div class="formfonttitle"><#BOP_isp_heart_item#> - IPSec VPN<!--untranslated--></div>
									<div id="divSwitchMenu" style="margin-top:-40px;float:right;"></div>
									<div style="margin:10px 0 10px 5px;" class="splitLine"></div>
									<div id="privateIP_notes" class="formfontdesc" style="display:none;color:#FFCC00;"></div>
									<div class="formfontdesc">
										<span class="hint-color"><#vpn_ipsec_note#></span>
									</div>

									<table id="ipsec_general_setting" width="100%" border="1" align="center" cellpadding="4" cellspacing="0" bordercolor="#6b8fa3" class="FormTable">
										<thead>
										<tr>
											<td colspan="2"><#t2BC#></td>
										</tr>
										</thead>
										<tr>
											<th><#vpn_ipsec_enable#></th>
											<td>
												<div align="center" class="left" style="float:left;cursor:pointer;" id="radio_ipsec_enable"></div>
												<div class="iphone_switch_container" style="height:32px; width:74px; position: relative; overflow: hidden;"></div>
												<script type="text/javascript">
												$('#radio_ipsec_enable').iphoneSwitch('<% nvram_get("ipsec_server_enable"); %>',
													function() {
														ipsecShowAndHide(1);
													},
													function() {
														ipsecShowAndHide(0);
													},
													{
														switch_on_container_path: '/switcherplugin/iphone_switch_container_off.png'
													}
												);
											</script>
											</td>
										</tr>
										<tr class="ipsec_setting_content">
											<th><#WLANAuthentication11a_ExAuthDBIPAddr_itemname#></th>
											<td class="general_server_addr">-</td>
										</tr>
										<tr class="ipsec_setting_content">
											<th><#PPPConnection_x_WANLink_itemname#></th>
											<td class="general_connection_status">-</td>
										</tr>
										<tr class="ipsec_setting_content">
											<th><#System_Log#></th>
											<td>
												<input class="button_gen" onclick="viewLog()" type="button" value="<#CTL_check_log#>"/>
											</td>
										</tr>
										<tr class="ipsec_setting_content">
											<th><#vpn_Adv#></th>
											<td>
												<select id="selSwitchMode" onchange="switchSettingsMode(this.options[this.selectedIndex].value)" class="input_option" style="margin-left:2px;">
													<option value="1" selected><#menu5_1_1#></option>
													<option value="2"><#menu5#></option>
												</select>
											</td>
										</tr>
										<tr class="ipsec_setting_content">
											<th><#vpn_ipsec_PreShared_Key#></th>
											<td>
												<input id="ipsec_preshared_key" name="ipsec_preshared_key" type="password" autocapitalization="off" onBlur="switchType(this, false);" onFocus="switchType(this, true);" class="input_25_table" maxlength="32" placeholder="<#vpn_preshared_key_hint#>" autocomplete="off" autocorrect="off" autocapitalize="off">
												<div id="preshared_key_strength"></div>
											</td>
										</tr>
										<tr class="ipsec_setting_content">
											<th><#vpn_access#></th>
											<td>
												<input type="radio" name="vpn_server_client_access" id="vpn_server_client_access_internet" class="input" value="1">
												<label for="vpn_server_client_access_internet">Internet only</label>
												<input type="radio" name="vpn_server_client_access" id="vpn_server_client_access_both" class="input" value="0">
												<label for="vpn_server_client_access_both"><#vpn_access_WANLAN#></label>
												<br>
												<span class="hint-color">The access setting will be applied to both IPSec VPN and Instant Guard.</span><!-- untranslated -->
											</td>
										</tr>
									</table>

									<div class="ipsec_setting_content">
										<table width="100%" border="1" align="center" cellpadding="4" cellspacing="0" bordercolor="#6b8fa3"  class="FormTable setting_adv" style="margin-top:15px;">
											<thead>
											<tr>
												<td colspan="2"><#menu5#></td>
											</tr>
											</thead>
											<tr>
												<th><#vpn_ipsec_IKE_ISAKMP_Port#></th>
												<td>500</td>
											</tr>
											<tr>
												<th><#vpn_ipsec_IKE_ISAKMP_NAT_Port#></th>
												<td>4500</td>
											</tr>
											<tr>
												<th><#vpn_client_ip#></th>
												<td>
													<input type="text" maxlength="11" class="input_12_table" name="ipsec_clients_start" onBlur="setClientsEnd();" value="10.10.10" autocomplete="off" autocorrect="off" autocapitalize="off"/>
													<span style="font-family: Lucida Console;color: #FFF;">.1 ~ </span>
													<span id="ipsec_clients_end" style="font-family: Lucida Console;color: #FFF;">10.10.10.254</span>
												</td>
											</tr>
											<tr>
												<th><#IPConnection_x_DNSServer1_itemname#></th>
												<td>
													<input type="text" maxlength="15" class="input_15_table" name="ipsec_dns1" onkeypress="return validator.isIPAddr(this, event)" autocomplete="off" autocorrect="off" autocapitalize="off">
													<span class="hint-color"><#feedback_optional#></span>
												</td>
											</tr>
											<tr>
												<th><#IPConnection_x_DNSServer2_itemname#></th>
												<td>
													<input type="text" maxlength="15" class="input_15_table" name="ipsec_dns2" onkeypress="return validator.isIPAddr(this, event)" autocomplete="off" autocorrect="off" autocapitalize="off">
													<span class="hint-color"><#feedback_optional#></span>
												</td>
											</tr>
											<tr>
												<th><#IPConnection_x_WINSServer1_itemname#></th>
												<td>
													<input type="text" maxlength="15" class="input_15_table" name="ipsec_wins1" onkeypress="return validator.isIPAddr(this, event)" autocomplete="off" autocorrect="off" autocapitalize="off">
													<span class="hint-color"><#feedback_optional#></span>
												</td>
											</tr>
											<tr>
												<th><#IPConnection_x_WINSServer2_itemname#></th>
												<td>
													<input type="text" maxlength="15" class="input_15_table" name="ipsec_wins2" onkeypress="return validator.isIPAddr(this, event)" autocomplete="off" autocorrect="off" autocapitalize="off">
													<span class="hint-color"><#feedback_optional#></span>
												</td>
											</tr>
											<tr id="tr_localPublicInterface" style="display:none;">
												<th><#vpn_ipsec_Local_Interface#></th>
												<td>
													<select name="ipsec_local_public_interface" class="input_option"></select>
												</td>
											</tr>
										</table>

										<div id="ipsec_advanced_settings" class="setting_general">
											<table width="100%" border="1" align="center" cellpadding="4" cellspacing="0" bordercolor="#6b8fa3"  class="FormTable" style="margin-top:15px;">
												<thead>
												<tr>
													<td colspan="2">IKEv1 Config</td><!-- untranslated -->
												</tr>
												</thead>
												<tr id="tr_adv_exchange_mode">
													<th><#vpn_ipsec_Exchange_Mode#></th>
													<td><#vpn_ipsec_Main_Mode#></td>
												</tr>
												<tr id="tr_adv_dead_peer_detection">
													<th><#vpn_ipsec_DPD#></th>
													<td>
														<input type="radio" name="ipsec_dead_peer_detection" id="ipsec_dead_peer_detection_en" class="input" value="1" onchange="changeAdvDeadPeerDetection(this)" checked>
														<label for='ipsec_dead_peer_detection_en' id="ipsec_dead_peer_detection_en_label"><#WLANConfig11b_WirelessCtrl_button1name#></label>
														<input type="radio" name="ipsec_dead_peer_detection" id="ipsec_dead_peer_detection_dis" class="input" value="0" onchange="changeAdvDeadPeerDetection(this)">
														<label for='ipsec_dead_peer_detection_dis' id="ipsec_dead_peer_detection_dis_label"><#WLANConfig11b_WirelessCtrl_buttonname#></label>
													</td>
												</tr>
												<tr id="tr_adv_dpd_interval">
													<th><#vpn_ipsec_DPD_Checking_Interval#></th>
													<td>
														<input type="text" class="input_3_table" name="ipsec_dpd" maxlength="3" value="10" onKeyPress="return validator.isNumber(this,event)" autocomplete="off" autocorrect="off" autocapitalize="off">
														<span class="hint-color">(10~900) <#Second#></span>
													</td>
												</tr>
											</table>

											<table width="100%" border="1" align="center" cellpadding="4" cellspacing="0" bordercolor="#6b8fa3"  class="FormTable " style="margin-top:15px;">
												<thead>
												<tr>
													<td colspan="2">IKEv2 Config</td><!-- untranslated -->
												</tr>
												</thead>
												<tr>
													<th><#vpn_export_cert#></th>
													<td>
														<div><#Status_Str#> : <span id="cert_status"></span></div>
														<div><#vpn_ipsec_cert_to#> : <span id="cert_issueTo"></span></div>
														<div><#vpn_ipsec_cert_by#> : <span id="cert_issueBy"></span></div>
														<div><#vpn_openvpn_KC_expire#> : <span id="cert_expire"></span></div>
														<div>
															<input class="button_gen_dis ipsec_active export" type="button" value="<#btn_Export#>">
															<span class="ipsec_inactive"><#Status_Inactive#></span>
															<input type="radio" name="ipsec_export_cert" id="ipsec_export_cert_windows" class="input" value="0" checked>
															<label for="ipsec_export_cert_windows"><#vpn_ipsec_windows#></label>
															<input type="radio" name="ipsec_export_cert" id="ipsec_export_cert_mobile" class="input" value="1">
															<label for="ipsec_export_cert_mobile"><#vpn_ipsec_mobile#></label>
														</div>
													</td>
												</tr>
												<tr>
													<th><a class="hintstyle" href="javascript:void(0);" onClick="openHint(33,2);"><#vpn_ipsec_re_cert#></a></th>
													<td style="position:relative;">
														<input class="button_gen_dis ipsec_active renew" type="button" value="<#CTL_renew#>">
														<span class="ipsec_inactive renew_hint"><#Status_Inactive#></span>
														<div class="renewLoadingIcon"></div>
													</td>
												</tr>
											</table>
											<div class="formfontdesc" style="margin-top:8px;">
												<#vpn_ipsec_setup#> :
													<span class="faq_link">1. <a id="faq_windows" href="" target="_blank" style="text-decoration:underline;">Windows</a></span>
													<span class="faq_link">2. <a id="faq_macOS" href="" target="_blank" style="text-decoration:underline;">Mac OS</a></span>
													<span class="faq_link">3. <a id="faq_iPhone" href="" target="_blank" style="text-decoration:underline;">iOS</a></span>
													<span class="faq_link">4. <a id="faq_android" href="" target="_blank" style="text-decoration:underline;">Android</a></span>
											</div>
											<!-- User table start-->
											<table id="tbAccountList" width="100%" border="1" align="center" cellpadding="4" cellspacing="0" class="FormTable_table" style="margin-top:15px;">
												<thead>
												<tr>
													<td colspan="4"><#Username_Pwd#>&nbsp;(<#List_limit#>&nbsp;8)</td>
												</tr>
												</thead>
												<tr>
													<th><#Username#></th>
													<th><#HSDPAConfig_Password_itemname#></th>
													<th><#vpn_ipsec_IKE_ver#></th>
													<th><#list_add_delete#></th>
												</tr>
												<tr>
													<td width="30%">
														<input type="text" class="input_22_table" maxlength="32" name="ipsec_client_list_username" onKeyPress="return validator.isString(this, event)" autocomplete="off" autocorrect="off" autocapitalize="off">
													</td>
													<td width="30%">
														<input type="text" class="input_22_table" maxlength="32" name="ipsec_client_list_password" onKeyPress="return validator.isString(this, event)" autocomplete="off" autocorrect="off" autocapitalize="off">
														<div id="client_pwd_strength"></div>
													</td>
													<td width="30%">
														<select name="ipsec_client_list_ike" onchange="" class="input_option">
															<option value="1">V1</option>
															<option value="3" selected="selected">V1&V2</option>
														</select>
													</td>
													<td width="10%">
														<div><input type="button" class="add_btn" onClick="addRow_Group(8);" value=""></div>
													</td>
												</tr>
											</table>
											<div id="ipsec_client_list_Block"></div>
											<!-- User table end-->
										</div>
									</div>

									<div id="ipsec_apply" class="apply_gen">
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
<div id="connection_ipsec_profile_panel" class="ipsec_connect_status_panel"></div> 
</form>
<div id="footer"></div>
</body>
</html>



