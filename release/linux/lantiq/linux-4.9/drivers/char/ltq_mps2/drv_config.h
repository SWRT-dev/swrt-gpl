/* drv_config.h.in.  Generated from configure.in by autoheader.  */

/******************************************************************************

                              Copyright (c) 2013
                            Lantiq Deutschland GmbH
                     Am Campeon 3; 85579 Neubiberg, Germany

  For licensing information, see the file 'LICENSE' in the root folder of
  this software module.

******************************************************************************/
#ifndef _DRV_CONFIG_H_
#define _DRV_CONFIG_H_
/*enable vpe0 configuration*/
#ifdef CONFIG_SOC_TYPE_GRX500_TEP
	#define SYSTEM_4KEC
	#define VPE0
#elif defined(CONFIG_SOC_GRX500)
	#define SYSTEM_GRX500
	#define FEAT_LINUX_PLATFORM_DRIVER
#else
	#define SYSTEM_VR9
	#ifndef CONFIG_VPE1
		#define VPE0
	#endif
#endif
/*If library is defined undef all parts of else part*/
#ifdef __LIBRARY__
	#undef __KERNEL__
	#undef SYSTEM_VR9
	#undef VPE0
#endif

/*Macros used in MPS/ICC*/
#define SUCCESS 0
#define ERROR -1
#define TRUE 1
#define FALSE 0
#endif /* _DRV_CONFIG_H_ */

