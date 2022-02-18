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
<title>软件中心 - 自定义脚本</title>
<link rel="stylesheet" type="text/css" href="index_style.css">
<link rel="stylesheet" type="text/css" href="form_style.css">
<style>
p{
font-weight: bolder;
}
.disabled-feature-ihacklog {
display: none;
}
</style>
<script language="JavaScript" type="text/javascript" src="/state.js"></script>
<script language="JavaScript" type="text/javascript" src="/general.js"></script>
<script language="JavaScript" type="text/javascript" src="/popup.js"></script>
<script language="JavaScript" type="text/javascript" src="/help.js"></script>
<script language="JavaScript" type="text/javascript" src="/tmhist.js"></script>
<script language="JavaScript" type="text/javascript" src="/tmmenu.js"></script>
<script language="JavaScript" type="text/javascript" src="/client_function.js"></script>
<script language="JavaScript" type="text/javascript" src="/js/jquery.js"></script>
<script type="text/javascript" src="/validator.js"></script>
<script type="text/javascript" src="/switcherplugin/jquery.iphone-switch.js"></script>
<script>
function initial() {
show_menu();
document.form.script_content.value = document.form.script_content.value.replace(/,yushi,/g,"\r\n");
}
function script_sel(val) {
document.form.action_script.value = "k3c_script_ch.sh";
document.form.submit();
}
function applyRule() {
document.form.script_content.value = document.form.myscript_content.value.replace(/\r\n/g,",yushi,");
document.form.script_content.value = document.form.script_content.value.replace(/\n/g,",yushi,");
showLoading();
document.form.submit();
}
function done_validating(action) {
refreshpage();
}
function reload_Soft_Center(){
location.href = "/Softcenter.asp";
}
</script>
</head>
<body onload="initial();" onunLoad="return unload_body();">
<div id="TopBanner"></div>
<div id="hiddenMask" class="popup_bg" style="z-index:10000;">
<table cellpadding="5" cellspacing="0" id="dr_sweet_advise" class="dr_sweet_advise" align="center">
<tr>
<td>
<div class="drword" id="drword" style="height:110px;">
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
<form method="post" name="form" id="ruleForm" action="/start_apply.htm" target="hidden_frame">
<input type="hidden" name="productid" value="<% nvram_get("productid"); %>">
<input type="hidden" name="current_page" value="Tools_Script.asp">
<input type="hidden" name="next_page" value="">
<input type="hidden" name="modified" value="0">
<input type="hidden" name="action_mode" value="toolscript">
<input type="hidden" name="action_wait" value="0">
<input type="hidden" name="action_script" value="k3c_script.sh">
<input type="hidden" name="first_time" value="">
<input type="hidden" name="script_content" value="">
<input type="hidden" name="preferred_lang" id="preferred_lang" value="<% nvram_get("preferred_lang"); %>">
<tr>
<td align="left" valign="top">
<table width="760px" border="0" cellpadding="5" cellspacing="0" class="FormTitle" id="FormTitle">
<tbody>
<tr>
<td bgcolor="#4D595D" valign="top">
<div>&nbsp;</div>
<div class="formfonttitle">软件中心 - 自定义脚本</div>
										<div style="float:right; width:15px; height:25px;margin-top:10px"><img id="return_btn" onclick="reload_Soft_Center();" align="right" style="cursor:pointer;position:absolute;margin-left:-30px;margin-top:-25px;" title="返回软件中心" src="/images/backprev.png" onMouseOver="this.src='/images/backprevclick.png'" onMouseOut="this.src='/images/backprev.png'"></img></div>
<div style="margin-left:5px;margin-top:10px;margin-bottom:10px"><img src="/images/New_ui/export/line_export.png"></div>
<table width="100%" border="1" align="center" cellpadding="4" cellspacing="0" bordercolor="#6b8fa3" class="FormTable">
<thead><tr>
<td colspan="2" >脚本显示和编辑</td>
</tr></thead>
<tr>
<td>
选择需要显示或编辑的脚本：
<select id="script_type" name="script_name" style="width:250px;margin:0px 0px 0px 2px;" class="input_option" onchange="script_sel(this.value);" >
<option value="init-start" <% nvram_match( "script_name", "init-start","selected"); %>>初始化完成后（init-start）</option>
<option value="services-start" <% nvram_match( "script_name", "services-start","selected"); %>>系统服务启动后（services-start）</option>
<option value="services-stop" <% nvram_match( "script_name", "services-stop","selected"); %>>系统服务关闭前（services-stop）</option>
<option value="wan-start" <% nvram_match( "script_name", "wan-start","selected"); %>>外网连接成功脚本（wan-start）</option>
<option value="firewall-start" <% nvram_match( "script_name", "firewall-start","selected"); %>>防火墙脚本（firewall-start）</option>
<option value="nat-start" <% nvram_match( "script_name", "nat-start","selected"); %>>NAT脚本（nat-start）</option>
<option value="pre-mount" <% nvram_match( "script_name", "pre-mount","selected"); %>>分区挂载前脚本（pre-mount）</option>
<option value="post-mount" <% nvram_match( "script_name", "post-mount","selected"); %>>分区挂载后脚本（post-mount）</option>
<option value="unmount" <% nvram_match( "script_name", "unmount","selected"); %>>分区卸载脚本（unmount）</option>
<option value="ddns-start" <% nvram_match( "script_name", "ddns-start","selected"); %>>DDNS脚本（ddns-start）</option>
</select>
<a href="https://github.com/RMerl/asuswrt-merlin/wiki/User-scripts" target=_blank>【脚本说明】</a>
</td>
</tr>
<tr>
<td>
<textarea cols="63" rows="35" wrap="off" id="myscript_content" style="width:99%;font-family:Courier New, Courier, mono; font-size:11px;background:#475A5F;color:#FFFFFF;">
<% nvram_dump("script_temp",""); %>
</textarea>
</td>
</tr>
</table>
<div class="apply_gen">
<input class="button_gen" onclick="applyRule()" type="button" value="保存"/>
<input type="button" onClick="location.href=location.href" value="刷新" class="button_gen">
</div>
</td>
</tr>
</tbody>
</table>
</td>
</tr>
</form>
</table>
</td>
<td width="10" align="center" valign="top">&nbsp;</td>
</tr>
</table>
<div id="footer"></div>
</body>
</html>
