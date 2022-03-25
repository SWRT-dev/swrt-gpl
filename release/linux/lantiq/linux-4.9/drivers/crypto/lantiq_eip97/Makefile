SRC := Sources
SRC_APP := $(SRC)/Adapter
SRC_DRV_FWK := $(SRC)/DriverFramework
SRC_EIP201 := $(SRC)/EIP201
SRC_GLB_CTRL := $(SRC)/EIP97/GlobalControl
SRC_RING_CTRL := $(SRC)/EIP97/RingControl
SRC_LOG := $(SRC)/Log
SRC_RINGHELPER := $(SRC)/RingHelper
SRC_SABUILDER := $(SRC)/SABuilder
SRC_TOKENBUILDER := $(SRC)/TokenBuilder
INCL_FILES := $(SRC)/incl

ccflags-y := -I$(src)/$(INCL_FILES)
ccflags-y += -I$(src)/$(SRC_DRV_FWK) -I$(src)/$(SRC_EIP201) -I$(src)/$(SRC_GLB_CTRL) -I$(src)/$(SRC_RING_CTRL)
ccflags-y += -I$(src)/$(SRC_RINGHELPER) -I$(src)/$(SRC_SABUILDER) -I$(src)/$(SRC_TOKENBUILDER) -I$(src)/$(SRC_LOG)

ltq_crypto-y += $(SRC_APP)/adapter_global_control_init.o
ltq_crypto-y += $(SRC_APP)/adapter_init.o
ltq_crypto-y += $(SRC_APP)/adapter_dmabuf.o
ltq_crypto-y += $(SRC_APP)/adapter_driver97_init.o
ltq_crypto-y += $(SRC_APP)/adapter_global_eip97.o
ltq_crypto-y += $(SRC_APP)/adapter_global_init.o
ltq_crypto-y += $(SRC_APP)/adapter_interrupts.o
ltq_crypto-y += $(SRC_APP)/adapter_pec_dma.o
ltq_crypto-y += $(SRC_APP)/adapter_ring_eip202.o
ltq_crypto-y += $(SRC_APP)/adapter_sglist.o
ltq_crypto-y += $(SRC_APP)/adapter_sleep.o
ltq_crypto-y += $(SRC_APP)/adapter_lock.o
ltq_crypto-y += $(SRC_APP)/adapter_lock_internal.o

ltq_crypto-y += $(SRC_DRV_FWK)/dmares_gen.o
ltq_crypto-y += $(SRC_DRV_FWK)/dmares_lkm.o

ltq_crypto-y += $(SRC_EIP201)/eip201.o

ltq_crypto-y += $(SRC_GLB_CTRL)/eip97_global_event.o
ltq_crypto-y += $(SRC_GLB_CTRL)/eip97_global_fsm.o
ltq_crypto-y += $(SRC_GLB_CTRL)/eip97_global_init.o
ltq_crypto-y += $(SRC_GLB_CTRL)/eip97_global_prng.o
ltq_crypto-y += $(SRC_RING_CTRL)/eip202_cd_format.o
ltq_crypto-y += $(SRC_RING_CTRL)/eip202_cdr_dscr.o
ltq_crypto-y += $(SRC_RING_CTRL)/eip202_cdr_event.o
ltq_crypto-y += $(SRC_RING_CTRL)/eip202_cdr_fsm.o
ltq_crypto-y += $(SRC_RING_CTRL)/eip202_cdr_init.o
ltq_crypto-y += $(SRC_RING_CTRL)/eip202_rd_format.o
ltq_crypto-y += $(SRC_RING_CTRL)/eip202_rdr_dscr.o
ltq_crypto-y += $(SRC_RING_CTRL)/eip202_rdr_event.o
ltq_crypto-y += $(SRC_RING_CTRL)/eip202_rdr_fsm.o
ltq_crypto-y += $(SRC_RING_CTRL)/eip202_rdr_init.o

ltq_crypto-y += $(SRC_LOG)/log.o

ltq_crypto-y += $(SRC_RINGHELPER)/ringhelper.o

ltq_crypto-y += $(SRC_SABUILDER)/sa_builder.o
ltq_crypto-y += $(SRC_SABUILDER)/sa_builder_basic.o
ltq_crypto-y += $(SRC_SABUILDER)/sa_builder_ipsec.o
ltq_crypto-y += $(SRC_SABUILDER)/sa_builder_srtp.o
ltq_crypto-y += $(SRC_SABUILDER)/sa_builder_ssltls.o

ltq_crypto-y += $(SRC_TOKENBUILDER)/token_builder_context.o
ltq_crypto-y += $(SRC_TOKENBUILDER)/token_builder_core.o

# Driver glue layer
ltq_crypto-y += $(SRC)/device_lkm_ltq.o
# Driver interface to crypto API
ltq_crypto-y += $(SRC)/ltq_crypto_core.o
ifeq ($(CONFIG_LTQ_MPE_IPSEC_SUPPORT), y)
# Driver interface to IPsec fast-path (TPT layer acceleration)
ltq_crypto-y += $(SRC)/ltq_ipsec_api.o
endif
obj-$(CONFIG_CRYPTO_DEV_LANTIQ_EIP97) += ltq_crypto.o
obj-$(CONFIG_LTQ_CRYPTO_TEST) += $(SRC)/ltq_module_test.o
ifeq ($(CONFIG_LTQ_MPE_IPSEC_SUPPORT), y)
obj-$(CONFIG_LTQ_CRYPTO_TEST) += $(SRC)/ltq_ipsec_api_test.o
obj-$(CONFIG_LTQ_CRYPTO_TEST) += $(SRC)/ltq_submit_one.o
endif
