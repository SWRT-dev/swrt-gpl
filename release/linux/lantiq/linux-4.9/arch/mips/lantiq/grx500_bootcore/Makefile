obj-y := prom.o clk.o sysctrl.o irq.o
obj-$(CONFIG_SERIAL_GRX500_BOOTCORE_CONSOLE) += early_printk.o
obj-$(CONFIG_SOC_GRX500_BOOTCORE) += ngi.o
obj-$(CONFIG_SOC_PRX300_BOOTCORE) += ngi_prx300.o
#obj-y := prom.o clk.o irq.o sysctrl.o early_printk.o ngi.o timer.o

ifdef CONFIG_RESET_IAP
obj-y += interaptiv_reset.o
endif
