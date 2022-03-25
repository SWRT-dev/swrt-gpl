#include "rc.h"
#include "swrt.h"

void start_uu(void)
{
	stop_uu();

	if(getpid()!=1) {
		notify_rc("start_uu");
		return;
	}

	if(nvram_get_int("uu_enable"))
#if defined(RTCONFIG_SWRT_UU)
		exec_uu_swrt();
#else
		exec_uu();
#endif
}


void stop_uu(void)
{
	doSystem("killall uuplugin_monitor.sh");
	if (pidof("uuplugin") > 0)
		doSystem("killall uuplugin");
}
