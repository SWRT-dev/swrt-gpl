function genArchitecture() {
    genHeader();
    genNavMenu();
    genMidHeader();
}

function genHeader() {
    let header = document.getElementsByTagName("header")[0];
    let code = "";
    // LOGO, MODEL NAME
    code += genLogoModelName();

    // LOGOUT
    code += genLogout();

    // REBOOT
    code += genReboot();

    // LANGUAGE MENU
    code += genLanguageList();

    header.innerHTML = code;
}

function genLogoModelName() {
    let code = "";
    code += `
		<div class="d-flex align-items-center me-auto">
			<div role="icon" class="icon-size-24 icon-menu me-3" id='mobile_menu'></div>
			<div role="icon" class="icon-size-logo icon-logo me-4"></div>
			<div class="model-name mx-3">${system.modelName}</div>
		</div>	
	`;

    return code;
}

function genLogout() {
    let code = "";
    code += `
		<div role="btn-logout" class="d-flex align-items-center mx-3" onclick="logout();">
            <div role="icon" class="icon-size-24 icon-logout"></div>
            <div role="info-text" class="ps-2 logout-title"><#t1Logout#></div>
        </div>	
	`;

    return code;
}

function genReboot() {
    let code = "";
    code += `	
		<div role="btn-reboot" class="d-flex align-items-center mx-3" onclick="reboot();">
            <div role="icon" class="icon-size-24 icon-reboot"></div>
            <div role="info-text" class="ps-2 reboot-title"><#BTN_REBOOT#></div>
        </div>
	`;

    return code;
}

function genLanguageList() {
    let code = "";
    let { currentLang, supportList } = system.language;
    code += `
		<div role="btn-language" class="d-flex align-items-center mx-3">
			<div role="icon" class="icon-size-24 icon-language"></div>
			<div class="dropdown ms-2">
				<button
					class="btn dropdown-toggle p-1 lang-title"
					type="button"
					id="dropdownLangMenu"
					data-bs-toggle="dropdown"
				>
					${supportList[currentLang]}
				</button>
		`;
    code += `<ul class="dropdown-menu text-center lang-menu" aria-labelledby="dropdownLangMenu">`;

    for (let [key, value] of Object.entries(supportList)) {
        if (key === currentLang) continue;

        code += `
			<li onclick="changeLanguage('${key}')">
				<a class="dropdown-item lang-item" href="#">${value}</a>
			</li>
		`;
    }

    code += `</ul>`;
    code += `			
			</div>
		</div>
	`;

    return code;
}
let menuList = [
    {
        name: "Home",
        icon: "icon-home",
        url: "index.asp",
        clicked: false,
        divide: false,
    },
    {
        name: `<#AiProtection_title_Dashboard_title#>`,
        icon: "icon-dashboard",
        url: "dashboard",
        clicked: false,
        divide: true,
    },
    {
        name: `<#AiProtection_title#>`,
        icon: "icon-aiProtection",
        url: "aiprotection",
        clicked: false,
        divide: false,
    },
    // {
    //     name: "Help & Support",
    //     icon: "icon-help",
    //     url: "",
    //     clicked: false,
    //     divide: true,
    // },
];
function genNavMenu() {
    /*
        {
            name: MENU名稱,
            icon: icon類型,
            url: 頁面URL名稱,
            clicked: [boolean] ,
            divide: [boolean] 分隔線,
        },
    
    */
    let nav = document.getElementsByTagName("nav")[0];

    // for mobile layout, show the close icon
    let code = `
        <div role="menu-close" class="d-flex justify-content-end">
            <div role="icon" class="icon-size-36 icon-close mt-2 mx-2" id="mobile_menu_close"></div>
        </div>
    `;

    for (let i = 0; i < menuList.length; i++) {
        let list = menuList[i];
        if (list.divide) {
            code += `<div class="ms-4 me-2 divide-line-menu"></div>`;
        }

        code += `
            <ul class="list-unstyled mb-0">
                <li role="menu">
                    <button
                        class="btn btn-toggle menu-block ps-4 pe-2 py-3 w-100 ${
                            list.clicked ? "menu-clicked" : ""
                        }" onclick="pageRedirect('${list.url}')">
                        <div role="icon" class="icon-size-24 ${list.icon}"></div>
                        <div role="menu-text" class="ms-3">${list.name}</div>
                    </button>
                </li>
            </ul>
        `;
    }

    code += `
        <div class="mt-auto mb-3 ps-4 pe-2 pt-3 copyright">2022 ASUSTeK Computer Inc. All rights reserved.</div>
    `;

    // code = `
    //     <div role="menu-close" class="d-flex justify-content-end">
    //         <div role="icon" class="icon-size-36 icon-close mt-2 mx-2" id="mobile_menu_close"></div>
    //     </div>

    //     <ul class="list-unstyled mb-1">
    //             <li role="menu">
    //                 <button
    //                     class="btn btn-toggle menu-block ps-4 pe-2 py-2 w-100 ">
    //                     <div role="icon" class="icon-size-24 "></div>
    //                     <div role="menu-text" class="ms-3">AiProtection 智慧安全防護</div>
    //                 </button>
    //                 <div class="py-1 "style="margin-left:56px;font-size: 14px;padding-left:16px">Statistic</div>
    //                 <div class="py-1" style="margin-left:56px;font-size: 14px;padding-left:16px">Bandwidth Monitor</div>
    //                 <div class="py-1 " style="margin-left:56px;font-size: 14px;padding-left:16px">Traffic Monitor</div>
    //                 <div class="py-1 menu-clicked" style="margin-left:56px;font-size: 14px;padding-left:16px">Web History</div>
    //             </li>
    //     </ul>

    //     <ul class="list-unstyled mb-1">
    //             <li role="menu">
    //                 <button
    //                     class="btn btn-toggle menu-block ps-4 pe-2 py-2 w-100 menu-clicked">
    //                     <div role="icon" class="icon-size-24 "></div>
    //                     <div role="menu-text" class="ms-3">AiProtection 智慧安全防護1</div>
    //                 </button>
    //             </li>
    //     </ul>

    // `;
    nav.innerHTML = code;
    document.getElementById("mobile_menu").addEventListener("click", function () {
        let menu_hide = nav.classList.contains("nav-menu-hide");
        menu_hide ? nav.classList.remove("nav-menu-hide") : nav.classList.add("nav-menu-hide");
    });
    document.getElementById("mobile_menu_close").addEventListener("click", function () {
        let menu_hide = nav.classList.contains("nav-menu-hide");
        menu_hide ? nav.classList.remove("nav-menu-hide") : nav.classList.add("nav-menu-hide");
    });
}

function genMidHeader() {
    let banner = document.querySelector('[role~="info-banner"]');
    let code = "";

    code += genTimeBanner();
    code += genOpMode();
    code += genFirmwareVersion();
    code += genSSID();
    // code += genStatusBanner();

    banner.innerHTML = code;

    //trigger Time counting
    setInterval(function () {
        system.time = getTime();
        document.getElementById("sys_time").innerHTML = `
            <span>${system.time.current}</span>
            <small class="banner-time-day">${system.time.weekday}</small>
        `;
    }, 1000);
}

function genTimeBanner() {
    let code = "";
    code += `
        <div role="time-banner" class="mx-2 my-1 px-2 py-1">
            <div class="ps-1 banner-title"><#General_x_SystemTime_itemname#></div>
            <div class="font-weight-bold banner-time" id="sys_time">
                <span>${system.time.current}</span>
                <small class="banner-time-day">${system.time.weekday}</small>
            </div>
        </div>
    `;

    return code;
}

function genOpMode() {
    let code = "";
    code += `
        <div role="op-mode-banner" class="mx-2 my-1 p-1">
            <div class="banner-left-bar ps-3 text-truncate banner-title"><#menu5_6_1_title#></div>
            <div class="font-weight-bold banner-text pt-2 ps-3 text-truncate">${system.currentOPMode.desc}</div>
        </div>
    `;

    return code;
}

function genFirmwareVersion() {
    let code = "";
    code += `
        <div role="fw-ver-banner" class="mx-2 my-1 p-1">
            <div class="banner-left-bar ps-3 text-truncate banner-title">
                <#General_x_FirmwareVersion_itemname#>
            </div>
            <div class="font-weight-bold banner-text pt-2 ps-3">${system.firmwareVer.number}</div>
        </div>
    `;

    return code;
}

function genSSID() {
    let code = "";
    code += `
        <div role="ssid-banner" class="mx-2 my-1 p-1">
            <div class="banner-left-bar ps-3 banner-title">SSID</div>
            <div class="d-flex font-weight-bold pt-2 ps-3 banner-text">
    `;

    for (let i = 0; i < Object.values(wireless).length; i++) {
        code += `<span class="text-truncate px-2">${Object.values(wireless)[i].ssid}</span>`;
    }

    code += `               
            </div>
        </div>
    `;

    return code;
}

// function genStatusBanner() {
//     let array = [
//         {
//             type: "Notification",
//             icon: "icon-notifications",
//             title: "CONNEDTED",
//             content: "",
//             active: false,
//         },
//         {
//             type: "APP",
//             icon: "icon-app",
//             title: "",
//             content: "",
//             active: true,
//         },
//         {
//             type: "Guest Network",
//             icon: "icon-guest-network-status",
//             title: "",
//             content: "",
//             active: false,
//         },
//         {
//             type: "WAN",
//             icon: "icon-wan-status",
//             title: "",
//             content: "",
//             active: false,
//         },
//         {
//             type: "USB",
//             icon: "icon-usb-status",
//             title: "",
//             content: "",
//             active: false,
//         },
//     ];
//     let code = "";
//     code += `
//         <div class="ms-auto d-flex align-items-center me-4">
//             <div role="status_banner_out" class="px-2 py-3">

//     `;

//     for (let item of array) {
//         code += `
//             <div
//                 role="icon state-banner"
//                 tabindex="0"
//                 class="icon-size-24 ${item.icon} icon-default-color ms-4 ${item.active ? "icon-color-active" : ""}
//                 data-bs-placement="bottom"
//                 data-bs-toggle="popover"
//                 data-bs-trigger="focus"
//                 title="${item.title}"
//                 data-bs-content="${item.content}"
//             ></div>
//         `;
//     }

//     code += `
//             </div>
//         </div>

//     `;

//     return code;
// }

function genFooter() {}

function changeLanguage(lang) {
    httpApi.nvramSet(
        {
            action_mode: "apply",
            rc_service: "email_info",
            flag: "set_language",
            preferred_lang: lang,
        },
        function () {
            setTimeout(function () {
                location.reload();
            }, 10);
        }
    );
}

function reboot() {
    if (confirm("<#Main_content_Login_Item7#>")) {
        httpApi.nvramSet({ action_mode: "reboot" }, function () {
            setTimeout(function () {
                location.reload();
            }, rebootTime * 1000);
        });
    }
}

function logout() {
    if (confirm("<#JS_logout#>")) {
        setTimeout('location = "../Logout.asp";', 1);
    }
}

function pageRedirect(target) {
    if (target === "index.asp") {
        location.href = target;
    } else {
        location.href = `/index.html?url=${target}&current_theme=${theme}`;
    }
}
