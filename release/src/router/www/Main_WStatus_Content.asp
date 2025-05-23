﻿<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml">
<html xmlns:v>
<head>
<meta http-equiv="X-UA-Compatible" content="IE=Edge"/>
<meta http-equiv="Content-Type" content="text/html; charset=UTF-8">
<meta HTTP-EQUIV="Pragma" CONTENT="no-cache">
<meta HTTP-EQUIV="Expires" CONTENT="-1">
<link rel="shortcut icon" href="images/favicon.png">
<link rel="icon" href="images/favicon.png">
<title><#Web_Title#> - <#menu5_7_4#></title>
<link rel="stylesheet" type="text/css" href="index_style.css"> 
<link rel="stylesheet" type="text/css" href="form_style.css">
<script type="text/javascript" src="/js/jquery.js"></script>
<script language="JavaScript" type="text/javascript" src="/state.js"></script>
<script language="JavaScript" type="text/javascript" src="/general.js"></script>
<script language="JavaScript" type="text/javascript" src="/popup.js"></script>
<script language="JavaScript" type="text/javascript" src="/help.js"></script>
<script language="JavaScript" type="text/javascript" src="/js/httpApi.js"></script>
<script>
var classObj= {
	ToHexCode:function(str){
		return encodeURIComponent(str).replace(/%/g,"\\x").toLowerCase();
	},
	UnHexCode:function(str){
		return str.replace(/(?:\\x[\da-fA-F]{2})+/g, m =>
decodeURIComponent(m.replace(/\\x/g, '%'))).replace(/\\n/g,
'<br>');
	}
}

var content = "";
function GenContent(){
	var dead = 0;
	$.ajax({
		url: '/wl_log.asp',
		dataType: 'text',
		timeout: 1500,
		error: function(xhr){
			if(dead > 30){
				$("#wl_log").html("Fail to grab wireless log.");
			}
			else{
				dead++;
				setTimeout("GenContent();", 1000);
			}
		},

		success: function(resp){
			content = decodeURIComponent(resp);
			content = classObj.UnHexCode(content);
			content = htmlEnDeCode.htmlEncode(content);
			if(content.length > 10){
				$("#wl_log").html(content);
			}
			else{
				$("#wl_log").html("Fail to grab wireless log.");
			}
		}
	});
}

function initial(){
	show_menu();
	GenContent();
	
}
</script>
</head>

<body onload="initial();" class="bg">
<div id="TopBanner"></div>
<div id="Loading" class="popup_bg"></div>

<iframe name="hidden_frame" id="hidden_frame" src="" width="0" height="0" frameborder="0"></iframe>
<form method="post" name="form" action="apply.cgi" target="hidden_frame">
<input type="hidden" name="current_page" value="Main_WStatus_Content.asp">
<input type="hidden" name="next_page" value="Main_WStatus_Content.asp">
<input type="hidden" name="group_id" value="">
<input type="hidden" name="modified" value="0">
<input type="hidden" name="action_mode" value="">
<input type="hidden" name="action_script" value="">
<input type="hidden" name="action_wait" value="">
<input type="hidden" name="first_time" value="">
<input type="hidden" name="preferred_lang" id="preferred_lang" value="<% nvram_get("preferred_lang"); %>">
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
		<!-- ===================================Beginning of Main Content===========================================-->
			<table width="98%" border="0" align="left" cellpadding="0" cellspacing="0">
				<tr>
					<td valign="top" >           
						<table width="760px" border="0" cellpadding="5" cellspacing="0" bordercolor="#6b8fa3" class="FormTitle" id="FormTitle">
							<tr bgcolor="#4D595D">
								<td valign="top">
									<div>&nbsp;</div>
									<div class="formfonttitle"><#System_Log#> - <#menu5_7_4#></div>
									<div style="margin:10px 0 10px 5px;" class="splitLine"></div>
									<div class="formfontdesc"><#WLLog_title#></div>
									<div style="margin-top:8px"> 
										<textarea id="wl_log" cols="63" rows="30" class="textarea_ssh_table" style="width:99%;font-family:'Courier New', Courier, mono; font-size:13px;" readonly="readonly" wrap="off"></textarea>
									</div>
									<div class="apply_gen">
										<input type="button" onClick="location.reload();" value="<#CTL_refresh#>" class="button_gen" >
									</div>
								</td>
							</tr>		
						</table>
					</td>
				</tr>
			</table>
		<!--===================================Ending of Main Content===========================================-->			
		</td>	  		
	</tr>
</table>
<div id="footer"></div>
</form>
</body>
</html>
