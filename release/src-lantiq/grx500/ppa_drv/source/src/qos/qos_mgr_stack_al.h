#ifndef __QOS_MGR_STACK_AL_H__20081103_1153__
#define __QOS_MGR_STACK_AL_H__20081103_1153__
#ifdef __KERNEL__
#include <linux/version.h>
#include <linux/if_arp.h>
#include <linux/if_pppox.h>
#include <linux/list.h>
#include <../net/bridge/br_private.h>
#endif /*end of __KERNEL__*/
#if IS_ENABLED(CONFIG_LTQ_PMCU) || defined(CONFIG_LTQ_CPU_FREQ)
/*PMCU specific Head File*/
#include <linux/version.h>
#include <cpufreq/ltq_cpufreq.h>
#endif /*end of CONFIG_LTQ_PMCU*/
#if IS_ENABLED(CONFIG_LTQ_DATAPATH)
#include <net/datapath_api.h>
#endif
/*
 * ####################################
 * Definition
 * ####################################
 */
/*! \def QOS_MGR_ETH_ALEN
	\brief Macro that specifies the maximum length of an Ethernet MAC address.
 */
#define QOS_MGR_ETH_ALEN	ETH_ALEN
/*! \def QOS_MGR_ETH_HLEN
	\brief Macro that specifies the maximum length of an Ethernet MAC header.
 */
#define QOS_MGR_ETH_HLEN	ETH_HLEN
/*! \def QOS_MGR_IF_NAME_SIZE
	\brief Macro that specifies the maximum size of one interface name
 */
//#define QOS_MGR_IF_NAME_SIZE	IFNAMSIZ
/*! \def QOS_MGR_USER
	\brief Macro that specifies the flag for the buffer type from User Space via ioctl
 */
#define QOS_MGR_USER __user
/*
 * definition for application layer
 */
#ifndef __KERNEL__
/*! \def ETH_ALEN
	\brief Macro that specifies the maximum length of an Ethernet MAC address.
 */
#define ETH_ALEN	6
#endif
#undef NIPQUAD
/*! \def NIPQUAD
	\brief Macro that specifies NIPQUAD definition for printing IPV4 address
 */
#define NIPQUAD(addr) \
	((unsigned char *)&addr)[0], \
((unsigned char *)&addr)[1], \
((unsigned char *)&addr)[2], \
((unsigned char *)&addr)[3]
#undef NIPQUAD_FMT
/*! \def NIPQUAD_FMT
	\brief Macro that specifies NIPQUAD_FMT format definition for printing IPV4 address
 */
#define NIPQUAD_FMT "%u.%u.%u.%u"
#undef NIP6
/*! \def NIP6
	\brief Macro that specifies NIP6 definition for printing IPV6 address
 */
#define NIP6(addr) \
	ntohs(((unsigned short *)addr)[0]), \
ntohs(((unsigned short *)addr)[1]), \
ntohs(((unsigned short *)addr)[2]), \
ntohs(((unsigned short *)addr)[3]), \
ntohs(((unsigned short *)addr)[4]), \
ntohs(((unsigned short *)addr)[5]), \
ntohs(((unsigned short *)addr)[6]), \
ntohs(((unsigned short *)addr)[7])
#undef NIP6_FMT
/*! \def NIP6_FMT
	\brief Macro that specifies NIP6_FMT format definition for printing IPV6 address
 */
#define NIP6_FMT "%04x:%04x:%04x:%04x:%04x:%04x:%04x:%04x"
/*
 * ####################################
 * Data Type
 * ####################################
 */
/*
 * data type for API
 */
/*!
	\brief Pointer to interface name
 */
typedef char QOS_MGR_IFNAME;

#define MEM_ALLOC(type, var) \
	do { \
		var = kzalloc(sizeof(type), GFP_ATOMIC); \
		if (!var) { \
			QOS_HAL_DEBUG_MSG(QOS_DEBUG_ERR, \
				"[%s: %d] DP subif allocation failed\n", __func__, __LINE__);\
			return QOS_HAL_STATUS_ERR; \
		}; \
	} while (0)

#ifdef __KERNEL__
/*!
	\brief Macro that specifies  network interface data structure
 */
typedef struct net_device	QOS_MGR_NETIF;
/*!
	\brief Macro that specifies QOS_MGR network interface status structure
 */
typedef struct net_device_stats	QOS_MGR_NETIF_STATS;
/*!
	\brief This is the data structure for the QOS_MGR ATM VC structure. In Linux, this is defined to the Linux atm_vcc structure
 */
typedef struct atm_vcc QOS_MGR_VCC;
/*!
	\brief QOS_MGR synchronization primitive for exclusion and/or synchronization
 */
typedef struct qos_mgr_lock{
	spinlock_t lock;
	unsigned long flags; /*!< flag */
	uint32_t cnt; /*!< lock counter */
}QOS_MGR_LOCK;
/*!
	\brief QOS_MGR atomic timer structure. In linux, it is atomic_t structure.
 */
typedef atomic_t QOS_MGR_ATOMIC;
/*!
	\brief QOS_MGR_SIZE_T. For Linux OS, the size_t is unsigned int.
 */
typedef size_t QOS_MGR_SIZE_T;
/*
 * definition
 */
/*!
	\brief spin_lock_bh
 */
#define qos_mgr_spin_lock_bh spin_lock_bh
/*!
	\brief spin_unlock_bh
 */
#define qos_mgr_spin_unlock_bh spin_unlock_bh

/*
 * ####################################
 * Inline Functions
 * ####################################
 */
#endif /*#ifdef __KERNEL__*/
/*
 * ####################################
 * Declaration
 * ####################################
 */
/** \addtogroup QOS_MGR_ADAPTATION_LAYER */
/*@{*/
/*! \brief This function returns the QOS_MGR ATM VC structure for the EoATM (RFC 2684 Ethernet over ATM) interface specified by netif.
	\param[in] netif Pointer to the network interface structure for the bridge interface where destination lookup is to be performed.
	\param[in] pvcc Pointer to the pointer to QOS_MGR_VCC structure which is set to the VC associated with the EoATM interface specified by netif.
	\return This function returns the one of the following values: \n
	- QOS_MGR_SUCCESS, if the VCC structure is found for the EoATM interface \n
	- QOS_MGR_FAILURE, on error
	\note
 */
int32_t qos_mgr_br2684_get_vcc(QOS_MGR_NETIF *netif, QOS_MGR_VCC **pvcc);
/*! \brief Initialize a lock for synchronization.
	\param[in] p_lock Pointer to the QOS_MGR lock variable which is allocated by the caller.
	\return This function returns the one of the following values: \n
	- QOS_MGR_SUCCESS, if QOS_MGR Lock initialization is success. \n
	- QOS_MGR_FAILURE, if the QOS_MGR Lock initialization fails. \n
	\note
 */
int32_t qos_mgr_lock_init(QOS_MGR_LOCK *p_lock);
/*! \brief Get or Acquire a QOS_MGR lock for synchronization.
	\param[in] p_lock Pointer to the QOS_MGR lock variable which has been already initialized by the caller.
	\return No value returned.
	\note
 */
void qos_mgr_lock_get(QOS_MGR_LOCK *p_lock);
/*! \brief Release a QOS_MGR Lock acquired for synchronization.
	\param[in] p_lock Pointer to the QOS_MGR lock variable which is to be released by the caller..
	\return No valure returned.
	\note
 */
void qos_mgr_lock_release(QOS_MGR_LOCK *p_lock);
/*! \brief Get or Acquire a QOS_MGR lock for synchronization.
	\param[in] p_lock Pointer to the QOS_MGR lock variable which has been already initialized by the caller.
	\return current flag.
	\note
 */
uint32_t qos_mgr_lock_get2(QOS_MGR_LOCK *p_lock);
/*! \brief Release a QOS_MGR Lock acquired for synchronization.
	\param[in] p_lock Pointer to the QOS_MGR lock variable which is to be released by the caller..
	\param[in] flag system flag
	\return No valure returned.
	\note
 */
void qos_mgr_lock_release2(QOS_MGR_LOCK *p_lock, uint32_t flag);
/*! \brief Destroy a QOS_MGR lock created with the qos_mgr_lock_init API
	\param[in] p_lock Pointer to the QOS_MGR lock variable which is allocated by the caller.
	\return No valure returned.
	\note
 */
void qos_mgr_lock_destroy(QOS_MGR_LOCK *p_lock);
/*! \brief Disable interrupt processing to protect certain QOS_MGR critical regions and save current interrupt state to a global variable in the AL.
	\return No valure returned.
	\note
 */
uint32_t qos_mgr_disable_int(void);
/*! \brief Enable interrupt processing to protect certain QOS_MGR critical regions. This must actually restore interrupt status from the last qos_mgr_disable_int call.
	\param[in] flag Interrupt status flag.
	\return No valure returned.
	\note
 */
void qos_mgr_enable_int(uint32_t flag);
/*! \brief This function dynamically allocates memory for QOS_MGR use.
	\param[in] size Specifies the number of bytes to be allocated.
	\return The return value is one of the following: \n
	- Non-NULL value, if memory allocation is successful. \n
	- NULL, if the QOS_MGR Lock initialization fails. \n
	\note
 */
void *qos_mgr_malloc(uint32_t size);
/*! \brief This function dynamically allocates memory in dma zone for QOS_MGR use.
	\param[in] size Specifies the number of bytes to be allocated.
	\return The return value is one of the following: \n
	- Non-NULL value, if memory allocation is successful. \n
	- NULL, if the QOS_MGR Lock initialization fails. \n
	\note
 */
void *qos_mgr_alloc_dma(uint32_t size);
/*! \brief This function frees dynamically allocated memory.
	\param[in] buff Pointer to buffer allocated by qos_mgr_malloc routine, which needs to be freed.
	\return The return value is one of the following: \n
	- QOS_MGR_SUCCESS, if memory free is successful. \n
	- QOS_MGR_FAILURE, if the memory free operation fails. \n
	\note
 */
int32_t qos_mgr_free(void *buff);
/*! \brief This function does a byte set to destination buffer with the specified fill byte for the specified number of bytes..
	\param[in] dst Pointer to destination buffer to set bytes.
	\param[in] fillbyte Byte value to fill in the destination buffer.
	\param[in] count Specifies the number of bytes to set to fillbyte.
	\return No return value.
	\note
 */
void qos_mgr_memset(void *dst, uint32_t fillbyte, uint32_t count);
/*! \brief Initialize the QOS_MGR atomic variable to specified value.
	\param[in] v Pointer to the QOS_MGR atomic variable which is to be initalized.
	\param[in] i Intended value to be set for atomic variable p_atomic.
	\return No return value.
	\note
 */
void qos_mgr_atomic_set(QOS_MGR_ATOMIC *v, int32_t i);
/*! \brief Atomic Increment of variable.
	\param[in] v Pointer to the QOS_MGR atomic variable which is to be incremented.
	\return No return value.
	\note
 */
int32_t qos_mgr_atomic_inc(QOS_MGR_ATOMIC *v);
/*! \brief Atomic decrement of variable.
	\param[in] v Pointer to the QOS_MGR atomic variable which is to be decremented.
	\return No return value.
	\note
 */
int32_t qos_mgr_atomic_dec(QOS_MGR_ATOMIC *v);
/*! \brief copy data from username to kernel
	\param[out] to destination buffer
	\param[in] from source buffer
	\param[in] n bytes to copy
	\note
 */
uint32_t qos_mgr_copy_from_user(void *to, const void QOS_MGR_USER * from, uint32_t n);
/*! \brief copy data from kernel to username
	\param[out] to destination buffer
	\param[in] from source buffer
	\param[in] n bytes to copy
	\note
 */
uint32_t qos_mgr_copy_to_user(void QOS_MGR_USER * to, const void *from, uint32_t n);
/*! \brief copy string, like strncpy
	\param[out] dest destination buffer
	\param[in] src source buffer
	\param[in] n maximum bytes to copy
	\note
 */
uint8_t *qos_mgr_strncpy(uint8_t *dest, const uint8_t *src, QOS_MGR_SIZE_T n);
/*! \brief get ioctl type, in linux, it is _IOC_TYPE
	\return
	\note
 */
uint32_t qos_mgr_ioc_type(uint32_t);
/*! \brief get ioctl nr, in linux, it is _IOC_NR
	\return
	\note
 */
uint32_t qos_mgr_ioc_nr(uint32_t);
/*! \brief get ioctl dir, in linux, it is _IOC_DIR
	\return
	\note
 */
uint32_t qos_mgr_ioc_dir(uint32_t);
/*! \brief get ioctl read flag, in linux, it is _IOC_READ
	\return
	\note
 */
uint32_t qos_mgr_ioc_read(void);
/*! \brief get ioctl write flag, in linux, it is _IOC_WRITE
	\return
	\note
 */
uint32_t qos_mgr_ioc_write(void);
/*! \brief get ioctl size, in linux, it is _IOC_SIZE
	\return
	\note
 */
uint32_t qos_mgr_ioc_size(uint32_t);
/*! \brief check ioctl access right, in linux, it is access_ok
	\return
	\note
 */
uint32_t qos_mgr_ioc_access_ok(uint32_t type, uint32_t addr, uint32_t size);
/*! \brief get ioctl verify write flag, in linux it is VERIFY_WRITE
	\return
	\note
 */
uint32_t qos_mgr_ioc_verify_write(void);
/*! \brief get ioctl verify read flag, in linux it is VERIFY_READ
	\return
	\note
 */
uint32_t qos_mgr_ioc_verify_read(void);

QOS_MGR_NETIF *qos_mgr_get_netif(const QOS_MGR_IFNAME *ifname);

/* @} */
#endif /* __QOS_MGR_STACK_AL_H__20081103_1153__*/

