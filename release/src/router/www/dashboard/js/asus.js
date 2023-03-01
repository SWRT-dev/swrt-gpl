var ui_support = httpApi.hookGet('get_ui_support');
function isSupport(_ptn) {
    return ui_support[_ptn] ? true : false;
}

var system = (function () {
    var obj = {};
    var nvram = httpApi.nvramGet([
        'productid',
        'wl_ifnames',
        'sw_mode',
        'wlc_psta',
        'wlc_express',
        'firmver',
        'buildno',
        'extendno',
        'swpjverno',
        'preferred_lang',
    ]);
    obj.BRCMplatform = isSupport('bcmwifi');
    obj.QCAplatform = isSupport('qcawifi');
    obj.MTKplatform = isSupport('rawifi');
    obj.RTKplatform = isSupport('rtkwifi');
    obj.INTELplatform = isSupport('lantiq');
    obj.swisscom = isSupport('swisscom');
    obj.modelName = nvram.productid;
    obj.wlIfCount = nvram.wl_ifnames.split(' ').length;
    obj.band2gSupport = isSupport('2.4G');
    obj.band5gSupport = isSupport('5G');
    obj.band6gSupport = isSupport('wifi6e');
    obj.band5g2Support = (function () {
        var wl_count = nvram.wl_ifnames.split(' ').length;
        // due to tri-band model will add "5G-2" in rc_support,
        // but some without secondary 5 GHz, eg: [2.4 GHz, 5 GHz, 6 GHz]
        if (wl_count === 3 && obj.band6gSupport) {
            return false;
        }

        return isSupport('5G-2');
    })();

    obj.band60gSupport = isSupport('wigig');
    obj.wlBandSeq = (function () {
        var nband_array = httpApi.hookGet('wl_nband_info');
        var temp = {};
        for (var i = 0; i < nband_array.length; i++) {
            // 2.4 GHz
            if (nband_array[i] === '2') {
                temp['2g'] = {
                    wl_unit: i,
                    name: '2.4 GHz',
                    prefix: '2g',
                };
            }

            // 5 GHz
            if (nband_array[i] === '1') {
                if (temp['5g1'] === undefined) {
                    temp['5g1'] = {
                        wl_unit: i,
                        name: '5 GHz',
                        prefix: '5g',
                    };
                } else {
                    // handle secondary 5 GHz
                    temp['5g1'].name = '5 GHz-1';
                    temp['5g2'] = {
                        wl_unit: i,
                        name: '5 GHz-2',
                        prefix: '5g_2',
                    };
                }
            }

            // 6 GHz
            if (nband_array[i] === '4') {
                let prefix = '5g_2';
                if (nband_array.length > 3) {
                    prefix = '6g';
                }

                temp['6g'] = {
                    wl_unit: i,
                    name: '6 GHz',
                    prefix,
                };
            }

            // 60 GHz
            if (nband_array[i] === '6') {
                temp['60g'] = {
                    wl_unit: i,
                    name: '60 GHz',
                    prefix: '60g',
                };
            }
        }

        return temp;
    })();

    obj.dualBandSupport = isSupport('dualband');
    obj.triBandSupport = isSupport('triband');
    obj.quadBandSupport = isSupport('quadband');
    obj.AMESHSupport = isSupport('amas');
    obj.smartConnectSupport = isSupport('smart_connect') || isSupport('bandstr');
    obj.wpa3Support = isSupport('wpa3');
    obj.newWiFiCertSupport = isSupport('wifi2017');
    obj.wifiLogoSupport = isSupport('wifilogo');

    obj.currentOPMode = (function () {
        let sw_mode = nvram.sw_mode;
        let wlc_psta = nvram.wlc_psta === '0' ? '0' : nvram.wlc_psta;
        let wlc_express = nvram.wlc_express === '0' ? '0' : nvram.wlc_express;
        if (sw_mode === '1') {
            // Router
            return { id: 'RT', desc: '<#wireless_router#>' };
        } else if (sw_mode === '3' && wlc_psta === '0') {
            // Access Point
            return { id: 'AP', desc: '<#OP_AP_item#>' };
        } else if ((sw_mode === '2' && wlc_psta === '0') || (sw_mode === '3' && wlc_psta === '2')) {
            // Repeater
            return { id: 'RE', desc: '<#OP_RE_item#>' };
        } else if (
            (sw_mode === '3' && wlc_psta === '1' && wlc_express === '0') ||
            (sw_mode === '3' && wlc_psta === '3' && wlc_express === '0') ||
            (sw_mode === '2' && wlc_psta === '1' && wlc_express === '0')
        ) {
            /*	Media Bridge
				Broadcom: sw_mode = 3 & wlc_psta = 1, sw_mode = 3 & wlc_psta = 3
				MTK/QCA: sw_mode = 2 & wlc_psta = 1
			*/
            return { id: 'MB', desc: '<#OP_MB_item#>' };
        } else if (sw_mode === '2' && wlc_psta === '0' && wlc_express === '1') {
            // Express Way 2G
            return { id: 'EW2', desc: '<#OP_RE2G_item#>' };
        } else if (sw_mode === '2' && wlc_psta === '0' && wlc_express === '2') {
            // Express Way 5G
            return { id: 'EW5', desc: '<#OP_RE5G_item#>' };
        } else if (sw_mode === '5') {
            // HOTSPOT;
            return { id: 'HS', desc: 'Hotspot' };
        }
    })();

    obj.firmwareVer = (function () {
        let fwString = '';
        let fwVer = nvram.firmver;
        let buildNo = nvram.buildno;
        let extendNo = nvram.extendno === '' ? '0' : nvram.extendno;
        let swpjVerNo = nvram.swpjverno;

        if (swpjVerNo !== '') {
            fwString = swpjVerNo + '_' + extendNo;
        }

        fwString = fwVer + '.' + buildNo + '_' + extendNo;
        return { full: fwString, number: fwString.split('-g')[0] };
    })();

    obj.time = (function () {
        let uptime = '<% uptime(); %>';
        let timeArray = uptime.split('(')[0];
        let bootTime = uptime.split('(')[1].split(' ')[0];
        let timeMillSec = Date.parse(timeArray);
        let sysTime = new Date(timeMillSec);
        let timeString = `${sysTime.getHours()}:${sysTime.getMinutes()}:${sysTime.getSeconds()}`;

        let weekday = ['Sun', 'Mon', 'Tue', 'Wed', 'Thu', 'Fri', 'Sat'];
        return { current: timeString, uptime: bootTime, weekday: weekday[sysTime.getDay()] };
    })();

    obj.labelMac = (function () {
        return httpApi.hookGet('get_label_mac', true);
    })();

    obj.client = (function () {
        let nmp = httpApi.hookGet('get_clientlist', true);
        let database = httpApi.hookGet('get_clientlist_from_json_database', true);
        let _client = { total: 0, activeCount: 0, detail: {}, wireless: {}, wiredCount: 0, wirelessCount: 0 };
        let mapArray = [''];
        for (let key of Object.keys(obj.wlBandSeq)) {
            _client.wireless[key] = { count: 0 };
            mapArray.push(key);
        }

        for (let [key, value] of Object.entries(database)) {
            // if (key === 'ClientAPILevel') {
            //     _client.ClientAPILevel = value;
            //     continue;
            // } else if (key === 'maclist' || key === 'ClientAPILevel') {
            if (key === 'maclist' || key === 'ClientAPILevel') {
                continue;
            }

            _client.detail[key] = { ...value };
            if (nmp[key] !== undefined) {
                if (nmp[key].isOnline !== '0') {
                    _client.activeCount++;
                    if (nmp[key].isWL !== '0') {
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
        let list = httpApi.hookGet('language_support_list', true);
        let currentLang = nvram.preferred_lang;

        return { currentLang, supportList: { ...list } };
    })();

    obj.portInfo = {};

    return obj;
})();

(function updateEthPortStatus() {
    httpApi.get_port_status_array(system.labelMac, (response) => {
        system.portInfo = { ...response };
    });

    setTimeout(updateEthPortStatus, 3000);
})();

function wlObjConstructor() {
    this.ssid = '';
    this.capability = [];
    this.channel = [];
    this.chanspecs = [];
    this.countryCode = '';
    this.sdkVersion = '';
    this.chipsetNumer = '';
    this.capability = '';
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

var tooltipTriggerList = [].slice.call(document.querySelectorAll('[data-bs-toggle="tooltip"]'));
var tooltipList = tooltipTriggerList.map(function (tooltipTriggerEl) {
    return new bootstrap.Tooltip(tooltipTriggerEl);
});
var popoverTriggerList = [].slice.call(document.querySelectorAll('[data-bs-toggle="popover"]'));
var popoverList = popoverTriggerList.map(function (popoverTriggerEl) {
    return new bootstrap.Popover(popoverTriggerEl);
});
