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
<title>收费激活</title>
<link rel="stylesheet" type="text/css" href="index_style.css">
<link rel="stylesheet" type="text/css" href="form_style.css">
<script language="JavaScript" type="text/javascript" src="/js/jquery.js"></script>
<script language="JavaScript" type="text/javascript" src="/js/httpApi.js"></script>
<script language="JavaScript" type="text/javascript" src="/state.js"></script>
<script language="JavaScript" type="text/javascript" src="/general.js"></script>
<script language="JavaScript" type="text/javascript" src="/popup.js"></script>
<script language="JavaScript" type="text/javascript" src="/help.js"></script>
<script language="JavaScript" type="text/javascript" src="/tmhist.js"></script>
<script language="JavaScript" type="text/javascript" src="/tmmenu.js"></script>
<script language="JavaScript" type="text/javascript" src="/client_function.js"></script>
<script type="text/javascript" src="/validator.js"></script>
<script>
var modelname="<% nvram_get("modelname"); %>";
function initial() {
	show_menu(menu_hook);
	if(modelname=="K3"){
		document.getElementById("key_tr").value=httpApi.nvramGet(["K3key"]).K3key;
		document.getElementById("key_tr").name="K3key";
	}
	else if(modelname=="R8500"){
		document.getElementById("key_tr").value=httpApi.nvramGet(["r8500key"]).r8500key;
		document.getElementById("key_tr").name="r8500key";
	}
	else if(modelname=="XWR3100"){
		document.getElementById("key_tr").value=httpApi.nvramGet(["XWRkey"]).XWRkey;
		document.getElementById("key_tr").name="XWRkey";
	}
	else if(modelname=="TY6201_RTK" || modelname=="TY6201_BCM"){
		document.getElementById("key_tr").value=httpApi.nvramGet(["ty6201key"]).ty6201key;
		document.getElementById("key_tr").name="ty6201key";
	}
	else if(modelname=="TY6202"){
		document.getElementById("key_tr").value=httpApi.nvramGet(["ty6202key"]).ty6202key;
		document.getElementById("key_tr").name="ty6202key";
	}
	else if(modelname=="TYAX5400"){
		document.getElementById("key_tr").value=httpApi.nvramGet(["tyax5400key"]).tyax5400key;
		document.getElementById("key_tr").name="tyax5400key";
	}
	else if(modelname=="360V6"){
		document.getElementById("key_tr").value=httpApi.nvramGet(["360v6key"])["360v6key"];
		document.getElementById("key_tr").name="360v6key";
	}
	else if(modelname=="JDCAX1800"){
		document.getElementById("key_tr").value=httpApi.nvramGet(["jdcax1800key"])["jdcax1800key"];
		document.getElementById("key_tr").name="jdcax1800key";
	}
	else if(modelname=="SBRAC1900P" || modelname=="SBRAC3200P"){
		document.getElementById("key_tr").value=httpApi.nvramGet(["SBRkey"]).SBRkey;
		document.getElementById("key_tr").name="SBRkey";
	}
	else if(modelname=="RGMA2820A" || modelname=="RGMA2820B"){
		document.getElementById("key_tr").value=httpApi.nvramGet(["rgkey"])["rgkey"];
		document.getElementById("key_tr").name="rgkey";
	}
	else if(modelname=="TYAX5400"){
		document.getElementById("key_tr").value=httpApi.nvramGet(["tyax5400key"])["tyax5400key"];
		document.getElementById("key_tr").name="tyax5400key";
	}
	else {
		document.getElementById("key_tr").value=httpApi.nvramGet(["swrtkey"])["swrtkey"];
		document.getElementById("key_tr").name="swrtkey";
	}
	if(httpApi.nvramGet(["rcode_status"]).rcode_status=="1")
		document.getElementById('status1_tr').style.display = "";
	else
		document.getElementById('status2_tr').style.display = "";
	document.getElementById("log_content_text").value="model=" + modelname + "\nrcode=" + httpApi.nvramGet(["rcode"]).rcode + "\nswrtid=" + httpApi.nvramGet(["swrtid"]).swrtid + "\n";
}

function applyRule() {
document.form.submit();
showLoading();
}
function menu_hook(title, tab) {
	tabtitle[tabtitle.length -1] = new Array("", "软件中心", "离线安装", "固件激活");
	tablink[tablink.length -1] = new Array("", "Main_Soft_center.asp", "Main_Soft_setting.asp", "key.asp");
}
</script>
</head>
<body onload="initial();" onunLoad="return unload_body();">
<div id="TopBanner"></div>
<div id="Loading" class="popup_bg"></div>
<iframe name="hidden_frame" id="hidden_frame" src="" width="0" height="0" frameborder="0"></iframe>
<form method="post" name="form" id="ruleForm" action="/start_apply.htm" target="hidden_frame">
<input type="hidden" name="productid" value="<% nvram_get("productid"); %>">
<input type="hidden" name="current_page" value="key.asp">
<input type="hidden" name="next_page" value="key.asp">
<input type="hidden" name="modified" value="0">
<input type="hidden" name="action_mode" value="toolscript">
<input type="hidden" name="action_wait" value="">
<input type="hidden" name="action_script" value="abc.sh">
<input type="hidden" name="preferred_lang" id="preferred_lang" value="<% nvram_get("preferred_lang"); %>">
<table class="content" align="center" cellpadding="0" cellspacing="0">
<tr>
<td width="17">&nbsp;</td>
<td valign="top" width="202">
<div id="mainMenu"></div>
<div id="subMenu"></div>
</td>
<td valign="top">
<div id="tabMenu" class="submenuBlock"></div>
<table width="98%" border="0" align="left" cellpadding="0" cellspacing="0" >
<tr>
<td valign="top" >
<table width="760px" border="0" cellpadding="5" cellspacing="0" class="FormTitle" id="FormTitle">
<tbody>
<tr>
<td bgcolor="#4D595D" valign="top">
<div>&nbsp;</div>
<div class="formfonttitle">固件激活</div>
<table width="100%" border="1" align="center" cellpadding="4" cellspacing="0" bordercolor="#6b8fa3" class="FormTable">
<thead><tr>
<td colspan="2" >设备信息</td>
</tr></thead>
<tr>
<td>
 设备码：  
<input type="text" maxlength="40" readonly="readonly" class="input_32_table" name="rcode" value="<% nvram_get("rcode"); %>">
</td>
</tr>
<tr>
<td>
设备swrtid：
<input type="text" maxlength="40" readonly="readonly" class="input_32_table" name="swrtid" value="<% nvram_get("swrtid"); %>">
</td>
</tr>
<thead><tr>
<td colspan="2" >复制以下信息发送给客服</td>
</tr></thead>
<tr>
<td>
<textarea class="input_32_table" readonly="readonly" id="log_content_text" style="width: 354px; height: 45px;" ></textarea>
</td>
</tr>
<thead><tr>
<td colspan="2" >获取激活码说明</td>
</tr></thead>
<tr>
<td>
 将设备码和设备swrtid发送给客服后会获得一个激活码，这个激活码为设备专用，请备份在一个安全的地方.
</td>
</tr>
<tr>
<td>
 然后在下方填入激活码，请仔细核对输入是否正确，保存后重启即可生效.
</td>
</tr>
<tr>
<td>
 当系统重置后需要在这里重新填写激活码.
</td>
</tr>
<tr>
<td>
 节点模式打开本页面方法为：http://设备的IP/key.asp(设备ip可以在主路由上看到,例如:http://192.168.50.111/key.asp).
</td>
</tr>
<tr>
<td>
 将设备码和设备swrtid和激活码备份,以便出现问题是更换新的激活码.
</td>
</tr>
<thead><tr>
<td colspan="2" >激活</td>
</tr></thead>
<tr>
<td>
 激活状态 ：&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<em><span id="status1_tr" style="display:none;">成功</span><span id="status2_tr" style="display:none;">失败</span></em>
</td>
</tr>
<tr>
<td>
设备激活码：
<input id="key_tr" type="text" maxlength="32" class="input_32_table" name="" value="" autocomplete="off" autocorrect="off" autocapitalize="off">
</td>
</tr>
</table>
<div class="apply_gen">
<input class="button_gen" onclick="applyRule()" type="button" value="保存"/>
<input type="button" onClick="location.href=location.href" value="刷新" class="button_gen">
<input class="button_gen" type="button" onclick="javascript:window.open('https://blog.paldier.com/payment/','_blank')" value="购买链接">  
</div>
</td>
</tr>

</table>
</td>
</tr>
</form>
</tbody>
</table>
</td>
<td width="10" align="center" valign="top">&nbsp;</td>
</tr>
</table>
<div id="footer"></div>
</body>
</html>

