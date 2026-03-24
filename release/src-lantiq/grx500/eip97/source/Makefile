SRC := $(src)
SRC_APP := ./Adapter
SRC_DRV_FWK := ./DriverFramework
SRC_EIP201 := ./EIP201
SRC_GLB_CTRL := ./EIP97/GlobalControl
SRC_RING_CTRL := ./EIP97/RingControl
SRC_LOG := ./Log
SRC_RINGHELPER := ./RingHelper
SRC_SABUILDER := ./SABuilder
SRC_TOKENBUILDER := ./TokenBuilder
INCL_FILES := ./incl

ccflags-y := -I$(SRC)/$(INCL_FILES) \
	-I$(SRC)/$(SRC_DRV_FWK) \
	-I$(SRC)/$(SRC_LOG) \
	-I$(SRC)/$(SRC_APP) \
	-I$(SRC)/$(SRC_EIP201) \
	-I$(SRC)/$(SRC_GLB_CTRL) \
	-I$(SRC)/$(SRC_RING_CTRL) \
	-I$(SRC)/$(SRC_RINGHELPER) \
	-I$(SRC)/$(SRC_SABUILDER) \
	-I$(SRC)/$(SRC_TOKENBUILDER)

ltq_crypto-objs += $(SRC_APP)/adapter_global_control_init.o
ltq_crypto-objs += $(SRC_APP)/adapter_init.o
ltq_crypto-objs += $(SRC_APP)/adapter_dmabuf.o
ltq_crypto-objs += $(SRC_APP)/adapter_driver97_init.o
ltq_crypto-objs += $(SRC_APP)/adapter_global_eip97.o
ltq_crypto-objs += $(SRC_APP)/adapter_global_init.o
ltq_crypto-objs += $(SRC_APP)/adapter_interrupts.o
ltq_crypto-objs += $(SRC_APP)/adapter_pec_dma.o
ltq_crypto-objs += $(SRC_APP)/adapter_ring_eip202.o
ltq_crypto-objs += $(SRC_APP)/adapter_sglist.o
ltq_crypto-objs += $(SRC_APP)/adapter_sleep.o
ltq_crypto-objs += $(SRC_APP)/adapter_lock.o
ltq_crypto-objs += $(SRC_APP)/adapter_lock_internal.o

ltq_crypto-objs += $(SRC_DRV_FWK)/dmares_gen.o
ltq_crypto-objs += $(SRC_DRV_FWK)/dmares_lkm.o

ltq_crypto-objs += $(SRC_EIP201)/eip201.o

ltq_crypto-objs += $(SRC_GLB_CTRL)/eip97_global_event.o
ltq_crypto-objs += $(SRC_GLB_CTRL)/eip97_global_fsm.o
ltq_crypto-objs += $(SRC_GLB_CTRL)/eip97_global_init.o
ltq_crypto-objs += $(SRC_GLB_CTRL)/eip97_global_prng.o
ltq_crypto-objs += $(SRC_RING_CTRL)/eip202_cd_format.o
ltq_crypto-objs += $(SRC_RING_CTRL)/eip202_cdr_dscr.o
ltq_crypto-objs += $(SRC_RING_CTRL)/eip202_cdr_event.o
ltq_crypto-objs += $(SRC_RING_CTRL)/eip202_cdr_fsm.o
ltq_crypto-objs += $(SRC_RING_CTRL)/eip202_cdr_init.o
ltq_crypto-objs += $(SRC_RING_CTRL)/eip202_rd_format.o
ltq_crypto-objs += $(SRC_RING_CTRL)/eip202_rdr_dscr.o
ltq_crypto-objs += $(SRC_RING_CTRL)/eip202_rdr_event.o
ltq_crypto-objs += $(SRC_RING_CTRL)/eip202_rdr_fsm.o
ltq_crypto-objs += $(SRC_RING_CTRL)/eip202_rdr_init.o

ltq_crypto-objs += $(SRC_LOG)/log.o

ltq_crypto-objs += $(SRC_RINGHELPER)/ringhelper.o

ltq_crypto-objs += $(SRC_SABUILDER)/sa_builder.o
ltq_crypto-objs += $(SRC_SABUILDER)/sa_builder_basic.o
ltq_crypto-objs += $(SRC_SABUILDER)/sa_builder_ipsec.o
ltq_crypto-objs += $(SRC_SABUILDER)/sa_builder_srtp.o
ltq_crypto-objs += $(SRC_SABUILDER)/sa_builder_ssltls.o

ltq_crypto-objs += $(SRC_TOKENBUILDER)/token_builder_context.o
ltq_crypto-objs += $(SRC_TOKENBUILDER)/token_builder_core.o

# Driver glue layer
ltq_crypto-objs += device_lkm_ltq.o
# Driver interface to crypto API
ltq_crypto-objs += ltq_crypto_core.o
ifneq ($(CONFIG_LTQ_MPE_IPSEC_SUPPORT),)
# Driver interface to IPsec fast-path (TPT layer acceleration)
ltq_crypto-objs += ltq_ipsec_api.o
endif
obj-$(CONFIG_CRYPTO_DEV_LANTIQ_EIP97) += ltq_crypto.o
obj-$(CONFIG_LTQ_CRYPTO_TEST) += ltq_module_test.o
ifneq ($(CONFIG_LTQ_MPE_IPSEC_SUPPORT),)
obj-$(CONFIG_LTQ_CRYPTO_TEST) += ltq_ipsec_api_test.o
obj-$(CONFIG_LTQ_CRYPTO_TEST) += ltq_submit_one.o
endif
