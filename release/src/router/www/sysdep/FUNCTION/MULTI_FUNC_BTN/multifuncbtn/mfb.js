let is_Web_iframe = ($(parent.document).find("#mainMenu").length > 0) ? true : false;
const vpn_title_text = (()=>{return (isSupport("vpn_fusion") ? `<#VPN_Fusion#>` : `<#vpnc_title#>`);})();
let btnsw_list = [
	{"title":vpn_title_text, "value":"4"},
	{"title":`Operation`, "value":"1"},/* untranslated */
	{"title":`LED`, "value":"2"},/* untranslated */
	{"title":`WiFi`, "value":"3"},/* untranslated */
	{"title":`No Function`, "value":"0"}/* untranslated */
];
btnsw_list = btnsw_list.filter(item => item.value != "1");
const support_vpn_btn = (isSupport("vpn_fusion") && isSwMode("rt"));
if(!support_vpn_btn){
	btnsw_list = btnsw_list.filter(item => item.value != "4")
}
let nv_btnsw_onoff = httpApi.nvramGet(["btnsw_onoff"]).btnsw_onoff;
let vpnc_profile_list = [];
if(support_vpn_btn){
	Get_VPNC_Profile();

	function Get_VPNC_Profile(){
		const vpnc_profile_attr = function(){
			this.desc = "";
			this.proto = "";
			this.vpnc_idx = "0";
			this.activate = "0";
			this.default_conn = false;
		};
		let vpnc_default_wan = httpApi.nvramGet(["vpnc_default_wan"]).vpnc_default_wan;
		let vpnc_clientlist = decodeURIComponent(httpApi.nvramCharToAscii(["vpnc_clientlist"]).vpnc_clientlist);
		let each_profile = vpnc_clientlist.split("<");
		$.each(each_profile, function(index, value){
			if(value != ""){
				let profile_data = value.split(">");
				let vpnc_profile = new vpnc_profile_attr();
				vpnc_profile.desc = profile_data[0];
				vpnc_profile.proto = profile_data[1];
				if(isSupport("vpn_fusion")){
					vpnc_profile.activate = profile_data[5];
					vpnc_profile.vpnc_idx = profile_data[6];
					vpnc_profile.default_conn = ((profile_data[6] == vpnc_default_wan) ? true : false);
				}
				vpnc_profile_list.push(JSON.parse(JSON.stringify(vpnc_profile)));
			}
		});
	}
}
function show_MFB_list(){
	if(nv_btnsw_onoff == "4" && vpnc_profile_list.length == 0){
		nv_btnsw_onoff = "0";
	}
	let $profile_list_content = $("#profile_list_content").empty();
	$.each(btnsw_list, function(index, item){
		Get_Component_Profile_Item(item).appendTo($profile_list_content);
	});

	let $profile_setting_cntr = $("#profile_setting_container");
	if($profile_setting_cntr.css("display") != "none"){
		 $profile_list_content.children("[data-btnsw-onoff='" + nv_btnsw_onoff + "']").click();
	}

	resize_iframe_height();
}
function Get_Component_Profile_Item(_profile_data){
	let $profile_item_container = $("<div>").addClass("profile_item_container").attr({"data-btnsw-onoff":_profile_data.value}).unbind("click").click(function(e){
		e = e || event;
		e.stopPropagation();
		let $profile_setting_cntr = $(".profile_setting_container");
		if($profile_setting_cntr.css("display") == "none"){
			let $popup_cntr = $(".popup_container.popup_element");
			$(".popup_element").css("display", "flex");
			$(".container").addClass("blur_effect");
			$popup_cntr.empty().append(function(){
				switch(_profile_data.value){
					case "0":
						return Get_Component_BTNSW_NoFun("popup");
						break;
					case "1":
						return Get_Component_BTNSW_OP("popup");
						break;
					case "2":
						return Get_Component_BTNSW_LED("popup");
						break;
					case "3":
						return Get_Component_BTNSW_WiFi("popup");
						break;
					case "4":
						return Get_Component_BTNSW_VPN("popup");
						break;
					default:
						break;
				}
			});
			Update_Setting_Profile($popup_cntr, _profile_data);
			adjust_popup_container_top($popup_cntr, 100);
		}
		else{
			$(this).addClass("selected").siblings().removeClass("selected");
			$profile_setting_cntr.empty().append(function(){
				switch(_profile_data.value){
					case "0":
						return Get_Component_BTNSW_NoFun();
						break;
					case "1":
						return Get_Component_BTNSW_OP();
						break;
					case "2":
						return Get_Component_BTNSW_LED();
						break;
					case "3":
						return Get_Component_BTNSW_WiFi();
						break;
					case "4":
						return Get_Component_BTNSW_VPN();
						break;
					default:
						break;
				}
			});
			Update_Setting_Profile($profile_setting_cntr, _profile_data);
		}
	});

	let $item_tag_cntr = $("<div>").addClass("item_tag_container").appendTo($profile_item_container);
	if(_profile_data.value == nv_btnsw_onoff){
		$("<div>").addClass("item_tag link").html("<#CTL_Default#>").appendTo($item_tag_cntr);
	}

	let $item_text_container = $("<div>").addClass("item_text_container").appendTo($profile_item_container)
	let $main_info = $("<div>").addClass("main_info").appendTo($item_text_container);
	$("<div>").addClass("main_info_text").html(htmlEnDeCode.htmlEncode(_profile_data.title)).appendTo($main_info);

	return $profile_item_container;
}
function Update_Setting_Profile(_obj, _profile_data){
	$(_obj).find(".setting_content_container").attr("data-btnsw-onoff", _profile_data.value);
	$(_obj).find("#btnsw_onoff").removeClass("off on").addClass((function(){
		if(nv_btnsw_onoff == _profile_data.value)
			return "on";
		else
			return "off";
	})());

	if(_profile_data.value == "4"){
		let $VPN_Profiles_cntr = $(_obj).find("[data-container=VPN_Profiles]");
		const btnsw_vpn_idx = httpApi.nvramGet(["btnsw_vpn_idx"]).btnsw_vpn_idx;
		$VPN_Profiles_cntr.find(".icon_radio").removeClass("clicked").filter("[data-idx="+btnsw_vpn_idx+"]").addClass("clicked");
		if($VPN_Profiles_cntr.find(".icon_radio.clicked").length == 0){
			$VPN_Profiles_cntr.find(".icon_radio[data-vpn-type=vpnc]").first().addClass("clicked");
		}
		$.each(vpnc_profile_list, function(index, item){
			if(item.activate == "0"){
				$VPN_Profiles_cntr.find("[data-idx="+item.vpnc_idx+"]").siblings("[data-component=icon_warning]").show();
			}
		});
		if(nv_btnsw_onoff == "4"){
			$VPN_Profiles_cntr.show();
		}
		else{
			$VPN_Profiles_cntr.hide();
		}
	}
}
function Get_Component_BTNSW_VPN(view_mode){
	let $container = $("<div>").addClass("setting_content_container no_action_container");

	if(view_mode == "popup"){
		Get_Component_Popup_Profile_Title(vpn_title_text).appendTo($container)
			.find("#title_close_btn").unbind("click").click(function(e){
				e = e || event;
				e.stopPropagation();
				close_popup_container($container);
			});
	}
	else
		Get_Component_Profile_Title(vpn_title_text).appendTo($container).find("#title_del_btn").remove();

	let $content_container = $("<div>").addClass("popup_content_container profile_setting").appendTo($container);

	Get_Component_Schematic({"fun_desc":`While switch on, VPN connection will be turn on automatically.`}).appendTo($content_container);

	let btnsw_onoff_parm = {"title":"Multi-Function Button Default", "type":"switch", "id":"btnsw_onoff", "set_value":"off"};
	Get_Component_Switch(btnsw_onoff_parm).appendTo($content_container)
		.find("#" + btnsw_onoff_parm.id + "").click(function(e){
			e = e || event;
			e.stopPropagation();
			if(vpnc_profile_list.length == 0){
				$(this).toggleClass("off on");
				show_customize_alert(stringSafeGet(`<#GuestNetwork_noProfile_hint#>`));
				return false;
			}
			else{
				let $VPN_Profiles_cntr = $content_container.find("[data-container=VPN_Profiles]");
				if($(this).hasClass("on")){
					$VPN_Profiles_cntr.show();
				}
				else{
					$VPN_Profiles_cntr.hide();
				}
				resize_iframe_height();
				let $selected_vpn = $VPN_Profiles_cntr.find(".icon_radio.clicked");
				if($selected_vpn.length){
					const vpn_idx = $selected_vpn.attr("data-idx");
					if(vpn_idx != "" && vpn_idx != undefined){
						const this_btnsw_onoff = $(this).closest(".setting_content_container").attr("data-btnsw-onoff");
						let postData = {"btnsw_onoff":this_btnsw_onoff, "switch_status":$(this).hasClass("on")};
						if(postData.switch_status == true){
							postData["btnsw_vpn_idx"] = vpn_idx;
						}
						Set_BTNSW_ONOFF(postData);
					}
					else{
						show_customize_alert(stringSafeGet(`<#GuestNetwork_noProfile_hint#>`));
						return false;
					}
				}
			}
		});

	Get_Component_VPN_Profiles().appendTo($content_container).find(".profile_item").click(function(e){
		e = e || event;
		e.stopPropagation();
		let $setting_content_cntr = $(this).closest(".setting_content_container");
		const this_btnsw_onoff = $setting_content_cntr.attr("data-btnsw-onoff");
		const btnsw_switch = $setting_content_cntr.find("#btnsw_onoff").hasClass("on");
		const vpn_idx = $(this).find(".icon_radio").attr("data-idx");
		Set_BTNSW_ONOFF({
			"btnsw_onoff": this_btnsw_onoff,
			"switch_status": btnsw_switch,
			"btnsw_vpn_idx": vpn_idx
		});
	});

	let vpnc_btn_parm = {"id":"vpnc_btn", "text":"<#btn_goSetting#>"};
	Get_Component_Btn(vpnc_btn_parm).appendTo($content_container)
	.find("#" + vpnc_btn_parm.id + "").click(function(e){
		e = e || event;
		e.stopPropagation();
		if(is_Web_iframe){
			top.location.href = "/Advanced_VPNClient_Content.asp";
		}
		else if(parent.businessWrapper){
			top.location.href = "/index.html?url=vpnc&current_theme=business";
		}
		else{
			top.location.href = "/VPN/vpnc.html" + ((typeof theme == "string" && theme != "") ? "?current_theme=" + theme + "" : "");
		}
	});

	return $container;
}
function Get_Component_BTNSW_OP(view_mode){
	let $container = $("<div>").addClass("setting_content_container no_action_container");

	if(view_mode == "popup"){
		Get_Component_Popup_Profile_Title(`Operation Profile`).appendTo($container)/* untranslated */
			.find("#title_close_btn").unbind("click").click(function(e){
				e = e || event;
				e.stopPropagation();
				close_popup_container($container);
			});
	}
	else
		Get_Component_Profile_Title(`Operation Profile`).appendTo($container).find("#title_del_btn").remove();/* untranslated */

	let $content_container = $("<div>").addClass("popup_content_container profile_setting").appendTo($container);

	Get_Component_Schematic().appendTo($content_container);

	let btnsw_onoff_parm = {"title":"Multi-Function Button Default", "type":"switch", "id":"btnsw_onoff", "set_value":"off"};
	Get_Component_Switch(btnsw_onoff_parm).appendTo($content_container)
		.find("#" + btnsw_onoff_parm.id + "").click(function(e){
			e = e || event;
			e.stopPropagation();
			const this_btnsw_onoff = $(this).closest(".setting_content_container").attr("data-btnsw-onoff");
			Set_BTNSW_ONOFF({"btnsw_onoff":this_btnsw_onoff, "switch_status":$(this).hasClass("on")});
		});

	return $container;
}
function Get_Component_BTNSW_LED(view_mode){
	let $container = $("<div>").addClass("setting_content_container no_action_container");

	if(view_mode == "popup"){
		Get_Component_Popup_Profile_Title(`LED`).appendTo($container)
			.find("#title_close_btn").unbind("click").click(function(e){
				e = e || event;
				e.stopPropagation();
				close_popup_container($container);
			});
	}
	else
		Get_Component_Profile_Title(`LED`).appendTo($container).find("#title_del_btn").remove();

	let $content_container = $("<div>").addClass("popup_content_container profile_setting").appendTo($container);

	Get_Component_Schematic({"fun_desc":`Turn on/off router's LED.`}).appendTo($content_container);/* untranslated */

	let btnsw_onoff_parm = {"title":"Multi-Function Button Default", "type":"switch", "id":"btnsw_onoff", "set_value":"off"};
	Get_Component_Switch(btnsw_onoff_parm).appendTo($content_container)
		.find("#" + btnsw_onoff_parm.id + "").click(function(e){
			e = e || event;
			e.stopPropagation();
			const this_btnsw_onoff = $(this).closest(".setting_content_container").attr("data-btnsw-onoff");
			Set_BTNSW_ONOFF({"btnsw_onoff":this_btnsw_onoff, "switch_status":$(this).hasClass("on")});
		});

	return $container;
}
function Get_Component_BTNSW_WiFi(view_mode){
	let $container = $("<div>").addClass("setting_content_container no_action_container");

	if(view_mode == "popup"){
		Get_Component_Popup_Profile_Title(`WiFi`).appendTo($container)
			.find("#title_close_btn").unbind("click").click(function(e){
				e = e || event;
				e.stopPropagation();
				close_popup_container($container);
			});
	}
	else
		Get_Component_Profile_Title(`WiFi`).appendTo($container).find("#title_del_btn").remove();

	let $content_container = $("<div>").addClass("popup_content_container profile_setting").appendTo($container);

	Get_Component_Schematic({"fun_desc":`Turn on/off router's WiFi network.`}).appendTo($content_container);/* untranslated */

	let btnsw_onoff_parm = {"title":"Multi-Function Button Default", "type":"switch", "id":"btnsw_onoff", "set_value":"off"};
	Get_Component_Switch(btnsw_onoff_parm).appendTo($content_container)
		.find("#" + btnsw_onoff_parm.id + "").click(function(e){
			e = e || event;
			e.stopPropagation();
			const this_btnsw_onoff = $(this).closest(".setting_content_container").attr("data-btnsw-onoff");
			Set_BTNSW_ONOFF({"btnsw_onoff":this_btnsw_onoff, "switch_status":$(this).hasClass("on")});
		});

	return $container;
}
function Get_Component_BTNSW_NoFun(view_mode){
	let $container = $("<div>").addClass("setting_content_container no_action_container");

	if(view_mode == "popup"){
		Get_Component_Popup_Profile_Title(`No Function`).appendTo($container)/* untranslated */
			.find("#title_close_btn").unbind("click").click(function(e){
				e = e || event;
				e.stopPropagation();
				close_popup_container($container);
			});
	}
	else
		Get_Component_Profile_Title(`No Function`).appendTo($container).find("#title_del_btn").remove();

	let $content_container = $("<div>").addClass("popup_content_container profile_setting").appendTo($container);

	Get_Component_Schematic({"fun_desc":`No action.`}).appendTo($content_container);/* untranslated */

	let btnsw_onoff_parm = {"title":"Multi-Function Button Default", "type":"switch", "id":"btnsw_onoff", "set_value":"off"};
	Get_Component_Switch(btnsw_onoff_parm).appendTo($content_container)
		.find("#" + btnsw_onoff_parm.id + "").click(function(e){
			e = e || event;
			e.stopPropagation();
			const this_btnsw_onoff = $(this).closest(".setting_content_container").attr("data-btnsw-onoff");
			if($(this).hasClass("off")){
				if(this_btnsw_onoff == nv_btnsw_onoff){
					show_customize_alert(`Please select another function to cancel the default.`);
					$(this).toggleClass("off on");
					return;
				}
			}
			Set_BTNSW_ONOFF({"btnsw_onoff":this_btnsw_onoff, "switch_status":$(this).hasClass("on")});
		});

	return $container;
}
function Get_Component_VPN_Profiles(){
	let $container = $("<div>").addClass("VPN_list_container").attr({"data-container": "VPN_Profiles"});

	let $vpnc_group = $("<div>").addClass("profile_group").appendTo($container);
	$("<div>").addClass("title").html(vpn_title_text).appendTo($vpnc_group);
	let vpnc_options = [];
	vpnc_profile_list.forEach(function(item){
		vpnc_options.push({"type":"vpnc", "text":item.desc, "value":item.vpnc_idx, "proto":item.proto, "activate":item.activate, "default_conn":item.default_conn});
	});
	if(vpnc_options.length > 0){
		let $content_cntr = $("<div>").addClass("profile_container").appendTo($vpnc_group);
		$.each(vpnc_options, function(index, value){
			Get_Component_VPN(value).appendTo($content_cntr);
		});
	}
	else{
		$("<div>").addClass("profile_hint").html(stringSafeGet(`<#GuestNetwork_noProfile_hint#>`)).appendTo($vpnc_group);
	}

	if($container.find(".icon_radio.clicked").length == 0){
		$container.find(".icon_radio[data-vpn-type=vpnc]").first().addClass("clicked");
	}
	return $container;

	function Get_Component_VPN(_profile){
		let $profile_cntr = $("<div>").addClass(()=>{
				return ("profile_item " + ((_profile.default_conn) ? "default_conn" : ""));
			})
			.unbind("click").click(function(e){
				e = e || event;
				e.stopPropagation();
				if($(this).find(".icon_radio").attr("data-disabled") == "true")
					return false;
				$(this).closest("[data-container=VPN_Profiles]").find(".icon_radio").removeClass("clicked");
				$(this).find(".icon_radio").addClass("clicked");
			});
		let $text_container = $("<div>").addClass("text_container").appendTo($profile_cntr)
		$("<div>").html(htmlEnDeCode.htmlEncode(_profile.text)).appendTo($text_container);
		if(_profile.default_conn)
			$("<div>").addClass("default_conn_text").html(`<#vpnc_default#>`).appendTo($text_container);

		let $select_container = $("<div>").addClass("select_container").appendTo($profile_cntr);
		$("<div>").addClass("icon_warning_amber").attr({"data-component":"icon_warning"}).unbind("click").click(function(e){
			e = e || event;
			e.stopPropagation();
			$(this).closest("[data-container=VPN_Profiles]").find(".error_hint_container").remove();
			let error_hint = "<#GuestNetwork_VPN_errHint0#> <#GuestNetwork_VPN_errHint1#>";
			$(this).closest(".profile_item").append(Get_Component_Error_Hint({"text":error_hint}).show());
		}).hide().appendTo($select_container);

		$("<div>").addClass("icon_error_outline").attr({"data-component":"icon_error"}).unbind("click").click(function(e){
			e = e || event;
			e.stopPropagation();
			$(this).closest("[data-container=VPN_Profiles]").find(".error_hint_container").remove();
			let error_hint = "<#GuestNetwork_VPN_errHint2#>";
			$(this).closest(".profile_item").append(Get_Component_Error_Hint({"text":error_hint}).show());
		}).hide().appendTo($select_container);

		$("<div>").addClass("icon_radio").attr({"data-idx":_profile.value, "data-vpn-type":_profile.type}).appendTo($select_container);
		return $profile_cntr;
	}
}
function Get_Component_Schematic(_parm){
	let $profile_setting_item = $("<div>").addClass("profile_setting_item schematic");
	let $schematic_cntr = $("<div>").addClass("schematic_container").appendTo($profile_setting_item);
	$("<div>").addClass("img_item").appendTo($schematic_cntr);
	$("<div>").addClass("notice_item").html(`*Schematic machine switch.`).appendTo($schematic_cntr);
	let fun_desc = "";
	if(_parm != undefined){
		fun_desc = (_parm.fun_desc != undefined) ? _parm.fun_desc : "";
	}
	$("<div>").addClass("fun_desc_item").html(fun_desc).appendTo($schematic_cntr);
	return $profile_setting_item;
}
function Set_BTNSW_ONOFF(_json_data){
	let nvramSet_obj = {"action_mode": "apply","rc_service": "restart_swbtn"};
	if(_json_data.switch_status){
		nvramSet_obj.btnsw_onoff = _json_data.btnsw_onoff;
	}
	else{
		nvramSet_obj.btnsw_onoff = "0";
	}
	if(_json_data.btnsw_vpn_idx){
		nvramSet_obj.btnsw_vpn_idx = _json_data.btnsw_vpn_idx
	}

	httpApi.nvramSet(nvramSet_obj, function(){
		let $profile_item_cntrs = $("#profile_list_content .profile_item_container");
		$profile_item_cntrs.find(".item_tag").remove();
		nv_btnsw_onoff = httpApi.nvramGet(["btnsw_onoff", "btnsw_vpn_idx"], true).btnsw_onoff;
		$profile_item_cntrs.filter("[data-btnsw-onoff="+nv_btnsw_onoff+"]").find(".item_tag_container").append($("<div>").addClass("item_tag link").html("<#CTL_Default#>"));
	});
}
