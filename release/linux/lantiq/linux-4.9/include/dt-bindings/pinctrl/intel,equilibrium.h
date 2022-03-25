#ifndef __DT_BINDINGS_PINCTRL_INTEL_EQUILIBRIUM_H_
#define __DT_BINDINGS_PINCTRL_INTEL_EQUILIBRIUM_H_

#define EQBR_DRCC_2_MA 0
#define EQBR_DRCC_4_MA 1
#define EQBR_DRCC_8_MA 2
#define EQBR_DRCC_12_MA 3

#define EQBR_MUX_0	0
#define EQBR_MUX_1	1
#define EQBR_MUX_2	2
#define EQBR_MUX_3	3
#define EQBR_MUX_4	4
#define EQBR_MUX_GPIO	EQBR_MUX_0

#define EQBR_PINBANK_PROPERTY	"intel,pinbank"
#define EQBR_PINBANK_GPIO_CTRL	"intel,gpio"
#define EQBR_PINBANK_REG	"reg"
#define EQBR_PINBANK_PINBASE	"intel,pinbase"
#define EQBR_PINBANK_PINNUM	"intel,ngpios"

#define EQBR_PINCTRL_GROUP	"intel,groups"
#define EQBR_PINCTRL_FUNCTION	"intel,function"
#define EQBR_PINCTRL_PINS	"intel,pins"
#define EQBR_PINCTRL_MUX	"intel,mux"

#endif /* __DT_BINDINGS_PINCTRL_INTEL_EQUILIBRIUM_H_ */
