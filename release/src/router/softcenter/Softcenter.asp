<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml">
<head>
<meta http-equiv="X-UA-Compatible" content="IE=Edge"/>
<meta http-equiv="Content-Type" content="text/html; charset=utf-8" />
<meta HTTP-EQUIV="Pragma" CONTENT="no-cache"/>
<meta HTTP-EQUIV="Expires" CONTENT="-1"/>
<link rel="shortcut icon" href="images/favicon.png"/>
<link rel="icon" href="images/favicon.png"/>
<title sclang>jffs advanced settings</title>
<link rel="stylesheet" type="text/css" href="index_style.css"/>
<link rel="stylesheet" type="text/css" href="form_style.css"/>
<link rel="stylesheet" type="text/css" href="usp_style.css"/>
<link rel="stylesheet" type="text/css" href="ParentalControl.css">
<link rel="stylesheet" type="text/css" href="css/icon.css">
<link rel="stylesheet" type="text/css" href="css/element.css">
<link rel="stylesheet" type="text/css" href="/device-map/device-map.css">
<link rel="stylesheet" type="text/css" href="/calendar/fullcalendar.css">
<script type="text/javascript" src="/state.js"></script>
<script type="text/javascript" src="/popup.js"></script>
<script type="text/javascript" src="/help.js"></script>
<script type="text/javascript" src="/validator.js"></script>
<script type="text/javascript" src="/js/jquery.js"></script>
<script type="text/javascript" src="/general.js"></script>
<script type="text/javascript" src="/switcherplugin/jquery.iphone-switch.js"></script>
<script type="text/javascript" src="/client_function.js"></script>
<script type="text/javascript" src="/js/httpApi.js"></script>
<script type="text/javascript" src="/js/i18n.js"></script>
<style>

</style>
<script>
var sc_disk='<% nvram_get("sc_disk"); %>';
var sc_mount='<% nvram_get("sc_mount"); %>';
var sctype;
var partitions_array = [];
function show_partition(){
require(['/require/modules/diskList.js?hash=' + Math.random().toString()], function(diskList){
var code="";
var free=dict['Free'];
var Unsupportfat=dict['Unsupportfat'];
var mounted_partition = 0;
partitions_array = [];
var usbDevicesList = diskList.list();
for(var i=0; i < usbDevicesList.length; i++){
for(var j=0; j < usbDevicesList[i].partition.length; j++){
partitions_array.push(usbDevicesList[i].partition[j].mountPoint);
var accessableSize = simpleNum(usbDevicesList[i].partition[j].size-usbDevicesList[i].partition[j].used);
var totalSize = simpleNum(usbDevicesList[i].partition[j].size);
sctype=usbDevicesList[i].partition[j].format || "";
if(usbDevicesList[i].partition[j].status == "unmounted")
continue;
if((usbDevicesList[i].partition[j].partName==sc_disk)&&((usbDevicesList[i].partition[j].format.indexOf("ext") != -1)||(usbDevicesList[i].partition[j].format.indexOf("tntfs") != -1)))
code +='<option value="'+ usbDevicesList[i].partition[j].partName+'" selected="selected">'+ usbDevicesList[i].partition[j].partName+'(' + free +':'+accessableSize+' GB ' + sctype + ')</option>';
else if(usbDevicesList[i].partition[j].format.indexOf("fat") != -1 || usbDevicesList[i].partition[j].format.indexOf("ntfs3") != -1)
code +='<option value="0">' + Unsupportfat + '</option>';
else
code +='<option value="'+ usbDevicesList[i].partition[j].partName+'" >'+ usbDevicesList[i].partition[j].partName+'(' + free +':'+accessableSize+' GB ' + sctype + ')</option>';
mounted_partition++;
}
}
if(mounted_partition == 0)
code +='<option value="0" sclang>No Disk</option>';
document.getElementById("usb_disk_id").innerHTML = code;
});
}
function update_ui(){
	if(sc_mount=="1")
		document.getElementById('usb_disk_tr').style.display = "";
	else if(sc_mount=="2"){
		document.getElementById("sc_cifs_url_id").value=httpApi.nvramGet(["sc_cifs_url"]).sc_cifs_url;
		document.getElementById("sc_cifs_user_id").value=httpApi.nvramGet(["sc_cifs_user"]).sc_cifs_user;
		document.getElementById("sc_cifs_pw_id").value=httpApi.nvramGet(["sc_cifs_pw"]).sc_cifs_pw;
		document.getElementById('sc_cifs_url_tr').style.display = "";
		document.getElementById('sc_cifs_user_tr').style.display = "";
		document.getElementById('sc_cifs_pw_tr').style.display = "";
	}
	else{
		document.getElementById('usb_disk_tr').style.display = "none";
		document.getElementById('sc_cifs_url_tr').style.display = "none";
		document.getElementById('sc_cifs_user_tr').style.display = "none";
		document.getElementById('sc_cifs_pw_tr').style.display = "none";
	}
}
function change_ui(value){
	if(value == "1"){
		document.getElementById('usb_disk_tr').style.display = "";
		document.getElementById('sc_cifs_url_tr').style.display = "none";
		document.getElementById('sc_cifs_user_tr').style.display = "none";
		document.getElementById('sc_cifs_pw_tr').style.display = "none";
	}
	else if(value=="2"){
		document.getElementById("sc_cifs_url_id").value=httpApi.nvramGet(["sc_cifs_url"]).sc_cifs_url;
		document.getElementById("sc_cifs_user_id").value=httpApi.nvramGet(["sc_cifs_user"]).sc_cifs_user;
		document.getElementById("sc_cifs_pw_id").value=httpApi.nvramGet(["sc_cifs_pw"]).sc_cifs_pw;
		document.getElementById('sc_cifs_url_tr').style.display = "";
		document.getElementById('sc_cifs_user_tr').style.display = "";
		document.getElementById('sc_cifs_pw_tr').style.display = "";
		document.getElementById('usb_disk_tr').style.display = "none";
	}
	else{
		document.getElementById('usb_disk_tr').style.display = "none";
		document.getElementById('sc_cifs_url_tr').style.display = "none";
		document.getElementById('sc_cifs_user_tr').style.display = "none";
		document.getElementById('sc_cifs_pw_tr').style.display = "none";
	}
}
function init() {
	show_menu();
	sc_load_lang("sc");
	show_partition();
	update_ui();
}
function applyRule() {
if(document.getElementById("usb_disk_id").value == 0 && document.getElementById("sc_mount_id").value == 1)
{
alert(dict['No Disk']);
return;
}
if(document.getElementById("sc_cifs_url_id").value == "" && document.getElementById("sc_mount_id").value == 2)
{
alert(dict['No Disk']);
return;
}
document.form.submit();
showLoading();
}
function reload_Soft_Center(){
location.href = "/Softcenter.asp";
}
</script>
</head>
<body onload="init();">
	<div id="TopBanner"></div>
	<div id="Loading" class="popup_bg"></div>
	<iframe name="hidden_frame" id="hidden_frame" src="" width="0" height="0" frameborder="0"></iframe>
	<form method="post" name="form" id="ruleForm" action="/start_apply.htm" target="hidden_frame">
	<input type="hidden" name="current_page" value="Softcenter.asp"/>
	<input type="hidden" name="next_page" value="Softcenter.asp"/>
	<input type="hidden" name="group_id" value=""/>
	<input type="hidden" name="modified" value="0"/>
	<input type="hidden" name="action_mode" value="toolscript"/>
	<input type="hidden" name="action_script" value="jffsinit.sh"/>
	<input type="hidden" name="action_wait" value=""/>
	<input type="hidden" name="first_time" value=""/>
	<input type="hidden" name="preferred_lang" id="preferred_lang" value="<% nvram_get("preferred_lang"); %>"/>
	<input type="hidden" name="firmver" value="<% nvram_get("firmver"); %>"/>
	<input type="hidden" name="sc_disk" value="<% nvram_get("sc_disk"); %>">
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
							<table width="760px" border="0" cellpadding="5" cellspacing="0" bordercolor="#6b8fa3" class="FormTitle" id="FormTitle">
								<tr>
									<td bgcolor="#4D595D" colspan="3" valign="top">
										<div>&nbsp;</div>
										<div style="float:left;" class="formfonttitle" sclang>jffs advanced settings</div>
										<div style="margin:30px 0 10px 5px;" class="splitLine"></div>
										<div class="formfontdesc" style="padding-top:5px;margin-top:0px;float: left;" id="cmdDesc" sclang>jffs advanced settings</div>
										<div style="color:#FC0;padding-top:5px;margin-top:25px;margin-left:0px;float: left;" id="NoteBox" >
											<li style="margin-top:5px;" sclang>This feature is to mount some space to jffs for softcenter.</li>
											<li style="margin-top:5px;" sclang>Support EXT/NTFS partition type.</li>
											<li style="margin-top:5px;" sclang>Minimum 1GB of free space.</li>
											<li style="margin-top:5px;" sclang>Must unmount the current partition before mounting other partitions.</li>
										</div>
										<table style="margin:10px 0px 0px 0px;" width="100%" border="1" align="center" cellpadding="4" cellspacing="0" bordercolor="#6b8fa3" class="FormTable" id="routing_table">
											<thead>
											<tr>
												<td colspan="2" sclang>Setting</td>
											</tr>
											</thead>
											<tr >
												<th width="30%" style="border-top: 0 none;" sclang>Enable</th>
												<td>
													<select id="sc_mount_id" name="sc_mount" class="input_option" onchange="change_ui(this.value);">
														<option value="0" <% nvram_match("sc_mount", "0", "selected"); %> sclang>Disable</option>
														<option value="1" <% nvram_match("sc_mount", "1", "selected"); %>>USB</option>
														<option value="2" <% nvram_match("sc_mount", "2", "selected"); %>>CIFS</option>
													</select>
												</td>
											</tr>
											<tr style="display:none;" id="usb_disk_tr">
												<th sclang>Select a partition to mount</th>
												<td>
													<select id="usb_disk_id" name="sc_disk" class="input_option">
														<option value="0" sclang>No Disk</option>
													</select>
												</td>
											</tr>
											<tr style="display:none;" id="sc_cifs_url_tr">
												<th sclang>Server path</th>
												<td>
													<input id="sc_cifs_url_id" type="text" class="input_32_table" maxlength="200" name="sc_cifs_url" autocorrect="off" autocapitalize="off"  placeholder="//192.168.1.1/test" sclang-input>
												</td>
											</tr>
											<tr style="display:none;" id="sc_cifs_user_tr">
												<th sclang>Username</th>
												<td>
													<input id="sc_cifs_user_id" type="text" class="input_32_table" maxlength="200" name="sc_cifs_user" autocorrect="off" autocapitalize="off" placeholder="default username is guest" sclang-input>
												</td>
											</tr>
											<tr style="display:none;" id="sc_cifs_pw_tr">
												<th sclang>Password</th>
												<td>
													<input id="sc_cifs_pw_id" type="text" class="input_32_table" maxlength="200" name="sc_cifs_pw" autocorrect="off" autocapitalize="off" placeholder="default password is empty" sclang-input>
												</td>
											</tr>
										</table>
										<div class="apply_gen">
											<input class="button_gen" onclick="applyRule()" type="button" value="Apply"/ sclang>
											<input type="button" onClick="location.href=location.href" value="Refresh" class="button_gen" sclang>
										</div>
										<div style="margin:30px 0 10px 5px;" class="splitLine"></div>
										<div class="scBottom">
											webui by： <i>paldier</i><br/>
										</div>

									</td>
								</tr>
							</table>
						</td>
						<td width="10" align="center" valign="top"></td>
					</tr>
				</table>
			</td>
		</tr>
	</table>
	</form>
	</td>
	<div id="footer"></div>
</body>
</html>

