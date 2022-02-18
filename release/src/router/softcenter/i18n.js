var dict = {};
//var systemLang = navigator.language.toLowerCase().slice(0,2);
var web_lang= "<% nvram_get("preferred_lang"); %>";
function sc_load_lang(path) {
	registerWords();
	setLanguage(web_lang,path);
}

function setLanguage(lang,path) {
	translate(lang,path);
}

function translate(lang,path) {
	if(sessionStorage.getItem(path + lang + "Data") != null){
		dict = JSON.parse(sessionStorage.getItem(path + lang + "Data"));
	}else{
		loadDict(lang,path);
	}

	$("[sclang]").each(function () {
		switch (this.tagName.toLowerCase()) {
			case "input":
				$(this).val(__tr($(this).attr("sclang")));
				break;
			default:
				$(this).text(__tr($(this).attr("sclang")));
		}
	});
	$("[sclang-input]").each(function () {
		$(this).attr("placeholder", __tr($(this).attr("sclang-input")));
	});
}

function __tr(src) {
	return (dict[src] || src);
}

function loadDict(lang,path) {
	$.ajax({
		async: false,
		type: "GET",
		url: "/res/"+ path + lang + ".json",
		success: function (msg) {
			dict = msg;
			sessionStorage.setItem(path + lang + 'Data', JSON.stringify(dict));
		},
		error: function() {
			loadDict("EN",path);
		}
	});
}

function registerWords() {
	$("[sclang]").each(function () {
		if ($(this).attr("sclang") === "") {
			switch (this.tagName.toLowerCase()) {
				case "input":
					$(this).attr("sclang", $(this).val());
					break;
				default:
					$(this).attr("sclang", $(this).text());
			}
		}
	});
	$("[sclang-input]").each(function () {
		if ($(this).attr("sclang-input") === "") {
			$(this).attr("sclang-input", $(this).attr("placeholder"));
		}
	});
}

