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
<title>EntWare - <#Entware_installed#></title>
<link rel="stylesheet" type="text/css" href="index_style.css">
<link rel="stylesheet" type="text/css" href="form_style.css">
<script language="JavaScript" type="text/javascript" src="/state.js"></script>
<script language="JavaScript" type="text/javascript" src="/general.js"></script>
<script language="JavaScript" type="text/javascript" src="/popup.js"></script>
<script language="JavaScript" type="text/javascript" src="/help.js"></script>
<script language="JavaScript" type="text/javascript" src="/js/jquery.js"></script>
<script language="JavaScript" type="text/javascript" src="/js/httpApi.js"></script>
<script>
function show_list(ent_state, ent_list, update_list){
	if (ent_state > 1) {
		$("#isbusy").show();
		var code = '<table width="100%" class="list_table" border="1" cellspacing="0" cellpadding="4"><tr><td><#Entware_autorefresh#></td></tr></table>';
		$("#app_list").html(code);
		setTimeout("get_list();", 2000);
	} else if (ent_state == 1) {
		$("#isbusy").hide();
		var code = '<table width="100%" class="list_table" border="1" cellspacing="0" cellpadding="4">';
		for (var i=0; i<ent_list.length-1; i++) {
			code += '<tr><td width="30%" align="left">' + ent_list[i].split(" - ")[0] + '</td><td width="40%" align="left">' + ent_list[i].split(" - ")[1];
			for (var j=0; j<update_list.length-1; j++) {
				if (ent_list[i].split(" - ")[0] == update_list[j].split(" - ")[0]) {
					code += '<br/>(<#Entware_updatable#>)' + update_list[j].split(" - ")[2] + '</td><td><input class="button_gen" type="button" name="' + ent_list[i].split(" - ")[0] + '" onclick="updateapp(this);" value="<#LANHostConfig_x_DDNSStatus_buttonname#>">';
					break;
				}	
			}
			code += '</td><td><input class="button_gen" type="button" name="' + ent_list[i].split(" - ")[0] + '" onclick="removeapp(this);" value="<#btn_remove#>"></td></tr>';
		}
		code += '</table>';
		$("#app_list").html(code);
		$("#app_list").show();
	} else {
		$("#isbusy").hide();
		var code = '<table width="100%" class="list_table" border="1" cellspacing="0" cellpadding="4"><tr><td><#Entware_notfound#></td></tr></table>';
		$("#app_list").html(code);
	}
};
function updateapp(app){
	var sure = confirm('<#Entware_update#>'+app.name+' ?');
	if (sure == true){
		$("#isbusy").show();
		var postdata = 'entware_app=' + app.name + '&entware_action=update';
		console.log(postdata);
		$.getJSON("entware.cgi", postdata, function(jdata){
			var state = jdata.entware_is_install;
			var list = [""];
			show_list(state, list);
		});
	}
};
function removeapp(app){
	if (confirm('<#Entware_del#>'+app.name+' ?') == true){
		$("#isbusy").show();
		var postdata = 'entware_app=' + app.name + '&entware_action=remove';
		console.log(postdata);
		$.getJSON("entware.cgi", postdata, function(jdata){
			var state = jdata.entware_is_install;
			var list = [""];
			show_list(state, list);
		});
	}
};
function get_list(){
	$.getJSON("entware.cgi", {"entware_action":"list_installed"}, function(jdata){
		var state = jdata.entware_is_install;
		var list = jdata.entware_app_list;
		var update = jdata.entware_update_list;
		show_list(state, list, update);
	});
};
function initial(){
show_menu();
get_list();
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
<table width="98%" border="0" align="left" cellpadding="0" cellspacing="0">
<tr>
<td valign="top" >
<table width="760px" border="0" cellpadding="5" cellspacing="0" bordercolor="#6b8fa3" class="FormTitle" id="FormTitle">
<tr bgcolor="#4D595D">
<td valign="top">
<div>&nbsp;</div>
<div class="formfonttitle"><#Entware_installedop#></div>
<div style="margin:10px 0 10px 5px;" class="splitLine"></div>
<div class="formfontdesc"><#Entware_desc5#></div>
<div class="formfontdesc"><#Entware_desc6#></div>
<div class="formfontdesc"><#Entware_desc7#></div>
<table width="100%" border="1" align="center" cellpadding="4" cellspacing="0" bordercolor="#6b8fa3" class="FormTable" id="isbusy" style="display:none">
<thead>
<tr><td width="30%"></td><td width="5%"><img src="/images/InternetScan.gif"></td><td><#Main_alert_proceeding_desc4#><#Main_alert_proceeding_desc1#></td></tr>
</thead>
<tr><td colspan="3">&nbsp;</td></tr>
</table>
<table width="100%" border="1" align="center" cellpadding="4" cellspacing="0" class="FormTable_table">
<thead><tr><td colspan="3"><#Entware_installedall#></td></tr></thead>
<tr>
<th width="30%"><#Entware_appname#></th>
<th width="40%"><#Entware_appver#></th>
<th width="30%"><#Entware_operation#></th>
</tr>
</table>
<div id="app_list"></div>
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

