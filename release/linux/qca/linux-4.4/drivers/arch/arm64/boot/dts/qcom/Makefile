dtb-$(CONFIG_ARCH_QCOM)	+= apq8016-sbc.dtb msm8916-mtp.dtb msm8952-mtp.dtb
ifneq ($(CONFIG_IPQ_MEM_PROFILE),$(filter $(CONFIG_IPQ_MEM_PROFILE),256 512))
dtb-$(CONFIG_ARCH_QCOM) += \
				qcom-ipq6018-emulation-c1.dtb \
				qcom-ipq6018-emulation-c2.dtb \
				qcom-ipq6018-emulation-c3.dtb \
				qcom-ipq6018-cp01-c2.dtb \
				qcom-ipq6018-db-cp01.dtb \
				qcom-ipq6018-db-cp02.dtb
endif
dtb-$(CONFIG_ARCH_QCOM) += \
				qcom-ipq6018-cp01-c1.dtb \
				qcom-ipq6018-cp02-c1.dtb \
				qcom-ipq6018-cp03-c1.dtb \
				qcom-ipq807x-hk01.dtb \
				qcom-ipq807x-hk01.c2.dtb \
				qcom-ipq807x-hk01.c3.dtb \
				qcom-ipq807x-hk01.c4.dtb \
				qcom-ipq807x-hk01.c5.dtb \
				qcom-ipq807x-hk02.dtb \
				qcom-ipq807x-hk06.dtb \
				qcom-ipq807x-hk07.dtb \
				qcom-ipq807x-hk08.dtb \
				qcom-ipq807x-hk09.dtb \
				qcom-ipq807x-hk10.dtb \
				qcom-ipq807x-ac01.dtb \
				qcom-ipq807x-ac02.dtb \
				qcom-ipq807x-ac03.dtb \
				qcom-ipq807x-ac04.dtb \
				qcom-ipq807x-oak02.dtb \
				qcom-ipq807x-oak03.dtb \
				qcom-ipq807x-db.hk01.dtb \
				qcom-ipq807x-db.hk02.dtb

always		:= $(dtb-y)
subdir-y	:= $(dts-dirs)
clean-files	:= *.dtb
