﻿<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml">
<head>
<meta http-equiv="X-UA-Compatible" content="IE=Edge">
<meta http-equiv="Content-Type" content="text/html; charset=utf-8" />
<meta HTTP-EQUIV="Pragma" CONTENT="no-cache">
<meta HTTP-EQUIV="Expires" CONTENT="-1">
<link rel="shortcut icon" href="images/favicon.png">
<link rel="icon" href="images/favicon.png">
<title><#Network_Tools#> - <#Network_Analysis#></title>
<link rel="stylesheet" type="text/css" href="index_style.css"> 
<link rel="stylesheet" type="text/css" href="form_style.css">
<link rel="stylesheet" type="text/css" href="/device-map/device-map.css">
<script type="text/javascript" src="/js/jquery.js"></script>
<script language="JavaScript" type="text/javascript" src="/state.js"></script>
<script language="JavaScript" type="text/javascript" src="/general.js"></script>
<script language="JavaScript" type="text/javascript" src="/popup.js"></script>
<script language="JavaScript" type="text/javascript" src="/help.js"></script>
<script language="JavaScript" type="text/javascript" src="/validator.js"></script>
<script type="text/javascript" src="/form.js"></script>
<script>
var wans_dualwan_orig = '<% nvram_get("wans_dualwan"); %>';
var wans_flag =  (wans_dualwan_orig.search("none") != -1 || !dualWAN_support) ? 0 : 1;
var ipv6_proto_orig = '<% nvram_get("ipv6_service"); %>';
var ipv61_proto_orig = '<% nvram_get("ipv61_service"); %>';

var current_page = window.location.pathname.split("/").pop();
var faq_index_tmp = get_faq_index(FAQ_List, current_page, 1);

function initial(){
	show_menu();
	showLANIPList();
	update_ntool_unit();
}

function validForm(){

	if(!validator.isValidHost(document.form.destIP.value)){
		document.form.destIP.focus();
		return false;
	}

	if(document.form.cmdMethod.value == "ping"){
		if(!validator.range(document.form.pingCNT, 1, 99))
			return false;
	}

	return true;
}

function onSubmitCtrl(o, s) {
	document.form.action_mode.value = s;
	updateOptions();
}

function updateOptions(){
	if(document.form.destIP.value == ""){
		document.form.destIP.value = AppListArray[0][1];
	}

	if(document.form.cmdMethod.value == "ping"){
		if(document.form.pingCNT.value == ""){
			document.form.pingCNT.value = 5;
		}
		if($("#cmd_for_ipv6_checkbox").is(":checked")){
			document.form.SystemCmd.value = "ping6 -c " + document.form.pingCNT.value + " " + document.form.destIP.value;
		}
		else{
			document.form.SystemCmd.value = "ping -c " + document.form.pingCNT.value + " " + document.form.destIP.value;
		}
	}
	else if(document.form.cmdMethod.value == "traceroute"){
		if($("#cmd_for_ipv6_checkbox").is(":checked")){
			document.form.SystemCmd.value = "traceroute6 " + document.form.destIP.value;
		}
		else{
			document.form.SystemCmd.value = "traceroute " + document.form.destIP.value;
		}
	}
	else
		document.form.SystemCmd.value = document.form.cmdMethod.value + " " + document.form.destIP.value;
	
	if(validForm()){
		document.form.submit();
		document.getElementById("cmdBtn").disabled = true;
		document.getElementById("cmdBtn").style.color = "#666";
		document.getElementById("loadingIcon").style.display = "";
		setTimeout("checkCmdRet();", 500);
	}
}

function hideCNT(_val){
	if(_val == "ping"){
		document.getElementById("pingCNT_tr").style.display = "";
		document.getElementById("cmdDesc").innerHTML = "<#NetworkTools_Ping#>";
	}
	else if(_val == "traceroute"){
		document.getElementById("pingCNT_tr").style.display = "none";
		document.getElementById("cmdDesc").innerHTML = "<#NetworkTools_tr#>";
	}
	else{
		document.getElementById("pingCNT_tr").style.display = "none";
		document.getElementById("cmd_for_ipv6").style.display = "none";
		document.getElementById("cmdDesc").innerHTML = "<#NetworkTools_nslookup#>";
	}
	update_ntool_unit();
}

function update_ntool_unit(){
	var wans_dualwan_array = '<% nvram_get("wans_dualwan"); %>'.split(" ");
	var wans_mode = '<%nvram_get("wans_mode");%>';
	if(sw_mode != 1 || document.form.cmdMethod.value == "nslookup" || !dualWAN_support || wans_mode != "lb" || wans_dualwan_array.indexOf("none") != -1){
		document.getElementById("wans_ntool_unit").style.display = "none";
		if(ipv6_proto_orig != "disabled" && (document.form.cmdMethod.value == "ping" || document.form.cmdMethod.value == "traceroute")){
			document.getElementById("cmd_for_ipv6").style.display = "";
			$("#cmd_for_ipv6_checkbox").attr('checked', true);
		}
		
		return;
	}
	document.getElementById("wans_ntool_unit").style.display = "";
	if(mtwancfg_support || based_modelid == "BRT-AC828"){   //dual_wans on
		if(document.form.wans_ntool_unit.value == '0'){

			if(ipv6_proto_orig != "disabled" && (document.form.cmdMethod.value == "ping" || document.form.cmdMethod.value == "traceroute")){
				document.getElementById("cmd_for_ipv6").style.display = "";
				$("#cmd_for_ipv6_checkbox").attr('checked', true);
			}
		}
		if(document.form.wans_ntool_unit.value == '1'){

			if(ipv61_proto_orig != "disabled" && (document.form.cmdMethod.value == "ping" || document.form.cmdMethod.value == "traceroute")){
				document.getElementById("cmd_for_ipv6").style.display = "";
				$("#cmd_for_ipv6_checkbox").attr('checked', true);
			}
		}
	}
}


var _responseLen;
var noChange = 0;
function checkCmdRet(){
	$.ajax({
		url: '/cmdRet_check.htm',
		dataType: 'html',
		
		error: function(xhr){
			setTimeout("checkCmdRet();", 1000);
		},
		success: function(response){
			var retArea = document.getElementById("textarea");
			var _cmdBtn = document.getElementById("cmdBtn");

			if(response.search("XU6J03M6") != -1){
				document.getElementById("loadingIcon").style.display = "none";
				_cmdBtn.disabled = false;
				_cmdBtn.style.color = "";
				retArea.value = response.replace("XU6J03M6", " ");
				retArea.scrollTop = retArea.scrollHeight;
				return false;
			}

			if(_responseLen == response.length)
				noChange++;
			else
				noChange = 0;

			if(noChange > 10){
				document.getElementById("loadingIcon").style.display = "none";
				_cmdBtn.disabled = false;
				_cmdBtn.style.color = "";
				setTimeout("checkCmdRet();", 1000);
			}
			else{
				_cmdBtn.disabled = true;
				_cmdBtn.style.color = "#666";
				document.getElementById("loadingIcon").style.display = "";
				setTimeout("checkCmdRet();", 1000);
			}

			retArea.value = response;
			retArea.scrollTop = retArea.scrollHeight;
			_responseLen = response.length;
		}
	});
}

var AppListArray = [
		["Google ", "www.google.com"], ["Facebook", "www.facebook.com"], ["Youtube", "www.youtube.com"], ["Yahoo", "www.yahoo.com"],
		["Baidu", "www.baidu.com"], ["Wikipedia", "www.wikipedia.org"], ["Windows Live", "www.live.com"], ["QQ", "www.qq.com"],
		["Twitter", "www.twitter.com"], ["Taobao", "www.taobao.com"], ["Blogspot", "www.blogspot.com"], 
		["Linkedin", "www.linkedin.com"], ["eBay", "www.ebay.com"], ["Bing", "www.bing.com"], 
		["Яндекс", "www.yandex.ru"], ["WordPress", "www.wordpress.com"], ["ВКонтакте", "www.vk.com"]
	];

function showLANIPList(){
	var code = "";
	for(var i = 0; i < AppListArray.length; i++){
		code += '<a><div onmouseover="over_var=1;" onmouseout="over_var=0;" onclick="setClientIP(\''+AppListArray[i][1]+'\');"><strong>'+AppListArray[i][0]+'</strong></div></a>';
	}
	code +='<!--[if lte IE 6.5]><iframe class="hackiframe2"></iframe><![endif]-->';	
	document.getElementById("ClientList_Block_PC").innerHTML = code;
}

function setClientIP(ipaddr){
	document.form.destIP.value = ipaddr;
	hideClients_Block();
	over_var = 0;
}

var over_var = 0;
var isMenuopen = 0;
function hideClients_Block(){
	document.getElementById("pull_arrow").src = "/images/unfold_more.svg";
	document.getElementById('ClientList_Block_PC').style.display='none';
	isMenuopen = 0;
}

function pullLANIPList(obj){
	if(isMenuopen == 0){		
		obj.src = "/images/unfold_less.svg"
		document.getElementById("ClientList_Block_PC").style.display = 'block';		
		document.form.destIP.focus();		
		isMenuopen = 1;
	}
	else
		hideClients_Block();
}
</script>
</head>
<body onload="initial();" class="bg">
<div id="TopBanner"></div>
<div id="Loading" class="popup_bg"></div>
<iframe name="hidden_frame" id="hidden_frame" src="" width="0" height="0" frameborder="0"></iframe>
<form method="POST" name="form" action="/apply.cgi" target="hidden_frame">
<input type="hidden" name="current_page" value="Main_Analysis_Content.asp">
<input type="hidden" name="next_page" value="Main_Analysis_Content.asp">
<input type="hidden" name="group_id" value="">
<input type="hidden" name="modified" value="0">
<input type="hidden" name="action_mode" value="">
<input type="hidden" name="action_script" value="">
<input type="hidden" name="action_wait" value="">
<input type="hidden" name="first_time" value="">
<input type="hidden" name="preferred_lang" id="preferred_lang" value="<% nvram_get("preferred_lang"); %>">
<input type="hidden" name="SystemCmd" value="">
<input type="hidden" name="firmver" value="<% nvram_get("firmver"); %>">

<table class="content" align="center" cellpadding="0" cellspacing="0">
	<tr>
		<td width="17">&nbsp;</td>
		<td valign="top" width="202">
			<div id="mainMenu"></div>
			<div id="subMenu"></div>
		</td>	
		<td valign="top">
			<div id="tabMenu" class="submenuBlock"></div>		
			<table width="98%" border="0" align="left" cellpadding="0" cellspacing="0">
				<tr>
					<td align="left" valign="top">				
						<table width="760px" border="0" cellpadding="5" cellspacing="0" bordercolor="#6b8fa3"  class="FormTitle" id="FormTitle">		
							<tr>
								<td bgcolor="#4D595D" colspan="3" valign="top">
								<div class="container">

									<div>&nbsp;</div>
									<div class="formfonttitle"><#Network_Tools#> - <#Network_Analysis#></div>
									<div class="formfonttitle_help"><i onclick="show_feature_desc(`How to use Network Analysis(Ping, Traceroute, Nsloopup) in ASUS Router?`)" class="icon_help"></i></div>
									<div style="margin:10px 0 10px 5px;" class="splitLine"></div>
									<div class="formfontdesc" id="cmdDesc"><#NetworkTools_Ping#></div>
									<table width="100%" border="1" align="center" cellpadding="4" cellspacing="0" bordercolor="#6b8fa3" class="FormTable">
										<tr>
											<th width="20%"><#NetworkTools_Method#></th>
											<td>
												<select id="cmdMethod" class="input_option" name="cmdMethod" onchange="hideCNT(this.value);">
													<option value="ping" selected>Ping</option>
													<option value="traceroute">Traceroute</option>
													<option value="nslookup">Nslookup</option>
 												</select>
											</td>										
										</tr>
										<tr id="wans_ntool_unit">
											<th width="20%"><#wan_interface#></th>
											<td>
												<select name="wans_ntool_unit" class="input_option" onChange="update_ntool_unit();">
													<option class="content_input_fd" value="0" <% nvram_match("wans_ntool_unit", "0","selected"); %>><#dualwan_primary#></option>
													<option class="content_input_fd" value="1" <% nvram_match("wans_ntool_unit", "1","selected"); %>><#dualwan_secondary#></option>
												</select>
											</td>
										</tr>
										<tr>
											<th width="20%"><#NetworkTools_target#></th>
											<td>
                                                <div class="clientlist_dropdown_main">
                                                    <input type="text" class="input_32_table" name="destIP" maxlength="100" value="" placeholder="ex: www.google.com" autocorrect="off" autocapitalize="off">
                                                    <img id="pull_arrow" height="14px;" src="/images/unfold_more.svg" onclick="pullLANIPList(this);" title="<#select_network_host#>" onmouseover="over_var=1;" onmouseout="over_var=0;">
                                                    <div id="ClientList_Block_PC" class="clientlist_dropdown"></div>
												</div>
												<span id="cmd_for_ipv6" style="display:none;"><input id="cmd_for_ipv6_checkbox" type="checkbox">for IPv6 address</span> <!-- Untranslated -->
											</td>
										</tr>
										<tr id="pingCNT_tr">
											<th width="20%"><#NetworkTools_Count#></th>
											<td>
												<input type="text" name="pingCNT" class="input_3_table" maxlength="2" value="" onKeyPress="return validator.isNumber(this, event);" placeholder="5" autocorrect="off" autocapitalize="off">
											</td>
										</tr>
									</table>

									<div class="apply_gen">
										<span><input class="btn_subusage button_gen" id="cmdBtn" onClick="onSubmitCtrl(this, ' Refresh ')" type="button" value="<#NetworkTools_Diagnose_btn#>"></span>
										<img id="loadingIcon" style="display:none;" src="/images/InternetScan.gif">
									</div>

									<div style="margin-top:8px" id="logArea">
										<textarea cols="63" rows="27" wrap="off" readonly="readonly" id="textarea" style="width:99%;font-family:Courier New, Courier, mono; font-size:11px;background:#475A5F;color:#FFFFFF;">
											<% nvram_dump("syscmd.log","syscmd.sh"); %>
										</textarea>
									</div>

									</div>	<!-- for .container  -->
									<div class="popup_container popup_element_second"></div>

								</td>
							</tr>
						</table>
					</td>
				</tr>
			</table>
			<!--===================================Ending of Main Content===========================================-->
		</td>
		<td width="10" align="center" valign="top"></td>
	</tr>
</table>
</form>

<div id="footer"></div>
</body>
<script type="text/javascript">
<!--[if !IE]>-->
	(function($){
		var textArea = document.getElementById('textarea');
		textArea.scrollTop = textArea.scrollHeight;
	})(jQuery);
<!--<![endif]-->
</script>
</html>
