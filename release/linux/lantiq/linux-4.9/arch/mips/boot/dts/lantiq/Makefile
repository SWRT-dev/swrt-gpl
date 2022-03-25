dtb-$(CONFIG_DT_EASY50712)	+= easy50712.dtb
dtb-$(CONFIG_DT_ANYWAN)	+= easy350_anywan.dtb
dtb-$(CONFIG_DT_EASY350550_BOOTCORE)	+= easy350550_bootcore.dtb
obj-y				+= $(patsubst %.dtb, %.dtb.o, $(dtb-y))

# Force kbuild to make empty built-in.o if necessary
obj-				+= dummy.o

always				:= $(dtb-y)
clean-files			:= *.dtb *.dtb.S
