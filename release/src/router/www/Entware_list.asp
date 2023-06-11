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
<title>Entware</title>
<link rel="stylesheet" type="text/css" href="index_style.css">
<link rel="stylesheet" type="text/css" href="form_style.css">
<script language="JavaScript" type="text/javascript" src="/state.js"></script>
<script language="JavaScript" type="text/javascript" src="/popup.js"></script>
<script language="JavaScript" type="text/javascript" src="/help.js"></script>
<script type="text/javascript" src="/js/jquery.js"></script>
<script>
function load_page(){
	document.getElementById("app_list").style.height = document.getElementById("FormTitle").style.height;
	document.getElementById("app_list").src = "http://bin.entware.net/" + "<%nvram_get("entware_arch");%>" + "/Packages.html";
}
function initial(){
	show_menu();
	if(rog_support){
		$("#app_list").css("background-color", "transparent");
	}
	setTimeout(load_page, 100);
}
function show_list(ent_state){
	if (ent_state > 1) {
		$("#isbusy").show();
		$("#inputdiv").hide();
		setTimeout("get_list();", 2000);
	} else if (ent_state == 1) {
		$("#isbusy").hide();
		$("#inputdiv").show();
	} else {
		$("#isbusy").hide();
		$("#inputdiv").hide();
		$("#applist_table").hide();
		$("#app_list").hide();
	}
};
function installapp(){
	var postdata = $("#newapp").serialize();
	console.log(postdata);
	$.getJSON("entware.cgi", postdata, function(jdata){
		var state = jdata.entware_is_install;
		show_list(state);
	});
};
function get_list(){
	$.getJSON("entware.cgi", {"entware_action":"list_installed"}, function(jdata){
		var state = jdata.entware_is_install;
		show_list(state);
	});
};
$(document).ready(function(){
	initial();
	get_list();
});
</script>
</head>
<body>
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
<table width="760px" border="0" cellpadding="5" cellspacing="0" bordercolor="#6b8fa3" class="FormTitle" id="FormTitle">
<tr bgcolor="#4D595D">
<td valign="top">
<div>&nbsp;</div>
<div class="formfonttitle"><#Entware_newapp#></div>
<div style="margin:10px 0 10px 5px;" class="splitLine"></div>
<div class="formfontdesc"><#Entware_desc8#></div>
<div id="inputdiv">
<form id="newapp">
<input type="text" class="input_30_table" name="entware_app" value="">
<input type="hidden" name="entware_action" value="install">
<input class="button_gen" onclick="installapp();" type="button" value="<#Excute#>">
</form>
</div>
<table width="100%" border="1" align="center" cellpadding="4" cellspacing="0" bordercolor="#6b8fa3" class="FormTable">
<thead>
<tr id="isbusy" style="display:none"><td width="30%"></td><td width="5%"><img src="/images/InternetScan.gif"></td><td><#Main_alert_proceeding_desc4#><#Main_alert_proceeding_desc1#></td></tr>
</thead>
<tr><td colspan="3">&nbsp;</td></tr>
</table>
<table width="100%" border="1" align="center" cellpadding="4" cellspacing="0" bordercolor="#6b8fa3" class="FormTable" id="applist_table">
<thead>
<tr>
<td>EntWare <#Entware_list#></td>
</tr>
</thead>
</table>
<div style="background-color: #4D595D; border-radius: 0 0 3px 3px;"><iframe id="app_list" style="width: 100%; border: none;"></iframe></div>
</td>
</tr>
</table>
</td>
<td width="10" align="center" valign="top">&nbsp;</td>
</tr>
</table>
<div id="footer"></div>
</body>
</html>

