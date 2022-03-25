#ifndef I2C_LANTIQ_H
#define I2C_LANTIQ_H

/* I2C register structure */
struct lantiq_reg_i2c {
	/* I2C Kernel Clock Control Register */
	unsigned int clc; /* 0x00000000 */
	/* Reserved */
	unsigned int res_0; /* 0x00000004 */
	/* I2C Identification Register */
	unsigned int id; /* 0x00000008 */
	/* Reserved */
	unsigned int res_1; /* 0x0000000C */
	/*
	 * I2C RUN Control Register
	 * This register enables and disables the I2C peripheral. Before
	 * enabling, the I2C has to be configured properly. After enabling
	 * no configuration is possible
	 */
	unsigned int run_ctrl; /* 0x00000010 */
	/*
	 * I2C End Data Control Register
	 * This register is used to either turn around the data transmission
	 * direction or to address another slave without sending a stop
	 * condition. Also the software can stop the slave-transmitter by
	 * sending a not-accolade when working as master-receiver or even
	 * stop data transmission immediately when operating as
	 * master-transmitter. The writing to the bits of this control
	 * register is only effective when in MASTER RECEIVES BYTES, MASTER
	 * TRANSMITS BYTES, MASTER RESTART or SLAVE RECEIVE BYTES state
	 */
	unsigned int endd_ctrl; /* 0x00000014 */
	/*
	 * I2C Fractional Divider Configuration Register
	 * These register is used to program the fractional divider of the I2C
	 * bus. Before the peripheral is switched on by setting the RUN-bit the
	 * two (fixed) values for the two operating frequencies are programmed
	 * into these (configuration) registers. The Register FDIV_HIGH_CFG has
	 * the same layout as I2C_FDIV_CFG.
	 */
	unsigned int fdiv_cfg; /* 0x00000018 */
	/*
	 * I2C Fractional Divider (highspeed mode) Configuration Register
	 * These register is used to program the fractional divider of the I2C
	 * bus. Before the peripheral is switched on by setting the RUN-bit the
	 * two (fixed) values for the two operating frequencies are programmed
	 * into these (configuration) registers. The Register FDIV_CFG has the
	 * same layout as I2C_FDIV_CFG.
	 */
	unsigned int fdiv_high_cfg; /* 0x0000001C */
	/* I2C Address Configuration Register */
	unsigned int addr_cfg; /* 0x00000020 */
	/* I2C Bus Status Register
	 * This register gives a status information of the I2C. This additional
	 * information can be used by the software to start proper actions.
	 */
	unsigned int bus_stat; /* 0x00000024 */
	/* I2C FIFO Configuration Register */
	unsigned int fifo_cfg; /* 0x00000028 */
	/* I2C Maximum Received Packet Size Register */
	unsigned int mrps_ctrl; /* 0x0000002C */
	/* I2C Received Packet Size Status Register */
	unsigned int rps_stat; /* 0x00000030 */
	/* I2C Transmit Packet Size Register */
	unsigned int tps_ctrl; /* 0x00000034 */
	/* I2C Filled FIFO Stages Status Register */
	unsigned int ffs_stat; /* 0x00000038 */
	/* Reserved */
	unsigned int res_2; /* 0x0000003C */
	/* I2C Timing Configuration Register */
	unsigned int tim_cfg; /* 0x00000040 */
	/* Reserved */
	unsigned int res_3[7]; /* 0x00000044 */
	/* I2C Error Interrupt Request Source Mask Register */
	unsigned int err_irqsm; /* 0x00000060 */
	/* I2C Error Interrupt Request Source Status Register */
	unsigned int err_irqss; /* 0x00000064 */
	/* I2C Error Interrupt Request Source Clear Register */
	unsigned int err_irqsc; /* 0x00000068 */
	/* Reserved */
	unsigned int res_4; /* 0x0000006C */
	/* I2C Protocol Interrupt Request Source Mask Register */
	unsigned int p_irqsm; /* 0x00000070 */
	/* I2C Protocol Interrupt Request Source Status Register */
	unsigned int p_irqss; /* 0x00000074 */
	/* I2C Protocol Interrupt Request Source Clear Register */
	unsigned int p_irqsc; /* 0x00000078 */
	/* Reserved */
	unsigned int res_5; /* 0x0000007C */
	/* I2C Raw Interrupt Status Register */
	unsigned int ris; /* 0x00000080 */
	/* I2C Interrupt Mask Control Register */
	unsigned int imsc; /* 0x00000084 */
	/* I2C Masked Interrupt Status Register */
	unsigned int mis; /* 0x00000088 */
	/* I2C Interrupt Clear Register */
	unsigned int icr; /* 0x0000008C */
	/* I2C Interrupt Set Register */
	unsigned int isr; /* 0x00000090 */
	/* I2C DMA Enable Register */
	unsigned int dmae; /* 0x00000094 */
	/* Reserved */
	unsigned int res_6[8154]; /* 0x00000098 */
	/* I2C Transmit Data Register */
	unsigned int txd; /* 0x00008000 */
	/* Reserved */
	unsigned int res_7[4095]; /* 0x00008004 */
	/* I2C Receive Data Register */
	unsigned int rxd; /* 0x0000C000 */
	/* Reserved */
	unsigned int res_8[4095]; /* 0x0000C004 */
};

/*
 * Clock Divider for Normal Run Mode
 * Max 8-bit divider value. IF RMC is 0 the module is disabled. Note: As long
 * as the new divider value RMC is not valid, the register returns 0x0000 00xx
 * on reading.
 */
#define I2C_CLC_RMC_MASK 0x0000FF00
/* field offset */
#define I2C_CLC_RMC_OFFSET 8

/* Fields of "I2C Identification Register" */
/* Module ID */
#define I2C_ID_ID_MASK 0x0000FF00
/* field offset */
#define I2C_ID_ID_OFFSET 8
/* Revision */
#define I2C_ID_REV_MASK 0x000000FF
/* field offset */
#define I2C_ID_REV_OFFSET 0

/* Fields of "I2C Interrupt Mask Control Register" */
/* Enable */
#define I2C_IMSC_BREQ_INT_EN 0x00000008
/* Enable */
#define I2C_IMSC_LBREQ_INT_EN 0x00000004


#define I2C_IMSC_SREQ_INT_EN 0x00000002
/* Enable */
#define I2C_IMSC_LSREQ_INT_EN 0x00000001

/* Fields of "I2C Fractional Divider Configuration Register" */
/* field offset */
#define I2C_FDIV_CFG_INC_OFFSET 16

/* Fields of "I2C Interrupt Mask Control Register" */
/* Enable */
#define I2C_IMSC_I2C_P_INT_EN 0x00000020
/* Enable */
#define I2C_IMSC_I2C_ERR_INT_EN 0x00000010

/* Fields of "I2C Error Interrupt Request Source Status Register" */
/* TXF_OFL */
#define I2C_ERR_IRQSS_TXF_OFL 0x00000008
/* TXF_UFL */
#define I2C_ERR_IRQSS_TXF_UFL 0x00000004
/* RXF_OFL */
#define I2C_ERR_IRQSS_RXF_OFL 0x00000002
/* RXF_UFL */
#define I2C_ERR_IRQSS_RXF_UFL 0x00000001

/* Fields of "I2C Raw Interrupt Status Register" */
/* Read: Interrupt occurred. */
#define I2C_RIS_I2C_ERR_INT_INTOCC 0x00000010
/* Read: Interrupt occurred. */
#define I2C_RIS_I2C_P_INT_INTOCC 0x00000020

/* Fields of "I2C FIFO Configuration Register" */
/* TX FIFO Flow Control */
#define I2C_FIFO_CFG_TXFC 0x00020000
/* RX FIFO Flow Control */
#define I2C_FIFO_CFG_RXFC 0x00010000
/* Byte aligned (character alignment) */
#define I2C_FIFO_CFG_TXFA_TXFA0 0x00000000
/* Half word aligned (character alignment of two characters) */
#define I2C_FIFO_CFG_TXFA_TXFA1 0x00001000
/* Word aligned (character alignment of four characters) */
#define I2C_FIFO_CFG_TXFA_TXFA2 0x00002000
/* Double word aligned (character alignment of eight */
#define I2C_FIFO_CFG_TXFA_TXFA3 0x00003000
/* Byte aligned (character alignment) */
#define I2C_FIFO_CFG_RXFA_RXFA0 0x00000000
/* Half word aligned (character alignment of two characters) */
#define I2C_FIFO_CFG_RXFA_RXFA1 0x00000100
/* Word aligned (character alignment of four characters) */
#define I2C_FIFO_CFG_RXFA_RXFA2 0x00000200
/* Double word aligned (character alignment of eight */
#define I2C_FIFO_CFG_RXFA_RXFA3 0x00000300
/* 1 word: 0 for 1 word, 2 for 4 words, prevent txunder */
#define I2C_FIFO_CFG_TXBS_TXBS0 0x00000000

/* Fields of "I2C FIFO Configuration Register" 2 is fine, but timeout when frequency down, check 0 */
/* 1 word */
#define I2C_FIFO_CFG_RXBS_RXBS0 0x00000002
/* Stop on Packet End Enable */
#define I2C_ADDR_CFG_SOPE_EN 0x00200000
/* Stop on Not Acknowledge Enable */
#define I2C_ADDR_CFG_SONA_EN 0x00100000
/* Enable */
#define I2C_ADDR_CFG_MnS_EN 0x00080000

/* Fields of "I2C Interrupt Clear Register" */
/* Clear */
#define I2C_ICR_BREQ_INT_CLR 0x00000008
/* Clear */
#define I2C_ICR_LBREQ_INT_CLR 0x00000004

/* Fields of "I2C Fractional Divider Configuration Register" */
/* field offset */
#define I2C_FDIV_CFG_DEC_OFFSET 0

/* Fields of "I2C Bus Status Register" */
/* Bus Status */
#define I2C_BUS_STAT_BS_MASK 0x00000003
/* Read from I2C Bus. */
#define I2C_BUS_STAT_RNW_READ 0x00000004
/* I2C Bus is free. */
#define I2C_BUS_STAT_BS_FREE 0x00000000
/*
 * The device is working as master and has claimed the control on the
 * I2C-bus (busy master).
 */
#define I2C_BUS_STAT_BS_BM 0x00000002

/* Fields of "I2C RUN Control Register" */
/* Enable */
#define I2C_RUN_CTRL_RUN_EN 0x00000001

/* Fields of "I2C End Data Control Register" */
/*
 * Set End of Transmission
 * Note:Do not write '1' to this bit when bus is free. This will cause an
 * abort after the first byte when a new transfer is started.
 */
#define I2C_ENDD_CTRL_SETEND 0x00000002

/* Fields of "I2C Protocol Interrupt Request Source Status Register" */
/* NACK */
#define I2C_P_IRQSS_NACK 0x00000010
/* AL */
#define I2C_P_IRQSS_AL 0x00000008
/* RX */
#define I2C_P_IRQSS_RX 0x00000040
/* TX_END */
#define I2C_P_IRQSS_TX_END 0x00000020


#endif /* I2C_LANTIQ_H */
