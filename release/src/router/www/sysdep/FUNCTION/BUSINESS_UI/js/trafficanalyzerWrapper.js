var businessWrapper = true;

var findColorAndReplace = function(styleSheet, oldColor, newColor){
	function hexToRgb(hex) {
		var result;

		result = /([a-f\d]{2})([a-f\d]{2})([a-f\d]{2})$/i.exec(hex);
		var retData = result ? [
			parseInt(result[1], 16),
			parseInt(result[2], 16),
			parseInt(result[3], 16)
		] : null;

		return retData ? retData.join(", ") : false;
	}

	$("#settingsWindow").contents().find('*').filter(function(){
		var getElement = $(this).css(styleSheet) || "";

		if(getElement.indexOf(hexToRgb(oldColor)) != -1){
			var targetStyleSheet = {}
			var originStyle = $(this).css(styleSheet);

			if(originStyle){
				targetStyleSheet[styleSheet] = (newColor != "transparent") ? originStyle.replace(hexToRgb(oldColor), hexToRgb(newColor)) : "transparent";
			}

			$(this).css(targetStyleSheet)
		}
	})
}

function setupWrapper(){
	var iframe = $("#settingsWindow").contents();

	// top banner
	iframe.find("#TopBanner").hide()
	iframe.find("#mainMenu").hide()
	iframe.find("#mainMenu").parent().hide()

	// menutree
	iframe.find(".menu_Split").hide()
	iframe.find("#QIS_wizard_menu").remove()
	iframe.find("#index_menu").remove()
	iframe.find("#index_menu").remove()
	iframe.find("#SDN_menu").remove()
	iframe.find("#AiMesh_menu").remove()
	iframe.find("#cloud_main_menu").remove()
	iframe.find("#AiProtection_HomeProtection_menu").remove()
	iframe.find("#TrafficAnalyzer_Statistic_menu").remove()
	iframe.find("#Guest_network_menu").remove()
	iframe.find("#Advanced_Smart_Home_Alexa_menu").remove()
	iframe.find("#Advanced_VPNServer_Content_menu").remove()

	// submenu
	iframe.find("#tabMenu").parent().css({
		"text-align": "center"
	})

	iframe.find("#tabMenu table").css({
		"width": "100%"
	})

	iframe.find(".tab").parent().css({
		"border-bottom": "solid 3px #6B7071"
	})

	iframe.find(".tabClicked").parent().css({
		"border-bottom": "solid 3px #006ce1"
	})

	// content
	iframe.find("#FormTitle")
		.parent().parent().parent().parent().wrap( "<div style='display:inline-block;width:100%;'></div>")

	iframe.find("#FormTitle").attr({
		"width": "100%"
	})

	iframe.find(".aimesh_introduction").css({
		"visibility": "hidden"
	})
	iframe.find(".formfonttitle").hide()
	iframe.find(".splitLine").hide()
	iframe.find(".button_gen_dis").removeClass("button_gen_dis").addClass("button_gen")
	iframe.find(".hint-color").removeClass("hint-color").addClass("warning_desc")
	setUpTrafficAnalyzer()

	// footer
	iframe.find("#footer").hide()
}

function setUpTrafficAnalyzer(){
	var taNvram = httpApi.nvramGet(["bwdpi_db_enable", "nfcm_enable"], true);
	var taStatus = (taNvram.bwdpi_db_enable == "1" || taNvram.nfcm_enable == "1") ? 1 : 0;
	document.getElementById("trafficAnalyzer_enable_switch").checked = taStatus;
}

function setupBusinessUI(){
	findColorAndReplace("color", "FFFFFF", "006CE1");
	findColorAndReplace("color", "FFCC00", "006CE1");
	findColorAndReplace("background-color", "475A5F", "transparent");
	findColorAndReplace("background-color", "4D595D", "transparent");

	setupWrapper();

	// content
	$("#settingsWindow").contents().find("#FormTitle").parent().parent().parent().parent()
		.wrap( "<div id='whiteThemeWrapper'></div>")

	$("#settingsWindow").contents().find(".hideForWrapper").hide()
	$("#settingsWindow").contents().find(".content").css({"padding-bottom":"unset"})
	$("#settingsWindow").contents().find("#traffic_unit").hide()

	setTimeout(function(){
		$("#settingsWindow").contents().find("body").hide().css("visibility", "visible").fadeIn();
	}, 100);
}

function clearTrafficAnalyzerLog() {
	var log_name = isSupport("dns_dpi") ? "dns_traffic_analyzer" : "traffic_analyzer";
	httpApi.cleanLog(log_name)
}

document.getElementById("trafficAnalyzer_enable_switch").addEventListener("change", function () {
	let taStatus = this.checked ? "1" : "0";

	let nvram = httpApi.nvramGet(["TM_EULA", "TM_EULA_time"]);

	if (nvram.TM_EULA === "0" || nvram.TM_EULA_time === "") {
		showTMEula("mals");
	} else {
		let applyObj = {
			"action_mode": "apply",
			"rc_service": "restart_wrs;restart_firewall",
			"bwdpi_db_enable": taStatus,
			"nfcm_enable": taStatus,
            "action_time": 4,
		};

		httpApi.nvramSet(applyObj, function () {
        	applyLoading(applyObj.action_time, "<#Main_alert_proceeding_desc1#>");

			setTimeout(function () {
				location.reload();
			}, applyObj.action_time * 1000);
		});
	}
});

function showTMEula(type) {
    let template = `
        <div class="d-flex justify-content-center mt-5">
            <div class="card-float">
                <div class="card-header-float"><#lyra_TrendMicro_agreement#></div>
                <div class="card-body-float">
                    <div class="mb-3"><#TM_eula_desc1#></div>

                    <div class="mb-3"><#TM_eula_desc2#></div>
                    <div><#TM_privacy_policy#></div>
                    <div><#TM_data_collection#></div>
                    
                    <div class="mt-3"><#TM_eula_desc3#></div>
                </div>
                <div class="d-flex justify-content-end card-footer-float">
                    <div class="text-center btn-regular" id="tm_eula_cancel">
                        <div><#CTL_Cancel#></div>
                    </div>
                    <div class="text-center btn-confirm" id="tm_eula_confirm">
                        <div><#CTL_ok#></div>
                    </div>
                </div>
            </div>
        </div>
    `;

    let element = document.createElement("div");
    element.className = "shadow-bg";
    element.innerHTML = template;
    document.body.appendChild(element);

    document.getElementById("tm_eula_cancel").addEventListener("click", function () {
		setUpTrafficAnalyzer()
        document.body.removeChild(element);
    });

    document.getElementById("tm_eula_confirm").addEventListener("click", function () {
		var taNvram = httpApi.nvramGet(["bwdpi_db_enable", "nfcm_enable"]);
		var taStatus = (taNvram.bwdpi_db_enable == "1" || taNvram.nfcm_enable == "1") ? 1 : 0;

        let applyObj = {
			"action_mode": "apply",
			"rc_service": "restart_wrs;restart_firewall",
			"bwdpi_db_enable": taStatus ? "0" : "1",
			"nfcm_enable": taStatus ? "0" : "1",
            "TM_EULA": "1",
            "action_time": 4
        };

        document.body.removeChild(element);
        applyLoading(applyObj.action_time, "<#Main_alert_proceeding_desc1#>");

        httpApi.nvramSet(applyObj, function () {
            setTimeout(function () {
                location.reload();
            }, applyObj.action_time * 1000);
        });
    });

    let tm_eula =
            "https://nw-dlcdnet.asus.com/support/forward.html?model=&type=TMeula&lang=" +
            system.language.currentLang +
            "&kw=&num=",
        tm_privacy = "https://nw-dlcdnet.asus.com/trend/tm_privacy",
        tm_data_collection = "https://nw-dlcdnet.asus.com/trend/tm_pdcd";

    document.getElementById("eula_url").setAttribute("href", tm_eula);
    document.getElementById("tm_eula_url").setAttribute("href", tm_privacy);
    document.getElementById("tm_disclosure_url").setAttribute("href", tm_data_collection);
}

function applyLoading(time, string, callback) {
	let template = `
		<div class="d-flex justify-content-center mt-5">
			<div class="card-float">
				<div class="card-header-float">${string}</div>
				<div class="card-body-float">
					<div class="d-flex align-items-center mt-3">
						<div class="spinner-border loader-circle-color spinner-border-lg" role="status" aria-hidden="true"></div>
						<div class="fs-4 fw-bold ms-4" id="loading_text"><#Main_alert_proceeding_desc1#>...</div>
					</div>                                        
				</div>     
			</div>
		</div>
	`;

	let element = document.createElement("div");
	element.className = "shadow-bg";
	element.innerHTML = template;
	document.body.appendChild(element);

	let count = 1;
	let counter = function (time) {
		let value = Math.round((count / time) * 100);
		document.getElementById("loading_text").innerHTML = value + "%";
		count++;

		if (value > 100) {
			if (callback) {callback();}
			document.body.removeChild(element);
		} else {
			setTimeout(function(){counter(time);}, 1000);
		}
	};

	counter(time);
}