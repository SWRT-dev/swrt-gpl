/*
 * "Copyright (c) 2018 Intel Corporation
 * DISTRIBUTABLE AS SAMPLE SOURCE SOFTWARE
 * This Distributable As Sample Source Software is subject to the terms
 * and conditions of the Intel Software License Agreement for the Intel(R)
 * Cable GW Software Development Kit"
 */

#define PP_PORT_DIST_BASE               0xF0004000ULL
#define PP_BM_BASE                      0xF0080000ULL
#define PP_BM_RAM_BASE                  (PP_BM_BASE + 0x10000)
#define PORT_DIST_MAP_REG_BASE          PP_PORT_DIST_BASE

#define BM_OFFSET_OF_PLCY(policy)                     (policy<<2)
#define BM_OFFSET_OF_POOL(pool)                       (pool<<2)
#define BM_OFFSET_OF_GRP(group)                       (group<<2)
#define BM_OFFSET_OF_POOL_IN_PLCY(pool, policy)                  \
	((BM_OFFSET_OF_PLCY(policy) * 4) \
	+ BM_OFFSET_OF_POOL(pool))

#define RX_DMA_PORT_CFG_REG_BASE         (0xF0006040)
#define RX_DMA_PORT_CFG_REG(_p)                     \
	(RX_DMA_PORT_CFG_REG_BASE + ((unsigned int)(_p)<<2))

#define BM_GRP_RSRVD_BUFF_REG_BASE                   (PP_BM_BASE + 0x200)
#define BM_GRP_RSRVD_BUFF_REG(group)                           \
	(BM_GRP_RSRVD_BUFF_REG_BASE + BM_OFFSET_OF_GRP(group))

#define BM_PLCY_MAX_ALLOWED_BASE                      (PP_BM_RAM_BASE + 0x0000)
#define BM_PLCY_MAX_ALLOWED(policy)                            \
	(BM_PLCY_MAX_ALLOWED_BASE + BM_OFFSET_OF_PLCY(policy))

#define BM_PLCY_MIN_GUARANTEED_BASE                   (PP_BM_RAM_BASE + 0x1000)
#define BM_PLCY_MIN_GUARANTEED(policy)                            \
	(BM_PLCY_MIN_GUARANTEED_BASE + BM_OFFSET_OF_PLCY(policy))

#define BM_PLCY_GRP_ASSOCIATED_BASE                   (PP_BM_RAM_BASE + 0x2000)
#define BM_PLCY_GRP_ASSOCIATED(policy)                            \
	(BM_PLCY_GRP_ASSOCIATED_BASE + BM_OFFSET_OF_PLCY(policy))

#define BM_PLCY_POOLS_MAPPING_BASE                    (PP_BM_RAM_BASE + 0x3000)
#define BM_PLCY_POOLS_MAPPING(policy)                            \
	(BM_PLCY_POOLS_MAPPING_BASE + BM_OFFSET_OF_PLCY(policy))

#define BM_PLCY_ALLOC_BUFF_CTR_BASE                   (PP_BM_RAM_BASE + 0xA000)
#define BM_PLCY_ALLOC_BUFF_CTR(policy)                            \
	(BM_PLCY_ALLOC_BUFF_CTR_BASE + BM_OFFSET_OF_PLCY(policy))

#define BM_PLCY_MAX_ALLOW_PER_POOL_BASE               (PP_BM_RAM_BASE + 0x4000)
#define BM_PLCY_MAX_ALLOW_PER_POOL(policy, pool)                       \
	(BM_PLCY_MAX_ALLOW_PER_POOL_BASE + BM_OFFSET_OF_PLCY(policy) + \
	(pool * 0x1000))

#define BM_PLCY_ALOC_BUF_PR_POL_CTR_BASE              (PP_BM_RAM_BASE + 0xB000)
#define BM_PLCY_ALOC_BUF_PR_POL_CTR(pool, policy) \
	(BM_PLCY_ALOC_BUF_PR_POL_CTR_BASE +       \
	BM_OFFSET_OF_POOL_IN_PLCY(pool, policy))

#define PORT_DIST_PORT_MAP_REG(_p)                                           \
	(PORT_DIST_MAP_REG_BASE + ((u32)(_p)<<2))
