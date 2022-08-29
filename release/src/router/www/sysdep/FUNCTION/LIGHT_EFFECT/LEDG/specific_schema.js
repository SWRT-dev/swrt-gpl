var gradient_color = {
	"id1": {"color" :[128,0,0,128,15,0,128,30,0,128,45,0],"left":0},
	"id2": {"color" :[128,35,0,128,50,0,128,63,0,128,75,0],"left":26},
	"id3": {"color" :[128,100,0,128,110,0,128,120,0,100,125,0],"left":51},
	"id4": {"color" :[0,128,30,0,128,60,0,128,100,0,60,128],"left":76},
	"id5": {"color" :[0,60,128,0,128,100,0,128,60,0,128,30],"left":106},
	"id6": {"color" :[0,100,128,0,80,128,0,30,128,0,0,128],"left":141},
	"id7": {"color" :[0,100,128,0,50,128,0,0,128,30,0,128],"left":171},
	"id8": {"color" :[0,0,128,20,0,128,40,0,128,60,0,128],"left":191},
	"id9": {"color" :[55,0,128,70,0,128,80,0,128,100,0,128],"left":211},
	"id10":{"color" :[128,0,128,128,0,100,128,0,60,128,0,30],"left":231},
	"id11":{"color" :[128,0,50,128,0,40,128,0,20,128,0,10],"left":251}
};
function retune_gradient_color(parm){
	var gradient_color_temp = gradient_color;
	if(parm.ledg_group == 5){//GS-AX3000 GS-AX5400
		$.each(gradient_color_temp, function(key, value) {
			var gradient_group_color = gradient_color_temp[key]["color"];
			var copy_group = gradient_group_color.slice(gradient_group_color.length-3);
			$.merge(gradient_group_color, copy_group);
		});
		gradient_color = gradient_color_temp;
	}
	else if(parm.ledg_group == 3){//GT-AX6000
		$.each(gradient_color_temp, function(key, value) {
			var gradient_group_color = gradient_color_temp[key]["color"];
			gradient_group_color.splice(9,12);
		});
		gradient_color = gradient_color_temp;
	}
	return gradient_color;
}
function set_gradient_css(parm){
	var gradient_css = {};
	if(parm.ledg_group >= 4){
		gradient_css = {"background":"linear-gradient(to right, rgb("+parm.rgb[0]+") 50%, rgb("+parm.rgb[1]+") 60%, rgb("+parm.rgb[2]+") 70%, rgb("+parm.rgb[3]+") 80%)"};
	}
	else if(parm.ledg_group == 3){
		gradient_css = {"background":"linear-gradient(to right, rgb("+parm.rgb[0]+") 50%, rgb("+parm.rgb[1]+") 60%, rgb("+parm.rgb[2]+") 70%"};
	}
	return gradient_css;
}
function retune_wave_color(parm){
	var ledg_rgb = "";
	if(parm.productid == "TUF-AX5400"){
		ledg_rgb = "128,70,0,128,70,0,128,70,0,128,70,0";
		if(parm.ledg_group == 5)
			ledg_rgb = "128,70,0,128,70,0,128,70,0,128,70,0,128,70,0";
	}
	else if(parm.productid == "GT-AX6000"){
		ledg_rgb = "128,0,0,128,0,0,128,0,0,128,0,0";
		if(parm.ledg_group == 3)
			ledg_rgb = "128,0,0,128,0,0,128,0,0";
	}
	else{
		if(parm.CoBrand == "2"){
			ledg_rgb = "128,50,35,128,50,35,128,50,35,128,50,35";
			if(parm.ledg_group == 5)
				ledg_rgb = "128,50,35,128,50,35,128,50,35,128,50,35,128,50,35";
		}
		else{
			ledg_rgb = "0,0,128,0,0,128,0,0,128,0,0,128";
			if(parm.ledg_group == 5)
				ledg_rgb = "0,0,128,0,0,128,0,0,128,0,0,128,0,0,128";
		}
	}
	return ledg_rgb;
}
