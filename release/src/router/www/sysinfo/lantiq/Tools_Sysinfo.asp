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
<title>System Information</title>
<link rel="stylesheet" type="text/css" href="index_style.css">
<link rel="stylesheet" type="text/css" href="form_style.css">
<link rel="stylesheet" type="text/css" href="/js/table/table.css">
<style>
p{
	font-weight: bolder;
}
.tableApi_table th {
	height: 20px;
	text-align: left;
}
.tableApi_table td {
	text-align: left;
}
.data_tr {
	height: 30px;
}
.row_title th {
	width: unset;
}
.FormTitle i {
    color: #FC0;
    font-style: normal;
}
.FormTitle em {
    color: #00ffe4;
    font-style: normal;
}
.FormTitle b {
    color: #1cfe16;
    font-style: normal;
	font-weight:normal;
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
<script type="text/javascript" src="/js/table/table.js"></script>
<script>
var ctf_dis = "<% nvram_get("ctf_disable"); %>";
var ctf_dis_force = "<% nvram_get("ctf_disable_force"); %>";
var odmpid = "<% nvram_get("odmpid");%>";
var ctf_fa = "<% nvram_get("ctf_fa_mode"); %>";
var qca_sfe = "<% nvram_get("qca_sfe"); %>";
var hwnat = "<% nvram_get("hwnat"); %>";
var modelname = "<% nvram_get("modelname"); %>";
var sc_mount = "<% nvram_get("sc_mount"); %>";
overlib_str_tmp = "";
overlib.isOut = true;
function initial(){
	show_menu();
	showbootTime();

	if (odmpid != "") {
        if(modelname == productid)
			document.getElementById("model_id").innerHTML = "<span>" + productid + "</span>";
		else
			document.getElementById("model_id").innerHTML = "<span>" + odmpid + "</span>";
		if(odmpid.toUpperCase() != based_modelid.toUpperCase())
			document.getElementById("model_id").innerHTML += " (base model: <span>" + based_modelid + "</span>)";
	}
	else
		document.getElementById("model_id").innerHTML = productid;
	var buildno = '<% nvram_get("buildno"); %>';
	var extendno = '<% nvram_get("extendno"); %>';
	if ((extendno == "") || (extendno == "0"))
		document.getElementById("fwver").innerHTML = buildno;
	else
		document.getElementById("fwver").innerHTML = buildno + '_' + extendno;
	if(sc_mount != "0")
		document.getElementById("sc_mount").innerHTML = '<span>Enabled</span>';
	else
		document.getElementById("sc_mount").innerHTML = '<span>Disabled</span>';

	var rc_caps = "<% nvram_get("rc_support"); %>";
	var rc_caps_arr = rc_caps.split(' ').sort();
	rc_caps = rc_caps_arr.toString().replace(/,/g, " ");
	document.getElementById("rc_td").innerHTML = rc_caps;
	hwaccel_state();
	update_temperatures();
	updateClientList();
	show_etherstate();
	update_sysinfo();
}
function update_temperatures(){
	$.ajax({
		url: '/ajax_coretmp.asp',
		dataType: 'script',
		error: function(xhr){
			update_temperatures();
		},
		success: function(response){
			curr_coreTmp_24_raw = curr_coreTmp_wl0_raw;
			if (band5g_support)
				curr_coreTmp_5_raw = curr_coreTmp_wl1_raw;
			if (wl_info.band5g_2_support)
			curr_coreTmp_52_raw = curr_coreTmp_wl2_raw;
			else if (wl_info.band6g_support)
				curr_coreTmp_6_raw = curr_coreTmp_wl2_raw;
			code = "<b>2.4 GHz:</b><span> " + curr_coreTmp_24_raw + "</span>";
			if (wl_info.band5g_2_support) {
				code += "&nbsp;&nbsp;-&nbsp;&nbsp;<b>5 GHz-1:</b> <span>" + curr_coreTmp_5_raw + "</span>";
				code += "&nbsp;&nbsp;-&nbsp;&nbsp;<b>5 GHz-2:</b> <span>" + curr_coreTmp_52_raw + "</span>";
			} else if (band5g_support) {
				code += "&nbsp;&nbsp;-&nbsp;&nbsp;<b>5 GHz:</b> <span>" + curr_coreTmp_5_raw + "</span>";
			}
			if (wl_info.band6g_support) {
				code += "&nbsp;&nbsp;-&nbsp;&nbsp;<b>6 GHz:</b> <span>" + curr_coreTmp_6_raw + "</span>";
			}
			if (curr_cpuTemp != "")
				code +="&nbsp;&nbsp;-&nbsp;&nbsp;<b>CPU:</b> <span>" + parseInt(curr_cpuTemp) +"&deg;C</span>";
			document.getElementById("temp_td").innerHTML = code;
			setTimeout("update_temperatures();", 3000);
		}
	});
}
function hwaccel_state(){
	var qos_enable = '<% nvram_get("qos_enable"); %>';
	var qos_type = '<% nvram_get("qos_type"); %>';
	if (hnd_support) {
		code = "Runner:<span> ";

		if ('<% nvram_get("runner_disable"); %>' == '1') {
			code += "Disabled";
			if ('<% nvram_get("runner_disable_force"); %>' == '1') {
				code += " <i>(by user)</i>";
			} else {
				if (qos_enable == '1')
					code += " <i>(QoS)</i>";
			}
		} else {
			code += "Enabled";
		}

		code += "</span>&nbsp;&nbsp;-&nbsp;&nbsp;Flow Cache:<span> ";
		if ('<% nvram_get("fc_disable"); %>' == '1') {
			code += "Disabled";
			if ('<% nvram_get("fc_disable_force"); %>' == '1') {
				code += " <i>(by user)</i>";
			} else {
				if ((qos_enable == '1') && (qos_type != '1'))
					code += " <i>(QoS)</i>";
			}
		} else {
			code += "Enabled";
		}
		code += "</span>";
	} else {
		if (ctf_dis_force == "1") {
			code = "<b>Disabled</b>";
			if (ctf_dis_force == "1")
				code += " <i>(by user)</i>";
			else {
				code += " <i> - incompatible with:<span>  ";	// Two trailing spaces
				if ('<% nvram_get("cstats_enable"); %>' == '1') code += 'IPTraffic, ';
				if ((qos_enable == '1') && (qos_type == '0')) code += 'QoS, ';
				if ('<% nvram_get("sw_mode"); %>' == '2') code += 'Repeater mode, ';
				if ('<% nvram_get("ctf_disable_modem"); %>' == '1') code += 'USB modem, ';
				// We're disabled but we don't know why
				if (code.slice(-2) == "  ") code += "&lt;unknown&gt;, ";
				// Trim two trailing chars, either "  " or ", "
				code = code.slice(0,-2) + "</span>";
			}
		} else if (ctf_dis_force == "0" || qca_sfe == "1" || hwnat == "1") {
			code = "<span><b>Enabled</b>";
			if (ctf_fa != "") {
				if (ctf_fa != "0")
					code += " <i>(CTF + FA)</i>";
				else
					code += "<i> (CTF only)</i>";
			}  else if (qca_sfe == "1")
					code += " <i>(SFE)</i>";
			else if (hwnat == "1")
					code += "<i> (HWNAT)</i>";
			code += "</span>";
		}
	}

	document.getElementById("hwaccel").innerHTML = code;
}


function showbootTime(){
        Days = Math.floor(boottime / (60*60*24));        
        Hours = Math.floor((boottime / 3600) % 24);
        Minutes = Math.floor(boottime % 3600 / 60);
        Seconds = Math.floor(boottime % 60);
        
        document.getElementById("boot_days").innerHTML = Days;
        document.getElementById("boot_hours").innerHTML = Hours;
        document.getElementById("boot_minutes").innerHTML = Minutes;
        document.getElementById("boot_seconds").innerHTML = Seconds;
        boottime += 1;
        setTimeout("showbootTime()", 1000);
}

function show_etherstate() {
	$.ajax({
		url: '/ajax_ethernet_ports.asp',
		async: false,
		dataType: 'script',
		error: function(xhr) {
			setTimeout("show_etherstate();", 1000);
		},
		success: function(response) {
			var wanLanStatus = get_wan_lan_status["portSpeed"];
			var wanCount = get_wan_lan_status["portCount"]["wanCount"];
			//parse nvram to array
			var parseStrToArray = function(_array) {
		        var speedMapping = new Array();
		        speedMapping["M"] = "100 Mbps";
		        speedMapping["G"] = "1 Gbps";
		        speedMapping["Q"] = "2.5 Gbps";
		        speedMapping["F"] = "5 Gbps";
		        speedMapping["T"] = "10 Gbps";
		        speedMapping["X"] = "Unplugged";
				var parseArray = [];
				for (var prop in _array) {
					if (_array.hasOwnProperty(prop)) {
						var newRuleArray = new Array();
						var port_name = prop;
						if(wanCount != undefined) {
							if(port_name.substr(0, 3) == "WAN") {
								if(parseInt(wanCount) > 1) {
									var port_idx = port_name.split(" ");
									port_name = port_idx[0] + " " + (parseInt(port_idx[1]) + 1);
								}
								else {
									port_name = "WAN";
								}
							}
						}
						newRuleArray.push(port_name);
						newRuleArray.push(speedMapping[_array[prop]]);
						parseArray.push(newRuleArray);
					}
				}
				return parseArray;
			};

			//set table Struct
			var tableStruct = {
				data: parseStrToArray(wanLanStatus),
				container: "tableContainer",
				title: "Ethernet Ports", /*untranslated*/
				header: [ 
					{
						"title" : "Port", /*untranslated*/
						"width" : "50%"
					},
					{
						"title" : "Link State", /*untranslated*/
						"width" : "50%"
					}
				]
			}

			if(tableStruct.data.length) {
				$("#tr_ethernet_ports").css("display", "");
				tableApi.genTableAPI(tableStruct);
			}

			setTimeout("show_etherstate();", 3000);
		}
	});
}

function show_connstate(){
	document.getElementById("conn_td").innerHTML = conn_stats_arr[0] + " / <% sysinfo("conn.max"); %>&nbsp;&nbsp;-&nbsp;&nbsp;" + conn_stats_arr[1] + " active";

}


function show_memcpu(){
	document.getElementById("cpu_stats_td").innerHTML = cpu_stats_arr[0] + ", " + cpu_stats_arr[1] + ", " + cpu_stats_arr[2];
	document.getElementById("mem_total_td").innerHTML = mem_stats_arr[0] + " MB";
	document.getElementById("mem_free_td").innerHTML = mem_stats_arr[1] + " MB";
	document.getElementById("mem_buffer_td").innerHTML = mem_stats_arr[2] + " MB";
	document.getElementById("mem_cache_td").innerHTML = mem_stats_arr[3] + " MB";
	if (parseInt(mem_stats_arr[5]) == 0)
		document.getElementById("mem_swap_td").innerHTML = "<span>No swap configured</span>";
	else
		document.getElementById("mem_swap_td").innerHTML = mem_stats_arr[4] + " / " + mem_stats_arr[5] + " MB";
	document.getElementById("nvram_td").innerHTML = mem_stats_arr[6] + " / " + <% sysinfo("nvram.total"); %> + " bytes";
	document.getElementById("jffs_td").innerHTML = mem_stats_arr[7];
}


function updateClientList(e){
	$.ajax({
		url: '/update_clients.asp',
		dataType: 'script',
		error: function(xhr) {
			setTimeout("updateClientList();", 1000);
		},
		success: function(response){
			setTimeout("updateClientList();", 3000);
		}
	});
}

function update_sysinfo(e){
	$.ajax({
		url: '/ajax_sysinfo.asp',
		dataType: 'script',
		error: function(xhr) {
			setTimeout("update_sysinfo();", 1000);
		},
		success: function(response){
			show_memcpu();
			show_connstate();
			setTimeout("update_sysinfo();", 3000);
		}
	});
}

</script>
</head>

<body onload="initial();" onunLoad="return unload_body();" class="bg">
<div id="TopBanner"></div>

<div id="Loading" class="popup_bg"></div>

<iframe name="hidden_frame" id="hidden_frame" src="" width="0" height="0" frameborder="0"></iframe>

<form method="post" name="form" id="ruleForm" action="/start_apply.htm" target="hidden_frame">
<input type="hidden" name="current_page" value="Tools_Sysinfo.asp">
<input type="hidden" name="next_page" value="Tools_Sysinfo.asp">
<input type="hidden" name="modified" value="0">
<input type="hidden" name="action_mode" value="apply">
<input type="hidden" name="action_script" value="">
<input type="hidden" name="action_wait" value="5">
<input type="hidden" name="first_time" value="">
<input type="hidden" name="SystemCmd" value="">
<input type="hidden" name="preferred_lang" id="preferred_lang" value="<% nvram_get("preferred_lang"); %>">
<input type="hidden" name="firmver" value="<% nvram_get("firmver"); %>">
<input type="hidden" name="ct_tcp_timeout" value="<% nvram_get("ct_tcp_timeout"); %>">
<input type="hidden" name="ct_udp_timeout" value="<% nvram_get("ct_udp_timeout"); %>">



<table class="content" align="center" cellpadding="0" cellspacing="0">
  <tr>
    <td width="17">&nbsp;</td>
    <td valign="top" width="202">
      <div id="mainMenu"></div>
      <div id="subMenu"></div></td>
    <td valign="top">
        <div id="tabMenu" class="submenuBlock"></div>

      <!--===================================Beginning of Main Content===========================================-->
      <table width="98%" border="0" align="left" cellpadding="0" cellspacing="0">
        <tr>
          <td valign="top">
            <table width="760px" border="0" cellpadding="4" cellspacing="0" bordercolor="#6b8fa3"  class="FormTitle" id="FormTitle">
                <tbody>
                <tr bgcolor="#4D595D">
                <td valign="top">
                <div>&nbsp;</div>
                <div class="formfonttitle">Tools - System Information</div>
		<div style="margin:10px 0 10px 5px;" class="splitLine"></div>

				<table width="100%" border="1" align="center" cellpadding="4" cellspacing="0" bordercolor="#6b8fa3"  class="FormTable">
					<thead>
						<tr>
							<td colspan="2">Router</td>
						</tr>
					</thead>
					<tr>
						<th>Model</th>
							<td id="model_id"><% nvram_get("productid"); %></td>
					</tr>
					<tr>
						<th>Firmware Version</th>
						<td id="fwver"></td>
					</tr>

					<tr>
						<th>Firmware Build</th>
						<td><% nvram_get("buildinfo"); %></td>
					</tr>
					<tr>
						<th>Bootloader (UBOOT)</th>
						<td><% nvram_get("bl_ver"); %></td>
					</tr>
					<tr>
						<th>Driver version</th>
						<td><% sysinfo("driver_version"); %></td>
					</tr>
					<tr>
						<th>Features</th>
						<td id="rc_td"></td>
					</tr>
					<tr>
						<th><#General_x_SystemUpTime_itemname#></th>
						<td><span id="boot_days"></span> <#Day#> <span id="boot_hours"></span> <#Hour#> <span id="boot_minutes"></span> <#Minute#> <span id="boot_seconds"></span> <#Second#></td>
					</tr>

					<tr>
						<th>Temperatures</th>
						<td id="temp_td"></td>
					</tr>
				</table>

				<table width="100%" border="1" align="center" cellpadding="4" cellspacing="0" bordercolor="#6b8fa3"  class="FormTable">
					<thead>
						<tr>
							<td colspan="2">CPU</td>
						</tr>
					</thead>

					<tr>
						<th>CPU Model</th>
						<td><% sysinfo("cpu.model"); %>	</td>
					</tr>
					<tr>
						<th>CPU Frequency</th>
						<td><% sysinfo("cpu.freq"); %> MHz</td>
					</tr>
					<tr>
						<th>CPU Load Average (1, 5, 15 mins)</th>
						<td id="cpu_stats_td"></td>
					</tr>

				</table>

				<table width="100%" border="1" align="center" cellpadding="4" cellspacing="0" bordercolor="#6b8fa3"  class="FormTable">
					<thead>
						<tr>
							<td colspan="2">Memory</td>
						</tr>
					</thead>
					<tr>
						<th>Total</th>
						<td id="mem_total_td"></td>
					</tr>

					<tr>
						<th>Free</th>
						<td id="mem_free_td"></td>
					</tr>

					<tr>
						<th>Buffers</th>
						<td id="mem_buffer_td"></td>
					</tr>

					<tr>
						<th>Cache</th>
						<td id="mem_cache_td"></td>
					</tr>

					<tr>
						<th>Swap</th>
						<td id="mem_swap_td"></td>
					</tr>
				</table>

				<table width="100%" border="1" align="center" cellpadding="4" cellspacing="0" bordercolor="#6b8fa3"  class="FormTable">
					<thead>
						<tr>
							<td colspan="2">Internal Storage</td>
						</tr>
					</thead>
					<tr>
						<th>NVRAM usage</th>
						<td id="nvram_td"></td>
					</tr>
					<tr>
						<th>JFFS</th>
						<td id="jffs_td"></td>
					</tr>
					<tr>
						<th>JFFS extension</th>
						<td id="sc_mount"></td>
					</tr>
				</table>

				<table width="100%" border="1" align="center" cellpadding="4" cellspacing="0" bordercolor="#6b8fa3"  class="FormTable">
					<thead>
						<tr>
							<td colspan="2">Network</td>
						</tr>
					</thead>
					<tr>
						<th>HW acceleration</th>
						<td id="hwaccel"></td>
					</tr>
					<tr>
						<th>Connections</th>
						<td id="conn_td"></td>
					</tr>
					<tr>
						<th>Ethernet Ports</th>
						<td id="tableContainer" style="margin-top:-10px;"></td>
					</tr>
				</table>
				</td>
				</tr>

	        </tbody>
            </table>
            </form>
            </td>

       </tr>
      </table>
      <!--===================================Ending of Main Content===========================================-->
    </td>
    <td width="10" align="center" valign="top">&nbsp;</td>
  </tr>
</table>
<div id="footer"></div>
</body>
</html>

