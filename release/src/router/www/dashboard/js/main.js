document.addEventListener('DOMContentLoaded', function () {
    document.querySelector('#model_name').innerHTML = system.modelName;
    document.querySelector('#op_mode').innerHTML = system.currentOPMode.desc;
    document.querySelector('#firm_ver').innerHTML = system.firmwareVer.number;

    // SSID
    let code = '';
    for (let i = 0; i < Object.values(wireless).length; i++) {
        code += `<span class="text-truncate px-2">${Object.values(wireless)[i].ssid}</span>`;
    }
    document.querySelector('#ssid_banner').innerHTML = code;

    // SYSTEM TIME
    code = `${system.time.current}<span class="banner-time-meridiem ps-1">${system.time.weekday}</span> `;
    document.querySelector('#sys_time').innerHTML = code;

    var tooltipTriggerList = [].slice.call(document.querySelectorAll('[data-bs-toggle="tooltip"]'));
    var tooltipList = tooltipTriggerList.map(function (tooltipTriggerEl) {
        return new bootstrap.Tooltip(tooltipTriggerEl);
    });

    var usbContent = ['JetFlash Transcend 16GB'];
    var popoverTriggerList = [].slice.call(document.querySelectorAll('[data-bs-toggle="popover"]'));
    var popoverList = popoverTriggerList.map(function (popoverTriggerEl) {
        let content = '';
        if (popoverTriggerEl.id === 'usb_banner') {
            content = usbContent[0];
        }
        return new bootstrap.Popover(popoverTriggerEl, {
            html: true,
            placement: 'left',
            trigger: 'click',
            content: content,
        });
    });

    var collapseElementList = [].slice.call(document.querySelectorAll('.collapse'));
    var collapseList = collapseElementList.map(function (collapseEl) {
        return new bootstrap.Collapse(collapseEl);
    });

    var popover1 = bootstrap.Popover.getInstance(document.getElementById('test'));
});

function logout() {
    if (confirm('<#JS_logout#>')) {
        setTimeout('location = "../Logout.asp";', 1);
    }
}

function reboot() {
    let rebootTime = <% get_default_reboot_time(); %>;
    if (confirm('<#Main_content_Login_Item7#>')) {
        httpApi.nvramSet({ action_mode: 'reboot',}, function () {
            setTimeout(function () {
                location.reload();
            }, rebootTime*1000);
        });
    }
}
