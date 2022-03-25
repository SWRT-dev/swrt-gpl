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
<title><#Entware_home#></title>
<link rel="stylesheet" type="text/css" href="index_style.css">
<link rel="stylesheet" type="text/css" href="form_style.css">
<script language="JavaScript" type="text/javascript" src="/state.js"></script>
<script language="JavaScript" type="text/javascript" src="/general.js"></script>
<script language="JavaScript" type="text/javascript" src="/popup.js"></script>
<script language="JavaScript" type="text/javascript" src="/help.js"></script>
<script language="JavaScript" type="text/javascript" src="/js/jquery.js"></script>
<script language="JavaScript" type="text/javascript" src="/js/httpApi.js"></script>
<script>
var asusopt = '<%nvram_get("apps_state_install");%>' || '<%nvram_get("webdav_aidisk");%>';
var entware_mount = '<% nvram_get("entware_mount"); %>';
var entware_disk = '<% nvram_get("entware_disk"); %>';
function get_disks(){
	require(['/require/modules/diskList.js'], function(diskList) {
		usbDevicesList = diskList.list();
		//console.log(usbDevicesList)
		var html = '';
		html += '<thead>'
		html += '<tr>'
		html += '<td colspan="8"><#Entware_usblist#></td>'
		html += '</tr>'
		html += '</thead>'	
		html += '<tr>'
		html += '<th style="width:auto"><#Entware_usbport#></th>'
		html += '<th style="width:auto"><#DiskLabel#></th>'
		html += '<th style="width:auto"><#Entware_usbsize#></th>'
		html += '<th style="width:auto"><#Entware_usbused#></th>'
		html += '<th style="width:auto"><#Entware_usbpermission#></th>'
		html += '<th style="width:auto"><#Entware_usbtype#></th>'
		html += '<th style="width:auto"><#Entware_usbmountp#></th>'
		html += '<th style="width:auto"><#Entware_usbpath#></th>'
		html += '</tr>'
		for (var i = 0; i < usbDevicesList.length; ++i){
			for (var j = 0; j < usbDevicesList[i].partition.length; ++j){
				//append options
				if((usbDevicesList[i].partition[j].status != "unmounted")&&((usbDevicesList[i].partition[j].format.indexOf("ext") != -1)||((usbDevicesList[i].partition[j].format.indexOf("ntfs") != -1) && (usbDevicesList[i].partition[j].format.indexOf("ntfs3") == -1)))){
					if(usbDevicesList[i].partition[j].partName==entware_disk)
						$("#entware_disk").append("<option value='" + usbDevicesList[i].partition[j].partName + "' selected='selected'>" + usbDevicesList[i].partition[j].partName + "</option>");
else
						$("#entware_disk").append("<option value='" + usbDevicesList[i].partition[j].partName + "'>" + usbDevicesList[i].partition[j].partName + "</option>");
				}
				var totalsize = ((usbDevicesList[i].partition[j].size)/1000000).toFixed(2);
				var usedsize = ((usbDevicesList[i].partition[j].used)/1000000).toFixed(2);
				var usedpercent = (usedsize/totalsize*100).toFixed(2) + " %";
				var used = usedsize + " GB" + " (" + usedpercent + ")"
				html += '<tr>'
				html += '<td>' + usbDevicesList[i].usbPath + '</td>'
				html += '<td>' + usbDevicesList[i].deviceName + '</td>'
				html += '<td>' + totalsize + " GB" + '</td>'
				html += '<td>' + used + '</td>'
				html += '<td>' + usbDevicesList[i].partition[j].status + '</td>'
				html += '<td>' + usbDevicesList[i].partition[j].format + '</td>'
				html += '<td>' + usbDevicesList[i].partition[j].mountPoint + '</td>'
				html += '<td>' + '/tmp/mnt/' + usbDevicesList[i].partition[j].partName + '</td>'
				html += '</tr>'
			}
		}
		$('#disk_table').html(html);
	});
}
function show_list(ent_state, ent_list){
	if (asusopt.length && asusopt != "0"){
		$("#isbusy").hide();
		$("#updateEntware").parent().parent().hide();
		$("#initEntware").parent().parent().show();
		$("#listtable").hide();
		$(".apply_gen").hide();
		return;
	}
	if (ent_state > 1) {
		$("#initEntware").parent().parent().hide();
		$("#updateEntware").parent().parent().hide();
		$("#isbusy").show();
		var code = '<table width="100%" class="list_table" border="1" cellspacing="0" cellpadding="4"><tr><td><#Entware_autorefresh#></td></tr></table>';
		$("#start_list").html(code);
		$(".apply_gen").hide();
		setTimeout("get_list();", 2000);
	} else if (ent_state == 1) {
		$("#isbusy").hide();
		$("#updateEntware").parent().parent().show();
		var n = ent_list.length;
		var code = '<table width="100%" border="1" cellspacing="0" cellpadding="4" align="center" class="list_table">';
		if (n >1) {
			for (var i=0; i<n-1; i++) {
				var entliststatus;
				if(ent_list[i][1] == "1")
					entliststatus = "√";
				else
					entliststatus = "×";
				code += '<tr><td width="50%" align="left">' + ent_list[i][0] + '</td><td width="10%" align="center">' + entliststatus + '</td><td width="20%">';
				code += '<input class="button_gen" type="button" name="' + ent_list[i][0] + '" onclick="startapp(this);" value="<#Entware_app_start#>"></td><td width="20%"><input class="button_gen" type="button" name="' + ent_list[i][0] + '" onclick="stopapp(this);" value="<#Entware_app_stop#>"></td></tr>';
			}
		} else {
			code += '<tr><td><#Entware_app_none#></td></tr>';
		}
		code += '</table>';
		$("#start_list").html(code);
		$("#start_list").show();
		$(".apply_gen").show();
	} else {
		$("#isbusy").hide();
		$("#updateEntware").parent().parent().hide();
		$("#initEntware").parent().parent().show();
		$("#listtable").hide();
		$(".apply_gen").hide();
	}
};
$(document).ready(function(){
$("#initEntware").click(function(){
	if (asusopt.length && asusopt != "0"){
		alert("<#Entware_desc4#>");
		return;
	}
	if(document.getElementById("entware_mount").value == "1" && document.getElementById("entware_disk").value == "0"){
		alert("<#no_usb_found#>");
		return;
	}
	$("#isbusy").show();
	var postdata = $("#entware_form").serialize();
	postdata += '&entware_mount=' + document.getElementById("entware_mount").value + '&entware_disk=' + document.getElementById("entware_disk").value;
	$.getJSON("entware.cgi", postdata, function(jdata){
		var ent_state = jdata.entware_is_install;
		show_list(ent_state);
	});
});
$("#updateEntware").click(function(){
	$("#isbusy").show();
	var postdata = 'entware_action=update';
	$.getJSON("entware.cgi", postdata, function(jdata){
		var ent_state = jdata.entware_is_install;
		show_list(ent_state);
	});
});
$("#startEntware").click(function(){
	if (asusopt.length && asusopt != "0"){
		alert("<#Entware_desc4#>");
		return;
	}
	if(document.getElementById("entware_mount").value == "1" && document.getElementById("entware_disk").value == "0"){
		alert("<#no_usb_found#>");
		return;
	}
	var postdata = $("#entware_form").serialize();
	postdata += '&entware_mount=' + document.getElementById("entware_mount").value + '&entware_disk=' + document.getElementById("entware_disk").value;
	$.getJSON("entware.cgi", postdata, function(jdata){
		var ent_state = jdata.entware_is_install;
		show_list(ent_state);
	});
});
});
function startapp(app){
	$("#isbusy").show();
	var postdata = 'entware_app=' + app.name + '&entware_action=start';
	console.log(postdata);
	$.getJSON("entware.cgi", postdata, function(jdata){
		var state = jdata.entware_is_install;
		show_list(state);
	});
};
function stopapp(app){
	$("#isbusy").show();
	var postdata = 'entware_app=' + app.name + '&entware_action=stop';
	console.log(postdata);
	$.getJSON("entware.cgi", postdata, function(jdata){
		var state = jdata.entware_is_install;
		show_list(state);
	});
};
function get_list(){
	$.getJSON("entware.cgi", function(jdata){
		var ent_state = jdata.entware_is_install;
		var ent_list = jdata.entware_start_list;
		show_list(ent_state, ent_list);
	});
};
function initial(){
show_menu();
get_list();
get_disks();
document.getElementById("entware_mount").value = entware_mount;
}
</script>
</head>
<body onload="initial();" class="bg">
<div id="TopBanner"></div>
<div id="Loading" class="popup_bg"></div>
<iframe name="hidden_frame" id="hidden_frame" src="" width="0" height="0" frameborder="0"></iframe>
<table class="content" align="center" cellpadding="0" cellspacing="0">
<tr>
<td width="17">&nbsp;</td>
<td valign="top" width="202">
<div id="mainMenu"></div>
<div id="subMenu"></div>
</td>
<td valign="top">
<div id="tabMenu" class="submenuBlock"></div>
<table width="98%" border="0" align="left" cellpadding="0" cellspacing="0" style="display: block;">
<tr>
<td valign="top" >
<table width="760px" border="0" cellpadding="5" cellspacing="0" bordercolor="#6b8fa3" class="FormTitle" id="FormTitle">
<tr bgcolor="#4D595D">
<td valign="top">
<div>&nbsp;</div>
<div class="formfonttitle"><#Entware_env#></div>
<div style="margin:10px 0 10px 5px;" class="splitLine"></div>
<div class="formfontdesc"><#Entware_desc1#></div>
<div class="formfontdesc"><#Entware_desc2#></div>
<div class="formfontdesc"><#Entware_desc3#></div>
<div class="formfontdesc"><#Entware_desc4#></div>
<div id="entware_disks_status" style="margin:10px 0px 0px 0px;">
<table width="100%" border="1" align="center" cellpadding="4" cellspacing="0" bordercolor="#6b8fa3" class="FormTable" id="disk_table">
</table>
<table width="100%" border="1" align="center" cellpadding="4" cellspacing="0" bordercolor="#6b8fa3" class="FormTable" id="entware_usb_disk">
<tr>
<th width="30%" style="border-top: 0 none;"><#CTL_Enabled#></th>
<td style="border-top: 0 none;">
<select class="input_option input_25_table" id="entware_mount" name="entware_mount">
<option value="0"><#checkbox_No#></option>
<option value="1"><#checkbox_Yes#></option>
</select>
</td>
</tr>
<tr>
<th><#Entware_mount#></th>
<td>
<select id="entware_disk" name="entware_disk" class="input_option input_25_table">
<option value="0"><#no_usb_found#></option>
</select>
</td>
</tr>
</table>
</div>
<div>&nbsp;</div>
<table width="100%" border="1" align="center" cellpadding="4" cellspacing="0" bordercolor="#6b8fa3" class="FormTable" id="isbusy" style="display:none">
<thead>
<tr><td width="30%"></td><td width="5%"><img src="/images/InternetScan.gif"></td><td><#Main_alert_proceeding_desc4#><#Main_alert_proceeding_desc1#></td></tr>
</thead>
<tr><td colspan="3">&nbsp;</td></tr>
</table>
<table width="100%" border="1" align="center" cellpadding="4" cellspacing="0" bordercolor="#6b8fa3" class="FormTable">
<thead><tr><td colspan="2"><#Entware_env#></td></tr></thead>
<tr style="display:none">
<td>
<form id="entware_form">
<input type="hidden" name="entware_app" value="entware">
<input type="hidden" name="entware_action" value="install">
<select name="entware_type" class="input_option">
<option value="generic"><#Entware_type1#></option>
<option value="alternative"><#Entware_type2#></option>
</select>
</form>
</td>
<td><input class="button_gen" id="initEntware" type="button" value="<#Excute#>"></td>
</tr>
<tr style="display:none">
<td><#Entware_updateentware#></td>
<td><input class="button_gen" id="updateEntware" type="button" value="<#LANHostConfig_x_DDNSStatus_buttonname#>"></td>
</tr>
</table>
<table width="100%" border="1" id="listtable" cellpadding="4" cellspacing="0" class="FormTable_table">
<thead><tr><td colspan="3"><#Entware_service#></td></tr></thead>
<tr>
<th width="50%"><#BM_UserList1#></th>
<th width="10%"><#Status_Str#></th>
<th width="40%"><#Entware_operation#></th>
</tr>
</table>
<form id="app_list_form">
<div id="start_list"></div>
<div class="apply_gen"><input type="button" class="button_gen" id="startEntware" value="<#CTL_apply#>"/></div>
</form>
</td>
</tr>
</table>
</td>
</tr>
</table>
</td>
</tr>
</table>
<div id="footer"></div>
</body>
</html>

