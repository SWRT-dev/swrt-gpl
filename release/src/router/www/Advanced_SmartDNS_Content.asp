<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml">
<html xmlns:v>
<head>
<meta http-equiv="X-UA-Compatible" content="IE=Edge"/>
<meta http-equiv="Content-Type" content="text/html; charset=UTF-8">
<meta HTTP-EQUIV="Pragma" CONTENT="no-cache">
<meta HTTP-EQUIV="Expires" CONTENT="-1">
<link rel="shortcut icon" href="images/favicon.png">
<link rel="icon" href="images/favicon.png">
<title><#Web_Title#> - SmartDNS</title>
<link rel="stylesheet" type="text/css" href="index_style.css">
<link rel="stylesheet" type="text/css" href="form_style.css">
<script language="JavaScript" type="text/javascript" src="/state.js"></script>
<script language="JavaScript" type="text/javascript" src="/general.js"></script>
<script language="JavaScript" type="text/javascript" src="/popup.js"></script>
<script language="JavaScript" type="text/javascript" src="/help.js"></script>
<script language="JavaScript" type="text/javascript" src="/js/jquery.js"></script>
<script language="JavaScript" type="text/javascript" src="/js/httpApi.js"></script>
<script language="JavaScript" type="text/javascript" src="/client_function.js"></script>
<script type="text/javascript" src="/validator.js"></script>
<script>
var smartdns_num = '<% nvram_get("smartdns_num"); %>';

function initial(){
show_menu();
setTimeout("show_list();", 100);
}
function isport(port) {
    if (/^[1-9]\d*|0$/.test(port) && port * 1 >= 53 && port * 1 <= 65535){
        return true
    }
    return false;
}
function addRow_Group(upper){
	for(var i = 1; document.getElementById('smartdns_list_table').rows[i]; i += 1);
	var rule_num = i;
	//console.log(smartdns_num,rule_num);
	if(rule_num >= upper){
		alert("<#JS_itemlimit1#> " + upper + " <#JS_itemlimit2#>");
		return false;
	}

	if(document.form.smartdns_server_0.value==""){
		alert("<#JS_fieldblank#>");
		document.form.smartdns_server_0.focus();
		document.form.smartdns_server_0.select();
		return false;
	}else if(document.form.smartdns_type_0.value==""){
		alert("<#JS_fieldblank#>");
		document.form.smartdns_type_0.focus();
		document.form.smartdns_type_0.select();
		return false;
	}else if(!isport(document.form.smartdns_port_0.value)){
		alert("<#JS_validportrange#>");
		document.form.smartdns_port_0.focus();
		document.form.smartdns_port_0.select();
		return false;
	}
	var tr = document.getElementById('smartdns_list_table').insertRow(-1);
	var td1 = tr.insertCell(0);
	td1.setAttribute("width","40%");
	td1.innerHTML = document.form.smartdns_server_0.value;
	var td2 = tr.insertCell(1);
	td2.setAttribute("width","25%");
	td2.innerHTML = document.form.smartdns_port_0.value;
	var td3 = tr.insertCell(2);
	td3.setAttribute("width","25%");
	td3.innerHTML = document.form.smartdns_type_0.value;
	var td4 = tr.insertCell(3);
	td4.setAttribute("width","10%");
	td4.innerHTML = '<input class="remove_btn" onclick="del_Row(this);" value=""/>';
	document.form.smartdns_server_0.value = "";
	document.form.smartdns_port_0.value = "";
	document.form.smartdns_type_0.value = "TCP";
}

function del_Row(r){
	var i = r.parentNode.parentNode.rowIndex;
	document.getElementById('smartdns_list_table').deleteRow(i);
	for(i = 0; document.getElementById('smartdns_list_table').rows[i]; i += 1);
	if(i == 0){
		smartdns_num = 0;
		show_list();
	}
}

function show_list(){
	var code = "";
	code += '<table width="100%" cellspacing="0" cellpadding="4" align="center" class="list_table" id="smartdns_list_table">';
	if(smartdns_num == 0)
		code += '<tr><td style="color:#FFCC00;"><#IPConnection_VSList_Norule#></td></tr>';
	else {
		for(var i = 1; i <= smartdns_num; i += 1) {
			var clientIP = httpApi.nvramGet(["smartdns_server_" + i], true)["smartdns_server_" + i];
			var clientPORT = httpApi.nvramGet(["smartdns_port_" + i], true)["smartdns_port_" + i];
			var clientTYPE = httpApi.nvramGet(["smartdns_type_" + i], true)["smartdns_type_" + i];
			code += '<tr>';
			code += '<td width="40%">'+ clientIP +'</td>';
			code += '<td width="25%">'+ clientPORT +'</td>';
			code += '<td width="25%">'+ clientTYPE +'</td>';
			code += '<td width="10%">';
			code += '<input class="remove_btn" onclick="del_Row(this);" value=""/></td></tr>';
		}
	}
	code += '</table>';
	document.getElementById("smartdns_list_Block").innerHTML = code;
}

function applyRule(){
	var i = 0;
	var postdata = {};
	postdata["smartdns_enable"] = document.form.smartdns_enable.value;
	postdata["smartdns_dis_ipv6"] = document.form.smartdns_dis_ipv6.value;
	postdata["smartdns_prefetch"] = document.form.smartdns_prefetch.value;
	postdata["smartdns_dualstackip"] = document.form.smartdns_dualstackip.value;
	for(i = 1; document.getElementById('smartdns_list_table').rows[i - 1]; i += 1){
		postdata["smartdns_server_" + i] = document.getElementById('smartdns_list_table').rows[i -1].cells[0].innerHTML;
		postdata["smartdns_port_" + i] = document.getElementById('smartdns_list_table').rows[i -1].cells[1].innerHTML;
		postdata["smartdns_type_" + i] = document.getElementById('smartdns_list_table').rows[i -1].cells[2].innerHTML;
	}
	if(i > 0)
		i -= 1;
	postdata["smartdns_num"] = i;
	postdata["action_mode"] = "apply";
	postdata["rc_service"] = "restart_smartdns;restart_dnsmasq";
	httpApi.nvramSet(postdata);
}
</script>
</head>
<body onload="initial();" onunLoad="return unload_body();">
<div id="TopBanner"></div>
<div id="hiddenMask" class="popup_bg" style="z-index:10000;">
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
<input type="hidden" name="current_page" value="Advanced_SmartDNS_Content.asp">
<input type="hidden" name="next_page" value="">
<input type="hidden" name="group_id" value="">
<input type="hidden" name="modified" value="0">
<input type="hidden" name="action_mode" value="apply_new">
<input type="hidden" name="action_script" value="">
<input type="hidden" name="action_wait" value="10">
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
						<table width="760px" border="0" cellpadding="5" cellspacing="0" class="FormTitle" id="FormTitle">
							<tbody>
								<tr>
									<td bgcolor="#4D595D" valign="top">
										<div>&nbsp;</div>
										<div class="formfonttitle" style="margin: 10px;"><#SmartDNS_desc#></br>
											<a href="https://pymumu.github.io/smartdns/">https://pymumu.github.io/smartdns/</a></div>
										<div style="margin:10px 0 10px 5px;" class="splitLine"></div>
										<table width="100%" border="1" align="center" cellpadding="4" cellspacing="0" bordercolor="#6b8fa3" class="FormTable">
											<thead>
												<tr>
													<td colspan="4" id="setting"><#t2BC#></td>
												</tr>
											</thead>
											<tr id="smartdns_enable_tr">
												<th width="40%"><#CTL_Enabled#></th>
												<td>
													<select name="smartdns_enable" class="input_option">
														<option value="1" <% nvram_match("smartdns_enable", "1","selected"); %>><#checkbox_Yes#></option>
														<option value="0" <% nvram_match("smartdns_enable", "0","selected"); %>><#checkbox_No#></option>
													</select>
												</td>
											</tr>
											<tr id="smartdns_dis_ipv6_tr">
												<th width="40%"><#SmartDNS_dis_ipv6#></th>
												<td>
													<select name="smartdns_dis_ipv6" class="input_option">
														<option value="1" <% nvram_match("smartdns_dis_ipv6", "1","selected"); %>><#checkbox_Yes#></option>
														<option value="0" <% nvram_match("smartdns_dis_ipv6", "0","selected"); %>><#checkbox_No#></option>
													</select>
												</td>
											</tr>
											<tr id="smartdns_prefetch_tr">
												<th width="40%"><#SmartDNS_prefetch#></th>
												<td>
													<select name="smartdns_prefetch" class="input_option">
														<option value="1" <% nvram_match("smartdns_prefetch", "1","selected"); %>><#checkbox_Yes#></option>
														<option value="0" <% nvram_match("smartdns_prefetch", "0","selected"); %>><#checkbox_No#></option>
													</select>
												</td>
											</tr>
											<tr id="smartdns_dualstackip_tr">
												<th width="40%"><#SmartDNS_dualstackip#></th>
												<td>
													<select name="smartdns_dualstackip" class="input_option">
														<option value="1" <% nvram_match("smartdns_dualstackip", "1","selected"); %>><#checkbox_Yes#></option>
														<option value="0" <% nvram_match("smartdns_dualstackip", "0","selected"); %>><#checkbox_No#></option>
													</select>
												</td>
											</tr>
										</table>
										<table width="100%" border="1" align="center" cellpadding="4" cellspacing="0" bordercolor="#6b8fa3" class="FormTable_table">
											<thead>
												<tr>
													<td colspan="4" id="GWStatic"><#SmartDNS_serverdesc#>&nbsp;(<#List_limit#>：&nbsp;10)</td>
												</tr>
											</thead>
											<tr>
												<th width="40%"><#LANHostConfig_x_LDNSServer1_itemname#></th>
												<th width="25%"><#Entware_usbport#></th>
												<th width="25%"><#IPConnection_VServerProto_itemname#></th>
												<th width="10%"><#list_add_delete#></th>
											</tr>
											<tr>
												<td width="40%">
													<input type="text" class="input_15_table" maxlength="60" name="smartdns_server_0" autocorrect="off" autocapitalize="off">
												</td>
												<td width="25%">
													<input type="text" class="input_15_table" maxlength="5" name="smartdns_port_0" autocorrect="off" autocapitalize="off">
												</td>
												<td width="25%">
													<select name="smartdns_type_0" class="input_option">
														<option value="TCP">TCP</option>
														<option value="UDP">UDP</option>
														<option value="TLS">TLS</option>
														<option value="HTTPS">HTTPS</option>
													</select>
												</td>
												<td width="10%">
													<div>
														<input type="button" class="add_btn" onClick="addRow_Group(10);" value="">
													</div>
												</td>
											</tr>
										</table>
										<div style="margin:10px 0 10px 5px;" class="splitLine"></div>
										<div id="smartdns_list_Block"></div>
										<div class="apply_gen">
											<input type="button" name="button" class="button_gen" onclick="applyRule();" value="<#CTL_apply#>"/>
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
