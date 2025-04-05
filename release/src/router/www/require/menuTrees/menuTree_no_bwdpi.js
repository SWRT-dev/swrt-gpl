/* menuTree_no_bwdpi.js */
define(function(){
	var menuTree = {
		list: [
			/*
			{
				menuName: "Title",
				index: "Assign an index to this menu, it is also used for filtering",
				tab: [
					{
						url: "Put url here", 
						tabName: "
								Assign a title for this tab, leave this field empty to bypass this tab, 
								fill in '__HIDE__' to hide the tab switcher,
								fill in '__INHERIT__' to inhert tab index from referred page.
						"
					}
				]
			}
			*/
			{
				menuName: "<#QIS#>",
				index: "menu_QIS", 
				tab: [
					{url: "QIS_wizard.htm", tabName: "__HIDE__"},
					{url: "NULL", tabName: "__INHERIT__"}
				] 
			},
			/* ============================================================================================================ */
			{
				menuName: "<#menu5_1_1#>",
				index: "menu_Split", 
				tab: [
					{url: "NULL", tabName: "__HIDE__"}
				] 
			},
			{
				menuName: "<#menu1#>",
				index: "menu_Index",
				tab: [
					{url: "<% networkmap_page(); %>", tabName: "__HIDE__"},
					{url: "NULL", tabName: "__INHERIT__"}
				]
			}, 
			{
				menuName: "AiMesh",
				index: "menu_AiMesh", 
				tab: [
					{url: "AiMesh.asp", tabName: "AiMesh"},
					{url: "NULL", tabName: "__INHERIT__"}
				]
			},
			{
				menuName: isSupport("sdn_mwl") ? `<#Network#>` : Guest_Network_naming,
				index: "menu_GuestNetwork",
				tab: [
					{url: (isSupport("mtlancfg") ? "SDN.asp" : "Guest_network.asp"), tabName: isSupport("sdn_mwl") ? `<#Network#>` : Guest_Network_naming},
					{url: "Captive_Portal.asp", tabName: "Free WiFi"},
					{url: "Captive_Portal_Advanced.asp", tabName: "<#Captive_Portal#>"},
					{url: "Guest_network_fbwifi.asp", tabName: "Facebook WiFi"},
					{url: "NULL", tabName: "__INHERIT__"}
				]
			},
			{
				menuName: "<#AiProtection_title_Dashboard_title#>",
				index: "menu_NewDashboard",
				tab: [
					{url: "index.html", tabName: "__HIDE__"},
					{url: "NULL", tabName: "__INHERIT__"}
				]
			},
			{
				menuName: "<#Menu_TrafficManager#>",
				index: "menu_QoS", 
				tab: [
					{url: "QoS_EZQoS.asp", tabName: "<#menu5_3_2#>"},
					{url: "Main_TrafficMonitor_realtime.asp", tabName: "<#traffic_monitor#>"},
					{url: "Main_TrafficMonitor_last24.asp", tabName: "__INHERIT__"},
					{url: "Main_TrafficMonitor_daily.asp", tabName: "__INHERIT__"},
					{url: "AdaptiveQoS_InternetSpeed.asp", tabName: "<#InternetSpeed#>"},
					{url: "AdaptiveQoS_ROG.asp", tabName: "<table style='margin-top:-7px;'><tr><td><img src='/images/ROG_Logo.png' style='border:0px;width:32px;'></td><td>ROG First</td></tr></table>"}, 
					{url: "Main_Spectrum_Content.asp", tabName: "<#Spectrum_title#>"},
					{url: "AdaptiveQoS_TrafficLimiter.asp", tabName: "Traffic Limiter"},
					{url: "Advanced_QOSUserPrio_Content.asp", tabName: "__INHERIT__"},
					{url: "Advanced_QOSUserRules_Content.asp", tabName: "__INHERIT__"},
				] 
			},
			{
				menuName: "<#UU_Accelerator#>",
				index: "menu_UU", 
				tab: [
					{url: "UUAccelerator.asp", tabName: "<#UU_Accelerator#>"},
					{url: "NULL", tabName: "__INHERIT__"}
				] 
			},
			{
				menuName: "<#Parental_Control#>",
				index: "menu_ParentalControl", 
				tab: [
					{url: "AiProtection_WebProtector.asp", tabName: "<#AiProtection_filter#>"},
					{url: "ParentalControl.asp", tabName: "<#Time_Scheduling#>"},
					{url: "YandexDNS.asp", tabName: "<#YandexDNS#>"},
					{url: "NULL", tabName: "__INHERIT__"}
				] 
			},
			{
				menuName: "<#Game_Boost#>",
				index: "menu_GameBoost", 
				tab: [
					{url: "GameBoost.asp", tabName: "<#Game_Boost#>"},
					{url: "NULL", tabName: "__INHERIT__"}
				] 
			},
			{
				menuName: "腾讯网游加速器",
				index: "menu_TencentAcceleration",
				tab: [
					{url: "GameBoost_Tencent.asp", tabName: "Tencent Game Acceleration"},
					{url: "NULL", tabName: "__INHERIT__"}
				]
			},
			{
				menuName: "<#Menu_usb_application#>",
				index: "menu_APP", 
				tab: [
					{url: "APP_Installation.asp", tabName: "__HIDE__"},
					{url: "aidisk.asp", tabName: "__INHERIT__"},
					{url: "mediaserver.asp", tabName: "<#UPnPMediaServer#>"},
					{url: "Advanced_AiDisk_samba.asp", tabName: "<#menu5_4_1#>".concat(WebDav_support?" / <#Cloud_Disk#>":"")},
					{url: "Advanced_AiDisk_ftp.asp", tabName: "<#menu5_4_2#>"},
					{url: "PrinterServer.asp", tabName: "__INHERIT__"},
					{url: "Advanced_Modem_Content.asp", tabName: "__INHERIT__"},
					{url: "Advanced_TimeMachine.asp", tabName: "__INHERIT__"},
					{url: "fileflex.asp", tabName: "__INHERIT__"},
					{url: "NULL", tabName: "__INHERIT__"}
				] 
			},
			{
				menuName: "<#AiCloud_Title#>",
				index: "menu_AiCloud", 
				tab: [
					{url: "cloud_main.asp", tabName: "AiCloud 2.0"},
					{url: "cloud_sync.asp", tabName: "<#smart_sync#>"},
					{url: "cloud_router_sync.asp", tabName: "<#Server_Sync#>"},
					{url: "cloud_settings.asp", tabName: "<#Settings#>"},
					{url: "cloud_syslog.asp", tabName: "<#Log#>"},
					{url: "NULL", tabName: "__INHERIT__"}
				] 
			},
			{
				menuName: "<#Softcenter_tool#>",
				index: "menu_Tools",
				tab: [
					{url: "Tools_Sysinfo.asp", tabName: "Sysinfo"},
					{url: "Softcenter.asp", tabName: "<#Softcenter_tool#>"},
					{url: "NULL", tabName: "__INHERIT__"}
				] 
			},
			/* ============================================================================================================ */
			{
				menuName: "<#menu5#>",
				index: "menu_Split",
				tab: [
					{url: "NULL", tabName: "__HIDE__"}
				] 
			},
			{
				menuName: "<#menu5_1#>",
				index: "menu_Wireless",
				tab: [
					{url: "Advanced_Wireless_Content.asp", tabName: "<#menu5_1_1#>"},
					{url: "Advanced_WWPS_Content.asp", tabName: "<#menu5_1_2#>"},
					{url: "Advanced_WMode_Content.asp", tabName: "WDS"},
					{url: "Advanced_ACL_Content.asp", tabName: "<#menu5_1_4#>"},
					{url: "Advanced_WSecurity_Content.asp", tabName: "<#menu5_1_5#>"},
					{url: "Advanced_WAdvanced_Content.asp", tabName: "<#menu5_1_6#>"},
					{url: "Advanced_WProxy_Content.asp", tabName: "<#WiFi_Proxy_item#>"},
					{url: "Advanced_Roaming_Block_Content.asp", tabName: "<#WiFi_Roaming_Block_List#>"},
					{url: "MLO.asp", tabName: `MLO`},
					{url: "NULL", tabName: "__INHERIT__"}
				] 
			},
			{
				menuName: "<#menu5_2#>",
				index: "menu_LAN",
				tab: [
					{url: "Advanced_LAN_Content.asp", tabName: "<#menu5_2_1#>"},
					{url: "Advanced_DHCP_Content.asp", tabName: "<#menu5_2_2#>"},
					//{url: "DNSFilter.asp", tabName: "DNSFilter"},
					{url: "Advanced_MultiSubnet_Content.asp", tabName: "<#menu5_2_2#>"},
					{url: "Advanced_GWStaticRoute_Content.asp", tabName: "<#menu5_2_3#>"},
					{url: "Advanced_IPTV_Content.asp", tabName: "IPTV"},
					{url: "Advanced_SwitchCtrl_Content.asp", tabName: "<#Switch_itemname#>"},
					{url: "Advanced_VLAN_Switch_Content.asp", tabName: "VLAN"},
					{url: "Advanced_VLAN_Profile_Content.asp", tabName: "__INHERIT__"},
					{url: "Advanced_SmartDNS_Content.asp", tabName: "SmartDNS"},
					{url: "NULL", tabName: "__INHERIT__"}
				] 
			},
			{
				menuName: "<#menu5_3#>",
				index: "menu_WAN",
				tab: [
					{url: "Advanced_WAN_Content.asp", tabName: "<#menu5_3_1#>"},
					{url: "Advanced_DSL_Content.asp", tabName: "<#menu5_3_1#>"},
					{url: "Advanced_Modem_Content.asp", tabName: "<#menu5_3_1#>"},
					{url: "Advanced_MobileBroadband_Content.asp", tabName: "<#menu5_3_1#>"},
					{url: "Advanced_WANPort_Content.asp", tabName: "<#dualwan#>"},
					{url: "Advanced_PortTrigger_Content.asp", tabName: "<#menu5_3_3#>"},
					{url: "Advanced_VirtualServer_Content.asp", tabName: "<#menu5_3_4#>"},
					{url: "Advanced_Exposed_Content.asp", tabName: "<#menu5_3_5#>"},
					{url: "Advanced_ASUSDDNS_Content.asp", tabName: "<#menu5_3_6#>"},
					{url: "Advanced_NATPassThrough_Content.asp", tabName: "<#NAT_passthrough_itemname#>"},
					{url: "NULL", tabName: "__INHERIT__"}
				]
			},
			{
				menuName: "Alexa & IFTTT",
				index: "menu_Alexa_IFTTT",
				tab: [
					{url: "Advanced_Smart_Home_Alexa.asp", tabName: "__INHERIT__"},
					{url: "Advanced_Smart_Home_IFTTT.asp", tabName: "__INHERIT__"},
					{url: "NULL", tabName: "__INHERIT__"}
				]
			},
			{
				menuName: "IPv6",
				index: "menu_IPv6",
				tab: [
					{url: "Advanced_IPv6_Content.asp", tabName: "IPv6"},
					{url: "Advanced_IPv61_Content.asp", tabName: "__INHERIT__"},
					{url: "NULL", tabName: "__INHERIT__"}
				]
			},
			{
				menuName: "VPN",
				index: "menu_VPN", 
				tab: [
					{url: "Advanced_VPNServer_Content.asp", tabName: "<#BOP_isp_heart_item#>"},
					{url: "Advanced_VPNClient_Content.asp", tabName: (vpn_fusion_support) ? "<#VPN_Fusion#>" : "<#vpnc_title#>"},
					{url: "Advanced_TOR_Content.asp", tabName: "TOR"},
					{url: "Advanced_Instant_Guard.asp", tabName: "<#Instant_Guard_title#>"},
					{url: "NULL", tabName: "__INHERIT__"}
				]
			},		
			{
				menuName: "<#menu5_5#>",
				index: "menu_Firewall",
				tab: [
					{url: "Advanced_BasicFirewall_Content.asp", tabName: "<#menu5_1_1#>"},
					{url: "Advanced_URLFilter_Content.asp", tabName: "<#menu5_5_2#>"},
					{url: "Advanced_KeywordFilter_Content.asp", tabName: "<#menu5_5_5#>"},
					{url: "Advanced_Firewall_Content.asp", tabName: "<#menu5_5_4#>"},			
					{url: "NULL", tabName: "__INHERIT__"}
				]
			},
			{
				menuName: "<#menu5_6#>",
				index: "menu_Setting",
				tab: [
					{url: "Advanced_OperationMode_Content.asp", tabName: "<#menu5_6_1#>"},
					{url: "Advanced_System_Content.asp", tabName: "<#menu5_6_2#>"},
					{url: "Advanced_FirmwareUpgrade_Content.asp", tabName: "<#menu5_6_3#>"},
					{url: "Advanced_SettingBackup_Content.asp", tabName: "<#menu5_6_4#>"},
					{url: "Advanced_PerformanceTuning_Content.asp", tabName: "<#fan_tuning#>"},
					{url: "Advanced_ADSL_Content.asp", tabName: "<#menu_dsl_setting#>"},
					{url: "Advanced_Feedback.asp", tabName: "<#menu_feedback#>"},
					{url: "Feedback_Info.asp", tabName: "__INHERIT__"},
					{url: "Advanced_SNMP_Content.asp", tabName: "SNMP"},
					{url: "Advanced_TR069_Content.asp", tabName: "TR-069"},
					{url: "Advanced_Notification_Content.asp", tabName: "Notification"},
					{url: "Advanced_Privacy.asp", tabName: "<#menu_privacy#>"},
					{url: "Advanced_MultiFuncBtn.asp", tabName: "Multi-Function Button"},
					{url: "NULL", tabName: "__INHERIT__"}
				]
			},
			{
				menuName: "<#System_Log#>",
				index: "menu_Log",
				tab: [
					{url: "Main_LogStatus_Content.asp", tabName: "<#menu5_7_2#>"},
					{url: "Main_WStatus_Content.asp", tabName: "<#menu5_7_4#>"},
					{url: "Main_DHCPStatus_Content.asp", tabName: "<#menu5_7_3#>"},
					{url: "Main_IPV6Status_Content.asp", tabName: "IPv6"},
					{url: "Main_RouteStatus_Content.asp", tabName: "<#menu5_7_6#>"},
					{url: "Main_IPTStatus_Content.asp", tabName: "<#menu5_7_5#>"},
					{url: "Main_AdslStatus_Content.asp", tabName: "<#menu_dsl_log#>"},
					{url: "Main_ConnStatus_Content.asp", tabName: "<#Connections#>"},
					/* {url: "###Main_ConnStatus_Content.asp", tabName: "Captive Portal Connection Log"}, */
					{url: "NULL", tabName: "__INHERIT__"}
				]
			},
			{
				menuName: "<#Network_Tools#>",
				index: "menu_NekworkTool",
				tab: [
					{url: "Main_Analysis_Content.asp", tabName: "<#Network_Analysis#>"},
					{url: "Main_Netstat_Content.asp", tabName: "Netstat"},
					{url: "Main_WOL_Content.asp", tabName: "<#NetworkTools_WOL#>"},
					{url: "Advanced_Smart_Connect.asp", tabName: "<#smart_connect_rule#>"},
					{url: "NULL", tabName: "__INHERIT__"}
				]
			},
			{
				menuName: "Entware",
				index: "menu_Split",
				tab: [
					{url: "NULL", tabName: "__HIDE__"}
				]
			},
			{
				menuName: "Entware",
				index: "menu_Entware",
				tab: [
					{url: "Entware_start.asp", tabName: "<#Entware_home#>"},
					{url: "Entware_installed.asp", tabName: "<#Entware_installed#>"},
					{url: "Entware_list.asp", tabName: "<#Entware_list#>"},
					{url: "NULL", tabName: "__INHERIT__"}
				] 
			},
			{
				menuName: "<#Softcenter#>",
				index: "menu_Split",
				tab: [
					{url: "NULL", tabName: "__HIDE__"}
				]
			},
			{
				menuName: "<#Softcenter#>",
				index: "menu_Softcenter",
				tab: [
					{url: "Main_Soft_center.asp", tabName: "<#Softcenter#>"},
					{url: "Main_Soft_setting.asp", tabName: "Offline Mode"},
					{url: "NULL", tabName: "__INHERIT__"}
				]
			}
		],

		exclude: {
			menus: function(){
				var retArray = [];

				if(!dnsfilter_support){
					retArray.push("DNSFilter.asp");
				}

				if(!multissid_support){
					retArray.push("menu_GuestNetwork");
				}

				if(!dashboard_support){
					retArray.push("menu_NewDashboard");
				}

				if(!bwdpi_support){
					retArray.push("menu_AiProtection");
					retArray.push("menu_TrafficAnalyzer");
					retArray.push("menu_BandwidthMonitor");
				}

				if(!adaptiveqos_support){	
					for(i=0; i<menuTree.list.length; i++){
						if(menuTree.list[i].menuName == '<#Adaptive_QoS#>'){
							menuTree.list[i].menuName = '<#menu5_3_2#>';
						}
					}
				}

				if(!usb_support){
					retArray.push("menu_APP");
				}

				if((!cloudsync_support && !aicloudipk_support) || nocloudsync_support || isSupport("BUSINESS")){
					retArray.push("menu_AiCloud");
				}

				if(!ifttt_support && !alexa_support){
					retArray.push("menu_Alexa_IFTTT");
				}

				if(!IPv6_support){
					retArray.push("menu_IPv6");
				}

				if(!networkTool_support){
					retArray.push("menu_NekworkTool");
				}
				
				if(!pptpd_support && !openvpnd_support && !vpnc_support){
					retArray.push("menu_VPN");
				}

				if(!tagged_based_vlan){
					retArray.push("menu_VLAN");
				}

				if(!wtfast_support) {
					retArray.push("menu_GameBoost");
				}

				if(!tencent_qmacc_support || !isSwMode("rt"))
					retArray.push("menu_TencentAcceleration");

				if(!uu_support){
					retArray.push("menu_UU");
				}

				if(!amesh_support)
					retArray.push("menu_AiMesh");
				else{
					if(ameshRouter_support){
						if(!isSwMode("rt") && !isSwMode("ap"))
							retArray.push("menu_AiMesh");
					}
					else if(ameshNode_support)
						retArray.push("menu_AiMesh");
				}

				/* Operation Mode */
				if(isSwMode("re")){
					retArray.push("menu_ParentalControl");
					retArray.push("menu_GuestNetwork");
					retArray.push("menu_AccessControl");
					retArray.push("menu_TrafficAnalyzer");
					retArray.push("menu_QoS");
					retArray.push("menu_BandwidthMonitor");
					retArray.push("menu_AiProtection");
					retArray.push("menu_WAN");
					retArray.push("menu_IPv6");
					retArray.push("menu_VPN");
					retArray.push("menu_VLAN");
					retArray.push("menu_Firewall");
					retArray.push("menu_ParentalControl");
					retArray.push("menu_Wireless");

					if(ifttt_support || alexa_support){
						retArray.push("menu_Alexa_IFTTT");
					}
				}
				else if(isSwMode("ap")){
					retArray.push("menu_ParentalControl");
					retArray.push("menu_AccessControl");
					retArray.push("menu_TrafficAnalyzer");
					retArray.push("menu_QoS");
					retArray.push("menu_BandwidthMonitor");
					retArray.push("menu_AiProtection");
					retArray.push("menu_WAN");
					retArray.push("menu_IPv6");
					retArray.push("menu_VPN");
					retArray.push("menu_VLAN");
					retArray.push("menu_Firewall");
					retArray.push("menu_ParentalControl");

					if(ifttt_support || alexa_support){
						retArray.push("menu_Alexa_IFTTT");
					}
				}
				else if(isSwMode("mb")){
					retArray.push("menu_ParentalControl");
					retArray.push("menu_GuestNetwork");
					retArray.push("menu_AccessControl");
					retArray.push("menu_TrafficAnalyzer");
					retArray.push("menu_QoS");
					retArray.push("menu_BandwidthMonitor");
					retArray.push("menu_AiProtection");
					retArray.push("menu_Wireless");
					retArray.push("menu_WAN");
					retArray.push("menu_IPv6");
					retArray.push("menu_VPN");
					retArray.push("menu_VLAN");
					retArray.push("menu_Firewall");
					retArray.push("menu_ParentalControl");

					if(ifttt_support || alexa_support){
						retArray.push("menu_Alexa_IFTTT");
					}
				}
				else if(isSwMode("ew")){
					retArray.push("menu_ParentalControl");
					retArray.push("menu_GuestNetwork");
					retArray.push("menu_AccessControl");
					retArray.push("menu_TrafficAnalyzer");
					retArray.push("menu_QoS");
					retArray.push("menu_BandwidthMonitor");
					retArray.push("menu_AiProtection");
					retArray.push("menu_WAN");
					retArray.push("menu_IPv6");
					retArray.push("menu_VPN");
					retArray.push("menu_VLAN");
					retArray.push("menu_Firewall");
					retArray.push("menu_ParentalControl");

					if(!concurrep_support){
						retArray.push("menu_Wireless");
					}
				}
				if (!softcenter_support || '<% nvram_get("sc_installed"); %>' != '1'){
					retArray.push("menu_Softcenter");
				}
				if (!entware_support){
					retArray.push("menu_Entware");
					for(i=0; i<menuTree.list.length; i++){
						if(menuTree.list[i].menuName == 'Entware')
							menuTree.list.splice(i,1);
					}
				}

				return retArray;
			},

			tabs: function(){
				var retArray = [];

				/* By RC Support */
				if (!smartdns_support){
					retArray.push("Advanced_SmartDNS_Content.asp");
				}
				if(!bwdpi_support){
					retArray.push("AdaptiveQoS_Bandwidth_Monitor.asp");
					retArray.push("AdaptiveQoS_WebHistory.asp");
					retArray.push("AdaptiveQoS_Adaptive.asp");
					retArray.push("AiProtection_HomeSecurity.asp");
					retArray.push("AiProtection_HomeProtection.asp");
					retArray.push("AiProtection_WebProtector.asp");
					retArray.push("AiProtection_AdBlock.asp");
					retArray.push("AiProtection_Key_Guard.asp");
					retArray.push("AiProtection_AdBlock.asp");
					retArray.push("TrafficAnalyzer_Statistic.asp");
				}

				if(!traffic_analyzer_support){
					retArray.push("TrafficAnalyzer_Statistic.asp");		
				}

				if(!traffic_limiter_support){
					retArray.push("AdaptiveQoS_TrafficLimiter.asp");		
				}

				if(downsize_4m_support){
					retArray.push("Main_ConnStatus_Content.asp");
					retArray.push("Main_TrafficMonitor_realtime.asp");
				}

				if(!pptpd_support && !openvpnd_support && !ipsec_srv_support){
					retArray.push("Advanced_VPNServer_Content.asp");
				}

				if(!vpnc_support){
					retArray.push("Advanced_VPNClient_Content.asp");
				}

				if(!isSupport("Instant_Guard"))
					retArray.push("Advanced_Instant_Guard.asp");

				if(!ParentalCtrl2_support){
					retArray.push("ParentalControl.asp");
				}

				if(!yadns_support){
					retArray.push("YandexDNS.asp");
				}

				if(!frs_feedback_support) {		
					retArray.push("Advanced_Feedback.asp");
					retArray.push("Feedback_Info.asp");
				}

				if(noftp_support){
					retArray.push("Advanced_AiDisk_ftp.asp");
				}
				
				if(!dualWAN_support){
					retArray.push("Advanced_WANPort_Content.asp");
					retArray.push("Advanced_Modem_Content.asp");
					retArray.push("Advanced_MobileBroadband_Content.asp");
				}
				else{
					if(!dualwan_enabled && usb_index == 0){
						retArray.push("Advanced_WAN_Content.asp");
						if(!gobi_support)
							retArray.push("Advanced_MobileBroadband_Content.asp");
						else
							retArray.push("Advanced_Modem_Content.asp");
					}
					else{
						retArray.push("Advanced_MobileBroadband_Content.asp");
						retArray.push("Advanced_Modem_Content.asp");
					}
				}

				if(!SwitchCtrl_support){
					retArray.push("Advanced_SwitchCtrl_Content.asp");		
				}

				if(!tr069_support){
					retArray.push("Advanced_TR069_Content.asp");
				}

				if(!snmp_support){
					retArray.push("Advanced_SNMP_Content.asp");
				}

				if(!nt_center_support){
					retArray.push("Advanced_Notification_Content.asp");
				}

				if((!smart_connect_support && !smart_connect_v2_support) || Qcawifi_support || Rawifi_support){
					retArray.push("Advanced_Smart_Connect.asp");
				}
				
				if(!adBlock_support){
					retArray.push("AiProtection_AdBlock.asp");
				}
				if(!keyGuard_support){
					retArray.push("AiProtection_Key_Guard.asp");
				}

				if(!tor_support){
					retArray.push("Advanced_TOR_Content.asp");
				}

				if(!dsl_support) {
					retArray.push("Advanced_DSL_Content.asp");
					retArray.push("Advanced_ADSL_Content.asp");
					retArray.push("Main_AdslStatus_Content.asp");
					retArray.push("Main_Spectrum_Content.asp");
				}
				else{
					retArray.push("Advanced_WAN_Content.asp");
					retArray.push("Advanced_OperationMode_Content.asp");			
					if(!spectrum_support)
						retArray.push("Main_Spectrum_Content.asp");
				}

				if(hwmodeSwitch_support){
					retArray.push("Advanced_OperationMode_Content.asp");		
				}

				if(noiptv_support){
					retArray.push("Advanced_IPTV_Content.asp");
				}


				if(!media_support || nomedia_support){
					retArray.push("mediaserver.asp");
				}

				if(!rog_support){
					retArray.push("AdaptiveQoS_ROG.asp");
				}

				if(!wtfast_support){
					retArray.push("GameBoost.asp");
				}

				if(!tencent_qmacc_support || !isSwMode("rt"))
					retArray.push("GameBoost_Tencent.asp");

				if(!alexa_support){
					retArray.push("Advanced_Smart_Home_Alexa.asp");
				}

				if(!ifttt_support){
					retArray.push("Advanced_Smart_Home_IFTTT.asp");
				}

				if(!IPv6_support){
					retArray.push("Main_IPV6Status_Content.asp");
				}

				if(!fbwifi_support){
					retArray.push("Guest_network_fbwifi.asp");
				}

				if(!mtlancfg_support || isSupport("SMART_HOME_MASTER_UI")){
					retArray.push("Advanced_VLAN_Switch_Content.asp");
					retArray.push("Advanced_VLAN_Profile_Content.asp");
				}

				if(!isSupport("mtlancfg") || !isSupport("mlo")){
					retArray.push("MLO.asp");
				}
				
				if(isSupport("sdn_mainfh")){
					retArray.push("Advanced_ACL_Content.asp");
				}

				if(!tagged_based_vlan){
					retArray.push("Advanced_TagBasedVLAN_Content.asp");
					retArray.push("Advanced_MultiSubnet_Content.asp");
				}
				else
					retArray.push("Advanced_DHCP_Content.asp");

				if(!wifiproxy_support || !concurrep_support || !isSwMode("re")){
					retArray.push("Advanced_WProxy_Content.asp");
				}
				
				if(!captivePortal_support) {
					retArray.push("Captive_Portal.asp");
					retArray.push("Captive_Portal_Advanced.asp");
				}
				else {
					if(!cp_freewifi_support)
						retArray.push("Captive_Portal.asp");
					if(!cp_advanced_support)
						retArray.push("Captive_Portal_Advanced.asp");
				}

				if(!cooler_support){
					retArray.push("Advanced_PerformanceTuning_Content.asp");
				}

				if(!timemachine_support){
					retArray.push("Advanced_TimeMachine.asp");
				}

				if(!modem_support){
					retArray.push("Advanced_Modem_Content.asp");
				}

				if(!rrsut_support)
					retArray.push("cloud_router_sync.asp");

				if(!amesh_support)
					retArray.push("Advanced_Roaming_Block_Content.asp");
				else{
					if(ameshRouter_support){
						if(!isSwMode("rt") && !isSwMode("ap"))
							retArray.push("Advanced_Roaming_Block_Content.asp");
					}
					else if(ameshNode_support)
						retArray.push("Advanced_Roaming_Block_Content.asp");
				}

				if(!fileflex_support)
					retArray.push("fileflex.asp");

				if(!dnsfilter_support)
					retArray.push("DNSFilter.asp");

				if(isSupport("mtlancfg")){
					retArray.push("Captive_Portal.asp");
					retArray.push("Captive_Portal_Advanced.asp");
					retArray.push("Guest_network_fbwifi.asp");
				}

				if(isSupport("BUSINESS")){
					retArray.push("APP_Installation.asp");
					retArray.push("aidisk.asp");
					retArray.push("PrinterServer.asp");
					retArray.push("Advanced_Modem_Content.asp");
					retArray.push("Advanced_TimeMachine.asp");
					retArray.push("fileflex.asp");
				}

				if(!isSupport("sw_btn")){
					retArray.push("Advanced_MultiFuncBtn.asp");
				}

				if(isSupport("wifi7")){
					retArray.push("cloud_sync.asp");
					retArray.push("cloud_router_sync.asp");
				}

				/* Operation Mode */
				if(isSwMode("re") || isSwMode("ew")){
					retArray.push("GameBoost.asp");
					retArray.push("TrafficAnalyzer_Statistic.asp");
					retArray.push("Advanced_DHCP_Content.asp");
					retArray.push("Advanced_MultiSubnet_Content.asp");
					retArray.push("Advanced_GWStaticRoute_Content.asp");
					retArray.push("Advanced_IPTV_Content.asp");
					retArray.push("Advanced_VLAN_Switch_Content.asp");
					retArray.push("Advanced_VLAN_Profile_Content.asp");
					retArray.push("Main_DHCPStatus_Content.asp");
					retArray.push("Main_IPV6Status_Content.asp");
					retArray.push("Main_RouteStatus_Content.asp");
					retArray.push("Main_IPTStatus_Content.asp");
					retArray.push("Main_ConnStatus_Content.asp");
					retArray.push("Advanced_Smart_Connect.asp");
					retArray.push("DNSFilter.asp");

					if(!concurrep_support){
						retArray.push("Advanced_Wireless_Content.asp");
						retArray.push("Advanced_ACL_Content.asp");
					}

					if(!userRSSI_support && !location_list_support){
						retArray.push("Advanced_WAdvanced_Content.asp");
					}

					retArray.push("Advanced_WWPS_Content.asp");
					retArray.push("Advanced_WMode_Content.asp");
					retArray.push("Advanced_WSecurity_Content.asp");
					retArray.push("DNSFilter.asp");
				}
				else if(isSwMode("ap")){
					retArray.push("GameBoost.asp");
					retArray.push("TrafficAnalyzer_Statistic.asp");	
					if(!dhcp_override_support){
						retArray.push("Advanced_DHCP_Content.asp");
					}
					retArray.push("Advanced_MultiSubnet_Content.asp");
					retArray.push("Advanced_GWStaticRoute_Content.asp");
					retArray.push("Advanced_IPTV_Content.asp");
					retArray.push("Main_DHCPStatus_Content.asp");
					retArray.push("Main_IPV6Status_Content.asp");
					retArray.push("Main_RouteStatus_Content.asp");
					retArray.push("Main_IPTStatus_Content.asp");
					retArray.push("Main_ConnStatus_Content.asp");
					//short term solution for only router mode support Captive Portal
					retArray.push("Captive_Portal.asp");
					retArray.push("Captive_Portal_Advanced.asp");
					//short term solution for only router mode support Facebook Wi-Fi
					retArray.push("Guest_network_fbwifi.asp");
					retArray.push("DNSFilter.asp");
				}
				else if(isSwMode("mb")){
					retArray.push("GameBoost.asp");
					retArray.push("TrafficAnalyzer_Statistic.asp");
					retArray.push("Advanced_DHCP_Content.asp");
					retArray.push("Advanced_MultiSubnet_Content.asp");
					retArray.push("Advanced_GWStaticRoute_Content.asp");
					retArray.push("Advanced_IPTV_Content.asp");
					retArray.push("Advanced_VLAN_Switch_Content.asp");
					retArray.push("Advanced_VLAN_Profile_Content.asp");
					retArray.push("Main_DHCPStatus_Content.asp");
					retArray.push("Main_IPV6Status_Content.asp");
					retArray.push("Main_RouteStatus_Content.asp");
					retArray.push("Main_IPTStatus_Content.asp");
					retArray.push("Main_ConnStatus_Content.asp");
					retArray.push("Advanced_Smart_Connect.asp");
					retArray.push("DNSFilter.asp");
				}

				/* System Status Changed */
				// --

				/* MODELDEP */
				if(based_modelid == "RT-N10U"){
					retArray.push("Advanced_WMode_Content.asp");
				}
				else if(based_modelid == "RT-AC87U" && '<% nvram_get("wl_unit"); %>' == '1'){
					retArray.push("Advanced_WSecurity_Content.asp");
				}
				else if(based_modelid == "RT-N300"){
					retArray.push("Advanced_WMode_Content.asp");
					retArray.push("Advanced_IPTV_Content.asp");
				}

				if(!internetSpeed_support && !internetSpeed_lite_support){
					retArray.push("AdaptiveQoS_InternetSpeed.asp");
				}

				return retArray;
			}
		}
	}

	return menuTree;
});
