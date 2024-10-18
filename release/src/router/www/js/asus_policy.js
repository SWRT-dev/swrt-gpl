ASUS_POLICY = {
    Dict: {
        ScrollDown: `<#ASUS_POLICY_Scroll_Down#>`,
        AgeCheck: `<#ASUS_POLICY_Age_Check#>`,
        AgeConfirm: `<#ASUS_eula_age_confirm#>`,
        Agree: `<#CTL_Agree#>`,
        Disagree: `<#CTL_Disagree#>`,
        Notice: `<#Notice#>`,
        Update_Notice: `<#ASUS_POLICY_Update_Notice#>`,
        Notice1: `<#ASUS_POLICY_Notice_1#>`,
        Notice2: `<#ASUS_POLICY_Notice_2#>`,
        ReadAgain: `<#ASUS_POLICY_Read_Again#>`,
        KnowRisk: `<#ASUS_POLICY_Know_Risk#>`,
        QisDesc: `<#ASUS_POLICY_QIS_Desc#>`,
        Ok: `<#CTL_ok#>`,
    },
    Content: {
        PP: {
            Title: `<#ASUS_PP_Title#>`,
            Desc: `<#ASUS_PP_Desc#>`,
            HTML: `<style>
                    .policy_title{
                        font-weight: bold;
                        text-decoration: underline;
                        margin-bottom: 1em;
                    }
                    .policy_desc{
                        font-weight: normal;
                        margin-bottom: 1em;
                    }
                    .policy_end{
                        border: 1px solid #ddd;
                        padding: 5px;
                        margin: 1em;
                    }
                    .policy_law {
                        font-weight: normal;
                    }
                    .policy_law ol {
                        padding-left: 1em;
                    }
                    .policy_law ol ol{
                        padding-left: 2em;
                    }
                    .policy_law li{
                        margin-top: 1em;
                        margin-bottom: 1em;
                    }
                    .policy_law li li::marker{
                        content: '(' counter(list-item) ') ';
                    }
                </style>
                <div>
                    <div class="policy_title"><#ASUS_PP_Title#></u></b></div>
                    <div class="policy_desc"><#ASUS_PP_Desc#></div>
                    <div class="policy_law">
                        <ol>
                            <li><#ASUS_PP_Law_1#>
                                <ol>
                                    <li><#ASUS_PP_Law_1_1#></li>
                                    <li><#ASUS_PP_Law_1_2#><#ASUS_PP_Law_1_3#></li>
                                </ol>
                            </li>
                            <li><#ASUS_PP_Law_2#>
                                <ol>
                                    <li><#ASUS_PP_Law_2_1#></li>
                                    <li><#ASUS_PP_Law_2_2#></li>
                                    <li><#ASUS_PP_Law_2_3#></li>
                                </ol>
                            </li>
                            <li><#ASUS_PP_Law_3#>
                                <ol>
                                    <li><#ASUS_PP_Law_3_1#><#ASUS_PP_Law_3_2#></li>
                                    <li><#ASUS_PP_Law_3_3#></li>
                                </ol>
                            </li>
                            <li><#ASUS_PP_Law_4#></li>
                        </ol>
                        <div class="policy_end"><#ASUS_PP_End#></div>
                    </div>
                </div>`,
        },
        EULA: {
            Title: `<#ASUS_EULA_Title#>`,
            Desc: `<#ASUS_EULA_Desc#>`,
            HTML: `<style>
                    .policy_title{
                        font-weight: bold;
                        text-decoration: underline;
                        margin-bottom: 1em;
                    }
                    .policy_desc{
                        font-weight: normal;
                        margin-bottom: 1em;
                    }
                    .policy_law ol {
                        margin: 0 0.5em 0 0.5em;
                    }
                    .policy_law ol {
                        padding-left: 1em;
                    }
                    .policy_law ol ol{
                        padding-left: 2em;
                    }
                    .policy_law li{
                        margin-top: 1em;
                        margin-bottom: 1em;
                    }
                    .policy_law li li::marker{
                        content: counters(list-item,".",list-item)" ";
                    }
                    .policy_law > ol > li:first-child{
                        font-weight: bold;
                    }
                    .policy_law > ol > li > div:first-child{
                        font-weight: normal;
                    }
                    .policy_law > ol > li li{
                        font-weight: normal;
                    }
                    .font-weight-bold {
                        font-weight: bold;
                    }
                </style>
                <div>
                    <div class="policy_title"><#ASUS_EULA_Title_B#></u></b></div>
                    <div class="policy_desc"><#ASUS_EULA_Desc#></div>
                    <div class="policy_law">
                        <ol>
                            <li><#ASUS_EULA_Law_1#>
                                <ol>
                                    <li><b><#ASUS_EULA_Law_1_1#></b>
                                        <ol>
                                            <li><#ASUS_EULA_Law_1_1_1#></li>
                                            <li><#ASUS_EULA_Law_1_1_2#></li>
                                        </ol>
                                    </li>
                                    <li><b><#ASUS_EULA_Law_1_2#></b>
                                        <ol>
                                            <li><#ASUS_EULA_Law_1_2_1#></li>
                                            <li><#ASUS_EULA_Law_1_2_2#></li>
                                        </ol>
                                    </li>
                                    <li><b><#ASUS_EULA_Law_1_3#></b>
                                        <ol>
                                            <li><#ASUS_EULA_Law_1_3_1#></li>
                                            <li><#ASUS_EULA_Law_1_3_2#></li>
                                            <li><#ASUS_EULA_Law_1_3_3#></li>
                                        </ol>
                                    </li>
                                    <li><b><#ASUS_EULA_Law_1_4#></b>
                                        <div>
                                            <#ASUS_EULA_Law_1_4_Desc#>                                            
                                        </div>
                                    </li>
                                </ol>
                            </li>
                            <li><#ASUS_EULA_Law_2#>
                                <ol>
                                    <li><b><#ASUS_EULA_Law_2_1#></b>
                                        <ol>
                                            <li><#ASUS_EULA_Law_2_1_1#></li>
                                            <li><#ASUS_EULA_Law_2_1_2#></li>
                                        </ol>
                                    </li>
                                    <li><b><#ASUS_EULA_Law_2_2#></b>
                                        <ol>
                                            <li><#ASUS_EULA_Law_2_2_1#></li>
                                            <li><#ASUS_EULA_Law_2_2_2#></li>
                                        </ol>
                                    </li>
                                </ol>
                            </li>
                            <li><#ASUS_EULA_Law_3#>
                                <div><#ASUS_EULA_Law_3_Desc#></div>
                            </li>
                            <li><#ASUS_EULA_Law_4#>
                                <div><#ASUS_EULA_Law_4_Desc#></div>
                            </li>
                            <li><#ASUS_EULA_Law_5#>
                                <div><#ASUS_EULA_Law_5_Desc#></div>
                            </li>
                            <li><#ASUS_EULA_Law_6#>
                                <ol>
                                    <li><#ASUS_EULA_Law_6_1#></li>
                                    <li><#ASUS_EULA_Law_6_2#></li>
                                </ol>
                            </li>
                            <li><#ASUS_EULA_Law_7#>
                                <div><#ASUS_EULA_Law_7_Desc#></div>
                            </li>
                            <li><#ASUS_EULA_Law_8#>
                                <ol>
                                    <li><b><#ASUS_EULA_Law_8_1#></b>
                                        <ol>
                                            <li><#ASUS_EULA_Law_8_1_1#></li>
                                            <li><#ASUS_EULA_Law_8_1_2#></li>
                                        </ol>
                                    </li>
                                    <li><b><#ASUS_EULA_Law_8_2#></b>
                                        <div><#ASUS_EULA_Law_8_2_Desc#></div>
                                    </li>
                                    <li><#ASUS_EULA_Law_8_3#></li>
                                </ol>
                            </li>
                            <li><#ASUS_EULA_Law_9#>
                                <div><#ASUS_EULA_Law_9_Desc#></div>
                            </li>
                            <li><#ASUS_EULA_Law_10#>
                                <div><#ASUS_EULA_Law_10_Desc#></div>
                            </li>
                            <li><#ASUS_EULA_Law_11#>
                                <div><#ASUS_EULA_Law_11_Desc#></div>
                            </li>
                            <li><#ASUS_EULA_Law_12#>
                                <div><#ASUS_EULA_Law_12_Desc#></div>
                            </li>
                        </ol>
                    </div>
                </div>`,
        },
        TM: {
            Title: `<#lyra_TrendMicro_agreement#>`,
            HTML: `<div class="d-flex flex-column gap-1">
                        <div><#TM_eula_desc1#></div>
                        <div><#TM_eula_desc2#></div>
                        <div><#TM_privacy_policy#></div>
                        <div><#TM_data_collection#></div>
                        <div><#TM_eula_desc3#></div>
                    </div>`
        },
        THIRDPARTY_PP: {
            Title: `<#Notice#>`,
            HTML: `<div class="d-flex flex-column gap-1">
                        <div class="thirdparty-pp-desc1"><#Thirdparty_PP_Desc1#></div>
                        <div class="thirdparty-pp-desc2"><#Thirdparty_PP_Desc2#></div>
                        <div class="thirdparty-pp-desc3"><#Thirdparty_PP_Desc3#></div>
                    </div>`,
            Feature: {
                'Speedtest': {
                    url_text: `Ookla PRIVACY POLICY`,
                    url: `https://www.speedtest.net/about/privacy`,
                    company: `Ookla`
                },
                'WTFast': {
                    url_text: `AAA Internet Publishing Inc. PRIVACY POLICY`,
                    url: `https://www.wtfast.com/en/privacy-policy/`,
                    company: `AAA Internet Publishing Inc.`
                },
                '网易UU加速器': {
                    url_text: `网易游戏隐私政策`,
                    url: `https://unisdk.update.netease.com/html/latest_v90.html`,
                    company: `网易游戏隐私政策`
                },
                'Surfshark': {
                    url_text: `Surfshark B.V. PRIVACY POLICY`,
                    url: `https://surfshark.com/privacy`,
                    company: `Surfshark B.V`
                },
                'Alexa': {
                    url_text: `Alexa Terms of Use`,
                    url: `https://us.amazon.com/gp/help/customer/display.html?ref_=hp_left_v4_sib&nodeId=G201809740`,
                    company: `Amazon.com Services LLC`
                },
                'IFTTT': {
                    url_text: `IFTTT INC. PRIVACY POLICY`,
                    url: `https://ifttt.com/terms`,
                    company: `IFTTT INC.`
                },
                'Google Assistant': {
                    url_text: `Google LLC PRIVACY POLICY`,
                    url: `https://policies.google.com/privacy`,
                    company: `GOOGLE LLC`
                },
                'FileFlex': {
                    url_text: `Qnext PRIVACY POLICY`,
                    url: `https://fileflex.com/privacy-policy/`,
                    company: `Qnext through FileFlex`
                }
            }
        }
    },

    PolicyModalStyle: `<style>
        .popup_bg {
            position: fixed;
            top: 0;
            right: 0;
            bottom: 0;
            left: 0;
            z-index: 2000;
            backdrop-filter: blur(2px);
        }
        .modal {
            position: fixed;
            top: 0;
            left: 0;
            z-index: 1060;
            display: block;
            width: 100%;
            height: 100%;
            overflow-x: hidden;
            overflow-y: auto;
            outline: 0;
        }
        .modal-dialog {
            position: relative;
            width: auto;
            margin: 0.5rem;
            pointer-events: none;
        }
        .modal-content {
            position: relative;
            display: -webkit-box;
            display: -ms-flexbox;
            display: flex;
            -webkit-box-orient: vertical;
            -webkit-box-direction: normal;
            -ms-flex-direction: column;
            flex-direction: column;
            width: 100%;
            pointer-events: auto;
            background-color: #fff;
            background-clip: padding-box;
            border: 1px solid rgba(0, 0, 0, .2);
            border-radius: 0.3rem;
            outline: 0;
        }
        .modal-header {
            display: -webkit-box;
            display: -ms-flexbox;
            display: flex;
            -webkit-box-align: start;
            -ms-flex-align: start;
            align-items: flex-start;
            -webkit-box-pack: justify;
            -ms-flex-pack: justify;
            justify-content: space-between;
            padding: 1rem;
            border-top-left-radius: 0.3rem;
            border-top-right-radius: 0.3rem;
        }
        .modal-title {
            color: #006ce1;
            font-weight: bold;
            font-size: 2em;
            margin-bottom: 0;
            line-height: 1.5;
        }
        .modal-body {
            position: relative;
            -webkit-box-flex: 1;
            -ms-flex: 1 1 auto;
            flex: 1 1 auto;
            padding: 1rem;
        }
        .modal-footer {
            display: -webkit-box;
            display: -ms-flexbox;
            display: flex;
            -webkit-box-align: center;
            -ms-flex-align: center;
            align-items: center;
            justify-content: space-evenly;
            padding: 1rem;
        }
        .policy-scroll-div {
            margin: 5px;
            overflow-y: auto;
            padding: 15px 12px 0 12px;
            border: 1px solid #ddd;
            border-radius: 5px;
            height: 30em;
            color: #000000;
        }
        
        .policy-scroll-div::-webkit-scrollbar {
            width: 10px;
        }
        
        .policy-scroll-div::-webkit-scrollbar-thumb {
            background-color: #7775;
            border-radius: 20px;
        }
        
        .policy-scroll-div::-webkit-scrollbar-thumb:hover {
            background-color: #7777;
        }
        
        .policy-scroll-div::-webkit-scrollbar-track:hover {
            background-color: #5555;
        }
        
        .policy-scroll-div a{
            color: #000000;
        }
        .btn {
            display: inline-block;
            font-weight: 400;
            text-align: center;
            white-space: nowrap;
            vertical-align: middle;
            -webkit-user-select: none;
            -moz-user-select: none;
            -ms-user-select: none;
            user-select: none;
            border: 1px solid transparent;
            font-size: 1rem;
            line-height: 1.5;
            transition: color .15s ease-in-out, background-color .15s ease-in-out, border-color .15s ease-in-out, box-shadow .15s ease-in-out;
            padding: 0.5rem 1rem;
            border-radius: 0.3rem;
        }
        .btn:hover {
            cursor: pointer;
        }
        .btn.disabled, .btn:disabled {
            opacity: .65;
            cursor: not-allowed;
        }
        .btn-block {
            display: block;
            width: 100%;
        }
        .btn-primary {
            color: #fff;
            background-color: #007bff;
            border-color: #007bff;
        }
        .btn-primary:hover {
            color: #fff;
            background-color: #0069d9;
            border-color: #0062cc;
        }
        .btn-primary.disabled, .btn-primary:disabled {
            color: #fff;
            background-color: #007bff;
            border-color: #007bff;
        }
        .btn-secondary {
            color: #fff;
            background-color: #6c757d;
            border-color: #6c757d;
        }
        .btn-secondary:hover {
            color: #fff;
            background-color: #5a6268;
            border-color: #545b62;
        }
        .btn-secondary.disabled, .btn-secondary:disabled {
            color: #fff;
            background-color: #6c757d;
            border-color: #6c757d;
        }
        .scroll-info {
            font-weight: bold;
            color: #ff5722;
            padding: 5px 10px;
            border-radius: 5px;
            margin: 1em;
            text-align: center;
        }
        .d-flex {
            display: flex !important;
        }
        .flex-column {
            -webkit-box-orient: vertical !important;
            -webkit-box-direction: normal !important;
            -ms-flex-direction: column !important;
            flex-direction: column !important;
        }
        .gap-1 {
            gap: 1em;
        }
        .notice_title {
            display: flex !important;
            justify-content: center !important;
            font-size: 1.5em;
            color: #0A5EDB;
            font-weight: bold;
        }
        
        .notice_content {
            color: #000000;
        }
    
        .gg-spinner {
            transform: scale(1)
        }
    
        .gg-spinner,
        .gg-spinner::after,
        .gg-spinner::before {
            box-sizing: border-box;
            position: relative;
            display: block;
            width: 20px;
            height: 20px
        }
    
        .gg-spinner::after,
        .gg-spinner::before {
            content: "";
            position: absolute;
            border-radius: 100px
        }
    
        .gg-spinner::before {
            animation: spinner 1s cubic-bezier(.6, 0, .4, 1) infinite;
            border: 3px solid transparent;
            border-top-color: currentColor
        }
    
        .gg-spinner::after {
            border: 3px solid;
            opacity: .2
        }
    
        @keyframes spinner {
            0% {
                transform: rotate(0deg)
            }
            to {
                transform: rotate(359deg)
            }
        }
        
        .age-label {
            color: #000000;            
        }
        
        .speedtest_logo {
            height: 40px;
            background: url(../images/speedtest/speedtest.svg) no-repeat center;
        }
    
        @media (min-width: 576px) {
            .modal-dialog {
                max-width: 500px;
                margin: 5rem auto;
            }
        }
      </style>`,

    ModalTheme: {
        ROG: `<style>
            .modal-content {
                background-color: #000000e6;
                border: 1px solid rgb(161, 10, 16);
            }
            .modal-title {
                color: #cf0a2c;
            }
            .policy-scroll-div,
            .policy-scroll-div a{
                color: #FFFFFF;
            }
            .btn-primary {
                background-color: #91071f;
                border: 0;
            }
            .btn-primary.disabled, .btn-primary:disabled {
                color: #fff;
                background-color: #91071f;
            }
            .btn-primary:hover {
                background-color: #cf0a2c;
            }
            .notice_title {
                color: #cf0a2c;
            }
            .notice_content {
                color: #FFFFFF;
            }
            .age-label {
                color: #FFFFFF;            
            }
        </style>`,

        RT: `<style>
            .modal-content {
                background-color: #2B373B;
            }
            .modal-title {
                color: #FFFFFF;
            }
            .policy-scroll-div,
            .policy-scroll-div a{
                color: #FFFFFF;
            }
            .notice_title {
                color: #FFFFFF;
            }
            .notice_content {
                color: #FFFFFF;
            }
            .age-label {
                color: #FFFFFF;            
            }
        </style>`,

        TUF: `<style>
            .modal-content {
                background-color: #000000e6;
                border: 1px solid #92650F;
            }
            .modal-title {
                color: #ffa523;
            }
            .policy-scroll-div,
            .policy-scroll-div a{
                color: #FFFFFF;
            }
            .btn-primary {
                background-color: #ffa523;
                border: 0;
            }
            .btn-primary.disabled, .btn-primary:disabled {
                color: #fff;
                background-color: #ffa523;
            }
            .btn-primary:hover {
                background-color: #D0982C;
            }
            .notice_title {
                color: #ffa523;
            }
            .notice_content {
                color: #FFFFFF;
            }
            .age-label {
                color: #FFFFFF;            
            }
        </style>`
    },

    QisTheme: {
        ROG: `<style>
            .policy-scroll-div,
            .policy-scroll-div a {
                color: #FFFFFF;
            }
            .footer {
                border-top: 3px solid rgb(161, 10, 16);
            }

            #ppAbort{
                color: rgb(161, 10, 16);
            }

            .ppBtn{
                background: rgb(161, 10, 16);
            }

            @media screen and (min-aspect-ratio: 11/13){
                .footer{
                    border: 0;
                    background: unset;
                }
                #ppAbort{
                    border: 2px solid rgb(161, 10, 16);
                }
            }
        </style>`,

        RT: `<style>
            .policy-scroll-div,
            .policy-scroll-div a {
                color: #FFFFFF;
            }
        </style>`,

        TUF: `<style>
            .policy-scroll-div,
            .policy-scroll-div a {
                color: #FFFFFF;
            }
            .footer {
                border-top: 3px solid #ffa523;
            }

            #ppAbort{
                color: #ffa523;
            }

            .ppBtn{
                background: #ffa523;
            }

            @media screen and (min-aspect-ratio: 11/13){
                .footer{
                    border: 0;
                    background: unset;
                }
                #ppAbort{
                    border: 2px solid #ffa523;
                }
            }
        </style>`
    },

    getTheme: () => {
        let ui_support = httpApi.hookGet("get_ui_support");

        function isSupport(_ptn) {
            return ui_support[_ptn] ? true : false;
        }

        let theme = 'RT';
        if (isSupport("rog")) {
            return "ROG";
        } else if (isSupport("tuf")) {
            return "TUF";
        } else if (isSupport("BUSINESS")) {
            return "";
        } else {
            return theme;
        }
    }
}

function areScriptsLoaded(scriptUrls) {
    const scripts = document.getElementsByTagName('script');
    for (let i = 0; i < scriptUrls.length; i++) {
        const scriptUrl = scriptUrls[i];
        let isLoaded = false;
        for (let j = 0; j < scripts.length; j++) {
            if (scripts[j].src.indexOf(scriptUrl) > 0) {
                isLoaded = true;
                break;
            }
        }
        if (!isLoaded) {
            return false;
        }
    }
    return true;
}

const dependencies = [
    'httpApi.js',
];

if (!areScriptsLoaded(dependencies)) {
    console.log('Dependencies did not load');
}

policy_status = {
    "Policy_lang": "EN",
    "EULA": "0",
    "EULA_time": "",
    "PP": "0",
    "PP_time": "",
    "TM": "0",
    "TM_time": "",
};

async function PolicyStatus() {
    await httpApi.newEula.get()
        .then(data => {
            policy_status.EULA = data.ASUS_NEW_EULA;
            policy_status.EULA_time = data.ASUS_NEW_EULA_time;
        });

    await httpApi.privateEula.get()
        .then(data => {
            policy_status.PP = data.ASUS_PP;
            policy_status.PP_time = data.ASUS_PP_time;
        })

    const TM_EULA = await httpApi.nvramGet(['TM_EULA', 'TM_EULA_time'], true);
    policy_status.TM = TM_EULA.TM_EULA;
    policy_status.TM_time = TM_EULA.TM_EULA_time;

    policy_status.Policy_lang = await httpApi.nvramGet(['preferred_lang'], true).preferred_lang;
}

PolicyStatus();


class PolicyScrollDiv extends HTMLElement {
    constructor() {
        super();
        this.attachShadow({mode: 'open'});

        const template = document.createElement('template');
        template.innerHTML = `
      <style>
        .policy-scroll-div {
            margin: 5px;
            overflow-y: auto;
            padding: 15px 12px 0 12px;
            border: 1px solid #ddd;
            border-radius: 5px;
            height: 50vh;
            color: #333;
        }
        .policy-scroll-div a {
            color: #333;
        }
      </style>
      <div class="policy-scroll-div"><slot></slot></div>
    `;
        const theme = ASUS_POLICY.getTheme();
        if (theme !== "") {
            template.innerHTML += ASUS_POLICY.QisTheme[theme];
        }

        this.shadowRoot.appendChild(template.content.cloneNode(true));
        this.div = this.shadowRoot.querySelector('div');
        this.div.addEventListener('click', this.handleClick.bind(this));
    }

    connectedCallback() {
        const {policy} = this.attributes;

        if (policy) {
            this.div.innerHTML = ASUS_POLICY.Content[policy.value].HTML;
        }
    }

    handleClick() {

        const scrollDiv = this.div;
        let currentScrollTop = scrollDiv.scrollTop;
        const targetScrollTop = currentScrollTop + scrollDiv.offsetHeight;
        const animationDuration = 1000;
        const frameDuration = 15;
        const scrollDistancePerFrame = (targetScrollTop - currentScrollTop) / (animationDuration / frameDuration);

        function animateScroll() {
            currentScrollTop += scrollDistancePerFrame;
            scrollDiv.scrollTop = currentScrollTop;

            if (currentScrollTop < targetScrollTop) {
                requestAnimationFrame(animateScroll);
            }
        }

        animateScroll();
    }
}

class PolicyPopupBg extends HTMLElement {
    constructor() {
        super();
        this.attachShadow({mode: 'open'});

        const template = document.createElement('template');
        template.innerHTML = `
      <style>
        .policy_popup_pg {
            position: absolute;
            top: 0;
            left: 0;
            height: 100vh;
            width: 100vw;
            z-index: 9;
            backdrop-filter: blur(2px);
        }
      </style>
      <div class="policy_popup_pg"><slot></slot></div>
    `;

        this.shadowRoot.appendChild(template.content.cloneNode(true));
        this.div = this.shadowRoot.querySelector('div');
    }

    connectedCallback() {


    }
}

class PolicyModal extends HTMLElement {
    constructor() {
        super();
        this.attachShadow({mode: 'open'});

        const template = document.createElement('template');
        template.innerHTML = `
          ${ASUS_POLICY.PolicyModalStyle}
          <div class="popup_bg">
            <div class="modal">
                <div class="modal-dialog">
                    <div class="modal-content">
                        <div class="modal-header">
                            <div class="modal-title"></div>
                        </div>
                        <div class="modal-body">
                            <div class="policy-scroll-div"></div>
                        </div>
                        <div class="modal-footer">
                        </div>
                    </div>
                </div>
            </div>
          </div>
        `;

        const theme = ASUS_POLICY.getTheme();
        if (theme !== "") {
            template.innerHTML += ASUS_POLICY.ModalTheme[theme];
        }

        this.shadowRoot.appendChild(template.content.cloneNode(true));
        this.div = this.shadowRoot.querySelector('div');
        this.div.querySelector('div.policy-scroll-div').addEventListener('click', this.handleScrollDivClick.bind(this.div.querySelector('div.policy-scroll-div')));
        this.div.querySelector('div.policy-scroll-div').addEventListener('scroll', this.handleScrollCheck.bind(this));

        this.ageCheckbox = document.createElement('input');
        this.ageCheckbox.type = 'checkbox';
        this.ageCheckbox.id = 'ASUS_EULA_enable';

        this.ageLabel = document.createElement('label');
        this.ageLabel.classList.add('age-label')
        this.ageLabel.setAttribute('for', 'ASUS_EULA_enable');
        this.ageLabel.textContent = ASUS_POLICY.Dict.AgeCheck;

        this.ageCheckDiv = document.createElement('div');
        this.ageCheckDiv.style.width = '100%';
        this.ageCheckDiv.appendChild(this.ageCheckbox);
        this.ageCheckDiv.appendChild(this.ageLabel);

        this.loadingImg = document.createElement('i');
        this.loadingImg.className = 'gg-spinner';

        this.closeBtn = document.createElement('button');
        this.closeBtn.type = 'button';
        this.closeBtn.className = 'btn btn-primary btn-block close';
        this.closeBtn.innerHTML = ASUS_POLICY.Dict.Ok;
        this.closeBtn.addEventListener('click', this.handleClickClose.bind(this));

        this.agreeBtn = document.createElement('button');
        this.agreeBtn.type = 'button';
        this.agreeBtn.className = 'btn btn-primary agree disabled';
        this.agreeBtn.innerHTML = ASUS_POLICY.Dict.Agree;
        this.agreeBtn.addEventListener('click', this.handleClickAgree.bind(this));


        this.disagreeBtn = document.createElement('button');
        this.disagreeBtn.type = 'button';
        this.disagreeBtn.className = 'btn btn-secondary disagree disabled';
        this.disagreeBtn.innerHTML = ASUS_POLICY.Dict.Disagree;
        this.disagreeBtn.addEventListener('click', this.handleClickDisagree.bind(this));

    }

    agreeCallback = () => {
    };
    disagreeCallback = () => {
    };

    setAgreeCallback(callback) {
        if (typeof callback === "function") {
            this.agreeCallback = callback;
        }
    }

    setDisagreeCallback(callback) {
        if (typeof callback === "function") {
            this.disagreeCallback = callback;
        }
    }

    readAgainCallback = () => {
    };
    knowRiskCallback = () => {
    };

    setReadAgainCallback(callback) {
        if (typeof callback === "function") {
            this.readAgainCallback = callback;
        }
    }

    setKnowRiskCallback(callback) {
        if (typeof callback === "function") {
            this.knowRiskCallback = callback;
        }
    }

    policy_type = "";
    submit_reload = "0";

    connectedCallback() {
        const {policy, submit_reload, party} = this.attributes;

        if (policy) {
            this.policy_type = policy.value;

            this.div.querySelector('div.modal-title').innerHTML = ASUS_POLICY.Content[policy.value].Title;
            this.div.querySelector('div.policy-scroll-div').innerHTML = ASUS_POLICY.Content[policy.value].HTML;


            switch (policy.value) {
                case "EULA":
                    if (policy_status.EULA === "0") {
                        const scrollInfoDiv = document.createElement('div');
                        scrollInfoDiv.className = "scroll-info";
                        scrollInfoDiv.innerHTML = ASUS_POLICY.Dict.ScrollDown;

                        this.div.querySelector('div.modal-body').appendChild(scrollInfoDiv);
                        this.div.querySelector('div.modal-body').appendChild(this.ageCheckDiv);
                        this.div.querySelector('div.modal-footer').appendChild(this.agreeBtn);
                    } else {
                        this.div.querySelector('div.modal-footer').appendChild(this.closeBtn);
                    }
                    break;
                case "PP":
                    if (policy_status.PP < "2" || policy_status.PP_time === "") {
                        const scrollInfoDiv = document.createElement('div');
                        scrollInfoDiv.className = "scroll-info";
                        scrollInfoDiv.innerHTML = ASUS_POLICY.Dict.ScrollDown;

                        this.div.querySelector('div.modal-body').appendChild(scrollInfoDiv);
                        this.div.querySelector('div.modal-footer').appendChild(this.disagreeBtn);
                        this.div.querySelector('div.modal-footer').appendChild(this.agreeBtn);
                    } else {
                        this.div.querySelector('div.modal-footer').appendChild(this.closeBtn);
                    }
                    break;
                case "TM":
                    this.div.querySelector('div.policy-scroll-div').querySelector('#tm_eula_url').setAttribute("href", "https://nw-dlcdnet.asus.com/trend/tm_privacy");
                    this.div.querySelector('div.policy-scroll-div').querySelector('#tm_disclosure_url').setAttribute("href", "https://nw-dlcdnet.asus.com/trend/tm_pdcd");
                    this.div.querySelector('div.policy-scroll-div').querySelector('#eula_url').setAttribute("href", `https://nw-dlcdnet.asus.com/support/forward.html?model=&type=TMeula&lang=${policy_status.Policy_lang}&kw=&num=`);
                    if (policy_status.TM === "0" || policy_status.TM_time === "") {
                        this.agreeBtn.classList.remove("disabled");
                        this.disagreeBtn.classList.remove("disabled");
                        this.div.querySelector('div.modal-footer').appendChild(this.disagreeBtn);
                        this.div.querySelector('div.modal-footer').appendChild(this.agreeBtn);
                    } else {
                        this.div.querySelector('div.modal-footer').appendChild(this.closeBtn);
                    }
                    break;
                case "THIRDPARTY_PP":
                    this.div.querySelector('div.policy-scroll-div .thirdparty-pp-desc1').innerHTML = this.div.querySelector('div.policy-scroll-div .thirdparty-pp-desc1').innerHTML.replace("%1$@", party.value).replace("[aa]%2$@[/aa]", `<a target="_blank" href="${ASUS_POLICY.Content[policy.value].Feature[party.value].url}" style="text-decoration: underline;cursor: pointer;">${ASUS_POLICY.Content[policy.value].Feature[party.value].url_text}</a>`);
                    this.div.querySelector('div.policy-scroll-div .thirdparty-pp-desc2').innerHTML = this.div.querySelector('div.policy-scroll-div .thirdparty-pp-desc2').innerHTML.replaceAll("%1$@", `${ASUS_POLICY.Content[policy.value].Feature[party.value].company}`).replaceAll("%2$@", party.value);
                    this.div.querySelector('div.policy-scroll-div .thirdparty-pp-desc3').innerHTML = this.div.querySelector('div.policy-scroll-div .thirdparty-pp-desc3').innerHTML.replaceAll("%1$@", `${ASUS_POLICY.Content[policy.value].Feature[party.value].company}`).replaceAll("%2$@", party.value);
                    this.div.querySelector('div.modal-footer').appendChild(this.closeBtn);
                    if (party.value == 'Speedtest') {
                        const speedtestDesc = document.createElement('div');
                        speedtestDesc.innerHTML = `<#InternetSpeed_desc#>`;
                        const referenceElement = this.div.querySelector('div.policy-scroll-div .thirdparty-pp-desc1');
                        referenceElement.parentNode.insertBefore(speedtestDesc, referenceElement);
                        const speedtestLogo = document.createElement('div');
                        speedtestLogo.innerHTML = `<div class="speedtest_logo"></div>`;
                        this.div.querySelector('div.policy-scroll-div').appendChild(speedtestLogo);
                    }
                    break;
            }
        }
        if (submit_reload) {
            this.submit_reload = submit_reload.value;
        }
    }

    enableBtn = function (btn, boolean) {
        if (btn !== null) {
            if (boolean) {
                btn.classList.remove("disabled");
            } else {
                btn.classList.add("disabled");
            }
        }
    }

    handleScrollDivClick() {
        const scrollDiv = this;
        let currentScrollTop = scrollDiv.scrollTop;
        const targetScrollTop = currentScrollTop + scrollDiv.offsetHeight;
        const animationDuration = 1000;
        const frameDuration = 15;
        const scrollDistancePerFrame = (targetScrollTop - currentScrollTop) / (animationDuration / frameDuration);

        function animateScroll() {
            currentScrollTop += scrollDistancePerFrame;
            scrollDiv.scrollTop = currentScrollTop;

            if (currentScrollTop < targetScrollTop) {
                requestAnimationFrame(animateScroll);
            }
        }

        animateScroll();
    }

    handleScrollCheck() {
        const scrollDiv = this.div.querySelector('div.policy-scroll-div');
        const scrollTop = scrollDiv.scrollTop;
        const offsetHeight = scrollDiv.offsetHeight;
        const scrollHeight = scrollDiv.scrollHeight;
        if ((scrollTop + offsetHeight) >= scrollHeight) {
            this.enableBtn(this.agreeBtn, true);
            this.enableBtn(this.disagreeBtn, true);
        }
    }

    handleClickClose() {
        top.document.body.style.removeProperty('overflow');
        this.closest('#policy_popup_modal').remove();
    }

    handleClickAgree(e) {
        if (!e.target.classList.contains("disabled")) {
            switch (this.policy_type) {
                case "EULA":
                    let yearChecked = this.ageCheckbox.checked;
                    if (!yearChecked) {
                        alert(ASUS_POLICY.Dict.AgeConfirm);
                        this.ageLabel.style.color = top.businessWrapper ? "red" : "#ff5722";
                        return false;
                    } else {
                        this.agreeCallback();
                        this.agreeBtn.innerHTML = '';
                        this.agreeBtn.appendChild(this.loadingImg);
                        httpApi.newEula.set(1, () => {
                            top.document.body.style.removeProperty('overflow');
                            this.remove();
                            if (this.submit_reload == 1) {
                                top.window.location.reload();
                            }
                        })
                    }
                    break
                case "PP":
                    this.agreeCallback();
                    this.agreeBtn.innerHTML = '';
                    this.agreeBtn.appendChild(this.loadingImg);
                    httpApi.privateEula.set("1", () => {
                        top.document.body.style.removeProperty('overflow');
                        this.remove();
                        if (this.submit_reload == 1) {
                            top.window.location.reload();
                        }
                    })
                    break
                case "TM":
                    this.agreeCallback();
                    this.agreeBtn.innerHTML = '';
                    this.agreeBtn.appendChild(this.loadingImg);
                    httpApi.enableEula('tm', "1", () => {
                        top.document.body.style.removeProperty('overflow');
                        this.remove();
                        if (this.submit_reload == 1) {
                            top.window.location.reload();
                        }
                    })
            }
        }
    }

    handleClickDisagree(e) {
        if (!e.target.classList.contains("disabled")) {
            switch (this.policy_type) {
                case "PP":
                    const parentNode = this.parentNode;
                    parentNode.removeChild(this);
                    this.disagreeCallback();
                    const policyModal = new PolicyWithdrawModalComponent({
                        policy: "PP",
                        submit_reload: this.submit_reload,
                        agreeCallback: this.agreeCallback,
                        disagreeCallback: this.disagreeCallback,
                        readAgainCallback: this.readAgainCallback,
                        knowRiskCallback: this.knowRiskCallback
                    });
                    parentNode.appendChild(policyModal.render());
                    break
                case "TM":
                    this.disagreeCallback();
                    httpApi.enableEula("tm", "0", () => {
                        top.document.body.style.removeProperty('overflow');
                        this.remove();
                        if (this.submit_reload == 1) {
                            top.window.location.reload();
                        }
                    })
                    break
            }
        }
    }
}

class DisagreeNoticeComponent {
    constructor(props) {
        this.props = props;
        this.element = document.createElement('div');
        this.element.innerHTML = `
            ${ASUS_POLICY.PolicyModalStyle}
            <div class="d-flex flex-column gap-1">
                <div class="notice_title">${ASUS_POLICY.Dict.Notice}</div>
                <div class="d-flex flex-column gap-1" style="margin: 0 10px;">
                    <div class="notice_content">${ASUS_POLICY.Dict.Notice1}</div>
                    <div style="color: #ff5722">${ASUS_POLICY.Dict.Notice2}</div>
                    <div style="color: #ff5722">
                        <ul>
                            <li>Account Binding</li>
                            <li>Config transfer</li>
                            <li>DDNS</li>
                            <li>Alexa</li>
                            <li>IFTTT</li>
                            <li>Google Assistant</li>
                            <li>Remote Connection</li>
                            <li>Notification</li>
                        </ul>
                    </div>
                </div>
            </div>`;

        const theme = ASUS_POLICY.getTheme();
        if (theme !== "") {
            this.element.innerHTML += ASUS_POLICY.ModalTheme[theme];
        }
    }

    render() {
        return this.element;
    }
}

class PolicyWithdrawModal extends HTMLElement {
    constructor() {
        super();
        this.attachShadow({mode: 'open'});

        const template = document.createElement('template');
        template.innerHTML = `
          ${ASUS_POLICY.PolicyModalStyle}
          <div class="popup_bg">
            <div class="modal">
                <div class="modal-dialog">
                    <div class="modal-content">
                        <div class="modal-body">
                        </div>
                        <div class="modal-footer">
                            <button type="button" class="btn btn-secondary know-risk">${ASUS_POLICY.Dict.KnowRisk}</button>
                            <button type="button" class="btn btn-primary read-again">${ASUS_POLICY.Dict.ReadAgain}</button>
                        </div>
                    </div>
                </div>
            </div>
          </div>
        `;

        const theme = ASUS_POLICY.getTheme();
        if (theme !== "") {
            template.innerHTML += ASUS_POLICY.ModalTheme[theme];
        }

        this.loadingImg = document.createElement('i');
        this.loadingImg.className = 'gg-spinner';

        this.shadowRoot.appendChild(template.content.cloneNode(true));
        this.div = this.shadowRoot.querySelector('div');
        this.div.querySelector('button.know-risk').addEventListener('click', this.handleClickKnowRisk.bind(this));
        this.div.querySelector('button.read-again').addEventListener('click', this.handleClickReadAgain.bind(this));
    }

    submit_reload = '';

    agreeCallback = () => {
    };
    disagreeCallback = () => {
    };

    setAgreeCallback(callback) {
        if (typeof callback === "function") {
            this.agreeCallback = callback;
        }
    }

    setDisagreeCallback(callback) {
        if (typeof callback === "function") {
            this.disagreeCallback = callback;
        }
    }

    readAgainCallback = () => {
    };
    knowRiskCallback = () => {
    };

    setReadAgainCallback(callback) {
        if (typeof callback === "function") {
            this.readAgainCallback = callback;
        }
    }

    setKnowRiskCallback(callback) {
        if (typeof callback === "function") {
            this.knowRiskCallback = callback;
        }
    }

    connectedCallback() {
        const {submit_reload = '0'} = this.attributes;
        const disagreeNoticeComponent = new DisagreeNoticeComponent();
        this.div.querySelector('div.modal-body').appendChild(disagreeNoticeComponent.render());

        if (submit_reload) {
            this.submit_reload = submit_reload.value;
        }
    }

    handleClickKnowRisk(e) {
        e.target.innerHTML = '';
        e.target.appendChild(this.loadingImg);
        this.knowRiskCallback();
        httpApi.privateEula.set("0", () => {
            top.document.body.style.removeProperty('overflow')
            this.remove();
            if (this.submit_reload == 1) {
                top.window.location.reload();
            }
        })
    }

    handleClickReadAgain() {
        const parentNode = this.parentNode;
        this.remove();
        this.readAgainCallback();
        if (window.location.pathname.toUpperCase().search("QIS_") < 0) {
            const policyModal = new PolicyModalComponent({
                policy: "PP",
                submit_reload: this.submit_reload,
                agreeCallback: this.agreeCallback,
                disagreeCallback: this.disagreeCallback,
                readAgainCallback: this.readAgainCallback,
                knowRiskCallback: this.knowRiskCallback
            });
            parentNode.appendChild(policyModal.render());
        }
    }
}

class PolicyUpdateModal extends HTMLElement {
    constructor() {
        super();
        this.attachShadow({mode: 'open'});

        const template = document.createElement('template');
        template.innerHTML = `
          ${ASUS_POLICY.PolicyModalStyle}
          <div class="popup_bg">
            <div class="modal">
                <div class="modal-dialog">
                    <div class="modal-content">
                        <div class="modal-body">
                            <div class="d-flex flex-column gap-1">
                                <div class="notice_title">${ASUS_POLICY.Dict.Notice}</div>
                                <div class="notice_content">${ASUS_POLICY.Dict.Update_Notice}</div>
                            </div>
                        </div>
                        <div class="modal-footer">
                            <button type="button" class="btn btn-primary ok">${ASUS_POLICY.Dict.Ok}</button>
                        </div>
                    </div>
                </div>
            </div>
          </div>
        `;
        const theme = ASUS_POLICY.getTheme();
        if (theme !== "") {
            template.innerHTML += ASUS_POLICY.ModalTheme[theme];
        }

        this.shadowRoot.appendChild(template.content.cloneNode(true));
        this.div = this.shadowRoot.querySelector('div');
        this.div.querySelector('button.ok').addEventListener('click', this.handleClickReadEula.bind(this));
    }

    connectedCallback() {

    }

    handleClickReadEula() {
        const parentNode = this.parentNode;
        this.remove();
        const policyModal = document.createElement('policy-modal');
        policyModal.setAttribute('policy', 'EULA');
        policyModal.setAttribute('submit_reload', '1');
        policyModal.style.display = 'block';
        parentNode.appendChild(policyModal);
    }
}

customElements.define('policy-scroll-div', PolicyScrollDiv);
customElements.define('policy-popup-bg', PolicyPopupBg);
customElements.define('policy-modal', PolicyModal);
customElements.define('policy-withdraw-modal', PolicyWithdrawModal);
customElements.define('policy-update-modal', PolicyUpdateModal);

class PolicyModalComponent {
    constructor(props) {
        this.props = props;

        const policy_modal = document.createElement('policy-modal');
        policy_modal.setAttribute('policy', this.props.policy);
        policy_modal.setAttribute('submit_reload', this.props.submit_reload);
        this.element = policy_modal;

        this.element.setAgreeCallback(this.props.agreeCallback);
        this.element.setDisagreeCallback(this.props.disagreeCallback);
        this.element.setReadAgainCallback(this.props.readAgainCallback);
        this.element.setKnowRiskCallback(this.props.knowRiskCallback);
        top.document.body.style.overflow = 'hidden';
    }

    render() {
        return this.element;
    }

    show() {
        if (top.document.getElementById('policy_popup_modal') == null) {
            const policy_popup_modal = document.createElement('div');
            policy_popup_modal.id = 'policy_popup_modal';
            top.document.body.appendChild(policy_popup_modal);
            policy_popup_modal.appendChild(this.element);
        }
    }
}

class PolicyWithdrawModalComponent {
    constructor(props) {
        this.props = props;

        const policy_modal = document.createElement('policy-withdraw-modal');
        policy_modal.setAttribute('policy', this.props.policy);
        policy_modal.setAttribute('submit_reload', this.props.submit_reload);
        this.element = policy_modal;

        this.element.setAgreeCallback(this.props.agreeCallback);
        this.element.setDisagreeCallback(this.props.disagreeCallback);
        this.element.setReadAgainCallback(this.props.readAgainCallback);
        this.element.setKnowRiskCallback(this.props.knowRiskCallback);
        top.document.body.style.overflow = 'hidden';
    }

    render() {
        return this.element;
    }

    show() {
        if (top.document.getElementById('policy_popup_modal') == null) {
            const policy_popup_modal = document.createElement('div');
            policy_popup_modal.id = 'policy_popup_modal';
            top.document.body.appendChild(policy_popup_modal);
            policy_popup_modal.appendChild(this.element);
        }
    }
}

class PolicyUpdateModalComponent {
    constructor(props) {
        this.props = props;
        this.element = document.createElement('policy-update-modal');
        top.document.body.style.overflow = 'hidden';
    }

    render() {
        return this.element;
    }

    show() {
        if (top.document.getElementById('policy_popup_modal') == null) {
            const policy_popup_modal = document.createElement('div');
            policy_popup_modal.id = 'policy_popup_modal';
            top.document.body.appendChild(policy_popup_modal);
            policy_popup_modal.appendChild(this.element);
        }
    }
}

class QisPolicyComponent {

    constructor(props) {

        this.props = props;

        const {policy, applyFunc} = props;

        const style = {
            scrollToBottomInfo: `font-weight: bold; color: #ff5722; padding: 5px 10px; border-radius: 5px;margin:1em;`,
            popupBtn: `width: 40%;height: 40px;cursor: pointer;border-radius: 35px;color: #007AFF;background: #FFF;border: 1px solid;`
        }


        const scrollDiv = document.createElement('policy-scroll-div');
        scrollDiv.setAttribute('policy', policy);
        scrollDiv.div.addEventListener('scroll', checkScrollHeight);

        const scrollToBottomInfo = document.createElement('div');
        scrollToBottomInfo.setAttribute('style', style.scrollToBottomInfo);
        scrollToBottomInfo.textContent = ASUS_POLICY.Dict.ScrollDown;

        const ageCheckbox = document.createElement('input');
        ageCheckbox.setAttribute('type', 'checkbox');
        ageCheckbox.setAttribute('id', 'ASUS_EULA_enable');

        const ageLabel = document.createElement('label');
        ageLabel.setAttribute('for', 'ASUS_EULA_enable');
        ageLabel.setAttribute('style', 'border:0;');
        ageLabel.textContent = ASUS_POLICY.Dict.AgeCheck;

        const ageCheckDiv = document.createElement('div');
        ageCheckDiv.setAttribute('style', 'width:100%');
        ageCheckDiv.appendChild(ageCheckbox);
        ageCheckDiv.appendChild(ageLabel);

        let abortBtn = null;
        let applyBtn = null;

        const handleClickAbortBtn = (e) => {
            if (!e.target.classList.contains("disabled")) {
                const policyModal = new PolicyWithdrawModalComponent({
                    policy: "PP",
                    submit_reload: 1,
                    readAgainCallback: clickPopupReadAgainBtn,
                    knowRiskCallback: clickPopupKnowRiskBtn
                });
                top.document.body.appendChild(policyModal.render());
            }
        }

        const handleClickApplyBtn = (e) => {
            if (!e.target.classList.contains("disabled")) {
                applyFunc();
            }
        }

        function checkScrollHeight(e) {
            let target = e.target;
            if ((target.scrollTop + target.offsetHeight) >= target.scrollHeight) {
                enableBtn(abortBtn, true);
                enableBtn(applyBtn, true);
            }
        }


        const theme = ASUS_POLICY.getTheme();
        if (theme !== "") {
            ageLabel.style.color = "#FFFFFF";
        }

        const enableBtn = function (btn, boolean) {
            if (btn !== null) {
                if (boolean) {
                    btn.classList.remove("disabled");
                } else {
                    btn.classList.add("disabled");
                }
            }
        }

        const clickPopupReadAgainBtn = () => {
            enableBtn(abortBtn, false);
            enableBtn(applyBtn, false);
            scrollDiv.div.scrollTop = 0;
        }

        const clickPopupKnowRiskBtn = () => {
            window.goTo.loadPage("welcome", true);
        }

        setInterval(() => {
            if (scrollDiv.offsetHeight > scrollDiv.scrollHeight) {
                enableBtn(abortBtn, true);
                enableBtn(applyBtn, true);
            }
        }, 1000)

        this.setAbortBtn = (element) => {
            abortBtn = element;
            abortBtn.removeEventListener('click', handleClickAbortBtn);
            abortBtn.addEventListener("click", handleClickAbortBtn);
        }

        this.setApplyBtn = (element) => {
            applyBtn = element;
            applyBtn.removeEventListener('click', handleClickApplyBtn);
            applyBtn.addEventListener('click', handleClickApplyBtn);
        }


        this.getAgeCheckbox = ageCheckbox;

        const policyDiv = document.createElement('div');
        policyDiv.className = 'policy_component';
        policyDiv.setAttribute('style', 'display:flex;flex-direction: column; align-items: center;');
        policyDiv.appendChild(scrollDiv);
        policyDiv.appendChild(scrollToBottomInfo);
        if (policy === "EULA") {
            policyDiv.appendChild(ageCheckDiv);
        }
        if (theme !== "") {
            policyDiv.insertAdjacentHTML('beforeend', ASUS_POLICY.QisTheme[theme]);
        }

        this.element = policyDiv;
    }

    render() {
        return this.element;
    }

}

class ThirdPartyPolicyModalComponent {
    constructor(props) {
        this.props = props;

        const policy_modal = document.createElement('policy-modal');
        policy_modal.setAttribute('policy', this.props.policy);
        policy_modal.setAttribute('party', this.props.party);
        this.element = policy_modal;

        this.element.setAgreeCallback(this.props.agreeCallback);
        this.element.setDisagreeCallback(this.props.disagreeCallback);
        this.element.setReadAgainCallback(this.props.readAgainCallback);
        this.element.setKnowRiskCallback(this.props.knowRiskCallback);
        top.document.body.style.overflow = 'hidden';
    }

    render() {
        return this.element;
    }

    show() {
        if (top.document.getElementById('policy_popup_modal') == null) {
            const policy_popup_modal = document.createElement('div');
            policy_popup_modal.id = 'policy_popup_modal';
            top.document.body.appendChild(policy_popup_modal);
            policy_popup_modal.appendChild(this.element);
        }
    }
}