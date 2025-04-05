﻿<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml">
<html xmlns:v>
<head>
<meta http-equiv="X-UA-Compatible" content="IE=Edge"/>
<meta http-equiv="Content-Type" content="text/html; charset=utf-8" />
<meta HTTP-EQUIV="Pragma" CONTENT="no-cache">
<meta HTTP-EQUIV="Expires" CONTENT="-1">
<link rel="shortcut icon" href="images/favicon.png">
<link rel="icon" href="images/favicon.png">
<title><#Web_Title#> - <#Instant_Guard_title#></title>
<link rel="stylesheet" type="text/css" href="index_style.css">
<link rel="stylesheet" type="text/css" href="form_style.css">
<link rel="stylesheet" type="text/css" href="other.css">
<link rel="stylesheet" type="text/css" href="/js/table/table.css">
<script type="text/javascript" src="/js/jquery.js"></script>
<script type="text/javascript" src="/state.js"></script>
<script type="text/javascript" src="/general.js"></script>
<script type="text/javascript" src="/popup.js"></script>
<script type="text/javascript" src="/help.js"></script>
<script type="text/javascript" src="/validator.js"></script>
<script language="JavaScript" type="text/javascript" src="switcherplugin/jquery.iphone-switch.js"></script>
<script type="text/javascript" src="/js/httpApi.js"></script>
<script type="text/javascript" language="JavaScript" src="/js/table/table.js"></script>
<script type="text/javascript" src="/form.js"></script>
<style>
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
</style>
<script>
//set table Struct
var tableStruct = {
	data: [],
	container: "tableContainer",
	title: "<#PPPConnection_x_WANLink_itemname#>",
	header: [
		{
			"title" : "Remote IP",/*untranslated*/
			"width" : "20%"
		},
		{
			"title" : "<#statusTitle_Client#>",
			"width" : "20%"
		},
		{
			"title" : "<#Access_Time#>",
			"width" : "20%"
		},
		{
			"title" : "<#Clientlist_device#>",
			"width" : "20%"
		},
		{
			"title" : "PSKRAUTHTIME",/*untranslated*/
			"width" : "20%"
		}
	]
};

var current_page = window.location.pathname.split("/").pop();
var faq_index_tmp = get_faq_index(FAQ_List, current_page, 1);
var faq_index_tmp2 = get_faq_index(FAQ_List, current_page, 2);

function initial(){
	show_menu();
	tableApi.genTableAPI(tableStruct);
	var ipsec_ig_enable = httpApi.nvramGet(["ipsec_ig_enable"])["ipsec_ig_enable"];
	if(ipsec_ig_enable == "1"){
		update_ipsec_conn();
		var interval = setInterval(function(){
			update_ipsec_conn();
		},5000);

		var ddns_enable_x = '<% nvram_get("ddns_enable_x"); %>';
		var ddns_hostname_x = '<% nvram_get("ddns_hostname_x"); %>';
		if(ddns_enable_x == "1" && ddns_hostname_x != "")
			$("#ig_server_address").html(ddns_hostname_x);

		var ipsec_block_intranet = httpApi.nvramGet(["ipsec_block_intranet"]).ipsec_block_intranet;
		if(ipsec_block_intranet == "")
			ipsec_block_intranet = "0";
		setRadioValue(document.form.vpn_server_client_access, ipsec_block_intranet);
		$("#tr_vpn_server_client_access").show();
	}
	else{
		$("#tr_vpn_server_client_access").hide();
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
			var ipsec_log = ipsecXML[0].innerHTML;
			$("textarea#textarea").html(htmlEnDeCode.htmlEncode(ipsec_log));
		}
	});
}
function set_ig_config(_status){
	if(_status == "0"){
		var confirmFlag = confirm(stringSafeGet("<#Instant_Guard_off#>\n<#Setting_factorydefault_hint2#>"));
		if(!confirmFlag){
			$('#radio_IG_enable').find('.iphone_switch').animate({backgroundPosition: 0}, "slow");
			curState = "1";
			return false;
		}
	}
	var postData = {"ipsec_ig_enable": _status, "do_rc": 1};
	httpApi.set_ig_config(postData);
	showLoading(3);
	setTimeout(function(){
		location.href = document.form.current_page.value
	}, 3*1000);
}
function update_ipsec_conn(){
	var statusText = [[""], ["<#Connected#>"], ["<#Connecting_str#>"], ["<#Connecting_str#>"]]
	var get_ipsec_conn = httpApi.hookGet("get_ipsec_conn", true);
	var parseArray = [];
	get_ipsec_conn.forEach(function(item, index, array){
		if(item[1] != undefined){
			var itemRow = item[1].split('<');
			for(var i = 0; i < itemRow.length; i += 1) {
				if(itemRow[i] != "") {
					var itemCol = itemRow[i].split('>');
					var eachRuleArray = new Array();
					if(itemCol[6].indexOf("IG") != -1){
						eachRuleArray.push(itemCol[0]);//ipaddr
						eachRuleArray.push(statusText[itemCol[1]]);//conn_status
						eachRuleArray.push(itemCol[2]);//conn_period
						var device = itemCol[3];
						if(itemCol[7] != undefined && itemCol[7] != "")
							device = itemCol[7];
						eachRuleArray.push(device);//account or device name
						eachRuleArray.push(itemCol[4]);//psk_reauth_time
						parseArray.push(eachRuleArray);
					}
				}
			}
		}
	});
	tableStruct.data = parseArray;
	tableApi.genTableAPI(tableStruct);
}
function vpnServerClientAccess(){
	var ipsec_ig_enable = httpApi.nvramGet(["ipsec_ig_enable"])["ipsec_ig_enable"];
	if(ipsec_ig_enable == "1"){
		var vpn_server_client_access = getRadioValue(document.form.vpn_server_client_access);
		httpApi.nvramSet({
			"action_mode" : "apply",
			"ipsec_block_intranet" : vpn_server_client_access,
			"rc_service" : "restart_firewall"
		});
	}
}
</script>
</head>
<body onload="initial();">
<div id="TopBanner"></div>
<div id="Loading" class="popup_bg"></div>
<iframe name="hidden_frame" id="hidden_frame" src="" width="0" height="0" frameborder="0"></iframe>
<form method="post" name="form" id="ruleForm" action="/start_apply.htm" target="hidden_frame">
<input type="hidden" name="productid" value="<% nvram_get("productid"); %>">
<input type="hidden" name="current_page" value="Advanced_Instant_Guard.asp">
<input type="hidden" name="next_page" value="">
<input type="hidden" name="group_id" value="">
<input type="hidden" name="modified" value="0">
<input type="hidden" name="action_mode" value="apply_new">
<input type="hidden" name="action_script" value="">
<input type="hidden" name="action_wait" value="3">
<input type="hidden" name="preferred_lang" id="preferred_lang" value="<% nvram_get("preferred_lang"); %>">
<input type="hidden" name="firmver" value="<% nvram_get("firmver"); %>">
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
								<div class="container">

									<div>&nbsp;</div>
									<div class="formfonttitle"><#Instant_Guard_title#></div>
									<div class="formfonttitle_help"><i onclick="show_feature_desc(`<#HOWTOSETUP#>`, `How to share secure connection to friends or family via Instant Guard app?`)" class="icon_help"></i></div>
									<div style="margin:10px 0 10px 5px;" class="splitLine"></div>
									<div class="formfontdesc"><#Instant_Guard_desc#></div>
									<table width="100%" border="1" align="center" cellpadding="4" cellspacing="0" bordercolor="#6b8fa3" class="FormTable">
										<thead>
										<tr>
											<td colspan="2"><#t2BC#></td>
										</tr>
										</thead>
										<tr>
											<th><#Instant_Guard_title#></th>
											<td>
												<div align="center" class="left" style="float:left;cursor:pointer;" id="radio_IG_enable"></div>
												<div class="iphone_switch_container" style="height:32px; width:74px; position: relative; overflow: hidden;"></div>
												<script type="text/javascript">
												$('#radio_IG_enable').iphoneSwitch('<% nvram_get("ipsec_ig_enable"); %>',
													function() {
														curState = "1";
														set_ig_config(1);
													},
													function() {
														curState = "0";
														set_ig_config(0);
													},
													{
														switch_on_container_path: '/switcherplugin/iphone_switch_container_off.png'
													}
												);
											</script>
											</td>
										</tr>
										<tr>
											<th><#WLANAuthentication11a_ExAuthDBIPAddr_itemname#></th>
											<td id="ig_server_address">-</td>
										</tr>
										<tr>
											<th><#System_Log#></th>
											<td>
												<input class="button_gen" onclick="viewLog()" type="button" value="<#CTL_check_log#>"/>
											</td>
										</tr>
										<tr id="tr_vpn_server_client_access">
											<th><#vpn_access#></th>
											<td>
												<input type="radio" name="vpn_server_client_access" id="vpn_server_client_access_internet" class="input" value="1" onchange="vpnServerClientAccess();">
												<label for="vpn_server_client_access_internet">Internet only</label>
												<input type="radio" name="vpn_server_client_access" id="vpn_server_client_access_both" class="input" value="0" onchange="vpnServerClientAccess();">
												<label for="vpn_server_client_access_both"><#vpn_access_WANLAN#></label>
												<br>
												<span class="hint-color">The access setting will be applied to both IPSec VPN and Instant Guard.</span><!-- untranslated -->
											</td>
										</tr>
									</table>
									 <div id="tableContainer"></div>


									 </div>	<!-- for .container  -->
									 <div class="popup_container popup_element_second"></div>

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
