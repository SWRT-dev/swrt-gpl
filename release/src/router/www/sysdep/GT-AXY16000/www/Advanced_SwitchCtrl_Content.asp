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
<title><#Web_Title#> - Switch Control</title>
<link rel="stylesheet" type="text/css" href="index_style.css"> 
<link rel="stylesheet" type="text/css" href="form_style.css">
<link rel="stylesheet" type="text/css" href="other.css">
<script type="text/javascript" src="/state.js"></script>
<script type="text/javascript" src="/general.js"></script>
<script type="text/javascript" src="/popup.js"></script>
<script type="text/javascript" src="/help.js"></script>
<script type="text/javascript" src="/detect.js"></script>
<script type="text/javascript" src="/js/jquery.js"></script>
<style>
.perNode_app_table{
	width: 740px;
	position: absolute;
	left: 50%;
	margin-top: 30px;
	margin-left: -370px;
}
</style>

<script>
var wanports_bond = '<% nvram_get("wanports_bond"); %>';
var sw_mode = '<% nvram_get("sw_mode"); %>';
var lacp_support = isSupport("lacp");
var lacp_enabled = '<% nvram_get("lacp_enabled"); %>' == 1 ?true: false;
var bonding_policy_support = false;
if( lacp_support
&& (based_modelid == "GT-AC5300" || based_modelid == "RT-AC86U" || based_modelid == "GT-AC2900" || based_modelid == "RT-AC87U" || based_modelid == "RT-AC5300" || based_modelid == "RT-AC88U" || based_modelid == "RT-AC3100")){
	bonding_policy_support = true;
	var bonding_policy_value = '<% nvram_get("bonding_policy"); %>';
}

var lacp_enabled_ori = '<% nvram_get("lacp_enabled"); %>';
var wireless = [<% wl_auth_list(); %>];	// [[MAC, associated, authorized], ...]

function disable_lacp_if_conflicts_with_iptv(){
	if((based_modelid == "RT-AX89U" || based_modelid == "GT-AXY16000")){
		// LAN1 and/or LAN2.
		if(document.form.switch_wantag.value == "none" && (document.form.switch_stb_x.value == 1 || document.form.switch_stb_x.value == 2 || document.form.switch_stb_x.value == 5)){
			var note_str = "This function is disabled because LAN1 or LAN2 is configured as IPTV STB port."; //untranslated
			document.form.lacp_enabled.style.display = "none";
			document.getElementById("lacp_note").innerHTML = note_str;
			document.getElementById("lacp_desc").style.display = "";
			document.form.lacp_enabled.disabled = true;
			document.form.lacp_enabled.value = "0";
		}
	}
}

function initial(){
	var nataccel = '<% nvram_get("qca_sfe"); %>';
	var nataccel_status = '<% nat_accel_status(); %>';

	if(nataccel == '1' && nataccel_status == '1'){
		document.getElementById("natAccelDesc").innerHTML = "<#NAT_Acceleration_enable#>";
	}
	else{
		document.getElementById("natAccelDesc").innerHTML = "<#NAT_Acceleration_ctf_disable#>";
	}
	if((based_modelid == "RT-AX89U" || based_modelid == "GT-AXY16000")){
			document.form.sfpp_max_speed.disabled = false;
			document.form.sfpp_force_on.disabled = false;
			document.getElementById("sfpp_max_speed_tr").style.display = "";
			document.getElementById("sfpp_force_on_tr").style.display = "";
	}
	if(qca_support){
		if(sw_mode == 2 || sw_mode == 3){
			document.getElementById("qca_tr").style.display = 'none';
		}
	}
	if (re_mode == "1"){
		$("#tabMenu").addClass("perNode_app_table");
		$(".submenuBlock").css("margin-top", "initial");
		show_loading_obj();
	} else {
		$("#content_table").addClass("content");
		$("#tabMenu").addClass("app_table app_table_usb");
		show_menu();
	}

	$("#tabMenu").css("display", "");

	if(lacp_support){
		document.getElementById("lacp_tr").style.display = "";
		if(lacp_enabled){
			document.getElementById("lacp_desc").style.display = "";
			if(bonding_policy_support){
				document.form.bonding_policy.value = bonding_policy_value;
				check_bonding_policy(document.form.lacp_enabled);
			}
		}
		else
			document.getElementById("lacp_desc").style.display = "none";
	}
	else{
		document.form.lacp_enabled.disabled = true;
	}

	if(lacp_support && wans_dualwan_array.indexOf("lan") != -1){
		var wan_lanport_text = "";
		if(based_modelid == "GT-AC5300")
			var bonding_port_settings = [{"val": "4", "text": "LAN5"}, {"val": "3", "text": "LAN6"}];
		else if(based_modelid == "RT-AC86U")
			var bonding_port_settings = [{"val": "4", "text": "LAN1"}, {"val": "3", "text": "LAN2"}];
		else
			var bonding_port_settings = [{"val": "1", "text": "LAN1"}, {"val": "2", "text": "LAN2"}];

		for(var i = 0; i < bonding_port_settings.length; i++){
			if(wans_lanport == bonding_port_settings[i].val){
				wan_lanport_text = bonding_port_settings[i].text.toUpperCase();
			}
		}

		if(wan_lanport_text!= ""){
			var note_str = "This function is disabled because " + wan_lanport_text + " is configured as WAN. If you want to enable it, please click <a href=\"http://router.asus.com/Advanced_WANPort_Content.asp\" target=\"_blank\" style=\"text-decoration:underline;\">here</a> to change dual wan settings."; //untranslated
			document.form.lacp_enabled.style.display = "none";
			document.getElementById("lacp_note").innerHTML = note_str;
			document.getElementById("lacp_desc").style.display = "";
			document.form.lacp_enabled.disabled = true;
		}
	}

	// LAN1 or LAN2 is used in WAN aggregation. (RT: Disable LAN LACP; AP/RP/MB: Disable WAN aggregation)
	if((based_modelid == "RT-AX89U" || based_modelid == "GT-AXY16000") && wan_bonding_support && document.form.bond_wan.value == "1" && (wanports_bond.indexOf("1") != -1 || wanports_bond.indexOf("2") != -1)){
		if (sw_mode == 2 || sw_mode == 3){
			document.form.bond_wan.disabled = false;
			document.form.bond_wan.value = "0";
		}else{
			var note_str = "This function is disabled because " + wanAggr_p2_name(wanports_bond) + " is configured as wan aggregation port. If you want to enable it, please click <a href=\"http://router.asus.com/Advanced_WAN_Content.asp\" target=\"_blank\" style=\"text-decoration:underline;\">here</a> to change wan aggregation settings."; //untranslated
			document.form.lacp_enabled.style.display = "none";
			document.form.lacp_enabled.disabled = true;
			document.getElementById("lacp_note").innerHTML = note_str;
			document.getElementById("lacp_desc").style.display = "";
			document.form.lacp_enabled.value = "0";
		}
	}

	disable_lacp_if_conflicts_with_iptv();
}

function applyRule(){
	if((based_modelid == "RT-AX89U" || based_modelid == "GT-AXY16000"))
		document.form.lacp_enabled.disabled = false;
	showLoading();
	document.form.submit();
}

function check_bonding_policy(obj){
	if(obj.value == "1"){
		if(bonding_policy_support){
			document.getElementById("lacp_policy_tr").style.display = "";
			document.form.bonding_policy.disabled = false;
		}

		document.getElementById("lacp_desc").style.display = "";
	}
	else{
		if(bonding_policy_support){
			document.getElementById("lacp_policy_tr").style.display = "none";
		}

		document.form.bonding_policy.disabled = true;
		document.getElementById("lacp_desc").style.display = "none";
	}
}
</script>
</head>

<body onload="initial();" onunLoad="return unload_body();">
<div id="TopBanner"></div>
<div id="hiddenMask" class="popup_bg">
	<table cellpadding="5" cellspacing="0" id="dr_sweet_advise" class="dr_sweet_advise" align="center">
		<tr>
		<td>
			<div class="drword" id="drword" style="height:110px;"><#Main_alert_proceeding_desc4#> <#Main_alert_proceeding_desc1#>...
				<br/>
				<br/>
	    </div>
		  <div class="drImg"><img src="images/alertImg.png"></div>
			<div style="height:70px;"></div>
		</td>
		</tr>
	</table>
<!--[if lte IE 6.5]><iframe class="hackiframe"></iframe><![endif]-->
</div>

<div id="Loading" class="popup_bg"></div>

<iframe name="hidden_frame" id="hidden_frame" src="" width="0" height="0" frameborder="0"></iframe>

<form method="post" name="form" id="ruleForm" action="/start_apply.htm" target="hidden_frame">
<input type="hidden" name="productid" value="<% nvram_get("productid"); %>">
<input type="hidden" name="current_page" value="Advanced_SwitchCtrl_Content.asp">
<input type="hidden" name="next_page" value="">
<input type="hidden" name="group_id" value="">
<input type="hidden" name="modified" value="0">
<input type="hidden" name="action_mode" value="apply_new">
<input type="hidden" name="action_script" value="reboot">
<input type="hidden" name="action_wait" value="30">
<input type="hidden" name="preferred_lang" id="preferred_lang" value="<% nvram_get("preferred_lang"); %>">
<input type="hidden" name="firmver" value="<% nvram_get("firmver"); %>">
<input type="hidden" name="switch_wantag" value="<% nvram_get("switch_wantag"); %>" disabled>
<input type="hidden" name="switch_stb_x" value="<% nvram_get("switch_stb_x"); %>" disabled>
<input type="hidden" name="bond_wan" value="<% nvram_get("bond_wan"); %>" disabled>
<input type="hidden" name="sfpp_force_on" value="<% nvram_get("sfpp_force_on"); %>" disabled>

<table id="content_table" align="center" cellspacing="0" style="margin:auto;">
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
		  <td bgcolor="#4D595D" valign="top"  >
		  <div>&nbsp;</div>
		  <div class="formfonttitle"><#menu5_2#> - <#Switch_itemname#></div>
      <div style="margin:10px 0 10px 5px;" class="splitLine"></div>
      <div class="formfontdesc"><#SwitchCtrl_desc#></div>
		  
		  <table width="100%" border="1" align="center" cellpadding="4" cellspacing="0" bordercolor="#6b8fa3"  class="FormTable">
		  	
	<tr id="qca_tr">
		<th><#NAT_Acceleration#></th>
          <td>
		<select name="qca_sfe" class="input_option">
			<option class="content_input_fd" value="0" <% nvram_match("qca_sfe", "0","selected"); %>><#WLANConfig11b_WirelessCtrl_buttonname#></option>
			<option class="content_input_fd" value="1" <% nvram_match("qca_sfe", "1","selected"); %>><#Auto#></option>
		</select>
		&nbsp
		<span id="natAccelDesc"></span>
	  </td>
      	</tr>					
	<tr>
		<th><#jumbo_frame#></th>
		<td>
		<select name="jumbo_frame_enable" class="input_option">
			<option class="content_input_fd" value="0" <% nvram_match("jumbo_frame_enable", "0","selected"); %>><#WLANConfig11b_WirelessCtrl_buttonname#></option>
			<option class="content_input_fd" value="1" <% nvram_match("jumbo_frame_enable", "1","selected"); %>><#WLANConfig11b_WirelessCtrl_button1name#></option>
		</select>
		</td>
	</tr>
	<tr id="sfpp_max_speed_tr" style="display:none">
		<th>SFP+ port maximum link speed</th><!--untranslated-->
		<td>
			<select name="sfpp_max_speed" class="input_option" disabled>
				<option value="0" <% nvram_match("sfpp_max_speed", "0","selected"); %>><#Auto#></option>
				<option value="1000" <% nvram_match("sfpp_max_speed", "1000","selected"); %>>1Gbps</option>
				<option value="10000" <% nvram_match("sfpp_max_speed", "10000","selected"); %>>10Gbps</option>
			</select>
		</td>
	</tr>

	<tr id="sfpp_force_on_tr" style="display:none">
		<th>SFP+ port TX clock</th><!--untranslated-->
		<td>
			<input type="radio" name="sfpp_force_on" value="0" <% nvram_match("sfpp_force_on", "0", "checked"); %>><#Auto#>
			<input type="radio" name="sfpp_force_on" value="1" <% nvram_match("sfpp_force_on", "1", "checked"); %>>ON<!--untranslated-->
		</td>
	</tr>

	<tr id="lacp_tr" style="display:none;">
	<th><a class="hintstyle" href="javascript:void(0);" onClick="openHint(29,1);"><#NAT_lacp#></a></th>
		<td>
			<select name="lacp_enabled" class="input_option"  onchange="check_bonding_policy(this);">
				<option class="content_input_fd" value="0" <% nvram_match("lacp_enabled", "0","selected"); %>><#WLANConfig11b_WirelessCtrl_buttonname#></option>
				<option class="content_input_fd" value="1" <% nvram_match("lacp_enabled", "1","selected"); %>><#WLANConfig11b_WirelessCtrl_button1name#></option>
			</select>
			<div id="lacp_desc" style="display:none"><span id="lacp_note"><#NAT_lacp_note#></span><div>
		</td>
	</tr>
	<tr id="lacp_policy_tr" style="display:none">
		<th>Bonding Policy</th>
		<td>
			<select name="bonding_policy" class="input_option" disabled>
				<option value="0">Default</option>
				<option value="1">Source Bonding</option>
				<option value="2">Destination Bonding</option>
			</select>
		</td>
	</tr>
		</table>	

		<div class="apply_gen">
			<input class="button_gen" onclick="applyRule()" type="button" value="<#CTL_apply#>"/>
		</div>
		
	  </td>
	</tr>

	</tbody>	
  </table>		
					
		</td>
	</form>					
</tr>
</table>
			<!--===================================End of Main Content===========================================-->
</td>

    <td width="10" align="center" valign="top">&nbsp;</td>
	</tr>
</table>

<div id="footer"></div>
</body>
</html>
