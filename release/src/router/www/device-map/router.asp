﻿<!DOCTYPE html>
<html>
<head>
	<meta charset="utf-8">
	<meta http-equiv="X-UA-Compatible" content="IE=11;IE=Edge"/>
	<meta http-equiv="Pragma" CONTENT="no-cache">
	<meta http-equiv="Expires" CONTENT="-1">
	<meta name="viewport" content="width=device-width, initial-scale=1.0">
	<link rel="shortcut icon" href="images/favicon.png">
	<title><#menu1#> - <#menu5_1#></title>	
	<link rel="stylesheet" href="../NM_style.css" type="text/css">
	<link rel="stylesheet" href="../form_style.css" type="text/css">
	<link rel="stylesheet" href="../css/networkMap.css" type="text/css">
	<link rel="stylesheet" type="text/css" href="../pwdmeter.css">
	<script src="../js/jquery.js" type="text/javascript"></script>
	<script src="../js/httpApi.js" type="text/javascript"></script>
	<script src="../state.js" type="text/javascript"></script>
	<script src="../js/device.js" type="text/javascript"></script>
	<script src="/validator.js" type="text/javascript"></script>
	<script src="/help.js" type="text/javascript"></script>
	<script src="../switcherplugin/jquery.iphone-switch.js"></script>
</head>
<body>
<style>
.pwdmeter_container{
	margin-top: 5px;
	margin-left: 1px;
	display:none;
}

.medium_pwdmeter{
	width: 241px;
}

.customized_scoreBar{
	background-size: 300%;
}
</style>
<script>
(function () {
	var dynamic_include_js = function(_src) {
		$('<script>')
			.attr('type', 'text/javascript')
			.attr('src', _src)
			.appendTo('head');
	};
	if(parent.amesh_support && (parent.isSwMode("rt") || parent.isSwMode("ap")))
		dynamic_include_js('/require/modules/amesh.js');
	if(parent.lantiq_support)
		dynamic_include_js('/calendar/jquery-ui.js');
})();
var wlc_band = httpApi.nvramGet(['wlc_band']).wlc_band;
var assassinMode_enable = (function(){
	if(system.modelName == 'TUF-AX3000' 
	&& system.territoryCode.indexOf('CN') != -1 
	&& httpApi.nvramGet(['location_code']).location_code == 'XX'){
		return true
	}
	
	return false;
})();

var smart_connect_mode_maxLength = 4;
if(get_wl_unit_by_band("6G2") != "") smart_connect_mode_maxLength++;

$(document).ready(function(){
	if(system.INTELplatform || system.modelName == 'RT-AC87U'){
		checkWLReady();
	}

	getVariable();
	getInterface();
	if(isSupport("ledg")){
		$("#light_effect_tab").show();
	}
});

var _smart_connect_enable = 0;

var nvram = new Object();
var variable = new Object();
var smart_connect_mode = '';
function getVariable(){	
	var _array = new Array('sw_mode', 'wps_enable');
	var _ssid = new Array();

	if(system.modelName == 'TUF-AX3000' && system.territoryCode.indexOf('CN') != -1){
		_array.push('location_code');
	}

	if(system.smartConnectSupport){
		_array.push('smart_connect_x', 'smart_connect_selif_x');
	}

	if(system.band2gSupport){
		var _element = new Array();
		if(isSwMode('re') && (concurrep_support || wlc_band == '0')){
			_element = ['wl0.1_nmode_x', 'wl0.1_auth_mode_x', 'wl0.1_crypto', 'wl0.1_mfp', 'wl0.1_wep_x', 'wl0.1_key', 'wl0.1_key1', 'wl0.1_key2', 'wl0.1_key3', 'wl0.1_key4'];
			_ssid.push('wl0.1_ssid');
			_ssid.push('wl0.1_wpa_psk');
		}
		else{
			_element = ['wl0_nmode_x', 'wl0_auth_mode_x', 'wl0_crypto', 'wl0_mfp', 'wl0_wep_x', 'wl0_key', 'wl0_key1', 'wl0_key2', 'wl0_key3', 'wl0_key4'];
			_ssid.push('wl0_ssid');
			_ssid.push('wl0_wpa_psk');
		}

		if(mbo_support){
			_element.push('wl0_mbo_enable');
		}
		
		_array.push.apply(_array, _element);
	}

	if(system.band5gSupport){
		var _element = new Array();
		if(isSwMode('re') && (concurrep_support || wlc_band == '1')){
			_element = ['wl1.1_nmode_x', 'wl1.1_auth_mode_x', 'wl1.1_crypto', 'wl1.1_mfp', 'wl1.1_wep_x', 'wl1.1_key', 'wl1.1_key1', 'wl1.1_key2', 'wl1.1_key3', 'wl1.1_key4'];
			_ssid.push('wl1.1_ssid');
			_ssid.push('wl1.1_wpa_psk');
		}
		else{
			_element = ['wl1_nmode_x', 'wl1_auth_mode_x', 'wl1_crypto', 'wl1_mfp', 'wl1_wep_x', 'wl1_key', 'wl1_key1', 'wl1_key2', 'wl1_key3', 'wl1_key4'];
			_ssid.push('wl1_ssid');
			_ssid.push('wl1_wpa_psk');
		}

		if(mbo_support){
			_element.push('wl1_mbo_enable');
		}

		_array.push.apply(_array, _element);
	}

	if(system.band5g2Support){
		var _element = new Array();
		if(isSwMode('re') && (concurrep_support || wlc_band == '2')){
			_element = ['wl2.1_nmode_x', 'wl2.1_auth_mode_x', 'wl2.1_crypto', 'wl2.1_mfp', 'wl2.1_wep_x', 'wl2.1_key', 'wl2.1_key1', 'wl2.1_key2', 'wl2.1_key3', 'wl2.1_key4'];
			_ssid.push('wl2.1_ssid');
			_ssid.push('wl2.1_wpa_psk');
		}
		else{
			_element = ['wl2_nmode_x', 'wl2_auth_mode_x', 'wl2_crypto', 'wl2_mfp', 'wl2_wep_x', 'wl2_key', 'wl2_key1', 'wl2_key2', 'wl2_key3', 'wl2_key4'];
			_ssid.push('wl2_ssid');
			_ssid.push('wl2_wpa_psk');
		}

		if(mbo_support){
			_element.push('wl2_mbo_enable');
		}
		
		_array.push.apply(_array, _element);
	}

	if(system.modelName == 'GT-AXE16000' || system.modelName == 'GT-BE98' || system.modelName == 'GT-BE98_PRO' || system.modelName == 'BQ16' || system.modelName == 'BQ16_PRO'){
		var _element = new Array();
		if(isSwMode('re') && (concurrep_support || wlc_band == '3')){
			_element = ['wl3.1_nmode_x', 'wl3.1_auth_mode_x', 'wl3.1_crypto', 'wl3.1_mfp', 'wl3.1_wep_x', 'wl3.1_key', 'wl3.1_key1', 'wl3.1_key2', 'wl3.1_key3', 'wl3.1_key4'];
			_ssid.push('wl3.1_ssid');
			_ssid.push('wl3.1_wpa_psk');
		}
		else{
			_element = ['wl3_nmode_x', 'wl3_auth_mode_x', 'wl3_crypto', 'wl3_mfp', 'wl3_wep_x', 'wl3_key', 'wl3_key1', 'wl3_key2', 'wl3_key3', 'wl3_key4'];
			_ssid.push('wl3_ssid');
			_ssid.push('wl3_wpa_psk');
		}

		if(mbo_support){
			_element.push('wl3_mbo_enable');
		}
		
		_array.push.apply(_array, _element);
	}

	if(isSupport("amas_fronthaul_network"))
		_array.push('fh_ap_enabled');

	nvram = httpApi.nvramGet(_array);

	// handle SSID ASCII
	_ssid.forEach(function(item){
		nvram[item] = decodeURIComponent(httpApi.nvramCharToAscii(_ssid)[item]);
	});

	variable = Object.assign(variable, nvram);

	smart_connect_mode = variable['smart_connect_selif_x'];
	smart_connect_mode = parseInt(smart_connect_mode).toString(2);

	while(smart_connect_mode.length < smart_connect_mode_maxLength){
		smart_connect_mode = '0' + smart_connect_mode;
	}

	_smart_connect_enable = $("#smartConnectSwitch").val();
}

var wlInterface = new Array();	
function getInterface(){
	wlInterface = [];	// initialize
	var _temp = new Array();
	var typeObj = {
		'quadBandSmartConnect': [['3', 'Quad-Band Smart Connect', '3']],
		'triBandSmartConnect': [['0', '<#smart_connect_tri#>', '0']],
		'dualBand6GHzSmartConnect': [['0', '2.4 / 5 GHz', '0'], ['2', '6 GHz', '2']],
		'dualBand5GHzHighSmartConnect': [['0', '2.4 / 5 GHz', '0'], ['2', '5 GHz-2', '2']],
		'dualBandSmartConnect': [['0', '<#smart_connect_dual#>', '0']],
		'triBand5GHzSmartConnect': [['0', '2.4 GHz', '0'], ['1', '5GHz Smart Connect', '1']],
		'GT6-5GHzSmartConnect': [['2', '2.4 GHz', '2'], ['1', '5GHz Smart Connect', '1']],
		'triBandMeshSmartConnect': [['0', '<#smart_connect_dual#>', '0'], ['2', '5 GHz-2', '2']],
		'GT6triBandMeshSmartConnect': [['0', '<#smart_connect_dual#>', '0'], ['1', '5 GHz-2', '1']],
		'triBand6GHzMeshSmartConnect': [['0', '<#smart_connect_dual#>', '0'], ['2', '6 GHz', '2']],
		'lyraHide': [['0', 'Wireless', '0']],
		'2.4G':  [['0', '2.4 GHz', '0']],
		'2.4G-AXE16000':  [['3', '2.4 GHz', '3']],
		'2.4G-BE98':  [['3', '2.4 GHz', '3']],
		'2.4G-BE98_PRO':  [['3', '2.4 GHz', '3']],
		'2.4G-GT6':  [['2', '2.4 GHz', '2']],
		'5GDualBand': [['1', '5 GHz', '1']],
		'5GTriBand': [['1', '5 GHz-1', '1'], ['2', '5 GHz-2', '2']],
		'GT6-5GTriBand': [['0', '5 GHz-1', '0'], ['1', '5 GHz-2', '1']],
		'6GTriBand': [['1', '5 GHz', '1'], ['2', '6 GHz', '2']],
		'6GQuadBand': [['0', '5 GHz-1', '0'], ['1', '5 GHz-2', '1'], ['2', '6 GHz', '2']],
		'dual6GQuadBand': [['0', '5 GHz', '0'], ['1', '6 GHz-1', '1'], ['2', '6 GHz-2', '2']],
		'60G': [['3', '60 GHz','3']],
		'5L5H6GSmartCommect': [['0', '5 GHz-1/5 GHz-2/6 GHz', '0'], ['3', '2.4 GHz', '3']],
		'2.4G5H6GSmartCommect': [['1', '2.4 GHz/5 GHz-2/6 GHz', '1'], ['0', '5 GHz-1', '0']],
		'2.4G5L6GSmartCommect': [['0', '2.4 GHz/5 GHz-1/6 GHz', '0'], ['1', '5 GHz-2', '1']],
		'2.4G5L5HSmartCommect': [['0', '2.4 GHz/5 GHz-1/5 GHz-2', '0'], ['2', '6 GHz', '2']],    
		'2.4G5L5HTriSmartCommect': [['0', '2.4 GHz/5 GHz-1/5 GHz-2', '0']],
        '5L5HSmartCommect': [['0', '5 GHz-1/5 GHz-2', '0'], ['3', '2.4 GHz', '3'], ['2', '6 GHz', '2']],
		'5H6GSmartCommect': [['1', '5 GHz-2/6 GHz', '1'], ['3', '2.4 GHz', '3'], ['0', '5 GHz-1', '0']],
		'5L6GSmartCommect': [['0', '5 GHz-1/6 GHz', '0'], ['3', '2.4 GHz', '3'], ['1', '5 GHz-2', '1']],
		'2.4G6GSmartCommect': [['3', '2.4 GHz/6 GHz', '3'], ['0', '5 GHz-1', '0'], ['1', '5 GHz-2', '1']],
		'5L5HGSmartCommect': [['0', '5 GHz-1/5 GHz-2', '0'], ['3', '2.4 GHz', '3'], ['2', '6 GHz', '2']],
		'5L5HDual2GSmartCommect': [['1', '5 GHz-1/5 GHz-2', '1'], ['0', '2.4 GHz', '0']],
		'2.4G5HSmartCommect': [['1', '2.4 GHz/5 GHz-2', '1'], ['0', '5 GHz-1', '0'], ['2', '6 GHz', '2']],
		'2.4G5HDual5LSmartCommect': [['0', '2.4 GHz/5 GHz-2', '0'], ['1', '5 GHz-1', '1']],
		'2.4G5LSmartCommect': [['0', '2.4 GHz/5 GHz-1', '0'], ['1', '5 GHz-2', '1'], ['2', '6 GHz', '2']],
		'2.4G5LDual5HSmartCommect': [['0', '2.4 GHz/5 GHz-1', '0'], ['2', '5 GHz-2', '2']],
		'2.4G5GDual6GSmartCommect': [['0', '2.4 GHz/5 GHz', '0'], ['1', '6 GHz-1', '1'], ['2', '6 GHz-2', '2']],
		'2.4G6G1Dual6GSmartCommect': [['3', '2.4 GHz/6 GHz-1', '3'], ['0', '5 GHz', '0'], ['2', '6 GHz-2', '2']],
		'5G6G1Dual6GSmartCommect': [['0', '5 GHz/6 GHz-1', '0'], ['3', '2.4 GHz', '3'], ['2', '6 GHz-2', '2']],
		'2.4G5G6G1Dual6GSmartCommect': [['0', '2.4 GHz/5 GHz/6 GHz-1', '0'], ['2', '6 GHz-2', '2']],
		'2.4G6G2Dual6GSmartCommect': [['3', '2.4 GHz/6 GHz-2', '3'], ['0', '5 GHz', '0'], ['1', '6 GHz-1', '1']],
		'5G6G2Dual6GSmartCommect': [['0', '5 GHz/6 GHz-2', '0'], ['3', '2.4 GHz', '3'], ['1', '6 GHz-1', '1']],
		'2.4G5G6G2Dual6GSmartCommect': [['3', '2.4 GHz/5 GHz/6 GHz-2', '3'], ['1', '6 GHz-1', '1']],
		'6G16G2Dual6GSmartCommect': [['1', '6 GHz-1/6 GHz-2', '1'], ['3', '2.4 GHz', '3'], ['0', '5 GHz', '0']],
		'2.4G6G16G2Dual6GSmartCommect': [['3', '2.4 GHz/6 GHz-1/6 GHz-2', '3'], ['0', '5 GHz', '0']],
		'5G6G16G2Dual6GSmartCommect': [['0', '5 GHz/6 GHz-1/6 GHz-2', '0'], ['3', '2.4 GHz', '3']],
		'2.4G5G6G16G2Dual6GSmartCommect': [['0', '2.4 GHz/5 GHz/6 GHz-1/6 GHz-2', '0']],
	}

	if(system.smartConnectSupport && variable.smart_connect_x != '0'){		// Smart Connect
		if(variable.smart_connect_x == '1'){	// Tri/Dual-Band Smart Connect		
			if(system.modelName === 'GT-AXE16000' || system.modelName === 'GT-BE98' || system.modelName === 'GT-BE98_PRO' || system.modelName == 'BQ16' || system.modelName == 'BQ16_PRO' || system.modelName == 'GT-BE96'){
				if(variable.smart_connect_selif_x === '15'){
					_temp = typeObj['quadBandSmartConnect'];
				}
				else if(variable.smart_connect_selif_x === '14'){
					_temp = typeObj['5L5H6GSmartCommect'];
				}
				else if(variable.smart_connect_selif_x === '13'){
					_temp = typeObj['2.4G5H6GSmartCommect'];
				}
				else if(variable.smart_connect_selif_x === '11'){
					_temp = typeObj['2.4G5L6GSmartCommect'];
					if(system.modelName === 'GT-BE98_PRO'){
						_temp = typeObj['2.4G5G6G1Dual6GSmartCommect'];
					}
				}
				else if(variable.smart_connect_selif_x === '7'){
					_temp = typeObj['2.4G5L5HSmartCommect'];
					if(system.modelName === 'GT-BE96'){		
						if(dwb_info.mode == 1){
							_temp = typeObj['2.4G5LDual5HSmartCommect'];
						}
						else{
							_temp = typeObj['2.4G5L5HTriSmartCommect'];
						}							
					}				
				}
				else if(variable.smart_connect_selif_x === '12'){
					_temp = typeObj['5H6GSmartCommect'];
				}
				else if(variable.smart_connect_selif_x === '10'){
					_temp = typeObj['5L6GSmartCommect'];

					if(system.modelName === 'GT-BE98_PRO'){
						_temp = typeObj['5G6G1Dual6GSmartCommect'];
					}
				}
				else if(variable.smart_connect_selif_x === '9'){
					_temp = typeObj['2.4G6GSmartCommect'];
					if(system.modelName === 'GT-BE98_PRO'){
						_temp = typeObj['2.4G6G1Dual6GSmartCommect'];
					}
					
				}
				else if(variable.smart_connect_selif_x === '6'){
					_temp = typeObj['5L5HSmartCommect'];
				}
				else if(variable.smart_connect_selif_x === '5'){
					_temp = typeObj['2.4G5HSmartCommect'];
					if(system.modelName === 'GT-BE96'){
						_temp = typeObj['2.4G5HDual5LSmartCommect'];
					}	
				}
				else if(variable.smart_connect_selif_x === '3'){
					_temp = typeObj['2.4G5LSmartCommect'];
					if(system.modelName === 'GT-BE98_PRO'){
						_temp = typeObj['2.4G5GDual6GSmartCommect'];
					}else if(system.modelName === 'GT-BE96'){
						_temp = typeObj['2.4G5LDual5HSmartCommect'];
					}				
				}
				else if(variable.smart_connect_selif_x === '17'){
					_temp = typeObj['2.4G6G2Dual6GSmartCommect'];
				}
				else if(variable.smart_connect_selif_x === '18'){
					_temp = typeObj['5G6G2Dual6GSmartCommect'];
				}
				else if(variable.smart_connect_selif_x === '19'){
					_temp = typeObj['2.4G5G6G2Dual6GSmartCommect'];
				}
				else if(variable.smart_connect_selif_x === '24'){
					_temp = typeObj['6G16G2Dual6GSmartCommect'];
				}
				else if(variable.smart_connect_selif_x === '25'){
					_temp = typeObj['2.4G6G16G2Dual6GSmartCommect'];
				}
				else if(variable.smart_connect_selif_x === '26'){
					_temp = typeObj['5G6G16G2Dual6GSmartCommect'];
				}
				else if(variable.smart_connect_selif_x === '27'){
					_temp = typeObj['2.4G5G6G16G2Dual6GSmartCommect'];
				}
			}
			else if(system.modelName === 'RT-BE96U'){
				if(variable.smart_connect_selif_x === '11'){
					_temp = typeObj['triBandSmartConnect'];
				}
				else if(variable.smart_connect_selif_x === '3'){
					_temp = typeObj['triBand6GHzMeshSmartConnect'];
				}				
			}
			else if(system.modelName === 'GT-BE96'){
				if(variable.smart_connect_selif_x === '7'){
					_temp = typeObj['triBandSmartConnect'];
				}
				else if(variable.smart_connect_selif_x === '3'){
					_temp = typeObj['dualBand5GHzHighSmartConnect'];
				}	
			}
			else{
				if(system.band5g2Support){
					if(dwb_info.mode == '1'){
						if(system.band6gSupport){
							//_temp = typeObj['triBand6GHzMeshSmartConnect'];
							_temp = typeObj['triBandSmartConnect'];
						}
						else{
							if(odmpid === 'GT6'){								
								_temp = typeObj['GT6triBandMeshSmartConnect'];
							}
							else {
								_temp = typeObj['triBandMeshSmartConnect'];
							}
							
							if(isSupport("amas_fronthaul_network")){
								var fh_ap_enabled = httpApi.nvramGet(["fh_ap_enabled"]).fh_ap_enabled;
								if(fh_ap_enabled == "2"){
									_temp[0][1] = '<#smart_connect_tri#>';
									_temp[1][1] = '5 GHz-2 (Backhaul)';
								}
							}
						}
					}
					else{
						_temp = typeObj['triBandSmartConnect'];					
					}
				}
				else{
					_temp = typeObj['dualBandSmartConnect'];
				}	
			}				
		}
		else if(variable.smart_connect_x == '3'){
			_temp = typeObj['dualBand6GHzSmartConnect'];
		}
		else{		// 5 GHz Smart Connect			
			if(odmpid === 'GT6'){								
				_temp = typeObj['GT6-5GHzSmartConnect'];
			}
			else if(odmpid === 'GT-BE98_Pro' || system.modelName === 'BQ16_PRO'){
                if(variable.smart_connect_selif_x === '30'){
					_temp = typeObj['5G6G16G2Dual6GSmartCommect'];
				}
                else if(variable.smart_connect_selif_x === '28'){
                    _temp = typeObj['6G16G2Dual6GSmartCommect'];
                }
				else if(variable.smart_connect_selif_x === '22'){
                    _temp = typeObj['5G6G2Dual6GSmartCommect'];
                }
                else if(variable.smart_connect_selif_x === '14'){
                    _temp = typeObj['5G6G1Dual6GSmartCommect'];
                }
			}
            else if(odmpid === 'GT-BE98' || system.modelName === 'BQ16'){
                if(variable.smart_connect_selif_x === '14'){
					_temp = typeObj['5L5H6GSmartCommect'];
				}
                else if(variable.smart_connect_selif_x === '12'){
                    _temp = typeObj['5H6GSmartCommect'];
                }
				else if(variable.smart_connect_selif_x === '10'){
                    _temp = typeObj['5L6GSmartCommect'];
                }
                else if(variable.smart_connect_selif_x === '6'){
                    _temp = typeObj['5L5HSmartCommect'];
                }
			}
			else if(system.modelName === 'GT-BE96'){
				_temp = typeObj['5L5HDual2GSmartCommect'];
			}
            else if(system.modelName === 'GT-AXE16000'){
				if(variable.smart_connect_selif_x === '14'){
					_temp = typeObj['5L5H6GSmartCommect'];
				}
				else if(variable.smart_connect_selif_x === '12'){
					_temp = typeObj['5H6GSmartCommect'];
				}
				else if(variable.smart_connect_selif_x === '10'){
					_temp = typeObj['5L6GSmartCommect'];
				}
				else if(variable.smart_connect_selif_x === '6'){
					_temp = typeObj['5L5HSmartCommect'];
				}
			}
			else {
				if(variable.smart_connect_selif_x === '14'){
					_temp = typeObj['5G6G16G2Dual6GSmartCommect'];
				}
				else if(variable.smart_connect_selif_x === '6'){
					_temp = typeObj['5L5HGSmartCommect'];
				}
				else{
					_temp = typeObj['triBand5GHzSmartConnect'];
				}				
			}				
		}
	}
	else if(system.lyraHideSupport){
		_temp = typeObj['lyraHide'];
	}
	else{
		if(system.band2gSupport){
			if(system.modelName === 'GT-AXE16000'  || system.modelName == 'BQ16'){
				_temp = _temp.concat(typeObj['2.4G-AXE16000']);
			}
			else if(system.modelName === 'GT-BE98'){
				_temp = _temp.concat(typeObj['2.4G-BE98']);
			}
			else if(system.modelName === 'GT-BE98_PRO' || system.modelName == 'BQ16_PRO'){
				_temp = _temp.concat(typeObj['2.4G-BE98_PRO']);				
			}
			else if(odmpid === 'GT6'){
				_temp = _temp.concat(typeObj['2.4G-GT6']);
			}
			else{
				_temp = _temp.concat(typeObj['2.4G']);
			}			
		}

		if(system.modelName === 'GT-AXE16000' || system.modelName === 'GT-BE98' || system.modelName == 'BQ16'){
			_temp = _temp.concat(typeObj['6GQuadBand']);
		}
		else if(system.modelName === 'GT-BE98_PRO' || system.modelName == 'BQ16_PRO'){
			_temp = _temp.concat(typeObj['dual6GQuadBand']);
		}
		else if(system.band5gSupport){
			if(system.band5g2Support){
				if(system.band6gSupport){
					_temp = _temp.concat(typeObj['6GTriBand']);
				}
				else{
					if(odmpid === 'GT6'){								
						_temp = _temp.concat(typeObj['GT6-5GTriBand']);
					}
					else {
						_temp = _temp.concat(typeObj['5GTriBand']);
					}					
				}			
			}
			else{
				_temp = _temp.concat(typeObj['5GDualBand']);
			}
		}

		if(system.band60gSupport){
			_temp = _temp.concat(typeObj['60G']);
		}	
	}

	wlInterface.push.apply(wlInterface, _temp);
	if(isSwMode('re')){
		if(concurrep_support){
			wlInterface.forEach(function(element){
				element[2] = element[0] + '.1';
			});
		}
		else{
			for(let i =0;i<wlInterface.length;i++){
				if(wlInterface[i][0] === wlc_band){
					wlInterface[i][2] = wlInterface[i][2] + '.1';
				}
			}
		}
	}

	genElement(wlInterface);
	setOptions();
}

function isJoinSmartConnect(wlIfIndex){
    wlIfIndex = wlIfIndex.toLowerCase();

	if(wlIfIndex == "2g") wlIfIndex = "2g1";
	if(wlIfIndex == "5g") wlIfIndex = "5g1";
	if(wlIfIndex == "6g") wlIfIndex = "6g1";

    const referenceArray = ["", "", "", "6g2", "6g1", "5g2", "5g1", "2g1"];
	var smart_connect_selif_x = httpApi.nvramGet(["smart_connect_selif_x"]).smart_connect_selif_x;

    var v2Band = smart_connect_mode.split("");
	for(var i=v2Band.length; i<referenceArray.length; i++){
		v2Band.unshift("0");
	}

    var version = isSupport("smart_connect_v2") ? "v2" : isSupport("smart_connect") || isSupport("bandstr") ? "v1" : "";
    var v1Type = httpApi.nvramGet(["smart_connect_x"]).smart_connect_x;

    if (version === "v2") {
        let index = referenceArray.findIndex((element) => element === wlIfIndex);
        return v2Band[index] === "1";
    } else {
        if (v1Type === "1") {
            return true;
        } else if (v1Type === "2") {
            if (wlIfIndex === "5g1" || wlIfIndex === "5g2") {
                return true;
            }
        } else if (v1Type === "3") {
            if (wlIfIndex === "2g1" || wlIfIndex === "5g1") {
                return true;
            }
        }

        return false;
    }
}

function get_band_by_wl_unit(wl_unit){
	var wlnband_list = httpApi.nvramGet(["wlnband_list"]).wlnband_list.split("&#60");
	return wlnband_list[wl_unit];
}

function genElement(){
	var code = '';
	var _temp = '';
	if(isSwMode('mb')){
		code += '<div class="unit-block">';
		code += '<div class="info-title"><#APSurvey_action_search_again_hint2#></div>';
		code += '<div class="button-right"><input type="button" class="button_gen" value="<#QIS_rescan#>" onclick="gotoSiteSurvey();"></div>';
		code += '</div>';
		$('#wl_settings_field').html(code);
		$('#apply_button').hide();
		return true;
	}

	// part for Assassin mode
	if(system.modelName == 'TUF-AX3000' && system.territoryCode.indexOf('CN') != -1){
		document.getElementById('assassin_mode').style.display= '';
	}

	// part of Smart Connect
	if(system.smartConnectSupport && variable.smart_connect_x != '0'){
		$('#smart_connect_field').show();
		if(system.modelName === 'GT-AXE16000' || system.modelName === 'GT-BE98' || system.modelName === 'GT-BE98_PRO' || system.modelName == 'BQ16' || system.modelName == 'BQ16_PRO'){
			var smartConnectType_ori = nvram['smart_connect_x'];
			
			code += '<div class="info-block">';
			code += '<div class="info-title"><#smart_connect#></div>';
			
		
			code += '<select class="input_option" id="smartConnectSwitch" onchange="enableSmartConnect(this.value)">'
			code += '<option value="0" '+ (variable.smart_connect_x === '0' ? 'selected': '') +'><#CTL_close#>';
			code += '<option value="1" '+ (variable.smart_connect_x !== '0' ? 'selected': '') +'><#CTL_Enabled#>';			
			code += '</select>'
	
			code += '<div id="smart_connect_mode_field">';		
			
			if(system.modelName === 'GT-BE98_PRO' || system.modelName === 'BQ16_PRO'){
				code += '<input id="smart_connect_check_0" type="checkbox" onchange="updateSmartConnect(0, this.checked)"'+ (smart_connect_mode[4] ==='1'? 'checked': '') +'>2.4 GHz';
				code += '<input id="smart_connect_check_1" type="checkbox" onchange="updateSmartConnect(1, this.checked)"'+ (smart_connect_mode[3] ==='1'? 'checked': '') +'>5 GHz';
				code += '<input id="smart_connect_check_3" type="checkbox" onchange="updateSmartConnect(3, this.checked)"'+ (smart_connect_mode[1] ==='1'? 'checked': '') +'>6 GHz-1';								
				code += '<input id="smart_connect_check_4" type="checkbox" onchange="updateSmartConnect(4, this.checked)"'+ (smart_connect_mode[0] ==='1'? 'checked': '') +'>6 GHz-2';
			}
			else{
				code += '<input id="smart_connect_check_0" type="checkbox" onchange="updateSmartConnect(0, this.checked)"'+ (smart_connect_mode[3] ==='1'? 'checked': '') +'>2.4 GHz';
				code += '<input id="smart_connect_check_1" type="checkbox" onchange="updateSmartConnect(1, this.checked)"'+ (smart_connect_mode[2] ==='1'? 'checked': '') +'>5 GHz-1';
				code += '<input id="smart_connect_check_2" type="checkbox" onchange="updateSmartConnect(2, this.checked)"'+ (smart_connect_mode[1] ==='1'? 'checked': '') +'>5 GHz-2';
				if(system.band6gSupport){
					code += '<input id="smart_connect_check_3" type="checkbox" onchange="updateSmartConnect(3, this.checked)"'+ (smart_connect_mode[0] ==='1'? 'checked': '') +'>6 GHz';
				}				
			}											
											
			code += '</div>';
			code += '</div>';
		}
		else{
			var smartConnectType_ori = nvram['smart_connect_x'];
			if(smartConnectType_ori != '0'){
				code += '<div class="info-block">';
				code += '<div class="info-title"><#smart_connect#></div>';
				code += '<div><select id="smart_connect_x" class="input_option" onchange="updateVariable(this.id, value)"></select></div>';
				code += '</div>';
			}
		}		

		$('#smart_connect_field').html(code);
		genSmartConnect();
	}
	
	code = '';
	if(system.INTELplatform || system.modelName == 'RT-AC87U'){
		code += '<div id="wl_ready" class="wl-ready" style="display:none;">Wireless is setting...</div>';
	}

	for(var i=0; i<wlInterface.length; i++){	
		var unit = wlInterface[i][2];
		var UNIT = wlInterface[i][0];

		// Mesh, description of dedicated backhaul
		code += '<div class="unit-block"><div class="division-block">'+ wlInterface[i][1] +'</div>';

		if(dwb_info.mode == '1' && (dwb_info.band == UNIT) && !isJoinSmartConnect(get_band_by_wl_unit(unit))){
			var show_dwb_hint = false;
			var wl_closed = httpApi.nvramGet(["wl" + dwb_info.band + "_closed"])["wl" + dwb_info.band + "_closed"];
			if(isSupport("amas_fronthaul_network")){
				if(variable.fh_ap_enabled == '0'){
					if(wl_closed == "1"){
						show_dwb_hint = true;
					}
				}
			}
			else{
				if(wl_closed == "1"){
					show_dwb_hint = true;
				}
			}

			if(show_dwb_hint){
				var $dwb_hint = $("<div>").addClass("dwb_hint").html('<#AiMesh_backhaul_band_5GHz-2_desc2#>');
				$dwb_hint.find(".faq-link").attr("onclick", "top.change_wl_unit_status(" + dwb_info.band + ");");

				code += `<div class="dwb_hint">${wl_nband_title[dwb_info.band]} <#AiMesh_backhaul_band_5GHz-2_desc1#></div>`;
				code += $dwb_hint[0].outerHTML;
				code += '</div>';

				continue;
			}
		}

		// SSID
		code += '<div class="info-block">';
		code += '<div class="info-title"><#QIS_finish_wireless_item1#></div>';
		code += '<div><input type="text" class="input-size-25" id="wl'+ unit +'_ssid" oninput="updateVariable(this.id, value, false)" maxlength="33" autocomplete="off" autocorrect="off" autocapitalize="off"></div>';
		code += '</div>';

		// Authentication method
		if(!system.lyraHideSupport){
			code += '<div class="info-block">';                                                                                                                                               
			code += '<div class="info-title"><#WLANConfig11b_AuthenticationMethod_itemname#></div>';
			code += '<div><select id="wl'+ unit +'_auth_mode_x" class="input_option" onchange="updateVariable(this.id, value)">'+ _temp +'</select></div>';
			if(unit != '2'){
				code += '<span id="wl'+ unit +'_open_suggest" style="color:#FC0;display:none">'+ stringSafeGet(`<#WLANConfig11b_AuthenticationMethod_EOT_desc#>`) +'</span>';
			}
			
			code += '</div>';
			code += '<div id="wl'+ unit +'_no_wp3_hint" class="wpa3_hint" style="display:none;">';
			code += '<span><#AiMesh_confirm_msg10#> <a id="wl'+ unit +'_wpa3FaqLink" class="faq-link" target="_blank" href="">FAQ</a></span>';
			code += '</div>';
		}

		var _authMode = variable['wl'+ unit +'_auth_mode_x'];
		var nmode_x = variable['wl'+ unit + '_nmode_x'];
		var wepEncryption = variable['wl'+ unit +'_wep_x'];
		if(_authMode == 'psk' || _authMode == 'psk2' || _authMode == 'sae' || _authMode == 'pskpsk2' || _authMode == 'psk2sae' || _authMode == 'owe' || _authMode == 'openowe'){
			// WPA Encryption
			if(!system.lyraHideSupport){
				code += '<div class="info-block">';
				code += '<div class="info-title"><#WLANConfig11b_WPAType_itemname#></div>';
				code += '<div><select id="wl'+ unit +'_crypto" class="input_option" onchange="updateVariable(this.id, value)"></select></div>';
				code += '</div>';
			}

            // WPA key
            if(_authMode != 'owe' && _authMode != 'openowe'){            
                code += '<div class="info-block">';
                code += '<div class="info-title"><#WPA-PSKKey#></div>';
                code += '<div><input type="password" class="input-size-25" id="wl'+ unit +'_wpa_psk" oninput="updateVariable(this.id, value, false)" onfocus="plainPasswordSwitch(this, \'focus\');" onblur="plainPasswordSwitch(this, \'blur\')"></div>';
                code += '</div>';
            }
		}
		else if(_authMode == 'shared' || (_authMode == 'open' && nmode_x == '2')){
			if(_authMode == 'shared' || (_authMode == 'open' && wepEncryption != '0')){
				//  WEP Encryption
				code += '<div class="info-block">';
				code += '<div class="info-title"><#WLANConfig11b_WEPType_itemname#></div>';
				code += '<div><select id="wl'+ unit +'_wep_x" class="input_option" onchange="updateVariable(this.id, value)"></select></div>';
				code += '</div>';

				// WEP key index
				code += '<div class="info-block">';
				code += '<div class="info-title"><#WLANConfig11b_WEPDefaultKey_itemname#></div>';
				code += '<div><select id="wl'+ unit +'_key" class="input_option" onchange="updateVariable(this.id, value)"></select></div>';
				code += '</div>';

				// WEP key
				code += '<div class="info-block">';
				code += '<div class="info-title"><#WLANConfig11b_WEPKey_itemname#></div>';
				code += '<div><input id="wl'+ unit +'_wep_key" type="text" class="input-size-25" oninput="updateVariable(this.id, value, false)"></div>';
				code += '</div> ';
			}		
		}
		else if(_authMode == 'wpa' || _authMode == 'wpa2' || _authMode == 'wpawpa2' || _authMode == 'wpa3' || _authMode == 'wpa2wpa3' || _authMode == 'suite-b'){
			// WPA Encryption
			code += '<div class="info-block">';
			code += '<div class="info-title"><#WLANConfig11b_WPAType_itemname#></div>';
			code += '<div><select id="wl'+ unit +'_crypto" class="input_option" onchange="updateVariable(this.id, value)"></select></div>';
			code += '</div>';
		}

		code += '</div>';
	}

	$('#wl_settings_field').html(code);
	var faq_fref = "https://nw-dlcdnet.asus.com/support/forward.html?model=&type=Faq&lang="+ui_lang+"&kw=&num=149";
	$(".faq-link").attr('href', faq_fref);

	appendWirelessHint();
}

function appendWirelessHint(){
	for(var i = 0; i < wlInterface.length; i++){
		var unit = wlInterface[i][2];
		var _authMode = variable['wl'+ unit +'_auth_mode_x'];

		$("#wl"+unit+"_ssid").keyup(function(){
			validator.ssidCheck($("#"+this.id));
		});

		if(_authMode == 'psk' || _authMode == 'psk2' || _authMode == 'sae' || _authMode == 'pskpsk2' || _authMode == 'psk2sae' || _authMode == 'owe' || _authMode == 'openowe'){
			$("#wl"+unit+"_wpa_psk").parent().append(Get_Component_PWD_Strength_Meter(unit));
			$("#scorebarBorder_"+ unit).addClass("pwdmeter_container medium_pwdmeter");
			$("#scorebarBorder_"+ unit +" *").addClass("medium_pwdmeter");
			$("#scorebar_"+ unit).addClass("customized_scoreBar");
			$("#wl"+unit+"_wpa_psk").keyup(function(){
				chkPass(this.value, "",undefined, this.id.split('_')[0].replace("wl", ""));
			});
		}
	}
}

function genSmartConnect(){
	var _optionArray = new Array();
	var code = '';
	var _smart_connect_x = variable['smart_connect_x']
	if(system.band5g2Support){
		if(dwb_info.mode == '1'){
			if(isSupport("wifi6e")){
				_optionArray = [['<#wl_securitylevel_0#>', '0'], ['<#smart_connect_tri#>', '1'], ['<#smart_connect_dual#>', '3']];
			}
			else{
				_optionArray = [['<#wl_securitylevel_0#>', '0'], ['<#smart_connect_dual#>', '1']];
				if(isSupport("amas_fronthaul_network")){
					var fh_ap_enabled = httpApi.nvramGet(["fh_ap_enabled"]).fh_ap_enabled;
					if(fh_ap_enabled == "2"){
						if(system.modelName !== 'RT-BE96U' || variable.smart_connect_selif_x !== '3'){						
							_optionArray[1][0] = '<#smart_connect_tri#>';
						}

						if(wl_nband_array.length == 3 && get_wl_unit_by_band("5G2") != ""){
							_optionArray[1][0] = '<#smart_connect_dual#>';
						}
					}
				}
			}		
		}
		else{
			if(isSupport("wifi6e")){
				_optionArray = [['<#wl_securitylevel_0#>', '0'], ['<#smart_connect_tri#>', '1'], ['<#smart_connect_dual#>', '3']];
			}
			else if(system.modelName == 'RT-BE96U'){
				_optionArray = [['<#wl_securitylevel_0#>', '0'], ['<#smart_connect_tri#>', '1'], ['2.4 GHz and 5 GHz Smart Connect', '3']];
				if(variable['smart_connect_selif_x'] == '3'){
					_smart_connect_x = '3';
				}
			}
			else if(system.modelName == 'GT-BE96'){
				_optionArray = [['<#wl_securitylevel_0#>', '0'], ['<#smart_connect_tri#>', '1'], ['5 GHz Smart Connect', '2']];
				if(variable['smart_connect_selif_x'] == '3'){
					_smart_connect_x = '3';
				}
			}				
			else{
				_optionArray = [['<#wl_securitylevel_0#>', '0'], ['<#smart_connect_tri#>', '1'], ['5GHz Smart Connect', '2']];
			}				
		}		
	}
	else{
		_optionArray = [['<#wl_securitylevel_0#>', '0'], ['<#smart_connect_dual#>', '1']];
	}

	for(var i=0; i<_optionArray.length; i++){
		code += '<option value="'+ _optionArray[i][1] +'">'+ _optionArray[i][0] +'</option>';
	}

	$('#smart_connect_x').html(code);
	$('#smart_connect_x').val(_smart_connect_x);	
}

function setOptions(){
	if(isSwMode('mb')){
		return true;
	}

	for(var i=0; i<wlInterface.length; i++){
		var _unit = wlInterface[i][2];
		var _UNIT = wlInterface[i][0];
		getSSID(_UNIT, 'wl'+ _unit + '_ssid', variable['wl'+ _unit + '_ssid']);
		genAuthMethod(_unit, 'wl'+ _unit + '_auth_mode_x', variable['wl'+ _unit + '_nmode_x'], variable['wl'+ _unit + '_auth_mode_x']);
	}
}

function getSSID(unit, id, ssid){
	if(document.getElementById(id)){
		document.getElementById(id).value = ssid;
	}
}

function genAuthMethod(unit, id, nmode_x, auth_mode_x){
	var auth_array = new Array();
	var authObj = {
		'allWithWPA3': [['Open System', 'open'], ['Shared Key', 'shared'], ['WPA-Personal', 'psk'], ['WPA2-Personal', 'psk2'], ['WPA3-Personal', 'sae'], ['WPA/WPA2-Personal', 'pskpsk2'], ['WPA2/WPA3-Personal', 'psk2sae'], ['WPA-Enterprise', 'wpa'], ['WPA2-Enterprise', 'wpa2'], ['WPA/WPA2-Enterprise', 'wpawpa2'], ['Radius with 802.1x', 'radius']],
		'allWithWPA3WPA3E': [['Open System', 'open'], ['Shared Key', 'shared'], ['WPA-Personal', 'psk'], ['WPA2-Personal', 'psk2'], ['WPA3-Personal', 'sae'], ['WPA/WPA2-Personal', 'pskpsk2'], ['WPA2/WPA3-Personal', 'psk2sae'], ['WPA-Enterprise', 'wpa'], ['WPA2-Enterprise', 'wpa2'], ['WPA3-Enterprise', 'wpa3'], ['WPA3-Enterprise 192-bit', 'suite-b'], ['WPA/WPA2-Enterprise', 'wpawpa2'], ['WPA2/WPA3-Enterprise', 'wpa2wpa3'], ['Radius with 802.1x', 'radius']],
		'allWithWPA3OWE': [['Open System', 'open'], ['Enhanced OPEN Transition', 'openowe'], ['Shared Key', 'shared'], ['WPA-Personal', 'psk'], ['WPA2-Personal', 'psk2'], ['WPA3-Personal', 'sae'], ['WPA/WPA2-Personal', 'pskpsk2'], ['WPA2/WPA3-Personal', 'psk2sae'], ['WPA-Enterprise', 'wpa'], ['WPA2-Enterprise', 'wpa2'], ['WPA3-Enterprise', 'wpa3'], ['WPA3-Enterprise 192-bit', 'suite-b'], ['WPA/WPA2-Enterprise', 'wpawpa2'], ['WPA2/WPA3-Enterprise', 'wpa2wpa3'], ['Radius with 802.1x', 'radius']],
		'allWithWPA3OWEWPA3E': [['Open System', 'open'], ['Enhanced OPEN Transition', 'openowe'], ['Shared Key', 'shared'], ['WPA-Personal', 'psk'], ['WPA2-Personal', 'psk2'], ['WPA3-Personal', 'sae'], ['WPA/WPA2-Personal', 'pskpsk2'], ['WPA2/WPA3-Personal', 'psk2sae'], ['WPA-Enterprise', 'wpa'], ['WPA2-Enterprise', 'wpa2'], ['WPA/WPA2-Enterprise', 'wpawpa2'], ['Radius with 802.1x', 'radius']],
		'allWithoutWPA3':  [['Open System', 'open'], ['Shared Key', 'shared'], ['WPA-Personal', 'psk'], ['WPA2-Personal', 'psk2'], ['WPA-Auto-Personal', 'pskpsk2'], ['WPA-Enterprise', 'wpa'], ['WPA2-Enterprise', 'wpa2'], ['WPA-Auto-Enterprise', 'wpawpa2'], ['Radius with 802.1x', 'radius']],
		'repeaterWithWPA3': [['Open System', 'open'], ['WPA2-Personal', 'psk2'], ['WPA3-Personal', 'sae'], ['WPA/WPA2-Personal', 'pskpsk2'], ['WPA2/WPA3-Personal', 'psk2sae']],
		'repeater': [['Open System', 'open'], ['WPA2-Personal', 'psk2'], ['WPA-Auto-Personal', 'pskpsk2']],
		'60G': [['Open System", "open'], ['WPA2-Personal', 'psk2']],
		'lantiqLegacy': [["Open System", "open"], ["WPA-Personal", "psk"], ["WPA2-Personal", "psk2"], ["WPA-Auto-Personal", "pskpsk2"], ["WPA-Enterprise", "wpa"], ["WPA2-Enterprise", "wpa2"], ["WPA-Auto-Enterprise", "wpawpa2"]],
		'wifiLogo': [['Open System', 'open'], ['WPA2-Personal', 'psk2'], ['WPA-Auto-Personal', 'pskpsk2'], ['WPA-Enterprise', 'wpa'], ['WPA2-Enterprise', 'wpa2'], ['WPA-Auto-Enterprise', 'wpawpa2']],
		'wifiNewCertWPA3': [['Open System', 'open'], ['Shared Key', 'shared'], ['WPA2-Personal', 'psk2'], ['WPA3-Personal', 'sae'], ['WPA/WPA2-Personal', 'pskpsk2'], ['WPA2/WPA3-Personal', 'psk2sae'], ['WPA2-Enterprise', 'wpa2'], ['WPA/WPA2-Enterprise', 'wpawpa2'], ['Radius with 802.1x', 'radius']],
		'wifiNewCertNoWPA3':  [['Open System', 'open'], ['Shared Key', 'shared'], ['WPA2-Personal', 'psk2'], ['WPA-Auto-Personal', 'pskpsk2'], ['WPA2-Enterprise', 'wpa2'], ['WPA-Auto-Enterprise', 'wpawpa2'], ['Radius with 802.1x', 'radius']],
		'normalWithWPA3':  [['Open System', 'open'], ['WPA2-Personal', 'psk2'], ['WPA3-Personal', 'sae'], ['WPA/WPA2-Personal', 'pskpsk2'], ['WPA2/WPA3-Personal', 'psk2sae'], ['WPA2-Enterprise', 'wpa2'], ['WPA/WPA2-Enterprise', 'wpawpa2']],
		'normalWithWPA3WPA3E':  [['Open System', 'open'], ['WPA2-Personal', 'psk2'], ['WPA3-Personal', 'sae'], ['WPA/WPA2-Personal', 'pskpsk2'], ['WPA2/WPA3-Personal', 'psk2sae'], ['WPA2-Enterprise', 'wpa2'], ['WPA3-Enterprise', 'wpa3'], ['WPA3-Enterprise 192-bit', 'suite-b'], ['WPA/WPA2-Enterprise', 'wpawpa2'], ['WPA2/WPA3-Enterprise', 'wpa2wpa3']],
		'normalWithoutWPA3': [['Open System', 'open'], ['WPA2-Personal', 'psk2'], ['WPA-Auto-Personal', 'pskpsk2'], ['WPA2-Enterprise', 'wpa2'], ['WPA-Auto-Enterprise', 'wpawpa2']],
		'6G': [['<#Wireless_Encryption_OWE#>', 'owe'], ['WPA3-Personal', 'sae']],
		'normalWithWPA3OWE': [['Open System', 'open'], ['Enhanced OPEN Transition', 'openowe'], ['WPA2-Personal', 'psk2'], ['WPA3-Personal', 'sae'], ['WPA/WPA2-Personal', 'pskpsk2'], ['WPA2/WPA3-Personal', 'psk2sae'], ['WPA2-Enterprise', 'wpa2'], ['WPA/WPA2-Enterprise', 'wpawpa2']],
		'normalWithWPA3OWEWPA3E': [['Open System', 'open'], ['Enhanced OPEN Transition', 'openowe'], ['WPA2-Personal', 'psk2'], ['WPA3-Personal', 'sae'], ['WPA/WPA2-Personal', 'pskpsk2'], ['WPA2/WPA3-Personal', 'psk2sae'], ['WPA2-Enterprise', 'wpa2'], ['WPA3-Enterprise', 'wpa3'], ['WPA3-Enterprise 192-bit', 'suite-b'], ['WPA/WPA2-Enterprise', 'wpawpa2'], ['WPA2/WPA3-Enterprise', 'wpa2wpa3']],
	}

	if(sw_mode == '2' || (system.modelName == 'RT-AC87U' && unit == '1')){
		if(system.wpa3Support){
			if(system.band6gSupport && unit == '2'){
				auth_array = authObj['6G'];
			}
			else{
				auth_array = authObj['repeaterWithWPA3'];
			}
		}
		else{
			auth_array = authObj['repeater'];
		}	
	}
	else if(unit == '3'){
		if(system.modelName === 'GT-AXE16000' || system.modelName === 'GT-BE98' || system.modelName === 'GT-BE98_PRO' || system.modelName == 'BQ16' || system.modelName == 'BQ16_PRO'){
			auth_array = authObj['normalWithWPA3OWE'];
		}
		else{
			auth_array = authObj['60G'];
		}
		
	}
	else if(nmode_x == '2'){	// Legacy mode
		if(system.modelName == 'BLUECAVE'){
			auth_array =  authObj['lantiqLegacy'];
		}
		else{
			if(system.wifiLogoSupport){
				auth_array = authObj['wifiLogo'];
			}
			else if(system.wpa3Support){
				if(system.newWiFiCertSupport){
					if(system.band6gSupport && unit == '2'){
						auth_array = authObj['6G'];
					}
					else{
						auth_array = authObj['wifiNewCertWPA3'];
						if(system.modelName === 'RT-AX92U' && unit === '2'
						|| system.BRCMplatform && band5g_11ax_support && system.modelName !== 'RT-AX92U'){
							auth_array.pop();
						}
					}
				}
				else{
					if(system.band6gSupport){
						if(unit == '2'){
							auth_array = authObj['6G'];
						}
						else{
							if(wpa3_enterprise_support){
								auth_array = authObj['allWithWPA3OWEWPA3E'];
								if(system.modelName === 'RT-AX92U' && unit === '2'
								|| system.BRCMplatform && band5g_11ax_support && system.modelName !== 'RT-AX92U'){
									auth_array.pop();
								}
							}
							else{
								auth_array = authObj['allWithWPA3OWE'];
								if(system.modelName === 'RT-AX92U' && unit === '2'
								|| system.BRCMplatform && band5g_11ax_support && system.modelName !== 'RT-AX92U'){
									auth_array.pop();
								}
							}							
						}
					}
					else{
						if(wpa3_enterprise_support){
							auth_array = authObj['allWithWPA3WPA3E'];
							if(system.modelName === 'RT-AX92U' && unit === '2'
							|| system.BRCMplatform && band5g_11ax_support && system.modelName !== 'RT-AX92U'){
								auth_array.pop();
							}
						}
						else{
							auth_array = authObj['allWithWPA3'];
							if(system.modelName === 'RT-AX92U' && unit === '2'
							|| system.BRCMplatform && band5g_11ax_support && system.modelName !== 'RT-AX92U'){
								auth_array.pop();
							}
						}						
					}
				}
			}
			else{
				if(system.newWiFiCertSupport){
					auth_array = authObj['wifiNewCertNoWPA3'];
					if(system.modelName === 'RT-AX92U' && unit === '2'
					|| system.BRCMplatform && band5g_11ax_support && system.modelName !== 'RT-AX92U'){
						auth_array.pop();
					}
				}
				else{
					auth_array = authObj['allWithoutWPA3'];
					if(system.modelName === 'RT-AX92U' && unit === '2'
					|| system.BRCMplatform && band5g_11ax_support && system.modelName !== 'RT-AX92U'){
						auth_array.pop();
					}
				}
			}
		}
	}
	else{	// normal case
		if(system.wpa3Support){
			if(system.band6gSupport){
				if(unit == '2' || (system.modelName === 'GT-BE98_PRO' && unit === '1')){
					auth_array = authObj['6G'];
				}
				else{
					if(wpa3_enterprise_support){
						auth_array = authObj['normalWithWPA3OWEWPA3E'];
					}
					else{
						auth_array = authObj['normalWithWPA3OWE'];
					}
				}
			}
			else{
				if(wpa3_enterprise_support){
					auth_array = authObj['normalWithWPA3WPA3E'];
				}
				else{
					auth_array = authObj['normalWithWPA3'];
				}
			}
		}
		else{
			auth_array = authObj['normalWithoutWPA3'];
		}
	}

	if(is_KR_sku){ //remove Open System
		auth_array = auth_array.filter(subArr => subArr[1] !== 'open');
	}

	if(isSupport("amas") && isSupport("amasRouter") && (isSwMode("rt") || isSwMode("ap"))){
		var re_count = httpApi.hookGet("get_cfg_clientlist").length;
		if(re_count > 1){
			auth_array = auth_array.filter(function(item){
				return (item[1] != "wpa2" && item[1] != "wpawpa2");//have re node then hide WPA2-Enterprise, WPA/WPA2-Enterprise
			});
		}
	}

	var code = '';
	for(var i=0; i<auth_array.length; i++){
		code += '<option value="'+ auth_array[i][1] +'">'+ auth_array[i][0] +'</option>'
	}

	if(document.getElementById(id)){
		document.getElementById(id).innerHTML = code;
		document.getElementById(id).value = auth_mode_x;
	}
	
	if(auth_mode_x == 'sae'){
		var get_capability_support = function(_node_info, _type){
			var bitwise_map = {"usb":0, "guest_network":1, "wpa3":2};
			var bitwise_value = -1;
			var capability_value = 0;
			var result = false;
			if(bitwise_map[_type] != undefined)
				bitwise_value = bitwise_map[_type];

			if("capability" in _node_info) {
				if("4" in _node_info.capability) {//4 is rc_support
					capability_value = _node_info.capability["4"];
					if(capability_value == "")
						capability_value = 0;
				}
			}
			if(bitwise_value == -1 || capability_value == 0)
				result = false;
			else
				result = (capability_value & (1 << bitwise_value)) ? true : false;

			return result;
		};

		var get_cfg_clientlist = httpApi.hookGet("get_cfg_clientlist");
		if(get_cfg_clientlist != undefined){
			var len = get_cfg_clientlist.length;
			for(var i = 1; i < len; i += 1){//filter CAP
				if(get_cfg_clientlist[i] != undefined && !get_capability_support(get_cfg_clientlist[i], "wpa3")){
					var id = "wl"+ unit +"_no_wp3_hint";
					if(document.getElementById(id)) document.getElementById(id).style.display = "";
					break;
				}
			}
		}
	}

	var wepEncryption = variable['wl'+ unit +'_wep_x'];
	if(auth_mode_x == 'psk' || auth_mode_x == 'psk2' || auth_mode_x == 'sae' || auth_mode_x == 'pskpsk2' || auth_mode_x == 'psk2sae'){
		genWPAEncryption(unit, 'wl'+ unit +'_crypto', auth_mode_x);
		getWPAKey(unit, 'wl'+ unit +'_wpa_psk', variable['wl'+ unit +'_wpa_psk']);
		if(document.getElementById('wl'+ unit +'_open_suggest')){
			document.getElementById('wl'+ unit +'_open_suggest').style.display = 'none';
		}
	}
	else if(auth_mode_x == 'shared'){
		genWEPEncryption(unit, 'wl'+ unit +'_wep_x');
		genWEPKeyIndex(unit, 'wl'+ unit +'_key');
		getWEPKey(unit, 'wl'+ unit +'_wep_key', variable['wl'+ unit +'_key']);
		if(document.getElementById('wl'+ unit +'_open_suggest')){
			document.getElementById('wl'+ unit +'_open_suggest').style.display = 'none';
		}
    }
    else if(auth_mode_x == 'owe' || auth_mode_x == 'openowe'){
        genWPAEncryption(unit, 'wl'+ unit +'_crypto', auth_mode_x);
		if(document.getElementById('wl'+ unit +'_open_suggest')){
			document.getElementById('wl'+ unit +'_open_suggest').style.display = 'none';
		}
    }
	else if(auth_mode_x == 'open'){
		if(document.getElementById('wl'+ unit +'_open_suggest') && owe_trans_support){
			document.getElementById('wl'+ unit +'_open_suggest').style.display = '';
		}
		
		if(nmode_x == '2'){
			if(wepEncryption != '0'){
				genWEPEncryption(unit, 'wl'+ unit +'_wep_x', auth_mode_x);
				genWEPKeyIndex(unit, 'wl'+ unit +'_key');
				getWEPKey(unit, 'wl'+ unit +'_wep_key', variable['wl'+ unit +'_key']);
			}
			else{
				if(document.getElementById('wl'+ unit +'_wep_x')){
					document.getElementById('wl'+ unit +'_wep_x').style.display = 'none';
				}
				
				if(document.getElementById('wl'+ unit +'k_keyey')){
					document.getElementById('wl'+ unit +'k_keyey').style.display = 'none';
				}				
			}
		}
	}
	else if(auth_mode_x == 'wpa' || auth_mode_x == 'wpa2' || auth_mode_x == 'wpawpa2' || auth_mode_x == 'wpa3' || auth_mode_x == 'wpa2wpa3' || auth_mode_x == 'suite-b'){
		genWPAEncryption(unit, 'wl'+ unit +'_crypto', auth_mode_x);
		if(document.getElementById('wl'+ unit +'_open_suggest')){
			document.getElementById('wl'+ unit +'_open_suggest').style.display = 'none';
		}
	}
}

function genWPAEncryption(unit, id, auth_mode_x){
	var wpaEncryptObj = {
		'tkip': [['TKIP', 'tkip']],
		'aes': [['AES', 'aes']],
		'auto': [['AES', 'aes'], ['TKIP+AES', 'tkip+aes']],
		'suite-b': [['Suite B', 'suite-b']],
	}
	var wpaEncryptArray = new Array();
	var _temp = new Array();
	if(auth_mode_x == 'psk2' || auth_mode_x == 'sae' || auth_mode_x == 'psk2sae' || auth_mode_x == 'wpa2' || auth_mode_x == 'wpa3' || auth_mode_x == 'wpa2wpa3' || auth_mode_x == 'owe' || auth_mode_x == 'openowe'){		// WPA2-Personal, WPA3-Personal, WPA2/WPA3-Personal, WPA2-Enterprise
		wpaEncryptArray.push.apply(wpaEncryptArray, wpaEncryptObj['aes']);
	}
	else if(auth_mode_x == 'pskpsk2' || auth_mode_x == 'wpawpa2'){		// WPA/WPA2-Personal, WPA/WPA2-Enterprise
		wpaEncryptArray.push.apply(wpaEncryptArray, wpaEncryptObj['auto']);
	}
	else if(auth_mode_x == 'psk' || auth_mode_x == 'wpa'){		// WPA-Personal, WPA-Enterprise
		wpaEncryptArray.push.apply(wpaEncryptArray, wpaEncryptObj['tkip']);
    	}
	else if(auth_mode_x == 'suite-b'){
		wpaEncryptArray.push.apply(wpaEncryptArray, wpaEncryptObj['suite-b']);
	}
    
	var code = '';
	var wpaCryption = variable['wl'+ unit +'_crypto'];
	for(var i=0; i<wpaEncryptArray.length; i++){
		var selected = '';
		if(wpaCryption == wpaEncryptArray[i][1]){
			selected = 'selected';
		}

		code += '<option value="'+ wpaEncryptArray[i][1] +'" '+ selected +'>'+ wpaEncryptArray[i][0] +'</option>';
	}

	if(document.getElementById(id)){
		document.getElementById(id).innerHTML = code;
		variable['wl'+ unit +'_crypto'] = document.getElementById(id).value;
	}
}

function getWPAKey(unit, id, key){
	if(document.getElementById(id)){
		document.getElementById(id).value = key;
	}
}

function genWEPEncryption(unit, id, auth_mode){
	var wepEncryptionArray = new Array();
	var wepEncryptObj = {
		'openWEP': [['None', '0'], ['WEP-64bits', '1'], ['WEP-128bits', '2']],
		'wep': [['WEP-64bits', '1'], ['WEP-128bits', '2']]
	}

	wepEncryptionArray = wepEncryptObj['wep'];
	if(auth_mode == 'open'){
		wepEncryptionArray = wepEncryptObj['openWEP'];;
	}
	
	var code = '';
	var wepEncryption = variable['wl'+ unit +'_wep_x'];
	for(var i=0; i< wepEncryptionArray.length; i++){
		var selected = '';
		if(wepEncryption == wepEncryptionArray[i][1]){
			selected = 'selected';
		}

		code += '<option value="'+ wepEncryptionArray[i][1] +'" '+ selected +'>'+ wepEncryptionArray[i][0] +'</option>';
	}

	document.getElementById(id).innerHTML = code;
	variable['wl'+ unit +'_wep_x'] = document.getElementById(id).value;
}

function genWEPKeyIndex(unit, id){
	var wepKeyIndexArray = [['1', '1'], ['2', '2'], ['3', '3'], ['4', '4']];
	var code = '';
	for(var i=0; i<wepKeyIndexArray.length; i++){
		code += '<option value="'+ wepKeyIndexArray[i][1] +'">'+ wepKeyIndexArray[i][0] +'</option>';
	}

	$('#' + id).html(code);
	var _wep_key = variable['wl'+ unit +'_key'];
	$('#' + id).val(_wep_key);
}

function getWEPKey(unit, id, keyIndex){
	var wepKey = variable['wl'+ unit +'_key'+ keyIndex];
	$('#wl'+ unit +'_wep_key').val(wepKey);
}

function apply(rc_flag){
	var postObj = new Object();
	var rc_time = httpApi.hookGet('get_default_reboot_time', true);
	if(!rc_flag){
		rc_flag = 'restart_wireless';
		rc_time = 10;
	}
	// handle data wants to post
	postObj = {
		'action_mode': 'apply',
		'rc_service': rc_flag
	}

	if(variable.smart_connect_x === '0'){
		delete variable['smart_connect_selif_x'];
	}

	if(system.modelName == 'RT-BE96U'){
		if(document.getElementById('smart_connect_x')){			
			if(document.getElementById('smart_connect_x').value == '1'){
				variable['smart_connect_selif_x'] = '11';
				variable['smart_connect_x'] = '1';
				variable['wl1_auth_mode_x'] = variable['wl0_auth_mode_x'];
				variable['wl1_crypto'] = variable['wl0_crypto'];
				variable['wl1_mfp'] = variable['wl0_mfp'];
				variable['wl1_wpa_psk'] = variable['wl0_wpa_psk'];
				if(variable['wl0_auth_mode_x'] === 'open' || variable['wl0_auth_mode_x'] === 'openowe'){
					variable['wl2_auth_mode_x'] = 'owe';
				}
				else if(variable['wl0_auth_mode_x'] === 'psk2' || variable['wl0_auth_mode_x'] === 'pskpsk2' || variable['wl0_auth_mode_x'] === 'psk2sae'){
					variable['wl2_auth_mode_x'] = 'sae';
					variable['wl2_crypto'] = 'aes';
					variable['wl2_mfp'] = '2';
					variable['wl2_wpa_psk'] = variable['wl0_wpa_psk'];
				}
				else if(variable['wl0_auth_mode_x'] === 'wpa2' || variable['wl0_auth_mode_x'] === 'wpawpa2' || variable['wl0_auth_mode_x'] === 'wpa2wpa3'){
					variable['wl2_auth_mode_x'] = 'wpa3';
					variable['wl2_crypto'] = 'aes';
					variable['wl2_mfp'] = '2';
				}
				else{
					variable['wl2_auth_mode_x'] = variable['wl0_auth_mode_x'];
					variable['wl2_crypto'] = variable['wl0_crypto'];
					variable['wl2_mfp'] = variable['wl0_mfp'];
					variable['wl2_wpa_psk'] = variable['wl0_wpa_psk'];
				}				
			}
			else{
				variable['smart_connect_selif_x'] = '3';
				variable['smart_connect_x'] = '1';
				variable['wl1_auth_mode_x'] = variable['wl0_auth_mode_x'];
				variable['wl1_crypto'] = variable['wl0_crypto'];
				variable['wl1_mfp'] = variable['wl0_mfp'];
				variable['wl1_wpa_psk'] = variable['wl0_wpa_psk'];
			}
		}
	}

	if(system.modelName == 'GT-BE96'){	
		if(variable['smart_connect_selif_x'] === '7'){
			variable['wl1_ssid'] = variable['wl0_ssid'];
			variable['wl1_auth_mode_x'] = variable['wl0_auth_mode_x'];
			variable['wl1_crypto'] = variable['wl0_crypto'];
			variable['wl1_mfp'] = variable['wl0_mfp'];
			variable['wl1_wpa_psk'] = variable['wl0_wpa_psk'];
			variable['wl2_ssid'] = variable['wl0_ssid'];
			variable['wl2_auth_mode_x'] = variable['wl0_auth_mode_x'];			
			variable['wl2_crypto'] = variable['wl0_crypto'];
			variable['wl2_mfp'] = variable['wl0_mfp'];
			variable['wl2_wpa_psk'] = variable['wl0_wpa_psk'];
		}
		else if(variable['smart_connect_selif_x'] === '6'){
			variable['wl2_ssid'] = variable['wl1_ssid'];
			variable['wl2_auth_mode_x'] = variable['wl1_auth_mode_x'];
			variable['wl2_crypto'] = variable['wl1_crypto'];
			variable['wl2_mfp'] = variable['wl1_mfp'];
			variable['wl2_wpa_psk'] = variable['wl1_wpa_psk'];
		}
		else if(variable['smart_connect_selif_x'] === '5'){
			variable['wl2_ssid'] = variable['wl0_ssid'];
			variable['wl2_auth_mode_x'] = variable['wl0_auth_mode_x'];
			variable['wl2_crypto'] = variable['wl0_crypto'];
			variable['wl2_mfp'] = variable['wl0_mfp'];
			variable['wl2_wpa_psk'] = variable['wl0_wpa_psk'];
		}
		else if(variable['smart_connect_selif_x'] === '3'){
			variable['wl1_ssid'] = variable['wl0_ssid'];
			variable['wl1_auth_mode_x'] = variable['wl0_auth_mode_x'];
			variable['wl1_crypto'] = variable['wl0_crypto'];
			variable['wl1_mfp'] = variable['wl0_mfp'];
			variable['wl1_wpa_psk'] = variable['wl0_wpa_psk'];
		}
		
	}

	if(validateInput()){
		if(isSupport("amas_fronthaul_network")){
			if(system.triBandSupport && dwb_info.mode && variable['smart_connect_x'] == "1"){
				variable["wl" + dwb_info.band + "_closed"] = "1";
			}
		}
		postObj = Object.assign(postObj, variable);
		httpApi.nvramSet(postObj, function(){
			if (Qcawifi_support || Rawifi_support) {
				var restart_needed_time = this.restart_needed_time;	// restart wireless time
				if (restart_needed_time) {
					var tmp_rc_time = parseInt(restart_needed_time);
					if (!isNaN(tmp_rc_time) && tmp_rc_time > 0 && tmp_rc_time < 300) {
						rc_time = tmp_rc_time;
					}
				}
			}
			parent.showLoading(rc_time);
			setTimeout(function(){
				location.reload();
			}, rc_time*1000);
		});
	}
}

function updateVariable(id, value, flag){
	var regen = (flag == undefined) ? true : flag;
	variable[id] = value;
	var prefix = id.split('_')[0];
	var wpsEnable = variable['wps_enable'];
	if(system.modelName == 'RT-BE96U'){
		if(value == '1'){
			variable.smart_connect_selif_x = '11';
		}
		else if(value == '3'){
			variable.smart_connect_selif_x = '3';
		}
	}
	else if(system.modelName == 'GT-BE96'){
		if(value == '1'){
			variable.smart_connect_selif_x = '7';
		}
		else if(value == '3'){
			variable.smart_connect_selif_x = '3';
		}
		else if(value == '2'){
			variable.smart_connect_selif_x = '6';
		}
	}

	if(band6g_support && (id == "smart_connect_x")){
		if(value == '0' || value == '3'){
			if(variable['wl0_auth_mode_x'] == 'psk2sae'){
				variable['wl0_auth_mode_x'] = 'psk2';
				variable['wl0_crypto'] = 'aes';
				if(mbo_support && nvram['wl0_mbo_enable'] == '1' && nvram['wl0_mfp'] == '0'){
					variable['wl0_mfp'] = '1';
				}
				else{
					variable['wl0_mfp'] = '0';
				}
			}

			if(variable['wl1_auth_mode_x'] == 'psk2sae'){
				variable['wl1_auth_mode_x'] = 'psk2';
				variable['wl1_crypto'] = 'aes';
				if(mbo_support && nvram['wl1_mbo_enable'] == '1' && nvram['wl1_mfp'] == '0'){
					variable['wl1_mfp'] = '1';
				}
				else{
					variable['wl1_mfp'] = '0';
				}
			}
		}
		else if(value == '1'){
			variable['wl0_auth_mode_x'] = 'psk2sae';
			variable['wl0_crypto'] = 'aes';
			variable['wl0_mfp'] = '1';
			variable['wl1_auth_mode_x'] = 'psk2sae';
			variable['wl1_crypto'] = 'aes';
			variable['wl1_mfp'] = '1';
			variable['wl2_auth_mode_x'] = 'sae';
			variable['wl2_crypto'] = 'aes';
			variable['wl2_mfp'] = '2';
		}

		_smart_connect_enable = value; // for amesh.js
	}

	// variable padding
	if(value == 'sae' || value == 'owe' || value == 'openowe' || value == 'wpa3' || value == 'suite-b'){
		variable[prefix + '_mfp'] = '2';
	}
	else if((value == 'psk2sae' || value == 'wpa2wpa3') && nvram[prefix + '_mfp'] == '0'){	
		variable[prefix + '_mfp'] = '1';
	}
	else if(value == 'psk2' || value == 'pskpsk2' || value == 'wpawpa2' || value == 'wpa2'){
		if(mbo_support && nvram[prefix + '_mbo_enable'] == '1' && nvram[prefix + '_mfp'] == '0'){
			variable[prefix + '_mfp'] = '1';
		}
		else if((value == 'pskpsk2' || value == 'wpawpa2') && nvram[prefix + '_mfp'] == '2'){
			variable[prefix + '_mfp'] = '1';
		}
	}

	if((value == 'shared' || value == 'wpa' || value == 'wpa2' || value == 'wpa3' || value == 'wpawpa2' || value == 'wpa2wpa3' || value == 'suite-b' || value == 'radius') && wpsEnable == '1'){
		variable['wps_enable'] = '0';
	}
	else{
		variable['wps_enable'] = nvram['wps_enable'];
	}

	if(isSupport("amas_fronthaul_network")){
		if(variable['smart_connect_x'] == "1")
			variable['fh_ap_enabled'] = httpApi.nvramDefaultGet(["fh_ap_enabled"]).fh_ap_enabled;
		else
			variable['fh_ap_enabled'] = "0";
	}

	if(regen){
		getInterface();
	}
}

function switchTab(id){
	var obj = {
		'wireless_tab': 'router.asp',
		'status_tab': 'router_status.asp',
		'light_effect_tab': 'router_light_effect.asp'
	}
	var path = window.location.pathname.split('/').pop();
	var targetPath = obj[id];
	if(targetPath == path){return false;}

	location.href = targetPath;
}

function checkWLReady(){
	$.ajax({
	    url: '/ajax_wl_ready.asp',
	    dataType: 'script',
	    error: function(xhr) {
			setTimeout("checkWLReady();", 2000);
	    },
	    success: function(response){	
			if((wave_ready != '' && wave_ready != '1')
			|| (qtn_ready != '' && qtn_ready != '1')){
	    		$("#wl_ready").show();
	    		setTimeout("checkWLReady();", 2000);
	    	}
	    	else{
	    		$("#wl_ready").hide();
			}
	    }
  	});
}

function gotoSiteSurvey(){
	if(sw_mode == 2)
		parent.location.href = '/QIS_wizard.htm?flag=sitesurvey_rep&band='+wl_unit;
	else
		parent.location.href = '/QIS_wizard.htm?flag=sitesurvey_mb';
}

function validateInput(){
	for(var i=0; i<wlInterface.length; i++){
		var unit = wlInterface[i][0];
		var prefix = 'wl' + unit;
		// SSID
		var id = prefix + '_ssid';
		var obj = document.getElementById(id);
		if(obj){
			if(!parent.validator.stringSSID(obj)){		
				obj.focus();
				return false;
			}
			if(system.triBandSupport && dwb_info.mode){
				var jsonPara = {};
				jsonPara["edit_wl_unit"] = unit;
				jsonPara["edit_wl_ssid"] = obj.value;
				jsonPara["dwb_unit"] = dwb_info.band;
				jsonPara["smart_connect"] = variable['smart_connect_x'];
				var ssid_array = [];
				ssid_array.push(variable["wl0_ssid"]);
				if(system.band5gSupport){
					ssid_array.push(variable["wl1_ssid"]);
				}
				if(system.band5g2Support){
					ssid_array.push(variable["wl2_ssid"]);
				}

				jsonPara["current_ssid"] = ssid_array;
				if(!validator.dwb_check_wl_setting(jsonPara)) {
					alert("The fronthaul SSID is the same as the backhaul SSID.");/* untranslated */
					obj.focus();
					return false;
				}
			}
		}

		//WPA PSK
		id = prefix + '_wpa_psk';
		obj = document.getElementById(id);
		if(obj){
			if(is_KR_sku){
				if(!parent.validator.psk_KR(obj)){
					obj.focus();
					return false;
				}
			}
			else{
				if(!parent.validator.psk(obj)){
					obj.focus();
					return false;
				}
			}

			// common strings check
			var wpaKey = obj.value;
			var is_common_string = parent.check_common_string(wpaKey, 'wpa_key');
			if(is_common_string){
				if(!confirm("<#JS_common_passwd#>")){
					obj.focus();
					return false;	
				}	
			}
		}

		// WEP key
		id = prefix + '_wep_key';
		obj = document.getElementById(id);
		if(obj){
            var wep_index = variable[prefix+'_key'];
			if(!validator.wlKey(obj)){
				obj.focus();
				return false;
			}
            variable[prefix+'_key'+wep_index] = variable[prefix+'_wep_key'];
		}

		if(system.AMESHSupport && (parent.isSwMode("rt") || parent.isSwMode("ap"))){
			id = prefix + '_auth_mode_x';
			if(!check_wl_auth_support($('#'+ id + ' option:selected'), unit))
				return false;
			else {
				var wl_parameter = {
					"original" : {
						"ssid" : decodeURIComponent(httpApi.nvramCharToAscii(['wl'+ unit +'_ssid'])['wl'+ unit +'_ssid']),
						"psk" :  decodeURIComponent(httpApi.nvramCharToAscii(['wl'+ unit +'_wpa_psk'])['wl'+ unit +'_wpa_psk'])
					},
					"current": {
						"ssid" : variable['wl'+ unit +'_ssid'],
						"psk" : variable['wl'+ unit +'_wpa_psk']
					}
				};

				if(!AiMesh_confirm_msg("Wireless_SSID_PSK", wl_parameter))
					return false;
			}
		}	
	}

	return true;
}

function updateSmartConnect(unit, checked){
	var offset = 0;

	var value = parseInt(smart_connect_mode, 2);
	var index = '';
	if(unit == '0'){
		offset = checked ? 1 : -1;
	}
	else if(unit == '1'){
		offset = checked ? 2 : -2;
	}
	else if(unit == '2'){
		offset = checked ? 4 : -4;
	}
	else if(unit == '3'){
		offset = checked ? 8 : -8;
	}
	else if(unit == '4'){
		offset = checked ? 16 : -16;
	}

	value += offset;
	if(value == 0
	|| value == 1
	|| value == 2
	|| value == 4
	|| value == 8
	|| value == 16){
		alert('<#smart_connect_alert#>');
		document.querySelector('#smart_connect_check_' + unit).checked = true;
		return false;
	}

	var temp = '';
	while(value > 0){
		var r = value % 2;
		value = parseInt(value / 2);
		temp = r.toString() + temp;
	}

	while(temp.length < smart_connect_mode_maxLength){
		temp = '0' + temp;
	}

	smart_connect_mode = temp;
	if(smart_connect_mode.substr(-1) === '1'){
		// have 2.4GHz
		variable.smart_connect_x = '1';
	}
	else{
		variable.smart_connect_x = '2';
	}
	variable.smart_connect_selif_x = parseInt(smart_connect_mode, 2).toString();
	
	getInterface();
}

function enableSmartConnect(value){
	if(value === '0'){
		document.querySelector('#smart_connect_mode_field').style.display = 'none';
		variable.smart_connect_x = '0';
		_smart_connect_enable = "0"; // for amseh.js
	}
	else{
		document.querySelector('#smart_connect_mode_field').style.display = '';
		if(smart_connect_mode[3] === '1'){
			variable.smart_connect_x = '1';
			_smart_connect_enable = "1"; // for amseh.js
		}
		else{
			variable.smart_connect_x = '2';
			_smart_connect_enable = "2"; // for amseh.js
		}
	}

	getInterface();
}
</script>
	<div class="main-block">
		<div class="display-flex flex-a-center">
			<div id="wireless_tab" class="tab-block tab-click" onclick="switchTab(this.id)"><#menu5_1#></div>
			<div id="status_tab" class="tab-block" onclick="switchTab(this.id)"><#Status_Str#></div>
			<div id="light_effect_tab" class="tab-block"style="display:none;" onclick="switchTab(this.id)">Aura RGB</div><!-- untranslated -->
		</div>
		<div id="assassin_mode" class="unit-block" style="display:none;">
			<div class="display-flex flex-a-center flex-j-spaceB">
				<div>刺客模式</div>
				<div>
					<div class="left" style="width:94px; " id="assassin_mode_enable"></div>
					<div class="clear"></div>
					<script type="text/javascript">
						$('#assassin_mode_enable').iphoneSwitch(assassinMode_enable,
								function() {
									variable.location_code = 'XX';
									apply('reboot');
									
									//document.internetForm.submit();
									return true;
								},
								function() {
									variable.location_code = 'CN';
									apply('reboot');
									//document.internetForm.submit();
									return true;
								}
						);
					</script>
				</div>
			</div>
		</div>
		<div id="smart_connect_field" class="unit-block" style="display:none;"></div>
		<div id="wl_settings_field"></div>
		<div id="apply_button" class="button">
			<input type="button" class="button_gen" value="<#CTL_apply#>" onclick="apply();">
		</div>
	</div>
</body>
</html>
