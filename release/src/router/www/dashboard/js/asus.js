httpApi.hookGetMore([
	"get_clientlist",
	"get_clientlist_from_json_database",
	"channel_list_2g",
	"channel_list_5g",
	"channel_list_5g_2",
	"chanspecs_2g",
	"chanspecs_5g",
	"chanspecs_5g_2",
	"get_ui_support",
	"language_support_list",
	"wl_cap_2g",
	"wl_cap_5g",
	"wl_cap_5g_2",
	"wl_nband_info",
	"get_label_mac",
	"uptime"
]);	

var ui_support = httpApi.hookGet("get_ui_support");
let timeArray = "";
function isSupport(_ptn) {
    return ui_support[_ptn] ? true : false;
}

var system = (function () {
    var obj = {};
    var nvram = httpApi.nvramGet([
        "productid",
        // "wl_ifnames",
        "sw_mode",
        "wlc_psta",
        "wlc_express",
        "firmver",
        "buildno",
        "extendno",
        "swpjverno",
        "preferred_lang",
    ]);

    obj.BRCMplatform = isSupport("bcmwifi");
    obj.QCAplatform = isSupport("qcawifi");
    obj.MTKplatform = isSupport("rawifi");
    obj.RTKplatform = isSupport("rtkwifi");
    obj.INTELplatform = isSupport("lantiq");
    obj.swisscom = isSupport("swisscom");
    obj.modelName = '<#Web_Title2#>';
    obj.band2g1Support = false;
    obj.band5g1Support = false;
    obj.band5g2Support = false;
    obj.band6g1Support = false;
    obj.band6g2Support = false;
    obj.band60g1Support = false;
    obj.wlBandSeq = (function () {
        let nband_array = httpApi.hookGet("wl_nband_info");
        let temp = {},
            count_2g = 0,
            count_5g = 0,
            count_6g = 0,
            count_60g = 0;

        for (let i = 0; i < nband_array.length; i++) {
            /*
                2: 2.4 GHz
                1: 5 GHz
                4: 6 GHz
                6 : 60 GHz
            */

            if (nband_array[i] === "2") {
                count_2g++;
                obj.band2g1Support = true;
                temp["2g" + count_2g] = {
                    wl_unit: i,
                    name: "2.4 GHz",
                    postfixHook: "2g",
                    postfixNvram: "2g" + count_2g,
                };
            }

            if (nband_array[i] === "1") {
                count_5g++;
                if (count_5g === 1) {
                    obj.band5g1Support = true;
                } else if (count_5g === 2) {
                    obj.band5g2Support = true;
                }

                temp["5g" + count_5g] = {
                    wl_unit: i,
                    name: "5 GHz",
                    postfixHook: "5g",
                    postfixNvram: "5g" + count_5g,
                };

                if (count_5g === 2) {
                    temp["5g1"].name = "5 GHz-1";
                    temp["5g2"].name = "5 GHz-2";
                    temp["5g2"].postfixHook = "5g_2";
                }
            }

            // 6 GHz
            if (nband_array[i] === "4") {
                count_6g++;
                if (count_6g === 1) {
                    obj.band6g1Support = true;
                } else if (count_6g === 2) {
                    obj.band6g2Support = true;
                }

                temp["6g" + count_6g] = {
                    wl_unit: i,
                    name: "6 GHz",
                    postfixHook: "6g",
                    postfixNvram: "6g" + count_6g,
                };

                if (count_6g === 2) {
                    temp["6g1"].name = "6 GHz-1";
                    temp["6g2"].name = "6 GHz-2";
                    temp["6g2"].postfixHook = "6g_2";
                }
            }

            // 60 GHz
            if (nband_array[i] === "6") {
                count_60g++;
                band60g1Support = true;
                temp["60g" + count_60g] = {
                    wl_unit: i,
                    name: "60 GHz",
                    postfixHook: "60g",
                    postfixNvram: "60g" + count_60g,
                };
            }
        }

        // sort the sequence to 2.4 GHz/5 GHz/6 GHz
        return Object.keys(temp)
            .sort()
            .reduce((objNew, key) => {
                objNew[key] = temp[key];
                return objNew;
            }, {});
    })();

    obj.dualBandSupport = isSupport("dualband");
    obj.triBandSupport = isSupport("triband");
    obj.quadBandSupport = isSupport("quadband");
    obj.AMESHSupport = isSupport("amas");
    obj.smartConnectSupport = isSupport("smart_connect") || isSupport("bandstr");
    obj.wpa3Support = isSupport("wpa3");
    obj.newWiFiCertSupport = isSupport("wifi2017");
    obj.wifiLogoSupport = isSupport("wifilogo");

    obj.currentOPMode = (function () {
        let sw_mode = nvram.sw_mode;
        let wlc_psta = nvram.wlc_psta === "0" ? "0" : nvram.wlc_psta;
        let wlc_express = nvram.wlc_express === "0" ? "0" : nvram.wlc_express;
        if (sw_mode === "1") {
            // Router
            return { id: "RT", desc: "<#wireless_router#>" };
        } else if (sw_mode === "3" && wlc_psta === "0") {
            // Access Point
            return { id: "AP", desc: "<#OP_AP_item#>" };
        } else if ((sw_mode === "2" && wlc_psta === "0") || (sw_mode === "3" && wlc_psta === "2")) {
            // Repeater
            return { id: "RE", desc: "<#OP_RE_item#>" };
        } else if (
            (sw_mode === "3" && wlc_psta === "1" && wlc_express === "0") ||
            (sw_mode === "3" && wlc_psta === "3" && wlc_express === "0") ||
            (sw_mode === "2" && wlc_psta === "1" && wlc_express === "0")
        ) {
            /*	Media Bridge
				Broadcom: sw_mode = 3 & wlc_psta = 1, sw_mode = 3 & wlc_psta = 3
				MTK/QCA: sw_mode = 2 & wlc_psta = 1
			*/
            return { id: "MB", desc: "<#OP_MB_item#>" };
        } else if (sw_mode === "2" && wlc_psta === "0" && wlc_express === "1") {
            // Express Way 2G
            return { id: "EW2", desc: "<#OP_RE2G_item#>" };
        } else if (sw_mode === "2" && wlc_psta === "0" && wlc_express === "2") {
            // Express Way 5G
            return { id: "EW5", desc: "<#OP_RE5G_item#>" };
        } else if (sw_mode === "5") {
            // HOTSPOT;
            return { id: "HS", desc: "Hotspot" };
        }
    })();

    obj.firmwareVer = (function () {
        let fwString = "";
        let fwVer = nvram.firmver;
        let buildNo = nvram.buildno;
        let extendNo = nvram.extendno === "" ? "0" : nvram.extendno;
        let swpjVerNo = nvram.swpjverno;

        if (swpjVerNo !== "") {
            fwString = swpjVerNo + "_" + extendNo;
        }

        fwString = fwVer + "." + buildNo + "_" + extendNo;
        return { full: fwString, number: fwString.split("-g")[0] };
    })();

    obj.time = getTime();
    obj.labelMac = (function () {
        return httpApi.hookGet("get_label_mac");
    })();

    obj.client = (function () {
        let nmp = httpApi.hookGet("get_clientlist");
        let database = httpApi.hookGet("get_clientlist_from_json_database");
        let _client = { total: 0, activeCount: 0, detail: {}, wireless: {}, wiredCount: 0, wirelessCount: 0 };
        let mapArray = [""];
        for (let key of Object.keys(obj.wlBandSeq)) {
            _client.wireless[key] = { count: 0 };
            mapArray.push(key);
        }

        for (let [key, value] of Object.entries(database)) {
            if (key === "maclist" || key === "ClientAPILevel") {
                continue;
            }

            _client.detail[key] = { ...value };
            if (nmp[key] !== undefined) {
                if (nmp[key].isOnline !== "0" && !nmp[key].amesh_isRe) {
                    _client.activeCount++;
                    if (nmp[key].isWL !== "0") {
                        let _index = mapArray[nmp[key].isWL];
                        _client.wireless[_index].count++;
                        _client.wirelessCount++;
                    } else {
                        _client.wiredCount++;
                    }
                }

                Object.assign(_client.detail[key], nmp[key]);
            }

            _client.total++;
        }

        return _client;
    })();

    obj.language = (function () {
        let list = httpApi.hookGet("language_support_list");
        let currentLang = nvram.preferred_lang;

        return { currentLang, supportList: { ...list } };
    })();

    obj.dualWANSupport = isSupport("dualwan");
    return obj;
})();

// (function updateEthPortStatus() {
//     httpApi.get_port_status_array(system.labelMac, (response) => {
//         system.portInfo = { ...response };
//     });

//     setTimeout(updateEthPortStatus, 3000);
// })();

function wlObjConstructor() {
    this.ssid = "";
    this.capability = [];
    this.channel = [];
    this.chanspecs = [];
    this.countryCode = "";
    this.sdkVersion = "";
    this.chipsetNumer = "";
    this.capability = "";
    this.nSupport = true;
    this.acSupport = false;
    this.axSupport = false;
    this.adSupport = false;
    this.aySupport = false;
    this.noVHTSupport = true;
    this.bw80MHzSupport = false;
    this.bw160MHzSupport = false;
    this.dfsSupport = false;
    this.acsCH13Support = false;
    this.acsBand1Support = false;
    this.acsBand3Support = false;
    this.channel160MHz = [];
    this.channel80MHz = [];
    this.channel40MHz = [];
    this.channel20MHz = [];
    this.QAM256Support = false;
    this.QAM1024Support = false;
    this.xboxOpt = false;
    this.heFrameSupport = false;
    this.mboSupport = false;
    this.twtSupport = false;
    this.bw160Enabled = false;
}

function getTime() {
    let uptime = "",
        bootTime = 0,
        timeMillSec = 0,
        timzone = "";

    if (system === undefined || system.time.timestamp % 60000 === 0) {
        // sync system time per 60 seconds
        uptime = httpApi.hookGet("uptime");
		cachedData.clear(["uptime"]);
        timeArray = uptime.split("(")[0].split(" ");
        // remove timezone and regenerate time string to get timemillisecond to avoid timezone offset issue
        timezone = timeArray.pop();
        timeArray.join("");
        bootTime = parseInt(uptime.split("(")[1].split(" ")[0]);
        timeMillSec = Date.parse(timeArray);
    } else if (system !== undefined && system.time.timestamp % 60000 !== 0) {
        timeMillSec = system.time.timestamp + 1000;
    }

    let sysTime = new Date(timeMillSec);

    let timeString = `${sysTime.getHours() < 10 ? "0" + sysTime.getHours() : sysTime.getHours()}:${
        sysTime.getMinutes() < 10 ? "0" + sysTime.getMinutes() : sysTime.getMinutes()
    }:${sysTime.getSeconds() < 10 ? "0" + sysTime.getSeconds() : sysTime.getSeconds()}`;
    let weekday = ["Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"];

    return {
        current: timeString,
        uptime: bootTime,
        weekday: weekday[sysTime.getDay()],
        timestamp: timeMillSec,
        timezone,
    };
}

var tooltipTriggerList = [].slice.call(document.querySelectorAll('[data-bs-toggle="tooltip"]'));
var tooltipList = tooltipTriggerList.map(function (tooltipTriggerEl) {
    return new bootstrap.Tooltip(tooltipTriggerEl);
});
var popoverTriggerList = [].slice.call(document.querySelectorAll('[data-bs-toggle="popover"]'));
var popoverList = popoverTriggerList.map(function (popoverTriggerEl) {
    return new bootstrap.Popover(popoverTriggerEl);
});

function webInterface(message){
	var returnData = {};
	if(message != undefined && message != ""){
		var messageFromApp = JSON.parse(message);

		if(messageFromApp.dataHandler != undefined){
			if(httpApi != undefined){
				httpApi.app_dataHandler = ((messageFromApp.dataHandler).toString() == "1") ? true : false;
				returnData.dataHandler = "1";
			}
		}

		if(messageFromApp.hideHeader != undefined){
			if((messageFromApp.hideHeader).toString() == "1"){
				$('.notInApp').attr('style', 'display:none !important;');
			}
		}
	}

	return returnData;
}

var postMessageToApp = function(){};
try{
	if(window.webkit.messageHandlers.appInterface){
		postMessageToApp = function(jsonObj){window.webkit.messageHandlers.appInterface.postMessage(JSON.stringify(jsonObj));}
	}
}catch(e){
	if(window.appInterface){
		postMessageToApp = function(jsonObj){window.appInterface.postMessage(JSON.stringify(jsonObj));}
	}
}