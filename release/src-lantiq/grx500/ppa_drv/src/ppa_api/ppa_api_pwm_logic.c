/******************************************************************************
 **
 ** FILE NAME	: ppa_api_pws.c
 ** PROJECT	: UEIP
 ** MODULES	: PPA API (Power Saving APIs)
 **
 ** DATE	: 16 DEC 2009
 ** AUTHOR	: Shao Guohua
 ** DESCRIPTION	: PPA Protocol Stack Power Saving Logic API Implementation
 ** COPYRIGHT	:		Copyright (c) 2009
 **			Lantiq Deutschland GmbH
 **			Am Campeon 3; 85579 Neubiberg, Germany
 **
 **	 For licensing information, see the file 'LICENSE' in the root folder of
 **	 this software module.
 **
 ** HISTORY
 ** $Date		$Author			$Comment
 ** 16 DEC 2009		Shao Guohua		Initiate Version
 *******************************************************************************/
#include <linux/version.h>
#include <generated/autoconf.h>

#include <linux/module.h>
#include <linux/types.h>
#include <linux/init.h>
#include <linux/slab.h>
#include <asm/time.h>

/*
 *	PPA Specific Head File
 */
#include <net/ppa_api.h>
#include "ppa_api_misc.h"
#include "ppa_api_netif.h"
#include "ppa_api_session.h"
#include "ppa_api_session.h"
#include "ppe_drv_wrapper.h"
static enum ltq_cpufreq_state ppa_psw_stat_curr	 = LTQ_CPUFREQ_PS_D0;

int32_t ppa_pwm_logic_init(void)
{
	return PPA_SUCCESS;
}

enum ltq_cpufreq_state ppa_pwm_get_current_status(int32_t flag)
{
	return ppa_psw_stat_curr;
}

int32_t ppa_pwm_set_current_state(enum ltq_cpufreq_state e_state, int32_t flag)
{
	return PPA_SUCCESS;
}

int32_t ppa_pwm_turn_state_off(enum ltq_cpufreq_state cur_state, enum ltq_cpufreq_state new_state)
{
	return (cur_state == LTQ_CPUFREQ_PS_D0 && new_state != LTQ_CPUFREQ_PS_D0);
}

int32_t ppa_pwm_pre_set_current_state(enum ltq_cpufreq_state e_state, int32_t flag)
{
	if(ppa_pwm_turn_state_off(ppa_psw_stat_curr, e_state)){
		return (ppa_get_hw_session_cnt() > 0 ? PPA_FAILURE : PPA_SUCCESS);
	}

	return PPA_SUCCESS;
}

int32_t ppa_pwm_post_set_current_state(enum ltq_cpufreq_state e_state, int32_t flag)
{
	unsigned long sys_flag;

	local_irq_save(sys_flag);
	if ( ppa_psw_stat_curr != e_state ) {
		int pwm_level = e_state == LTQ_CPUFREQ_PS_D0 ? PPA_PWM_LEVEL_D0 : PPA_PWM_LEVEL_D3;

		ppa_drv_ppe_clk_change(pwm_level, 0);

		ppa_psw_stat_curr = e_state;
	}
	local_irq_restore(sys_flag);

	return PPA_SUCCESS;
}
