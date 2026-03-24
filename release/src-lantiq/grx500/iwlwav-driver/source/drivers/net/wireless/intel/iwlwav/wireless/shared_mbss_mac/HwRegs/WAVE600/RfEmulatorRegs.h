
/***********************************************************************************
File:				RfEmulatorRegs.h
Module:				rfEmulator
SOC Revision:		
Purpose:
Description:		This File was auto generated using SOC Online

************************************************************************************/
#ifndef _RF_EMULATOR_REGS_H_
#define _RF_EMULATOR_REGS_H_

/*---------------------------------------------------------------------------------
/						Registers Addresses													 
/----------------------------------------------------------------------------------*/
#include "HwMemoryMap.h"

#define RF_EMULATOR_BASE_ADDRESS                             MEMORY_MAP_UNIT_69_BASE_ADDRESS
#define	REG_RF_EMULATOR_TAP_0                           (RF_EMULATOR_BASE_ADDRESS + 0x0)
#define	REG_RF_EMULATOR_TAP_1                           (RF_EMULATOR_BASE_ADDRESS + 0x4)
#define	REG_RF_EMULATOR_TAP_2                           (RF_EMULATOR_BASE_ADDRESS + 0x8)
#define	REG_RF_EMULATOR_TAP_3                           (RF_EMULATOR_BASE_ADDRESS + 0xC)
#define	REG_RF_EMULATOR_TAP_4                           (RF_EMULATOR_BASE_ADDRESS + 0x10)
#define	REG_RF_EMULATOR_TAP_5                           (RF_EMULATOR_BASE_ADDRESS + 0x14)
#define	REG_RF_EMULATOR_TAP_6                           (RF_EMULATOR_BASE_ADDRESS + 0x18)
#define	REG_RF_EMULATOR_TAP_7                           (RF_EMULATOR_BASE_ADDRESS + 0x1C)
#define	REG_RF_EMULATOR_TAP_8                           (RF_EMULATOR_BASE_ADDRESS + 0x20)
#define	REG_RF_EMULATOR_TAP_9                           (RF_EMULATOR_BASE_ADDRESS + 0x24)
#define	REG_RF_EMULATOR_TAP_10                          (RF_EMULATOR_BASE_ADDRESS + 0x28)
#define	REG_RF_EMULATOR_TAP_11                          (RF_EMULATOR_BASE_ADDRESS + 0x2C)
#define	REG_RF_EMULATOR_TAP_12                          (RF_EMULATOR_BASE_ADDRESS + 0x30)
#define	REG_RF_EMULATOR_TAP_13                          (RF_EMULATOR_BASE_ADDRESS + 0x34)
#define	REG_RF_EMULATOR_TAP_14                          (RF_EMULATOR_BASE_ADDRESS + 0x38)
#define	REG_RF_EMULATOR_TAP_15                          (RF_EMULATOR_BASE_ADDRESS + 0x3C)
#define	REG_RF_EMULATOR_TAP_16                          (RF_EMULATOR_BASE_ADDRESS + 0x40)
#define	REG_RF_EMULATOR_TAP_17                          (RF_EMULATOR_BASE_ADDRESS + 0x44)
#define	REG_RF_EMULATOR_TAP_18                          (RF_EMULATOR_BASE_ADDRESS + 0x48)
#define	REG_RF_EMULATOR_TAP_19                          (RF_EMULATOR_BASE_ADDRESS + 0x4C)
#define	REG_RF_EMULATOR_TAP_20                          (RF_EMULATOR_BASE_ADDRESS + 0x50)
#define	REG_RF_EMULATOR_TAP_21                          (RF_EMULATOR_BASE_ADDRESS + 0x54)
#define	REG_RF_EMULATOR_TAP_22                          (RF_EMULATOR_BASE_ADDRESS + 0x58)
#define	REG_RF_EMULATOR_TAP_23                          (RF_EMULATOR_BASE_ADDRESS + 0x5C)
#define	REG_RF_EMULATOR_TAP_24                          (RF_EMULATOR_BASE_ADDRESS + 0x60)
#define	REG_RF_EMULATOR_TAP_25                          (RF_EMULATOR_BASE_ADDRESS + 0x64)
#define	REG_RF_EMULATOR_TAP_26                          (RF_EMULATOR_BASE_ADDRESS + 0x68)
#define	REG_RF_EMULATOR_TAP_27                          (RF_EMULATOR_BASE_ADDRESS + 0x6C)
#define	REG_RF_EMULATOR_TAP_28                          (RF_EMULATOR_BASE_ADDRESS + 0x70)
#define	REG_RF_EMULATOR_TAP_29                          (RF_EMULATOR_BASE_ADDRESS + 0x74)
#define	REG_RF_EMULATOR_TAP_30                          (RF_EMULATOR_BASE_ADDRESS + 0x78)
#define	REG_RF_EMULATOR_TAP_31                          (RF_EMULATOR_BASE_ADDRESS + 0x7C)
#define	REG_RF_EMULATOR_TAP_32                          (RF_EMULATOR_BASE_ADDRESS + 0x80)
#define	REG_RF_EMULATOR_TAP_33                          (RF_EMULATOR_BASE_ADDRESS + 0x84)
#define	REG_RF_EMULATOR_TAP_34                          (RF_EMULATOR_BASE_ADDRESS + 0x88)
#define	REG_RF_EMULATOR_TAP_35                          (RF_EMULATOR_BASE_ADDRESS + 0x8C)
#define	REG_RF_EMULATOR_TAP_36                          (RF_EMULATOR_BASE_ADDRESS + 0x90)
#define	REG_RF_EMULATOR_TAP_37                          (RF_EMULATOR_BASE_ADDRESS + 0x94)
#define	REG_RF_EMULATOR_TAP_38                          (RF_EMULATOR_BASE_ADDRESS + 0x98)
#define	REG_RF_EMULATOR_TAP_39                          (RF_EMULATOR_BASE_ADDRESS + 0x9C)
#define	REG_RF_EMULATOR_TAP_40                          (RF_EMULATOR_BASE_ADDRESS + 0xA0)
#define	REG_RF_EMULATOR_TAP_41                          (RF_EMULATOR_BASE_ADDRESS + 0xA4)
#define	REG_RF_EMULATOR_TAP_42                          (RF_EMULATOR_BASE_ADDRESS + 0xA8)
#define	REG_RF_EMULATOR_TAP_43                          (RF_EMULATOR_BASE_ADDRESS + 0xAC)
#define	REG_RF_EMULATOR_TAP_44                          (RF_EMULATOR_BASE_ADDRESS + 0xB0)
#define	REG_RF_EMULATOR_TAP_45                          (RF_EMULATOR_BASE_ADDRESS + 0xB4)
#define	REG_RF_EMULATOR_TAP_46                          (RF_EMULATOR_BASE_ADDRESS + 0xB8)
#define	REG_RF_EMULATOR_TAP_47                          (RF_EMULATOR_BASE_ADDRESS + 0xBC)
#define	REG_RF_EMULATOR_TAP_48                          (RF_EMULATOR_BASE_ADDRESS + 0xC0)
#define	REG_RF_EMULATOR_TAP_49                          (RF_EMULATOR_BASE_ADDRESS + 0xC4)
#define	REG_RF_EMULATOR_TAP_50                          (RF_EMULATOR_BASE_ADDRESS + 0xC8)
#define	REG_RF_EMULATOR_TAP_51                          (RF_EMULATOR_BASE_ADDRESS + 0xCC)
#define	REG_RF_EMULATOR_TAP_52                          (RF_EMULATOR_BASE_ADDRESS + 0xD0)
#define	REG_RF_EMULATOR_TAP_53                          (RF_EMULATOR_BASE_ADDRESS + 0xD4)
#define	REG_RF_EMULATOR_TAP_54                          (RF_EMULATOR_BASE_ADDRESS + 0xD8)
#define	REG_RF_EMULATOR_TAP_55                          (RF_EMULATOR_BASE_ADDRESS + 0xDC)
#define	REG_RF_EMULATOR_TAP_56                          (RF_EMULATOR_BASE_ADDRESS + 0xE0)
#define	REG_RF_EMULATOR_TAP_57                          (RF_EMULATOR_BASE_ADDRESS + 0xE4)
#define	REG_RF_EMULATOR_TAP_58                          (RF_EMULATOR_BASE_ADDRESS + 0xE8)
#define	REG_RF_EMULATOR_TAP_59                          (RF_EMULATOR_BASE_ADDRESS + 0xEC)
#define	REG_RF_EMULATOR_TAP_60                          (RF_EMULATOR_BASE_ADDRESS + 0xF0)
#define	REG_RF_EMULATOR_TAP_61                          (RF_EMULATOR_BASE_ADDRESS + 0xF4)
#define	REG_RF_EMULATOR_TAP_62                          (RF_EMULATOR_BASE_ADDRESS + 0xF8)
#define	REG_RF_EMULATOR_TAP_63                          (RF_EMULATOR_BASE_ADDRESS + 0xFC)
#define	REG_RF_EMULATOR_TAP_64                          (RF_EMULATOR_BASE_ADDRESS + 0x100)
#define	REG_RF_EMULATOR_TAP_65                          (RF_EMULATOR_BASE_ADDRESS + 0x104)
#define	REG_RF_EMULATOR_TAP_66                          (RF_EMULATOR_BASE_ADDRESS + 0x108)
#define	REG_RF_EMULATOR_TAP_67                          (RF_EMULATOR_BASE_ADDRESS + 0x10C)
#define	REG_RF_EMULATOR_TAP_68                          (RF_EMULATOR_BASE_ADDRESS + 0x110)
#define	REG_RF_EMULATOR_TAP_69                          (RF_EMULATOR_BASE_ADDRESS + 0x114)
#define	REG_RF_EMULATOR_TAP_70                          (RF_EMULATOR_BASE_ADDRESS + 0x118)
#define	REG_RF_EMULATOR_TAP_71                          (RF_EMULATOR_BASE_ADDRESS + 0x11C)
#define	REG_RF_EMULATOR_TAP_72                          (RF_EMULATOR_BASE_ADDRESS + 0x120)
#define	REG_RF_EMULATOR_TAP_73                          (RF_EMULATOR_BASE_ADDRESS + 0x124)
#define	REG_RF_EMULATOR_TAP_74                          (RF_EMULATOR_BASE_ADDRESS + 0x128)
#define	REG_RF_EMULATOR_TAP_75                          (RF_EMULATOR_BASE_ADDRESS + 0x12C)
#define	REG_RF_EMULATOR_TAP_76                          (RF_EMULATOR_BASE_ADDRESS + 0x130)
#define	REG_RF_EMULATOR_TAP_77                          (RF_EMULATOR_BASE_ADDRESS + 0x134)
#define	REG_RF_EMULATOR_TAP_78                          (RF_EMULATOR_BASE_ADDRESS + 0x138)
#define	REG_RF_EMULATOR_TAP_79                          (RF_EMULATOR_BASE_ADDRESS + 0x13C)
#define	REG_RF_EMULATOR_TAP_80                          (RF_EMULATOR_BASE_ADDRESS + 0x140)
#define	REG_RF_EMULATOR_TAP_81                          (RF_EMULATOR_BASE_ADDRESS + 0x144)
#define	REG_RF_EMULATOR_TAP_82                          (RF_EMULATOR_BASE_ADDRESS + 0x148)
#define	REG_RF_EMULATOR_TAP_83                          (RF_EMULATOR_BASE_ADDRESS + 0x14C)
#define	REG_RF_EMULATOR_TAP_84                          (RF_EMULATOR_BASE_ADDRESS + 0x150)
#define	REG_RF_EMULATOR_TAP_85                          (RF_EMULATOR_BASE_ADDRESS + 0x154)
#define	REG_RF_EMULATOR_TAP_86                          (RF_EMULATOR_BASE_ADDRESS + 0x158)
#define	REG_RF_EMULATOR_TAP_87                          (RF_EMULATOR_BASE_ADDRESS + 0x15C)
#define	REG_RF_EMULATOR_TAP_88                          (RF_EMULATOR_BASE_ADDRESS + 0x160)
#define	REG_RF_EMULATOR_TAP_89                          (RF_EMULATOR_BASE_ADDRESS + 0x164)
#define	REG_RF_EMULATOR_TAP_90                          (RF_EMULATOR_BASE_ADDRESS + 0x168)
#define	REG_RF_EMULATOR_TAP_91                          (RF_EMULATOR_BASE_ADDRESS + 0x16C)
#define	REG_RF_EMULATOR_TAP_92                          (RF_EMULATOR_BASE_ADDRESS + 0x170)
#define	REG_RF_EMULATOR_TAP_93                          (RF_EMULATOR_BASE_ADDRESS + 0x174)
#define	REG_RF_EMULATOR_TAP_94                          (RF_EMULATOR_BASE_ADDRESS + 0x178)
#define	REG_RF_EMULATOR_TAP_95                          (RF_EMULATOR_BASE_ADDRESS + 0x17C)
#define	REG_RF_EMULATOR_TAP_96                          (RF_EMULATOR_BASE_ADDRESS + 0x180)
#define	REG_RF_EMULATOR_TAP_97                          (RF_EMULATOR_BASE_ADDRESS + 0x184)
#define	REG_RF_EMULATOR_TAP_98                          (RF_EMULATOR_BASE_ADDRESS + 0x188)
#define	REG_RF_EMULATOR_TAP_99                          (RF_EMULATOR_BASE_ADDRESS + 0x18C)
#define	REG_RF_EMULATOR_TAP_100                         (RF_EMULATOR_BASE_ADDRESS + 0x190)
#define	REG_RF_EMULATOR_TAP_101                         (RF_EMULATOR_BASE_ADDRESS + 0x194)
#define	REG_RF_EMULATOR_TAP_102                         (RF_EMULATOR_BASE_ADDRESS + 0x198)
#define	REG_RF_EMULATOR_TAP_103                         (RF_EMULATOR_BASE_ADDRESS + 0x19C)
#define	REG_RF_EMULATOR_TAP_104                         (RF_EMULATOR_BASE_ADDRESS + 0x1A0)
#define	REG_RF_EMULATOR_TAP_105                         (RF_EMULATOR_BASE_ADDRESS + 0x1A4)
#define	REG_RF_EMULATOR_TAP_106                         (RF_EMULATOR_BASE_ADDRESS + 0x1A8)
#define	REG_RF_EMULATOR_TAP_107                         (RF_EMULATOR_BASE_ADDRESS + 0x1AC)
#define	REG_RF_EMULATOR_TAP_108                         (RF_EMULATOR_BASE_ADDRESS + 0x1B0)
#define	REG_RF_EMULATOR_TAP_109                         (RF_EMULATOR_BASE_ADDRESS + 0x1B4)
#define	REG_RF_EMULATOR_TAP_110                         (RF_EMULATOR_BASE_ADDRESS + 0x1B8)
#define	REG_RF_EMULATOR_TAP_111                         (RF_EMULATOR_BASE_ADDRESS + 0x1BC)
#define	REG_RF_EMULATOR_TAP_112                         (RF_EMULATOR_BASE_ADDRESS + 0x1C0)
#define	REG_RF_EMULATOR_TAP_113                         (RF_EMULATOR_BASE_ADDRESS + 0x1C4)
#define	REG_RF_EMULATOR_TAP_114                         (RF_EMULATOR_BASE_ADDRESS + 0x1C8)
#define	REG_RF_EMULATOR_TAP_115                         (RF_EMULATOR_BASE_ADDRESS + 0x1CC)
#define	REG_RF_EMULATOR_TAP_116                         (RF_EMULATOR_BASE_ADDRESS + 0x1D0)
#define	REG_RF_EMULATOR_TAP_117                         (RF_EMULATOR_BASE_ADDRESS + 0x1D4)
#define	REG_RF_EMULATOR_TAP_118                         (RF_EMULATOR_BASE_ADDRESS + 0x1D8)
#define	REG_RF_EMULATOR_TAP_119                         (RF_EMULATOR_BASE_ADDRESS + 0x1DC)
#define	REG_RF_EMULATOR_TAP_120                         (RF_EMULATOR_BASE_ADDRESS + 0x1E0)
#define	REG_RF_EMULATOR_TAP_121                         (RF_EMULATOR_BASE_ADDRESS + 0x1E4)
#define	REG_RF_EMULATOR_TAP_122                         (RF_EMULATOR_BASE_ADDRESS + 0x1E8)
#define	REG_RF_EMULATOR_TAP_123                         (RF_EMULATOR_BASE_ADDRESS + 0x1EC)
#define	REG_RF_EMULATOR_TAP_124                         (RF_EMULATOR_BASE_ADDRESS + 0x1F0)
#define	REG_RF_EMULATOR_TAP_125                         (RF_EMULATOR_BASE_ADDRESS + 0x1F4)
#define	REG_RF_EMULATOR_TAP_126                         (RF_EMULATOR_BASE_ADDRESS + 0x1F8)
#define	REG_RF_EMULATOR_TAP_127                         (RF_EMULATOR_BASE_ADDRESS + 0x1FC)
#define	REG_RF_EMULATOR_TAP_128                         (RF_EMULATOR_BASE_ADDRESS + 0x200)
#define	REG_RF_EMULATOR_TAP_129                         (RF_EMULATOR_BASE_ADDRESS + 0x204)
#define	REG_RF_EMULATOR_TAP_130                         (RF_EMULATOR_BASE_ADDRESS + 0x208)
#define	REG_RF_EMULATOR_TAP_131                         (RF_EMULATOR_BASE_ADDRESS + 0x20C)
#define	REG_RF_EMULATOR_TAP_132                         (RF_EMULATOR_BASE_ADDRESS + 0x210)
#define	REG_RF_EMULATOR_TAP_133                         (RF_EMULATOR_BASE_ADDRESS + 0x214)
#define	REG_RF_EMULATOR_TAP_134                         (RF_EMULATOR_BASE_ADDRESS + 0x218)
#define	REG_RF_EMULATOR_TAP_135                         (RF_EMULATOR_BASE_ADDRESS + 0x21C)
#define	REG_RF_EMULATOR_TAP_136                         (RF_EMULATOR_BASE_ADDRESS + 0x220)
#define	REG_RF_EMULATOR_TAP_137                         (RF_EMULATOR_BASE_ADDRESS + 0x224)
#define	REG_RF_EMULATOR_TAP_138                         (RF_EMULATOR_BASE_ADDRESS + 0x228)
#define	REG_RF_EMULATOR_TAP_139                         (RF_EMULATOR_BASE_ADDRESS + 0x22C)
#define	REG_RF_EMULATOR_TAP_140                         (RF_EMULATOR_BASE_ADDRESS + 0x230)
#define	REG_RF_EMULATOR_TAP_141                         (RF_EMULATOR_BASE_ADDRESS + 0x234)
#define	REG_RF_EMULATOR_TAP_142                         (RF_EMULATOR_BASE_ADDRESS + 0x238)
#define	REG_RF_EMULATOR_TAP_143                         (RF_EMULATOR_BASE_ADDRESS + 0x23C)
#define	REG_RF_EMULATOR_TAP_144                         (RF_EMULATOR_BASE_ADDRESS + 0x240)
#define	REG_RF_EMULATOR_TAP_145                         (RF_EMULATOR_BASE_ADDRESS + 0x244)
#define	REG_RF_EMULATOR_TAP_146                         (RF_EMULATOR_BASE_ADDRESS + 0x248)
#define	REG_RF_EMULATOR_TAP_147                         (RF_EMULATOR_BASE_ADDRESS + 0x24C)
#define	REG_RF_EMULATOR_TAP_148                         (RF_EMULATOR_BASE_ADDRESS + 0x250)
#define	REG_RF_EMULATOR_TAP_149                         (RF_EMULATOR_BASE_ADDRESS + 0x254)
#define	REG_RF_EMULATOR_TAP_150                         (RF_EMULATOR_BASE_ADDRESS + 0x258)
#define	REG_RF_EMULATOR_TAP_151                         (RF_EMULATOR_BASE_ADDRESS + 0x25C)
#define	REG_RF_EMULATOR_TAP_152                         (RF_EMULATOR_BASE_ADDRESS + 0x260)
#define	REG_RF_EMULATOR_TAP_153                         (RF_EMULATOR_BASE_ADDRESS + 0x264)
#define	REG_RF_EMULATOR_TAP_154                         (RF_EMULATOR_BASE_ADDRESS + 0x268)
#define	REG_RF_EMULATOR_TAP_155                         (RF_EMULATOR_BASE_ADDRESS + 0x26C)
#define	REG_RF_EMULATOR_TAP_156                         (RF_EMULATOR_BASE_ADDRESS + 0x270)
#define	REG_RF_EMULATOR_TAP_157                         (RF_EMULATOR_BASE_ADDRESS + 0x274)
#define	REG_RF_EMULATOR_TAP_158                         (RF_EMULATOR_BASE_ADDRESS + 0x278)
#define	REG_RF_EMULATOR_TAP_159                         (RF_EMULATOR_BASE_ADDRESS + 0x27C)
#define	REG_RF_EMULATOR_TAP_160                         (RF_EMULATOR_BASE_ADDRESS + 0x280)
#define	REG_RF_EMULATOR_TAP_161                         (RF_EMULATOR_BASE_ADDRESS + 0x284)
#define	REG_RF_EMULATOR_TAP_162                         (RF_EMULATOR_BASE_ADDRESS + 0x288)
#define	REG_RF_EMULATOR_TAP_163                         (RF_EMULATOR_BASE_ADDRESS + 0x28C)
#define	REG_RF_EMULATOR_TAP_164                         (RF_EMULATOR_BASE_ADDRESS + 0x290)
#define	REG_RF_EMULATOR_TAP_165                         (RF_EMULATOR_BASE_ADDRESS + 0x294)
#define	REG_RF_EMULATOR_TAP_166                         (RF_EMULATOR_BASE_ADDRESS + 0x298)
#define	REG_RF_EMULATOR_TAP_167                         (RF_EMULATOR_BASE_ADDRESS + 0x29C)
#define	REG_RF_EMULATOR_TAP_168                         (RF_EMULATOR_BASE_ADDRESS + 0x2A0)
#define	REG_RF_EMULATOR_TAP_169                         (RF_EMULATOR_BASE_ADDRESS + 0x2A4)
#define	REG_RF_EMULATOR_TAP_170                         (RF_EMULATOR_BASE_ADDRESS + 0x2A8)
#define	REG_RF_EMULATOR_TAP_171                         (RF_EMULATOR_BASE_ADDRESS + 0x2AC)
#define	REG_RF_EMULATOR_TAP_172                         (RF_EMULATOR_BASE_ADDRESS + 0x2B0)
#define	REG_RF_EMULATOR_TAP_173                         (RF_EMULATOR_BASE_ADDRESS + 0x2B4)
#define	REG_RF_EMULATOR_TAP_174                         (RF_EMULATOR_BASE_ADDRESS + 0x2B8)
#define	REG_RF_EMULATOR_TAP_175                         (RF_EMULATOR_BASE_ADDRESS + 0x2BC)
#define	REG_RF_EMULATOR_TAP_176                         (RF_EMULATOR_BASE_ADDRESS + 0x2C0)
#define	REG_RF_EMULATOR_TAP_177                         (RF_EMULATOR_BASE_ADDRESS + 0x2C4)
#define	REG_RF_EMULATOR_TAP_178                         (RF_EMULATOR_BASE_ADDRESS + 0x2C8)
#define	REG_RF_EMULATOR_TAP_179                         (RF_EMULATOR_BASE_ADDRESS + 0x2CC)
#define	REG_RF_EMULATOR_TAP_180                         (RF_EMULATOR_BASE_ADDRESS + 0x2D0)
#define	REG_RF_EMULATOR_TAP_181                         (RF_EMULATOR_BASE_ADDRESS + 0x2D4)
#define	REG_RF_EMULATOR_TAP_182                         (RF_EMULATOR_BASE_ADDRESS + 0x2D8)
#define	REG_RF_EMULATOR_TAP_183                         (RF_EMULATOR_BASE_ADDRESS + 0x2DC)
#define	REG_RF_EMULATOR_TAP_184                         (RF_EMULATOR_BASE_ADDRESS + 0x2E0)
#define	REG_RF_EMULATOR_TAP_185                         (RF_EMULATOR_BASE_ADDRESS + 0x2E4)
#define	REG_RF_EMULATOR_TAP_186                         (RF_EMULATOR_BASE_ADDRESS + 0x2E8)
#define	REG_RF_EMULATOR_TAP_187                         (RF_EMULATOR_BASE_ADDRESS + 0x2EC)
#define	REG_RF_EMULATOR_TAP_188                         (RF_EMULATOR_BASE_ADDRESS + 0x2F0)
#define	REG_RF_EMULATOR_TAP_189                         (RF_EMULATOR_BASE_ADDRESS + 0x2F4)
#define	REG_RF_EMULATOR_TAP_190                         (RF_EMULATOR_BASE_ADDRESS + 0x2F8)
#define	REG_RF_EMULATOR_TAP_191                         (RF_EMULATOR_BASE_ADDRESS + 0x2FC)
#define	REG_RF_EMULATOR_TAP_192                         (RF_EMULATOR_BASE_ADDRESS + 0x300)
#define	REG_RF_EMULATOR_TAP_193                         (RF_EMULATOR_BASE_ADDRESS + 0x304)
#define	REG_RF_EMULATOR_TAP_194                         (RF_EMULATOR_BASE_ADDRESS + 0x308)
#define	REG_RF_EMULATOR_TAP_195                         (RF_EMULATOR_BASE_ADDRESS + 0x30C)
#define	REG_RF_EMULATOR_TAP_196                         (RF_EMULATOR_BASE_ADDRESS + 0x310)
#define	REG_RF_EMULATOR_TAP_197                         (RF_EMULATOR_BASE_ADDRESS + 0x314)
#define	REG_RF_EMULATOR_TAP_198                         (RF_EMULATOR_BASE_ADDRESS + 0x318)
#define	REG_RF_EMULATOR_TAP_199                         (RF_EMULATOR_BASE_ADDRESS + 0x31C)
#define	REG_RF_EMULATOR_TAP_200                         (RF_EMULATOR_BASE_ADDRESS + 0x320)
#define	REG_RF_EMULATOR_TAP_201                         (RF_EMULATOR_BASE_ADDRESS + 0x324)
#define	REG_RF_EMULATOR_TAP_202                         (RF_EMULATOR_BASE_ADDRESS + 0x328)
#define	REG_RF_EMULATOR_TAP_203                         (RF_EMULATOR_BASE_ADDRESS + 0x32C)
#define	REG_RF_EMULATOR_TAP_204                         (RF_EMULATOR_BASE_ADDRESS + 0x330)
#define	REG_RF_EMULATOR_TAP_205                         (RF_EMULATOR_BASE_ADDRESS + 0x334)
#define	REG_RF_EMULATOR_TAP_206                         (RF_EMULATOR_BASE_ADDRESS + 0x338)
#define	REG_RF_EMULATOR_TAP_207                         (RF_EMULATOR_BASE_ADDRESS + 0x33C)
#define	REG_RF_EMULATOR_TAP_208                         (RF_EMULATOR_BASE_ADDRESS + 0x340)
#define	REG_RF_EMULATOR_TAP_209                         (RF_EMULATOR_BASE_ADDRESS + 0x344)
#define	REG_RF_EMULATOR_TAP_210                         (RF_EMULATOR_BASE_ADDRESS + 0x348)
#define	REG_RF_EMULATOR_TAP_211                         (RF_EMULATOR_BASE_ADDRESS + 0x34C)
#define	REG_RF_EMULATOR_TAP_212                         (RF_EMULATOR_BASE_ADDRESS + 0x350)
#define	REG_RF_EMULATOR_TAP_213                         (RF_EMULATOR_BASE_ADDRESS + 0x354)
#define	REG_RF_EMULATOR_TAP_214                         (RF_EMULATOR_BASE_ADDRESS + 0x358)
#define	REG_RF_EMULATOR_TAP_215                         (RF_EMULATOR_BASE_ADDRESS + 0x35C)
#define	REG_RF_EMULATOR_TAP_216                         (RF_EMULATOR_BASE_ADDRESS + 0x360)
#define	REG_RF_EMULATOR_TAP_217                         (RF_EMULATOR_BASE_ADDRESS + 0x364)
#define	REG_RF_EMULATOR_TAP_218                         (RF_EMULATOR_BASE_ADDRESS + 0x368)
#define	REG_RF_EMULATOR_TAP_219                         (RF_EMULATOR_BASE_ADDRESS + 0x36C)
#define	REG_RF_EMULATOR_TAP_220                         (RF_EMULATOR_BASE_ADDRESS + 0x370)
#define	REG_RF_EMULATOR_TAP_221                         (RF_EMULATOR_BASE_ADDRESS + 0x374)
#define	REG_RF_EMULATOR_TAP_222                         (RF_EMULATOR_BASE_ADDRESS + 0x378)
#define	REG_RF_EMULATOR_TAP_223                         (RF_EMULATOR_BASE_ADDRESS + 0x37C)
#define	REG_RF_EMULATOR_TAP_224                         (RF_EMULATOR_BASE_ADDRESS + 0x380)
#define	REG_RF_EMULATOR_TAP_225                         (RF_EMULATOR_BASE_ADDRESS + 0x384)
#define	REG_RF_EMULATOR_TAP_226                         (RF_EMULATOR_BASE_ADDRESS + 0x388)
#define	REG_RF_EMULATOR_TAP_227                         (RF_EMULATOR_BASE_ADDRESS + 0x38C)
#define	REG_RF_EMULATOR_TAP_228                         (RF_EMULATOR_BASE_ADDRESS + 0x390)
#define	REG_RF_EMULATOR_TAP_229                         (RF_EMULATOR_BASE_ADDRESS + 0x394)
#define	REG_RF_EMULATOR_TAP_230                         (RF_EMULATOR_BASE_ADDRESS + 0x398)
#define	REG_RF_EMULATOR_TAP_231                         (RF_EMULATOR_BASE_ADDRESS + 0x39C)
#define	REG_RF_EMULATOR_TAP_232                         (RF_EMULATOR_BASE_ADDRESS + 0x3A0)
#define	REG_RF_EMULATOR_TAP_233                         (RF_EMULATOR_BASE_ADDRESS + 0x3A4)
#define	REG_RF_EMULATOR_TAP_234                         (RF_EMULATOR_BASE_ADDRESS + 0x3A8)
#define	REG_RF_EMULATOR_TAP_235                         (RF_EMULATOR_BASE_ADDRESS + 0x3AC)
#define	REG_RF_EMULATOR_TAP_236                         (RF_EMULATOR_BASE_ADDRESS + 0x3B0)
#define	REG_RF_EMULATOR_TAP_237                         (RF_EMULATOR_BASE_ADDRESS + 0x3B4)
#define	REG_RF_EMULATOR_TAP_238                         (RF_EMULATOR_BASE_ADDRESS + 0x3B8)
#define	REG_RF_EMULATOR_TAP_239                         (RF_EMULATOR_BASE_ADDRESS + 0x3BC)
#define	REG_RF_EMULATOR_TAP_240                         (RF_EMULATOR_BASE_ADDRESS + 0x3C0)
#define	REG_RF_EMULATOR_TAP_241                         (RF_EMULATOR_BASE_ADDRESS + 0x3C4)
#define	REG_RF_EMULATOR_TAP_242                         (RF_EMULATOR_BASE_ADDRESS + 0x3C8)
#define	REG_RF_EMULATOR_TAP_243                         (RF_EMULATOR_BASE_ADDRESS + 0x3CC)
#define	REG_RF_EMULATOR_TAP_244                         (RF_EMULATOR_BASE_ADDRESS + 0x3D0)
#define	REG_RF_EMULATOR_TAP_245                         (RF_EMULATOR_BASE_ADDRESS + 0x3D4)
#define	REG_RF_EMULATOR_TAP_246                         (RF_EMULATOR_BASE_ADDRESS + 0x3D8)
#define	REG_RF_EMULATOR_TAP_247                         (RF_EMULATOR_BASE_ADDRESS + 0x3DC)
#define	REG_RF_EMULATOR_TAP_248                         (RF_EMULATOR_BASE_ADDRESS + 0x3E0)
#define	REG_RF_EMULATOR_TAP_249                         (RF_EMULATOR_BASE_ADDRESS + 0x3E4)
#define	REG_RF_EMULATOR_TAP_250                         (RF_EMULATOR_BASE_ADDRESS + 0x3E8)
#define	REG_RF_EMULATOR_TAP_251                         (RF_EMULATOR_BASE_ADDRESS + 0x3EC)
#define	REG_RF_EMULATOR_TAP_252                         (RF_EMULATOR_BASE_ADDRESS + 0x3F0)
#define	REG_RF_EMULATOR_TAP_253                         (RF_EMULATOR_BASE_ADDRESS + 0x3F4)
#define	REG_RF_EMULATOR_TAP_254                         (RF_EMULATOR_BASE_ADDRESS + 0x3F8)
#define	REG_RF_EMULATOR_TAP_255                         (RF_EMULATOR_BASE_ADDRESS + 0x3FC)
#define	REG_RF_EMULATOR_TAP_256                         (RF_EMULATOR_BASE_ADDRESS + 0x400)
#define	REG_RF_EMULATOR_TAP_257                         (RF_EMULATOR_BASE_ADDRESS + 0x404)
#define	REG_RF_EMULATOR_TAP_258                         (RF_EMULATOR_BASE_ADDRESS + 0x408)
#define	REG_RF_EMULATOR_TAP_259                         (RF_EMULATOR_BASE_ADDRESS + 0x40C)
#define	REG_RF_EMULATOR_TAP_260                         (RF_EMULATOR_BASE_ADDRESS + 0x410)
#define	REG_RF_EMULATOR_TAP_261                         (RF_EMULATOR_BASE_ADDRESS + 0x414)
#define	REG_RF_EMULATOR_TAP_262                         (RF_EMULATOR_BASE_ADDRESS + 0x418)
#define	REG_RF_EMULATOR_TAP_263                         (RF_EMULATOR_BASE_ADDRESS + 0x41C)
#define	REG_RF_EMULATOR_TAP_264                         (RF_EMULATOR_BASE_ADDRESS + 0x420)
#define	REG_RF_EMULATOR_TAP_265                         (RF_EMULATOR_BASE_ADDRESS + 0x424)
#define	REG_RF_EMULATOR_TAP_266                         (RF_EMULATOR_BASE_ADDRESS + 0x428)
#define	REG_RF_EMULATOR_TAP_267                         (RF_EMULATOR_BASE_ADDRESS + 0x42C)
#define	REG_RF_EMULATOR_TAP_268                         (RF_EMULATOR_BASE_ADDRESS + 0x430)
#define	REG_RF_EMULATOR_TAP_269                         (RF_EMULATOR_BASE_ADDRESS + 0x434)
#define	REG_RF_EMULATOR_TAP_270                         (RF_EMULATOR_BASE_ADDRESS + 0x438)
#define	REG_RF_EMULATOR_TAP_271                         (RF_EMULATOR_BASE_ADDRESS + 0x43C)
#define	REG_RF_EMULATOR_TAP_272                         (RF_EMULATOR_BASE_ADDRESS + 0x440)
#define	REG_RF_EMULATOR_TAP_273                         (RF_EMULATOR_BASE_ADDRESS + 0x444)
#define	REG_RF_EMULATOR_TAP_274                         (RF_EMULATOR_BASE_ADDRESS + 0x448)
#define	REG_RF_EMULATOR_TAP_275                         (RF_EMULATOR_BASE_ADDRESS + 0x44C)
#define	REG_RF_EMULATOR_TAP_276                         (RF_EMULATOR_BASE_ADDRESS + 0x450)
#define	REG_RF_EMULATOR_TAP_277                         (RF_EMULATOR_BASE_ADDRESS + 0x454)
#define	REG_RF_EMULATOR_TAP_278                         (RF_EMULATOR_BASE_ADDRESS + 0x458)
#define	REG_RF_EMULATOR_TAP_279                         (RF_EMULATOR_BASE_ADDRESS + 0x45C)
#define	REG_RF_EMULATOR_TAP_280                         (RF_EMULATOR_BASE_ADDRESS + 0x460)
#define	REG_RF_EMULATOR_TAP_281                         (RF_EMULATOR_BASE_ADDRESS + 0x464)
#define	REG_RF_EMULATOR_TAP_282                         (RF_EMULATOR_BASE_ADDRESS + 0x468)
#define	REG_RF_EMULATOR_TAP_283                         (RF_EMULATOR_BASE_ADDRESS + 0x46C)
#define	REG_RF_EMULATOR_TAP_284                         (RF_EMULATOR_BASE_ADDRESS + 0x470)
#define	REG_RF_EMULATOR_TAP_285                         (RF_EMULATOR_BASE_ADDRESS + 0x474)
#define	REG_RF_EMULATOR_TAP_286                         (RF_EMULATOR_BASE_ADDRESS + 0x478)
#define	REG_RF_EMULATOR_TAP_287                         (RF_EMULATOR_BASE_ADDRESS + 0x47C)
#define	REG_RF_EMULATOR_TAP_288                         (RF_EMULATOR_BASE_ADDRESS + 0x480)
#define	REG_RF_EMULATOR_TAP_289                         (RF_EMULATOR_BASE_ADDRESS + 0x484)
#define	REG_RF_EMULATOR_TAP_290                         (RF_EMULATOR_BASE_ADDRESS + 0x488)
#define	REG_RF_EMULATOR_TAP_291                         (RF_EMULATOR_BASE_ADDRESS + 0x48C)
#define	REG_RF_EMULATOR_TAP_292                         (RF_EMULATOR_BASE_ADDRESS + 0x490)
#define	REG_RF_EMULATOR_TAP_293                         (RF_EMULATOR_BASE_ADDRESS + 0x494)
#define	REG_RF_EMULATOR_TAP_294                         (RF_EMULATOR_BASE_ADDRESS + 0x498)
#define	REG_RF_EMULATOR_TAP_295                         (RF_EMULATOR_BASE_ADDRESS + 0x49C)
#define	REG_RF_EMULATOR_TAP_296                         (RF_EMULATOR_BASE_ADDRESS + 0x4A0)
#define	REG_RF_EMULATOR_TAP_297                         (RF_EMULATOR_BASE_ADDRESS + 0x4A4)
#define	REG_RF_EMULATOR_TAP_298                         (RF_EMULATOR_BASE_ADDRESS + 0x4A8)
#define	REG_RF_EMULATOR_TAP_299                         (RF_EMULATOR_BASE_ADDRESS + 0x4AC)
#define	REG_RF_EMULATOR_TAP_300                         (RF_EMULATOR_BASE_ADDRESS + 0x4B0)
#define	REG_RF_EMULATOR_TAP_301                         (RF_EMULATOR_BASE_ADDRESS + 0x4B4)
#define	REG_RF_EMULATOR_TAP_302                         (RF_EMULATOR_BASE_ADDRESS + 0x4B8)
#define	REG_RF_EMULATOR_TAP_303                         (RF_EMULATOR_BASE_ADDRESS + 0x4BC)
#define	REG_RF_EMULATOR_TAP_304                         (RF_EMULATOR_BASE_ADDRESS + 0x4C0)
#define	REG_RF_EMULATOR_TAP_305                         (RF_EMULATOR_BASE_ADDRESS + 0x4C4)
#define	REG_RF_EMULATOR_TAP_306                         (RF_EMULATOR_BASE_ADDRESS + 0x4C8)
#define	REG_RF_EMULATOR_TAP_307                         (RF_EMULATOR_BASE_ADDRESS + 0x4CC)
#define	REG_RF_EMULATOR_TAP_308                         (RF_EMULATOR_BASE_ADDRESS + 0x4D0)
#define	REG_RF_EMULATOR_TAP_309                         (RF_EMULATOR_BASE_ADDRESS + 0x4D4)
#define	REG_RF_EMULATOR_TAP_310                         (RF_EMULATOR_BASE_ADDRESS + 0x4D8)
#define	REG_RF_EMULATOR_TAP_311                         (RF_EMULATOR_BASE_ADDRESS + 0x4DC)
#define	REG_RF_EMULATOR_TAP_312                         (RF_EMULATOR_BASE_ADDRESS + 0x4E0)
#define	REG_RF_EMULATOR_TAP_313                         (RF_EMULATOR_BASE_ADDRESS + 0x4E4)
#define	REG_RF_EMULATOR_TAP_314                         (RF_EMULATOR_BASE_ADDRESS + 0x4E8)
#define	REG_RF_EMULATOR_TAP_315                         (RF_EMULATOR_BASE_ADDRESS + 0x4EC)
#define	REG_RF_EMULATOR_TAP_316                         (RF_EMULATOR_BASE_ADDRESS + 0x4F0)
#define	REG_RF_EMULATOR_TAP_317                         (RF_EMULATOR_BASE_ADDRESS + 0x4F4)
#define	REG_RF_EMULATOR_TAP_318                         (RF_EMULATOR_BASE_ADDRESS + 0x4F8)
#define	REG_RF_EMULATOR_TAP_319                         (RF_EMULATOR_BASE_ADDRESS + 0x4FC)
#define	REG_RF_EMULATOR_TAP_320                         (RF_EMULATOR_BASE_ADDRESS + 0x500)
#define	REG_RF_EMULATOR_TAP_321                         (RF_EMULATOR_BASE_ADDRESS + 0x504)
#define	REG_RF_EMULATOR_TAP_322                         (RF_EMULATOR_BASE_ADDRESS + 0x508)
#define	REG_RF_EMULATOR_TAP_323                         (RF_EMULATOR_BASE_ADDRESS + 0x50C)
#define	REG_RF_EMULATOR_TAP_324                         (RF_EMULATOR_BASE_ADDRESS + 0x510)
#define	REG_RF_EMULATOR_TAP_325                         (RF_EMULATOR_BASE_ADDRESS + 0x514)
#define	REG_RF_EMULATOR_TAP_326                         (RF_EMULATOR_BASE_ADDRESS + 0x518)
#define	REG_RF_EMULATOR_TAP_327                         (RF_EMULATOR_BASE_ADDRESS + 0x51C)
#define	REG_RF_EMULATOR_TAP_328                         (RF_EMULATOR_BASE_ADDRESS + 0x520)
#define	REG_RF_EMULATOR_TAP_329                         (RF_EMULATOR_BASE_ADDRESS + 0x524)
#define	REG_RF_EMULATOR_TAP_330                         (RF_EMULATOR_BASE_ADDRESS + 0x528)
#define	REG_RF_EMULATOR_TAP_331                         (RF_EMULATOR_BASE_ADDRESS + 0x52C)
#define	REG_RF_EMULATOR_TAP_332                         (RF_EMULATOR_BASE_ADDRESS + 0x530)
#define	REG_RF_EMULATOR_TAP_333                         (RF_EMULATOR_BASE_ADDRESS + 0x534)
#define	REG_RF_EMULATOR_TAP_334                         (RF_EMULATOR_BASE_ADDRESS + 0x538)
#define	REG_RF_EMULATOR_TAP_335                         (RF_EMULATOR_BASE_ADDRESS + 0x53C)
#define	REG_RF_EMULATOR_TAP_336                         (RF_EMULATOR_BASE_ADDRESS + 0x540)
#define	REG_RF_EMULATOR_TAP_337                         (RF_EMULATOR_BASE_ADDRESS + 0x544)
#define	REG_RF_EMULATOR_TAP_338                         (RF_EMULATOR_BASE_ADDRESS + 0x548)
#define	REG_RF_EMULATOR_TAP_339                         (RF_EMULATOR_BASE_ADDRESS + 0x54C)
#define	REG_RF_EMULATOR_TAP_340                         (RF_EMULATOR_BASE_ADDRESS + 0x550)
#define	REG_RF_EMULATOR_TAP_341                         (RF_EMULATOR_BASE_ADDRESS + 0x554)
#define	REG_RF_EMULATOR_TAP_342                         (RF_EMULATOR_BASE_ADDRESS + 0x558)
#define	REG_RF_EMULATOR_TAP_343                         (RF_EMULATOR_BASE_ADDRESS + 0x55C)
#define	REG_RF_EMULATOR_TAP_344                         (RF_EMULATOR_BASE_ADDRESS + 0x560)
#define	REG_RF_EMULATOR_TAP_345                         (RF_EMULATOR_BASE_ADDRESS + 0x564)
#define	REG_RF_EMULATOR_TAP_346                         (RF_EMULATOR_BASE_ADDRESS + 0x568)
#define	REG_RF_EMULATOR_TAP_347                         (RF_EMULATOR_BASE_ADDRESS + 0x56C)
#define	REG_RF_EMULATOR_TAP_348                         (RF_EMULATOR_BASE_ADDRESS + 0x570)
#define	REG_RF_EMULATOR_TAP_349                         (RF_EMULATOR_BASE_ADDRESS + 0x574)
#define	REG_RF_EMULATOR_TAP_350                         (RF_EMULATOR_BASE_ADDRESS + 0x578)
#define	REG_RF_EMULATOR_TAP_351                         (RF_EMULATOR_BASE_ADDRESS + 0x57C)
#define	REG_RF_EMULATOR_TAP_352                         (RF_EMULATOR_BASE_ADDRESS + 0x580)
#define	REG_RF_EMULATOR_TAP_353                         (RF_EMULATOR_BASE_ADDRESS + 0x584)
#define	REG_RF_EMULATOR_TAP_354                         (RF_EMULATOR_BASE_ADDRESS + 0x588)
#define	REG_RF_EMULATOR_TAP_355                         (RF_EMULATOR_BASE_ADDRESS + 0x58C)
#define	REG_RF_EMULATOR_TAP_356                         (RF_EMULATOR_BASE_ADDRESS + 0x590)
#define	REG_RF_EMULATOR_TAP_357                         (RF_EMULATOR_BASE_ADDRESS + 0x594)
#define	REG_RF_EMULATOR_TAP_358                         (RF_EMULATOR_BASE_ADDRESS + 0x598)
#define	REG_RF_EMULATOR_TAP_359                         (RF_EMULATOR_BASE_ADDRESS + 0x59C)
#define	REG_RF_EMULATOR_TAP_360                         (RF_EMULATOR_BASE_ADDRESS + 0x5A0)
#define	REG_RF_EMULATOR_TAP_361                         (RF_EMULATOR_BASE_ADDRESS + 0x5A4)
#define	REG_RF_EMULATOR_TAP_362                         (RF_EMULATOR_BASE_ADDRESS + 0x5A8)
#define	REG_RF_EMULATOR_TAP_363                         (RF_EMULATOR_BASE_ADDRESS + 0x5AC)
#define	REG_RF_EMULATOR_TAP_364                         (RF_EMULATOR_BASE_ADDRESS + 0x5B0)
#define	REG_RF_EMULATOR_TAP_365                         (RF_EMULATOR_BASE_ADDRESS + 0x5B4)
#define	REG_RF_EMULATOR_TAP_366                         (RF_EMULATOR_BASE_ADDRESS + 0x5B8)
#define	REG_RF_EMULATOR_TAP_367                         (RF_EMULATOR_BASE_ADDRESS + 0x5BC)
#define	REG_RF_EMULATOR_TAP_368                         (RF_EMULATOR_BASE_ADDRESS + 0x5C0)
#define	REG_RF_EMULATOR_TAP_369                         (RF_EMULATOR_BASE_ADDRESS + 0x5C4)
#define	REG_RF_EMULATOR_TAP_370                         (RF_EMULATOR_BASE_ADDRESS + 0x5C8)
#define	REG_RF_EMULATOR_TAP_371                         (RF_EMULATOR_BASE_ADDRESS + 0x5CC)
#define	REG_RF_EMULATOR_TAP_372                         (RF_EMULATOR_BASE_ADDRESS + 0x5D0)
#define	REG_RF_EMULATOR_TAP_373                         (RF_EMULATOR_BASE_ADDRESS + 0x5D4)
#define	REG_RF_EMULATOR_TAP_374                         (RF_EMULATOR_BASE_ADDRESS + 0x5D8)
#define	REG_RF_EMULATOR_TAP_375                         (RF_EMULATOR_BASE_ADDRESS + 0x5DC)
#define	REG_RF_EMULATOR_TAP_376                         (RF_EMULATOR_BASE_ADDRESS + 0x5E0)
#define	REG_RF_EMULATOR_TAP_377                         (RF_EMULATOR_BASE_ADDRESS + 0x5E4)
#define	REG_RF_EMULATOR_TAP_378                         (RF_EMULATOR_BASE_ADDRESS + 0x5E8)
#define	REG_RF_EMULATOR_TAP_379                         (RF_EMULATOR_BASE_ADDRESS + 0x5EC)
#define	REG_RF_EMULATOR_TAP_380                         (RF_EMULATOR_BASE_ADDRESS + 0x5F0)
#define	REG_RF_EMULATOR_TAP_381                         (RF_EMULATOR_BASE_ADDRESS + 0x5F4)
#define	REG_RF_EMULATOR_TAP_382                         (RF_EMULATOR_BASE_ADDRESS + 0x5F8)
#define	REG_RF_EMULATOR_TAP_383                         (RF_EMULATOR_BASE_ADDRESS + 0x5FC)
#define	REG_RF_EMULATOR_TAP_384                         (RF_EMULATOR_BASE_ADDRESS + 0x600)
#define	REG_RF_EMULATOR_TAP_385                         (RF_EMULATOR_BASE_ADDRESS + 0x604)
#define	REG_RF_EMULATOR_TAP_386                         (RF_EMULATOR_BASE_ADDRESS + 0x608)
#define	REG_RF_EMULATOR_TAP_387                         (RF_EMULATOR_BASE_ADDRESS + 0x60C)
#define	REG_RF_EMULATOR_TAP_388                         (RF_EMULATOR_BASE_ADDRESS + 0x610)
#define	REG_RF_EMULATOR_TAP_389                         (RF_EMULATOR_BASE_ADDRESS + 0x614)
#define	REG_RF_EMULATOR_TAP_390                         (RF_EMULATOR_BASE_ADDRESS + 0x618)
#define	REG_RF_EMULATOR_TAP_391                         (RF_EMULATOR_BASE_ADDRESS + 0x61C)
#define	REG_RF_EMULATOR_TAP_392                         (RF_EMULATOR_BASE_ADDRESS + 0x620)
#define	REG_RF_EMULATOR_TAP_393                         (RF_EMULATOR_BASE_ADDRESS + 0x624)
#define	REG_RF_EMULATOR_TAP_394                         (RF_EMULATOR_BASE_ADDRESS + 0x628)
#define	REG_RF_EMULATOR_TAP_395                         (RF_EMULATOR_BASE_ADDRESS + 0x62C)
#define	REG_RF_EMULATOR_TAP_396                         (RF_EMULATOR_BASE_ADDRESS + 0x630)
#define	REG_RF_EMULATOR_TAP_397                         (RF_EMULATOR_BASE_ADDRESS + 0x634)
#define	REG_RF_EMULATOR_TAP_398                         (RF_EMULATOR_BASE_ADDRESS + 0x638)
#define	REG_RF_EMULATOR_TAP_399                         (RF_EMULATOR_BASE_ADDRESS + 0x63C)
#define	REG_RF_EMULATOR_TAP_400                         (RF_EMULATOR_BASE_ADDRESS + 0x640)
#define	REG_RF_EMULATOR_TAP_401                         (RF_EMULATOR_BASE_ADDRESS + 0x644)
#define	REG_RF_EMULATOR_TAP_402                         (RF_EMULATOR_BASE_ADDRESS + 0x648)
#define	REG_RF_EMULATOR_TAP_403                         (RF_EMULATOR_BASE_ADDRESS + 0x64C)
#define	REG_RF_EMULATOR_TAP_404                         (RF_EMULATOR_BASE_ADDRESS + 0x650)
#define	REG_RF_EMULATOR_TAP_405                         (RF_EMULATOR_BASE_ADDRESS + 0x654)
#define	REG_RF_EMULATOR_TAP_406                         (RF_EMULATOR_BASE_ADDRESS + 0x658)
#define	REG_RF_EMULATOR_TAP_407                         (RF_EMULATOR_BASE_ADDRESS + 0x65C)
#define	REG_RF_EMULATOR_TAP_408                         (RF_EMULATOR_BASE_ADDRESS + 0x660)
#define	REG_RF_EMULATOR_TAP_409                         (RF_EMULATOR_BASE_ADDRESS + 0x664)
#define	REG_RF_EMULATOR_TAP_410                         (RF_EMULATOR_BASE_ADDRESS + 0x668)
#define	REG_RF_EMULATOR_TAP_411                         (RF_EMULATOR_BASE_ADDRESS + 0x66C)
#define	REG_RF_EMULATOR_TAP_412                         (RF_EMULATOR_BASE_ADDRESS + 0x670)
#define	REG_RF_EMULATOR_TAP_413                         (RF_EMULATOR_BASE_ADDRESS + 0x674)
#define	REG_RF_EMULATOR_TAP_414                         (RF_EMULATOR_BASE_ADDRESS + 0x678)
#define	REG_RF_EMULATOR_TAP_415                         (RF_EMULATOR_BASE_ADDRESS + 0x67C)
#define	REG_RF_EMULATOR_TAP_416                         (RF_EMULATOR_BASE_ADDRESS + 0x680)
#define	REG_RF_EMULATOR_TAP_417                         (RF_EMULATOR_BASE_ADDRESS + 0x684)
#define	REG_RF_EMULATOR_TAP_418                         (RF_EMULATOR_BASE_ADDRESS + 0x688)
#define	REG_RF_EMULATOR_TAP_419                         (RF_EMULATOR_BASE_ADDRESS + 0x68C)
#define	REG_RF_EMULATOR_TAP_420                         (RF_EMULATOR_BASE_ADDRESS + 0x690)
#define	REG_RF_EMULATOR_TAP_421                         (RF_EMULATOR_BASE_ADDRESS + 0x694)
#define	REG_RF_EMULATOR_TAP_422                         (RF_EMULATOR_BASE_ADDRESS + 0x698)
#define	REG_RF_EMULATOR_TAP_423                         (RF_EMULATOR_BASE_ADDRESS + 0x69C)
#define	REG_RF_EMULATOR_TAP_424                         (RF_EMULATOR_BASE_ADDRESS + 0x6A0)
#define	REG_RF_EMULATOR_TAP_425                         (RF_EMULATOR_BASE_ADDRESS + 0x6A4)
#define	REG_RF_EMULATOR_TAP_426                         (RF_EMULATOR_BASE_ADDRESS + 0x6A8)
#define	REG_RF_EMULATOR_TAP_427                         (RF_EMULATOR_BASE_ADDRESS + 0x6AC)
#define	REG_RF_EMULATOR_TAP_428                         (RF_EMULATOR_BASE_ADDRESS + 0x6B0)
#define	REG_RF_EMULATOR_TAP_429                         (RF_EMULATOR_BASE_ADDRESS + 0x6B4)
#define	REG_RF_EMULATOR_TAP_430                         (RF_EMULATOR_BASE_ADDRESS + 0x6B8)
#define	REG_RF_EMULATOR_TAP_431                         (RF_EMULATOR_BASE_ADDRESS + 0x6BC)
#define	REG_RF_EMULATOR_TAP_432                         (RF_EMULATOR_BASE_ADDRESS + 0x6C0)
#define	REG_RF_EMULATOR_TAP_433                         (RF_EMULATOR_BASE_ADDRESS + 0x6C4)
#define	REG_RF_EMULATOR_TAP_434                         (RF_EMULATOR_BASE_ADDRESS + 0x6C8)
#define	REG_RF_EMULATOR_TAP_435                         (RF_EMULATOR_BASE_ADDRESS + 0x6CC)
#define	REG_RF_EMULATOR_TAP_436                         (RF_EMULATOR_BASE_ADDRESS + 0x6D0)
#define	REG_RF_EMULATOR_TAP_437                         (RF_EMULATOR_BASE_ADDRESS + 0x6D4)
#define	REG_RF_EMULATOR_TAP_438                         (RF_EMULATOR_BASE_ADDRESS + 0x6D8)
#define	REG_RF_EMULATOR_TAP_439                         (RF_EMULATOR_BASE_ADDRESS + 0x6DC)
#define	REG_RF_EMULATOR_TAP_440                         (RF_EMULATOR_BASE_ADDRESS + 0x6E0)
#define	REG_RF_EMULATOR_TAP_441                         (RF_EMULATOR_BASE_ADDRESS + 0x6E4)
#define	REG_RF_EMULATOR_TAP_442                         (RF_EMULATOR_BASE_ADDRESS + 0x6E8)
#define	REG_RF_EMULATOR_TAP_443                         (RF_EMULATOR_BASE_ADDRESS + 0x6EC)
#define	REG_RF_EMULATOR_TAP_444                         (RF_EMULATOR_BASE_ADDRESS + 0x6F0)
#define	REG_RF_EMULATOR_TAP_445                         (RF_EMULATOR_BASE_ADDRESS + 0x6F4)
#define	REG_RF_EMULATOR_TAP_446                         (RF_EMULATOR_BASE_ADDRESS + 0x6F8)
#define	REG_RF_EMULATOR_TAP_447                         (RF_EMULATOR_BASE_ADDRESS + 0x6FC)
#define	REG_RF_EMULATOR_TAP_448                         (RF_EMULATOR_BASE_ADDRESS + 0x700)
#define	REG_RF_EMULATOR_TAP_449                         (RF_EMULATOR_BASE_ADDRESS + 0x704)
#define	REG_RF_EMULATOR_TAP_450                         (RF_EMULATOR_BASE_ADDRESS + 0x708)
#define	REG_RF_EMULATOR_TAP_451                         (RF_EMULATOR_BASE_ADDRESS + 0x70C)
#define	REG_RF_EMULATOR_TAP_452                         (RF_EMULATOR_BASE_ADDRESS + 0x710)
#define	REG_RF_EMULATOR_TAP_453                         (RF_EMULATOR_BASE_ADDRESS + 0x714)
#define	REG_RF_EMULATOR_TAP_454                         (RF_EMULATOR_BASE_ADDRESS + 0x718)
#define	REG_RF_EMULATOR_TAP_455                         (RF_EMULATOR_BASE_ADDRESS + 0x71C)
#define	REG_RF_EMULATOR_TAP_456                         (RF_EMULATOR_BASE_ADDRESS + 0x720)
#define	REG_RF_EMULATOR_TAP_457                         (RF_EMULATOR_BASE_ADDRESS + 0x724)
#define	REG_RF_EMULATOR_TAP_458                         (RF_EMULATOR_BASE_ADDRESS + 0x728)
#define	REG_RF_EMULATOR_TAP_459                         (RF_EMULATOR_BASE_ADDRESS + 0x72C)
#define	REG_RF_EMULATOR_TAP_460                         (RF_EMULATOR_BASE_ADDRESS + 0x730)
#define	REG_RF_EMULATOR_TAP_461                         (RF_EMULATOR_BASE_ADDRESS + 0x734)
#define	REG_RF_EMULATOR_TAP_462                         (RF_EMULATOR_BASE_ADDRESS + 0x738)
#define	REG_RF_EMULATOR_TAP_463                         (RF_EMULATOR_BASE_ADDRESS + 0x73C)
#define	REG_RF_EMULATOR_TAP_464                         (RF_EMULATOR_BASE_ADDRESS + 0x740)
#define	REG_RF_EMULATOR_TAP_465                         (RF_EMULATOR_BASE_ADDRESS + 0x744)
#define	REG_RF_EMULATOR_TAP_466                         (RF_EMULATOR_BASE_ADDRESS + 0x748)
#define	REG_RF_EMULATOR_TAP_467                         (RF_EMULATOR_BASE_ADDRESS + 0x74C)
#define	REG_RF_EMULATOR_TAP_468                         (RF_EMULATOR_BASE_ADDRESS + 0x750)
#define	REG_RF_EMULATOR_TAP_469                         (RF_EMULATOR_BASE_ADDRESS + 0x754)
#define	REG_RF_EMULATOR_TAP_470                         (RF_EMULATOR_BASE_ADDRESS + 0x758)
#define	REG_RF_EMULATOR_TAP_471                         (RF_EMULATOR_BASE_ADDRESS + 0x75C)
#define	REG_RF_EMULATOR_TAP_472                         (RF_EMULATOR_BASE_ADDRESS + 0x760)
#define	REG_RF_EMULATOR_TAP_473                         (RF_EMULATOR_BASE_ADDRESS + 0x764)
#define	REG_RF_EMULATOR_TAP_474                         (RF_EMULATOR_BASE_ADDRESS + 0x768)
#define	REG_RF_EMULATOR_TAP_475                         (RF_EMULATOR_BASE_ADDRESS + 0x76C)
#define	REG_RF_EMULATOR_TAP_476                         (RF_EMULATOR_BASE_ADDRESS + 0x770)
#define	REG_RF_EMULATOR_TAP_477                         (RF_EMULATOR_BASE_ADDRESS + 0x774)
#define	REG_RF_EMULATOR_TAP_478                         (RF_EMULATOR_BASE_ADDRESS + 0x778)
#define	REG_RF_EMULATOR_TAP_479                         (RF_EMULATOR_BASE_ADDRESS + 0x77C)
#define	REG_RF_EMULATOR_TAP_480                         (RF_EMULATOR_BASE_ADDRESS + 0x780)
#define	REG_RF_EMULATOR_TAP_481                         (RF_EMULATOR_BASE_ADDRESS + 0x784)
#define	REG_RF_EMULATOR_TAP_482                         (RF_EMULATOR_BASE_ADDRESS + 0x788)
#define	REG_RF_EMULATOR_TAP_483                         (RF_EMULATOR_BASE_ADDRESS + 0x78C)
#define	REG_RF_EMULATOR_TAP_484                         (RF_EMULATOR_BASE_ADDRESS + 0x790)
#define	REG_RF_EMULATOR_TAP_485                         (RF_EMULATOR_BASE_ADDRESS + 0x794)
#define	REG_RF_EMULATOR_TAP_486                         (RF_EMULATOR_BASE_ADDRESS + 0x798)
#define	REG_RF_EMULATOR_TAP_487                         (RF_EMULATOR_BASE_ADDRESS + 0x79C)
#define	REG_RF_EMULATOR_TAP_488                         (RF_EMULATOR_BASE_ADDRESS + 0x7A0)
#define	REG_RF_EMULATOR_TAP_489                         (RF_EMULATOR_BASE_ADDRESS + 0x7A4)
#define	REG_RF_EMULATOR_TAP_490                         (RF_EMULATOR_BASE_ADDRESS + 0x7A8)
#define	REG_RF_EMULATOR_TAP_491                         (RF_EMULATOR_BASE_ADDRESS + 0x7AC)
#define	REG_RF_EMULATOR_TAP_492                         (RF_EMULATOR_BASE_ADDRESS + 0x7B0)
#define	REG_RF_EMULATOR_TAP_493                         (RF_EMULATOR_BASE_ADDRESS + 0x7B4)
#define	REG_RF_EMULATOR_TAP_494                         (RF_EMULATOR_BASE_ADDRESS + 0x7B8)
#define	REG_RF_EMULATOR_TAP_495                         (RF_EMULATOR_BASE_ADDRESS + 0x7BC)
#define	REG_RF_EMULATOR_TAP_496                         (RF_EMULATOR_BASE_ADDRESS + 0x7C0)
#define	REG_RF_EMULATOR_TAP_497                         (RF_EMULATOR_BASE_ADDRESS + 0x7C4)
#define	REG_RF_EMULATOR_TAP_498                         (RF_EMULATOR_BASE_ADDRESS + 0x7C8)
#define	REG_RF_EMULATOR_TAP_499                         (RF_EMULATOR_BASE_ADDRESS + 0x7CC)
#define	REG_RF_EMULATOR_TAP_500                         (RF_EMULATOR_BASE_ADDRESS + 0x7D0)
#define	REG_RF_EMULATOR_TAP_501                         (RF_EMULATOR_BASE_ADDRESS + 0x7D4)
#define	REG_RF_EMULATOR_TAP_502                         (RF_EMULATOR_BASE_ADDRESS + 0x7D8)
#define	REG_RF_EMULATOR_TAP_503                         (RF_EMULATOR_BASE_ADDRESS + 0x7DC)
#define	REG_RF_EMULATOR_TAP_504                         (RF_EMULATOR_BASE_ADDRESS + 0x7E0)
#define	REG_RF_EMULATOR_TAP_505                         (RF_EMULATOR_BASE_ADDRESS + 0x7E4)
#define	REG_RF_EMULATOR_TAP_506                         (RF_EMULATOR_BASE_ADDRESS + 0x7E8)
#define	REG_RF_EMULATOR_TAP_507                         (RF_EMULATOR_BASE_ADDRESS + 0x7EC)
#define	REG_RF_EMULATOR_TAP_508                         (RF_EMULATOR_BASE_ADDRESS + 0x7F0)
#define	REG_RF_EMULATOR_TAP_509                         (RF_EMULATOR_BASE_ADDRESS + 0x7F4)
#define	REG_RF_EMULATOR_TAP_510                         (RF_EMULATOR_BASE_ADDRESS + 0x7F8)
#define	REG_RF_EMULATOR_TAP_511                         (RF_EMULATOR_BASE_ADDRESS + 0x7FC)
#define	REG_RF_EMULATOR_TAP_512                         (RF_EMULATOR_BASE_ADDRESS + 0x800)
#define	REG_RF_EMULATOR_TAP_513                         (RF_EMULATOR_BASE_ADDRESS + 0x804)
#define	REG_RF_EMULATOR_TAP_514                         (RF_EMULATOR_BASE_ADDRESS + 0x808)
#define	REG_RF_EMULATOR_TAP_515                         (RF_EMULATOR_BASE_ADDRESS + 0x80C)
#define	REG_RF_EMULATOR_TAP_516                         (RF_EMULATOR_BASE_ADDRESS + 0x810)
#define	REG_RF_EMULATOR_TAP_517                         (RF_EMULATOR_BASE_ADDRESS + 0x814)
#define	REG_RF_EMULATOR_TAP_518                         (RF_EMULATOR_BASE_ADDRESS + 0x818)
#define	REG_RF_EMULATOR_TAP_519                         (RF_EMULATOR_BASE_ADDRESS + 0x81C)
#define	REG_RF_EMULATOR_TAP_520                         (RF_EMULATOR_BASE_ADDRESS + 0x820)
#define	REG_RF_EMULATOR_TAP_521                         (RF_EMULATOR_BASE_ADDRESS + 0x824)
#define	REG_RF_EMULATOR_TAP_522                         (RF_EMULATOR_BASE_ADDRESS + 0x828)
#define	REG_RF_EMULATOR_TAP_523                         (RF_EMULATOR_BASE_ADDRESS + 0x82C)
#define	REG_RF_EMULATOR_TAP_524                         (RF_EMULATOR_BASE_ADDRESS + 0x830)
#define	REG_RF_EMULATOR_TAP_525                         (RF_EMULATOR_BASE_ADDRESS + 0x834)
#define	REG_RF_EMULATOR_TAP_526                         (RF_EMULATOR_BASE_ADDRESS + 0x838)
#define	REG_RF_EMULATOR_TAP_527                         (RF_EMULATOR_BASE_ADDRESS + 0x83C)
#define	REG_RF_EMULATOR_TAP_528                         (RF_EMULATOR_BASE_ADDRESS + 0x840)
#define	REG_RF_EMULATOR_TAP_529                         (RF_EMULATOR_BASE_ADDRESS + 0x844)
#define	REG_RF_EMULATOR_TAP_530                         (RF_EMULATOR_BASE_ADDRESS + 0x848)
#define	REG_RF_EMULATOR_TAP_531                         (RF_EMULATOR_BASE_ADDRESS + 0x84C)
#define	REG_RF_EMULATOR_TAP_532                         (RF_EMULATOR_BASE_ADDRESS + 0x850)
#define	REG_RF_EMULATOR_TAP_533                         (RF_EMULATOR_BASE_ADDRESS + 0x854)
#define	REG_RF_EMULATOR_TAP_534                         (RF_EMULATOR_BASE_ADDRESS + 0x858)
#define	REG_RF_EMULATOR_TAP_535                         (RF_EMULATOR_BASE_ADDRESS + 0x85C)
#define	REG_RF_EMULATOR_TAP_536                         (RF_EMULATOR_BASE_ADDRESS + 0x860)
#define	REG_RF_EMULATOR_TAP_537                         (RF_EMULATOR_BASE_ADDRESS + 0x864)
#define	REG_RF_EMULATOR_TAP_538                         (RF_EMULATOR_BASE_ADDRESS + 0x868)
#define	REG_RF_EMULATOR_TAP_539                         (RF_EMULATOR_BASE_ADDRESS + 0x86C)
#define	REG_RF_EMULATOR_TAP_540                         (RF_EMULATOR_BASE_ADDRESS + 0x870)
#define	REG_RF_EMULATOR_TAP_541                         (RF_EMULATOR_BASE_ADDRESS + 0x874)
#define	REG_RF_EMULATOR_TAP_542                         (RF_EMULATOR_BASE_ADDRESS + 0x878)
#define	REG_RF_EMULATOR_TAP_543                         (RF_EMULATOR_BASE_ADDRESS + 0x87C)
#define	REG_RF_EMULATOR_TAP_544                         (RF_EMULATOR_BASE_ADDRESS + 0x880)
#define	REG_RF_EMULATOR_TAP_545                         (RF_EMULATOR_BASE_ADDRESS + 0x884)
#define	REG_RF_EMULATOR_TAP_546                         (RF_EMULATOR_BASE_ADDRESS + 0x888)
#define	REG_RF_EMULATOR_TAP_547                         (RF_EMULATOR_BASE_ADDRESS + 0x88C)
#define	REG_RF_EMULATOR_TAP_548                         (RF_EMULATOR_BASE_ADDRESS + 0x890)
#define	REG_RF_EMULATOR_TAP_549                         (RF_EMULATOR_BASE_ADDRESS + 0x894)
#define	REG_RF_EMULATOR_TAP_550                         (RF_EMULATOR_BASE_ADDRESS + 0x898)
#define	REG_RF_EMULATOR_TAP_551                         (RF_EMULATOR_BASE_ADDRESS + 0x89C)
#define	REG_RF_EMULATOR_TAP_552                         (RF_EMULATOR_BASE_ADDRESS + 0x8A0)
#define	REG_RF_EMULATOR_TAP_553                         (RF_EMULATOR_BASE_ADDRESS + 0x8A4)
#define	REG_RF_EMULATOR_TAP_554                         (RF_EMULATOR_BASE_ADDRESS + 0x8A8)
#define	REG_RF_EMULATOR_TAP_555                         (RF_EMULATOR_BASE_ADDRESS + 0x8AC)
#define	REG_RF_EMULATOR_TAP_556                         (RF_EMULATOR_BASE_ADDRESS + 0x8B0)
#define	REG_RF_EMULATOR_TAP_557                         (RF_EMULATOR_BASE_ADDRESS + 0x8B4)
#define	REG_RF_EMULATOR_TAP_558                         (RF_EMULATOR_BASE_ADDRESS + 0x8B8)
#define	REG_RF_EMULATOR_TAP_559                         (RF_EMULATOR_BASE_ADDRESS + 0x8BC)
#define	REG_RF_EMULATOR_TAP_560                         (RF_EMULATOR_BASE_ADDRESS + 0x8C0)
#define	REG_RF_EMULATOR_TAP_561                         (RF_EMULATOR_BASE_ADDRESS + 0x8C4)
#define	REG_RF_EMULATOR_TAP_562                         (RF_EMULATOR_BASE_ADDRESS + 0x8C8)
#define	REG_RF_EMULATOR_TAP_563                         (RF_EMULATOR_BASE_ADDRESS + 0x8CC)
#define	REG_RF_EMULATOR_TAP_564                         (RF_EMULATOR_BASE_ADDRESS + 0x8D0)
#define	REG_RF_EMULATOR_TAP_565                         (RF_EMULATOR_BASE_ADDRESS + 0x8D4)
#define	REG_RF_EMULATOR_TAP_566                         (RF_EMULATOR_BASE_ADDRESS + 0x8D8)
#define	REG_RF_EMULATOR_TAP_567                         (RF_EMULATOR_BASE_ADDRESS + 0x8DC)
#define	REG_RF_EMULATOR_TAP_568                         (RF_EMULATOR_BASE_ADDRESS + 0x8E0)
#define	REG_RF_EMULATOR_TAP_569                         (RF_EMULATOR_BASE_ADDRESS + 0x8E4)
#define	REG_RF_EMULATOR_TAP_570                         (RF_EMULATOR_BASE_ADDRESS + 0x8E8)
#define	REG_RF_EMULATOR_TAP_571                         (RF_EMULATOR_BASE_ADDRESS + 0x8EC)
#define	REG_RF_EMULATOR_TAP_572                         (RF_EMULATOR_BASE_ADDRESS + 0x8F0)
#define	REG_RF_EMULATOR_TAP_573                         (RF_EMULATOR_BASE_ADDRESS + 0x8F4)
#define	REG_RF_EMULATOR_TAP_574                         (RF_EMULATOR_BASE_ADDRESS + 0x8F8)
#define	REG_RF_EMULATOR_TAP_575                         (RF_EMULATOR_BASE_ADDRESS + 0x8FC)
#define	REG_RF_EMULATOR_TAP_576                         (RF_EMULATOR_BASE_ADDRESS + 0x900)
#define	REG_RF_EMULATOR_TAP_577                         (RF_EMULATOR_BASE_ADDRESS + 0x904)
#define	REG_RF_EMULATOR_TAP_578                         (RF_EMULATOR_BASE_ADDRESS + 0x908)
#define	REG_RF_EMULATOR_TAP_579                         (RF_EMULATOR_BASE_ADDRESS + 0x90C)
#define	REG_RF_EMULATOR_TAP_580                         (RF_EMULATOR_BASE_ADDRESS + 0x910)
#define	REG_RF_EMULATOR_TAP_581                         (RF_EMULATOR_BASE_ADDRESS + 0x914)
#define	REG_RF_EMULATOR_TAP_582                         (RF_EMULATOR_BASE_ADDRESS + 0x918)
#define	REG_RF_EMULATOR_TAP_583                         (RF_EMULATOR_BASE_ADDRESS + 0x91C)
#define	REG_RF_EMULATOR_TAP_584                         (RF_EMULATOR_BASE_ADDRESS + 0x920)
#define	REG_RF_EMULATOR_TAP_585                         (RF_EMULATOR_BASE_ADDRESS + 0x924)
#define	REG_RF_EMULATOR_TAP_586                         (RF_EMULATOR_BASE_ADDRESS + 0x928)
#define	REG_RF_EMULATOR_TAP_587                         (RF_EMULATOR_BASE_ADDRESS + 0x92C)
#define	REG_RF_EMULATOR_TAP_588                         (RF_EMULATOR_BASE_ADDRESS + 0x930)
#define	REG_RF_EMULATOR_TAP_589                         (RF_EMULATOR_BASE_ADDRESS + 0x934)
#define	REG_RF_EMULATOR_TAP_590                         (RF_EMULATOR_BASE_ADDRESS + 0x938)
#define	REG_RF_EMULATOR_TAP_591                         (RF_EMULATOR_BASE_ADDRESS + 0x93C)
#define	REG_RF_EMULATOR_TAP_592                         (RF_EMULATOR_BASE_ADDRESS + 0x940)
#define	REG_RF_EMULATOR_TAP_593                         (RF_EMULATOR_BASE_ADDRESS + 0x944)
#define	REG_RF_EMULATOR_TAP_594                         (RF_EMULATOR_BASE_ADDRESS + 0x948)
#define	REG_RF_EMULATOR_TAP_595                         (RF_EMULATOR_BASE_ADDRESS + 0x94C)
#define	REG_RF_EMULATOR_TAP_596                         (RF_EMULATOR_BASE_ADDRESS + 0x950)
#define	REG_RF_EMULATOR_TAP_597                         (RF_EMULATOR_BASE_ADDRESS + 0x954)
#define	REG_RF_EMULATOR_TAP_598                         (RF_EMULATOR_BASE_ADDRESS + 0x958)
#define	REG_RF_EMULATOR_TAP_599                         (RF_EMULATOR_BASE_ADDRESS + 0x95C)
#define	REG_RF_EMULATOR_TAP_600                         (RF_EMULATOR_BASE_ADDRESS + 0x960)
#define	REG_RF_EMULATOR_TAP_601                         (RF_EMULATOR_BASE_ADDRESS + 0x964)
#define	REG_RF_EMULATOR_TAP_602                         (RF_EMULATOR_BASE_ADDRESS + 0x968)
#define	REG_RF_EMULATOR_TAP_603                         (RF_EMULATOR_BASE_ADDRESS + 0x96C)
#define	REG_RF_EMULATOR_TAP_604                         (RF_EMULATOR_BASE_ADDRESS + 0x970)
#define	REG_RF_EMULATOR_TAP_605                         (RF_EMULATOR_BASE_ADDRESS + 0x974)
#define	REG_RF_EMULATOR_TAP_606                         (RF_EMULATOR_BASE_ADDRESS + 0x978)
#define	REG_RF_EMULATOR_TAP_607                         (RF_EMULATOR_BASE_ADDRESS + 0x97C)
#define	REG_RF_EMULATOR_TAP_608                         (RF_EMULATOR_BASE_ADDRESS + 0x980)
#define	REG_RF_EMULATOR_TAP_609                         (RF_EMULATOR_BASE_ADDRESS + 0x984)
#define	REG_RF_EMULATOR_TAP_610                         (RF_EMULATOR_BASE_ADDRESS + 0x988)
#define	REG_RF_EMULATOR_TAP_611                         (RF_EMULATOR_BASE_ADDRESS + 0x98C)
#define	REG_RF_EMULATOR_TAP_612                         (RF_EMULATOR_BASE_ADDRESS + 0x990)
#define	REG_RF_EMULATOR_TAP_613                         (RF_EMULATOR_BASE_ADDRESS + 0x994)
#define	REG_RF_EMULATOR_TAP_614                         (RF_EMULATOR_BASE_ADDRESS + 0x998)
#define	REG_RF_EMULATOR_TAP_615                         (RF_EMULATOR_BASE_ADDRESS + 0x99C)
#define	REG_RF_EMULATOR_TAP_616                         (RF_EMULATOR_BASE_ADDRESS + 0x9A0)
#define	REG_RF_EMULATOR_TAP_617                         (RF_EMULATOR_BASE_ADDRESS + 0x9A4)
#define	REG_RF_EMULATOR_TAP_618                         (RF_EMULATOR_BASE_ADDRESS + 0x9A8)
#define	REG_RF_EMULATOR_TAP_619                         (RF_EMULATOR_BASE_ADDRESS + 0x9AC)
#define	REG_RF_EMULATOR_TAP_620                         (RF_EMULATOR_BASE_ADDRESS + 0x9B0)
#define	REG_RF_EMULATOR_TAP_621                         (RF_EMULATOR_BASE_ADDRESS + 0x9B4)
#define	REG_RF_EMULATOR_TAP_622                         (RF_EMULATOR_BASE_ADDRESS + 0x9B8)
#define	REG_RF_EMULATOR_TAP_623                         (RF_EMULATOR_BASE_ADDRESS + 0x9BC)
#define	REG_RF_EMULATOR_TAP_624                         (RF_EMULATOR_BASE_ADDRESS + 0x9C0)
#define	REG_RF_EMULATOR_TAP_625                         (RF_EMULATOR_BASE_ADDRESS + 0x9C4)
#define	REG_RF_EMULATOR_TAP_626                         (RF_EMULATOR_BASE_ADDRESS + 0x9C8)
#define	REG_RF_EMULATOR_TAP_627                         (RF_EMULATOR_BASE_ADDRESS + 0x9CC)
#define	REG_RF_EMULATOR_TAP_628                         (RF_EMULATOR_BASE_ADDRESS + 0x9D0)
#define	REG_RF_EMULATOR_TAP_629                         (RF_EMULATOR_BASE_ADDRESS + 0x9D4)
#define	REG_RF_EMULATOR_TAP_630                         (RF_EMULATOR_BASE_ADDRESS + 0x9D8)
#define	REG_RF_EMULATOR_TAP_631                         (RF_EMULATOR_BASE_ADDRESS + 0x9DC)
#define	REG_RF_EMULATOR_TAP_632                         (RF_EMULATOR_BASE_ADDRESS + 0x9E0)
#define	REG_RF_EMULATOR_TAP_633                         (RF_EMULATOR_BASE_ADDRESS + 0x9E4)
#define	REG_RF_EMULATOR_TAP_634                         (RF_EMULATOR_BASE_ADDRESS + 0x9E8)
#define	REG_RF_EMULATOR_TAP_635                         (RF_EMULATOR_BASE_ADDRESS + 0x9EC)
#define	REG_RF_EMULATOR_TAP_636                         (RF_EMULATOR_BASE_ADDRESS + 0x9F0)
#define	REG_RF_EMULATOR_TAP_637                         (RF_EMULATOR_BASE_ADDRESS + 0x9F4)
#define	REG_RF_EMULATOR_TAP_638                         (RF_EMULATOR_BASE_ADDRESS + 0x9F8)
#define	REG_RF_EMULATOR_TAP_639                         (RF_EMULATOR_BASE_ADDRESS + 0x9FC)
#define	REG_RF_EMULATOR_TAP_640                         (RF_EMULATOR_BASE_ADDRESS + 0xA00)
#define	REG_RF_EMULATOR_TAP_641                         (RF_EMULATOR_BASE_ADDRESS + 0xA04)
#define	REG_RF_EMULATOR_TAP_642                         (RF_EMULATOR_BASE_ADDRESS + 0xA08)
#define	REG_RF_EMULATOR_TAP_643                         (RF_EMULATOR_BASE_ADDRESS + 0xA0C)
#define	REG_RF_EMULATOR_TAP_644                         (RF_EMULATOR_BASE_ADDRESS + 0xA10)
#define	REG_RF_EMULATOR_TAP_645                         (RF_EMULATOR_BASE_ADDRESS + 0xA14)
#define	REG_RF_EMULATOR_TAP_646                         (RF_EMULATOR_BASE_ADDRESS + 0xA18)
#define	REG_RF_EMULATOR_TAP_647                         (RF_EMULATOR_BASE_ADDRESS + 0xA1C)
#define	REG_RF_EMULATOR_TAP_648                         (RF_EMULATOR_BASE_ADDRESS + 0xA20)
#define	REG_RF_EMULATOR_TAP_649                         (RF_EMULATOR_BASE_ADDRESS + 0xA24)
#define	REG_RF_EMULATOR_TAP_650                         (RF_EMULATOR_BASE_ADDRESS + 0xA28)
#define	REG_RF_EMULATOR_TAP_651                         (RF_EMULATOR_BASE_ADDRESS + 0xA2C)
#define	REG_RF_EMULATOR_TAP_652                         (RF_EMULATOR_BASE_ADDRESS + 0xA30)
#define	REG_RF_EMULATOR_TAP_653                         (RF_EMULATOR_BASE_ADDRESS + 0xA34)
#define	REG_RF_EMULATOR_TAP_654                         (RF_EMULATOR_BASE_ADDRESS + 0xA38)
#define	REG_RF_EMULATOR_TAP_655                         (RF_EMULATOR_BASE_ADDRESS + 0xA3C)
#define	REG_RF_EMULATOR_TAP_656                         (RF_EMULATOR_BASE_ADDRESS + 0xA40)
#define	REG_RF_EMULATOR_TAP_657                         (RF_EMULATOR_BASE_ADDRESS + 0xA44)
#define	REG_RF_EMULATOR_TAP_658                         (RF_EMULATOR_BASE_ADDRESS + 0xA48)
#define	REG_RF_EMULATOR_TAP_659                         (RF_EMULATOR_BASE_ADDRESS + 0xA4C)
#define	REG_RF_EMULATOR_TAP_660                         (RF_EMULATOR_BASE_ADDRESS + 0xA50)
#define	REG_RF_EMULATOR_TAP_661                         (RF_EMULATOR_BASE_ADDRESS + 0xA54)
#define	REG_RF_EMULATOR_TAP_662                         (RF_EMULATOR_BASE_ADDRESS + 0xA58)
#define	REG_RF_EMULATOR_TAP_663                         (RF_EMULATOR_BASE_ADDRESS + 0xA5C)
#define	REG_RF_EMULATOR_TAP_664                         (RF_EMULATOR_BASE_ADDRESS + 0xA60)
#define	REG_RF_EMULATOR_TAP_665                         (RF_EMULATOR_BASE_ADDRESS + 0xA64)
#define	REG_RF_EMULATOR_TAP_666                         (RF_EMULATOR_BASE_ADDRESS + 0xA68)
#define	REG_RF_EMULATOR_TAP_667                         (RF_EMULATOR_BASE_ADDRESS + 0xA6C)
#define	REG_RF_EMULATOR_TAP_668                         (RF_EMULATOR_BASE_ADDRESS + 0xA70)
#define	REG_RF_EMULATOR_TAP_669                         (RF_EMULATOR_BASE_ADDRESS + 0xA74)
#define	REG_RF_EMULATOR_TAP_670                         (RF_EMULATOR_BASE_ADDRESS + 0xA78)
#define	REG_RF_EMULATOR_TAP_671                         (RF_EMULATOR_BASE_ADDRESS + 0xA7C)
#define	REG_RF_EMULATOR_TAP_672                         (RF_EMULATOR_BASE_ADDRESS + 0xA80)
#define	REG_RF_EMULATOR_TAP_673                         (RF_EMULATOR_BASE_ADDRESS + 0xA84)
#define	REG_RF_EMULATOR_TAP_674                         (RF_EMULATOR_BASE_ADDRESS + 0xA88)
#define	REG_RF_EMULATOR_TAP_675                         (RF_EMULATOR_BASE_ADDRESS + 0xA8C)
#define	REG_RF_EMULATOR_TAP_676                         (RF_EMULATOR_BASE_ADDRESS + 0xA90)
#define	REG_RF_EMULATOR_TAP_677                         (RF_EMULATOR_BASE_ADDRESS + 0xA94)
#define	REG_RF_EMULATOR_TAP_678                         (RF_EMULATOR_BASE_ADDRESS + 0xA98)
#define	REG_RF_EMULATOR_TAP_679                         (RF_EMULATOR_BASE_ADDRESS + 0xA9C)
#define	REG_RF_EMULATOR_TAP_680                         (RF_EMULATOR_BASE_ADDRESS + 0xAA0)
#define	REG_RF_EMULATOR_TAP_681                         (RF_EMULATOR_BASE_ADDRESS + 0xAA4)
#define	REG_RF_EMULATOR_TAP_682                         (RF_EMULATOR_BASE_ADDRESS + 0xAA8)
#define	REG_RF_EMULATOR_TAP_683                         (RF_EMULATOR_BASE_ADDRESS + 0xAAC)
#define	REG_RF_EMULATOR_TAP_684                         (RF_EMULATOR_BASE_ADDRESS + 0xAB0)
#define	REG_RF_EMULATOR_TAP_685                         (RF_EMULATOR_BASE_ADDRESS + 0xAB4)
#define	REG_RF_EMULATOR_TAP_686                         (RF_EMULATOR_BASE_ADDRESS + 0xAB8)
#define	REG_RF_EMULATOR_TAP_687                         (RF_EMULATOR_BASE_ADDRESS + 0xABC)
#define	REG_RF_EMULATOR_TAP_688                         (RF_EMULATOR_BASE_ADDRESS + 0xAC0)
#define	REG_RF_EMULATOR_TAP_689                         (RF_EMULATOR_BASE_ADDRESS + 0xAC4)
#define	REG_RF_EMULATOR_TAP_690                         (RF_EMULATOR_BASE_ADDRESS + 0xAC8)
#define	REG_RF_EMULATOR_TAP_691                         (RF_EMULATOR_BASE_ADDRESS + 0xACC)
#define	REG_RF_EMULATOR_TAP_692                         (RF_EMULATOR_BASE_ADDRESS + 0xAD0)
#define	REG_RF_EMULATOR_TAP_693                         (RF_EMULATOR_BASE_ADDRESS + 0xAD4)
#define	REG_RF_EMULATOR_RX_SNR                          (RF_EMULATOR_BASE_ADDRESS + 0xB00)
#define	REG_RF_EMULATOR_TX_SNR                          (RF_EMULATOR_BASE_ADDRESS + 0xB04)
#define	REG_RF_EMULATOR_RX_ANT0_SEED0                   (RF_EMULATOR_BASE_ADDRESS + 0xB08)
#define	REG_RF_EMULATOR_RX_ANT0_SEED1                   (RF_EMULATOR_BASE_ADDRESS + 0xB0C)
#define	REG_RF_EMULATOR_RX_ANT0_SEED2                   (RF_EMULATOR_BASE_ADDRESS + 0xB10)
#define	REG_RF_EMULATOR_RX_ANT1_SEED0                   (RF_EMULATOR_BASE_ADDRESS + 0xB14)
#define	REG_RF_EMULATOR_RX_ANT1_SEED1                   (RF_EMULATOR_BASE_ADDRESS + 0xB18)
#define	REG_RF_EMULATOR_RX_ANT1_SEED2                   (RF_EMULATOR_BASE_ADDRESS + 0xB1C)
#define	REG_RF_EMULATOR_RX_ANT2_SEED0                   (RF_EMULATOR_BASE_ADDRESS + 0xB20)
#define	REG_RF_EMULATOR_RX_ANT2_SEED1                   (RF_EMULATOR_BASE_ADDRESS + 0xB24)
#define	REG_RF_EMULATOR_RX_ANT2_SEED2                   (RF_EMULATOR_BASE_ADDRESS + 0xB28)
#define	REG_RF_EMULATOR_RX_ANT3_SEED0                   (RF_EMULATOR_BASE_ADDRESS + 0xB2C)
#define	REG_RF_EMULATOR_RX_ANT3_SEED1                   (RF_EMULATOR_BASE_ADDRESS + 0xB30)
#define	REG_RF_EMULATOR_RX_ANT3_SEED2                   (RF_EMULATOR_BASE_ADDRESS + 0xB34)
#define	REG_RF_EMULATOR_TX_ANT0_SEED0                   (RF_EMULATOR_BASE_ADDRESS + 0xB38)
#define	REG_RF_EMULATOR_TX_ANT0_SEED1                   (RF_EMULATOR_BASE_ADDRESS + 0xB3C)
#define	REG_RF_EMULATOR_TX_ANT0_SEED2                   (RF_EMULATOR_BASE_ADDRESS + 0xB40)
#define	REG_RF_EMULATOR_TX_ANT1_SEED0                   (RF_EMULATOR_BASE_ADDRESS + 0xB44)
#define	REG_RF_EMULATOR_TX_ANT1_SEED1                   (RF_EMULATOR_BASE_ADDRESS + 0xB48)
#define	REG_RF_EMULATOR_TX_ANT1_SEED2                   (RF_EMULATOR_BASE_ADDRESS + 0xB4C)
#define	REG_RF_EMULATOR_TX_ANT2_SEED0                   (RF_EMULATOR_BASE_ADDRESS + 0xB50)
#define	REG_RF_EMULATOR_TX_ANT2_SEED1                   (RF_EMULATOR_BASE_ADDRESS + 0xB54)
#define	REG_RF_EMULATOR_TX_ANT2_SEED2                   (RF_EMULATOR_BASE_ADDRESS + 0xB58)
#define	REG_RF_EMULATOR_TX_ANT3_SEED0                   (RF_EMULATOR_BASE_ADDRESS + 0xB5C)
#define	REG_RF_EMULATOR_TX_ANT3_SEED1                   (RF_EMULATOR_BASE_ADDRESS + 0xB60)
#define	REG_RF_EMULATOR_TX_ANT3_SEED2                   (RF_EMULATOR_BASE_ADDRESS + 0xB64)
#define	REG_RF_EMULATOR_LOSS                            (RF_EMULATOR_BASE_ADDRESS + 0xB68)
#define	REG_RF_EMULATOR_RF_CONTROL                      (RF_EMULATOR_BASE_ADDRESS + 0xB70)
#define	REG_RF_EMULATOR_TSSI2_ANT0                      (RF_EMULATOR_BASE_ADDRESS + 0xB74)
#define	REG_RF_EMULATOR_TSSI2_ANT1                      (RF_EMULATOR_BASE_ADDRESS + 0xB78)
#define	REG_RF_EMULATOR_TSSI2_ANT2                      (RF_EMULATOR_BASE_ADDRESS + 0xB7C)
#define	REG_RF_EMULATOR_TSSI2_ANT3                      (RF_EMULATOR_BASE_ADDRESS + 0xB80)
#define	REG_RF_EMULATOR_TSSI5_ANT0                      (RF_EMULATOR_BASE_ADDRESS + 0xB84)
#define	REG_RF_EMULATOR_TSSI5_ANT1                      (RF_EMULATOR_BASE_ADDRESS + 0xB88)
#define	REG_RF_EMULATOR_TSSI5_ANT2                      (RF_EMULATOR_BASE_ADDRESS + 0xB8C)
#define	REG_RF_EMULATOR_TSSI5_ANT3                      (RF_EMULATOR_BASE_ADDRESS + 0xB90)
#define	REG_RF_EMULATOR_FECTRL_ANT                      (RF_EMULATOR_BASE_ADDRESS + 0xB94)
#define	REG_RF_EMULATOR_FECTRL_C                        (RF_EMULATOR_BASE_ADDRESS + 0xB98)
#define	REG_RF_EMULATOR_IMP_ANT0_PGA_IQ_GMM             (RF_EMULATOR_BASE_ADDRESS + 0xBA0)
#define	REG_RF_EMULATOR_IMP_ANT0_PGA_IQ_PMM             (RF_EMULATOR_BASE_ADDRESS + 0xBA4)
#define	REG_RF_EMULATOR_IMP_ANT1_PGA_IQ_GMM             (RF_EMULATOR_BASE_ADDRESS + 0xBA8)
#define	REG_RF_EMULATOR_IMP_ANT1_PGA_IQ_PMM             (RF_EMULATOR_BASE_ADDRESS + 0xBAC)
#define	REG_RF_EMULATOR_IMP_ANT2_PGA_IQ_GMM             (RF_EMULATOR_BASE_ADDRESS + 0xBB0)
#define	REG_RF_EMULATOR_IMP_ANT2_PGA_IQ_PMM             (RF_EMULATOR_BASE_ADDRESS + 0xBB4)
#define	REG_RF_EMULATOR_IMP_ANT3_PGA_IQ_GMM             (RF_EMULATOR_BASE_ADDRESS + 0xBB8)
#define	REG_RF_EMULATOR_IMP_ANT3_PGA_IQ_PMM             (RF_EMULATOR_BASE_ADDRESS + 0xBBC)
#define	REG_RF_EMULATOR_IMP_ANT0_TPC_DC_I               (RF_EMULATOR_BASE_ADDRESS + 0xBC0)
#define	REG_RF_EMULATOR_IMP_ANT0_TPC_DC_Q               (RF_EMULATOR_BASE_ADDRESS + 0xBC4)
#define	REG_RF_EMULATOR_IMP_ANT1_TPC_DC_I               (RF_EMULATOR_BASE_ADDRESS + 0xBC8)
#define	REG_RF_EMULATOR_IMP_ANT1_TPC_DC_Q               (RF_EMULATOR_BASE_ADDRESS + 0xBCC)
#define	REG_RF_EMULATOR_IMP_ANT2_TPC_DC_I               (RF_EMULATOR_BASE_ADDRESS + 0xBD0)
#define	REG_RF_EMULATOR_IMP_ANT2_TPC_DC_Q               (RF_EMULATOR_BASE_ADDRESS + 0xBD4)
#define	REG_RF_EMULATOR_IMP_ANT3_TPC_DC_I               (RF_EMULATOR_BASE_ADDRESS + 0xBD8)
#define	REG_RF_EMULATOR_IMP_ANT3_TPC_DC_Q               (RF_EMULATOR_BASE_ADDRESS + 0xBDC)
#define	REG_RF_EMULATOR_IMP_ANT0_PGA_DC_OFFSET_I        (RF_EMULATOR_BASE_ADDRESS + 0xBE0)
#define	REG_RF_EMULATOR_IMP_ANT0_PASSIVEMIXER_IQ_GMM    (RF_EMULATOR_BASE_ADDRESS + 0xBE0)
#define	REG_RF_EMULATOR_IMP_ANT0_PGA_DC_OFFSET_Q        (RF_EMULATOR_BASE_ADDRESS + 0xBE4)
#define	REG_RF_EMULATOR_IMP_ANT0_PASSIVEMIXER_IQ_PMM    (RF_EMULATOR_BASE_ADDRESS + 0xBE4)
#define	REG_RF_EMULATOR_IMP_ANT1_PASSIVEMIXER_IQ_GMM    (RF_EMULATOR_BASE_ADDRESS + 0xBE8)
#define	REG_RF_EMULATOR_IMP_ANT1_PGA_DC_OFFSET_I        (RF_EMULATOR_BASE_ADDRESS + 0xBE8)
#define	REG_RF_EMULATOR_IMP_ANT1_PASSIVEMIXER_IQ_PMM    (RF_EMULATOR_BASE_ADDRESS + 0xBEC)
#define	REG_RF_EMULATOR_IMP_ANT1_PGA_DC_OFFSET_Q        (RF_EMULATOR_BASE_ADDRESS + 0xBEC)
#define	REG_RF_EMULATOR_IMP_ANT2_PGA_DC_OFFSET_I        (RF_EMULATOR_BASE_ADDRESS + 0xBF0)
#define	REG_RF_EMULATOR_IMP_ANT2_PASSIVEMIXER_IQ_GMM    (RF_EMULATOR_BASE_ADDRESS + 0xBF0)
#define	REG_RF_EMULATOR_IMP_ANT2_PGA_DC_OFFSET_Q        (RF_EMULATOR_BASE_ADDRESS + 0xBF4)
#define	REG_RF_EMULATOR_IMP_ANT2_PASSIVEMIXER_IQ_PMM    (RF_EMULATOR_BASE_ADDRESS + 0xBF4)
#define	REG_RF_EMULATOR_IMP_ANT3_PASSIVEMIXER_IQ_GMM    (RF_EMULATOR_BASE_ADDRESS + 0xBF8)
#define	REG_RF_EMULATOR_IMP_ANT3_PGA_DC_OFFSET_I        (RF_EMULATOR_BASE_ADDRESS + 0xBF8)
#define	REG_RF_EMULATOR_IMP_ANT3_PGA_DC_OFFSET_Q        (RF_EMULATOR_BASE_ADDRESS + 0xBFC)
#define	REG_RF_EMULATOR_IMP_ANT3_PASSIVEMIXER_IQ_PMM    (RF_EMULATOR_BASE_ADDRESS + 0xBFC)
#define	REG_RF_EMULATOR_SPARE_REG                       (RF_EMULATOR_BASE_ADDRESS + 0xC00)
#define	REG_RF_EMULATOR_RF_CONTROL_EXT                  (RF_EMULATOR_BASE_ADDRESS + 0xC04)
#define	REG_RF_EMULATOR_EXTERNAL_PSEL                   (RF_EMULATOR_BASE_ADDRESS + 0xD00)
/*---------------------------------------------------------------------------------
/						Data Type Definition										
/----------------------------------------------------------------------------------*/
/*REG_RF_EMULATOR_TAP_0 0x0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h11Re0 : 10; //h11_re_0, reset value: 0x1ff, access type: RW
		uint32 h11Im0 : 10; //h11_im_0, reset value: 0x0, access type: RW
		uint32 h11Re1 : 10; //h11_re_1, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap0_u;

/*REG_RF_EMULATOR_TAP_1 0x4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h11Im1 : 10; //h11_im_1, reset value: 0x0, access type: RW
		uint32 h11Re2 : 10; //h11_re_2, reset value: 0x0, access type: RW
		uint32 h11Im2 : 10; //h11_im_2, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap1_u;

/*REG_RF_EMULATOR_TAP_2 0x8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h11Re3 : 10; //h11_re_3, reset value: 0x0, access type: RW
		uint32 h11Im3 : 10; //h11_im_3, reset value: 0x0, access type: RW
		uint32 h11Re4 : 10; //h11_re_4, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap2_u;

/*REG_RF_EMULATOR_TAP_3 0xC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h11Im4 : 10; //h11_im_4, reset value: 0x0, access type: RW
		uint32 h11Re5 : 10; //h11_re_5, reset value: 0x0, access type: RW
		uint32 h11Im5 : 10; //h11_im_5, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap3_u;

/*REG_RF_EMULATOR_TAP_4 0x10 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h11Re6 : 10; //h11_re_6, reset value: 0x0, access type: RW
		uint32 h11Im6 : 10; //h11_im_6, reset value: 0x0, access type: RW
		uint32 h11Re7 : 10; //h11_re_7, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap4_u;

/*REG_RF_EMULATOR_TAP_5 0x14 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h11Im7 : 10; //h11_im_7, reset value: 0x0, access type: RW
		uint32 h11Re8 : 10; //h11_re_8, reset value: 0x0, access type: RW
		uint32 h11Im8 : 10; //h11_im_8, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap5_u;

/*REG_RF_EMULATOR_TAP_6 0x18 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h11Re9 : 10; //h11_re_9, reset value: 0x0, access type: RW
		uint32 h11Im9 : 10; //h11_im_9, reset value: 0x0, access type: RW
		uint32 h11Re10 : 10; //h11_re_10, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap6_u;

/*REG_RF_EMULATOR_TAP_7 0x1C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h11Im10 : 10; //h11_im_10, reset value: 0x0, access type: RW
		uint32 h11Re11 : 10; //h11_re_11, reset value: 0x0, access type: RW
		uint32 h11Im11 : 10; //h11_im_11, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap7_u;

/*REG_RF_EMULATOR_TAP_8 0x20 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h11Re12 : 10; //h11_re_12, reset value: 0x0, access type: RW
		uint32 h11Im12 : 10; //h11_im_12, reset value: 0x0, access type: RW
		uint32 h11Re13 : 10; //h11_re_13, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap8_u;

/*REG_RF_EMULATOR_TAP_9 0x24 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h11Im13 : 10; //h11_im_13, reset value: 0x0, access type: RW
		uint32 h11Re14 : 10; //h11_re_14, reset value: 0x0, access type: RW
		uint32 h11Im14 : 10; //h11_im_14, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap9_u;

/*REG_RF_EMULATOR_TAP_10 0x28 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h11Re15 : 10; //h11_re_15, reset value: 0x0, access type: RW
		uint32 h11Im15 : 10; //h11_im_15, reset value: 0x0, access type: RW
		uint32 h11Re16 : 10; //h11_re_16, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap10_u;

/*REG_RF_EMULATOR_TAP_11 0x2C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h11Im16 : 10; //h11_im_16, reset value: 0x0, access type: RW
		uint32 h11Re17 : 10; //h11_re_17, reset value: 0x0, access type: RW
		uint32 h11Im17 : 10; //h11_im_17, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap11_u;

/*REG_RF_EMULATOR_TAP_12 0x30 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h11Re18 : 10; //h11_re_18, reset value: 0x0, access type: RW
		uint32 h11Im18 : 10; //h11_im_18, reset value: 0x0, access type: RW
		uint32 h11Re19 : 10; //h11_re_19, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap12_u;

/*REG_RF_EMULATOR_TAP_13 0x34 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h11Im19 : 10; //h11_im_19, reset value: 0x0, access type: RW
		uint32 h11Re20 : 10; //h11_re_20, reset value: 0x0, access type: RW
		uint32 h11Im20 : 10; //h11_im_20, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap13_u;

/*REG_RF_EMULATOR_TAP_14 0x38 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h11Re21 : 10; //h11_re_21, reset value: 0x0, access type: RW
		uint32 h11Im21 : 10; //h11_im_21, reset value: 0x0, access type: RW
		uint32 h11Re22 : 10; //h11_re_22, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap14_u;

/*REG_RF_EMULATOR_TAP_15 0x3C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h11Im22 : 10; //h11_im_22, reset value: 0x0, access type: RW
		uint32 h11Re23 : 10; //h11_re_23, reset value: 0x0, access type: RW
		uint32 h11Im23 : 10; //h11_im_23, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap15_u;

/*REG_RF_EMULATOR_TAP_16 0x40 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h11Re24 : 10; //h11_re_24, reset value: 0x0, access type: RW
		uint32 h11Im24 : 10; //h11_im_24, reset value: 0x0, access type: RW
		uint32 h11Re25 : 10; //h11_re_25, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap16_u;

/*REG_RF_EMULATOR_TAP_17 0x44 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h11Im25 : 10; //h11_im_25, reset value: 0x0, access type: RW
		uint32 h11Re26 : 10; //h11_re_26, reset value: 0x0, access type: RW
		uint32 h11Im26 : 10; //h11_im_26, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap17_u;

/*REG_RF_EMULATOR_TAP_18 0x48 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h11Re27 : 10; //h11_re_27, reset value: 0x0, access type: RW
		uint32 h11Im27 : 10; //h11_im_27, reset value: 0x0, access type: RW
		uint32 h11Re28 : 10; //h11_re_28, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap18_u;

/*REG_RF_EMULATOR_TAP_19 0x4C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h11Im28 : 10; //h11_im_28, reset value: 0x0, access type: RW
		uint32 h11Re29 : 10; //h11_re_29, reset value: 0x0, access type: RW
		uint32 h11Im29 : 10; //h11_im_29, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap19_u;

/*REG_RF_EMULATOR_TAP_20 0x50 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h11Re30 : 10; //h11_re_30, reset value: 0x0, access type: RW
		uint32 h11Im30 : 10; //h11_im_30, reset value: 0x0, access type: RW
		uint32 h11Re31 : 10; //h11_re_31, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap20_u;

/*REG_RF_EMULATOR_TAP_21 0x54 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h11Im31 : 10; //h11_im_31, reset value: 0x0, access type: RW
		uint32 h11Re32 : 10; //h11_re_32, reset value: 0x0, access type: RW
		uint32 h11Im32 : 10; //h11_im_32, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap21_u;

/*REG_RF_EMULATOR_TAP_22 0x58 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h11Re33 : 10; //h11_re_33, reset value: 0x0, access type: RW
		uint32 h11Im33 : 10; //h11_im_33, reset value: 0x0, access type: RW
		uint32 h11Re34 : 10; //h11_re_34, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap22_u;

/*REG_RF_EMULATOR_TAP_23 0x5C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h11Im34 : 10; //h11_im_34, reset value: 0x0, access type: RW
		uint32 h11Re35 : 10; //h11_re_35, reset value: 0x0, access type: RW
		uint32 h11Im35 : 10; //h11_im_35, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap23_u;

/*REG_RF_EMULATOR_TAP_24 0x60 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h11Re36 : 10; //h11_re_36, reset value: 0x0, access type: RW
		uint32 h11Im36 : 10; //h11_im_36, reset value: 0x0, access type: RW
		uint32 h11Re37 : 10; //h11_re_37, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap24_u;

/*REG_RF_EMULATOR_TAP_25 0x64 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h11Im37 : 10; //h11_im_37, reset value: 0x0, access type: RW
		uint32 h11Re38 : 10; //h11_re_38, reset value: 0x0, access type: RW
		uint32 h11Im38 : 10; //h11_im_38, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap25_u;

/*REG_RF_EMULATOR_TAP_26 0x68 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h11Re39 : 10; //h11_re_39, reset value: 0x0, access type: RW
		uint32 h11Im39 : 10; //h11_im_39, reset value: 0x0, access type: RW
		uint32 h11Re40 : 10; //h11_re_40, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap26_u;

/*REG_RF_EMULATOR_TAP_27 0x6C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h11Im40 : 10; //h11_im_40, reset value: 0x0, access type: RW
		uint32 h11Re41 : 10; //h11_re_41, reset value: 0x0, access type: RW
		uint32 h11Im41 : 10; //h11_im_41, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap27_u;

/*REG_RF_EMULATOR_TAP_28 0x70 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h11Re42 : 10; //h11_re_42, reset value: 0x0, access type: RW
		uint32 h11Im42 : 10; //h11_im_42, reset value: 0x0, access type: RW
		uint32 h11Re43 : 10; //h11_re_43, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap28_u;

/*REG_RF_EMULATOR_TAP_29 0x74 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h11Im43 : 10; //h11_im_43, reset value: 0x0, access type: RW
		uint32 h11Re44 : 10; //h11_re_44, reset value: 0x0, access type: RW
		uint32 h11Im44 : 10; //h11_im_44, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap29_u;

/*REG_RF_EMULATOR_TAP_30 0x78 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h11Re45 : 10; //h11_re_45, reset value: 0x0, access type: RW
		uint32 h11Im45 : 10; //h11_im_45, reset value: 0x0, access type: RW
		uint32 h11Re46 : 10; //h11_re_46, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap30_u;

/*REG_RF_EMULATOR_TAP_31 0x7C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h11Im46 : 10; //h11_im_46, reset value: 0x0, access type: RW
		uint32 h11Re47 : 10; //h11_re_47, reset value: 0x0, access type: RW
		uint32 h11Im47 : 10; //h11_im_47, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap31_u;

/*REG_RF_EMULATOR_TAP_32 0x80 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h11Re48 : 10; //h11_re_48, reset value: 0x0, access type: RW
		uint32 h11Im48 : 10; //h11_im_48, reset value: 0x0, access type: RW
		uint32 h11Re49 : 10; //h11_re_49, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap32_u;

/*REG_RF_EMULATOR_TAP_33 0x84 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h11Im49 : 10; //h11_im_49, reset value: 0x0, access type: RW
		uint32 h11Re50 : 10; //h11_re_50, reset value: 0x0, access type: RW
		uint32 h11Im50 : 10; //h11_im_50, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap33_u;

/*REG_RF_EMULATOR_TAP_34 0x88 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h11Re51 : 10; //h11_re_51, reset value: 0x0, access type: RW
		uint32 h11Im51 : 10; //h11_im_51, reset value: 0x0, access type: RW
		uint32 h11Re52 : 10; //h11_re_52, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap34_u;

/*REG_RF_EMULATOR_TAP_35 0x8C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h11Im52 : 10; //h11_im_52, reset value: 0x0, access type: RW
		uint32 h11Re53 : 10; //h11_re_53, reset value: 0x0, access type: RW
		uint32 h11Im53 : 10; //h11_im_53, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap35_u;

/*REG_RF_EMULATOR_TAP_36 0x90 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h11Re54 : 10; //h11_re_54, reset value: 0x0, access type: RW
		uint32 h11Im54 : 10; //h11_im_54, reset value: 0x0, access type: RW
		uint32 h11Re55 : 10; //h11_re_55, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap36_u;

/*REG_RF_EMULATOR_TAP_37 0x94 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h11Im55 : 10; //h11_im_55, reset value: 0x0, access type: RW
		uint32 h11Re56 : 10; //h11_re_56, reset value: 0x0, access type: RW
		uint32 h11Im56 : 10; //h11_im_56, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap37_u;

/*REG_RF_EMULATOR_TAP_38 0x98 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h11Re57 : 10; //h11_re_57, reset value: 0x0, access type: RW
		uint32 h11Im57 : 10; //h11_im_57, reset value: 0x0, access type: RW
		uint32 h11Re58 : 10; //h11_re_58, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap38_u;

/*REG_RF_EMULATOR_TAP_39 0x9C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h11Im58 : 10; //h11_im_58, reset value: 0x0, access type: RW
		uint32 h11Re59 : 10; //h11_re_59, reset value: 0x0, access type: RW
		uint32 h11Im59 : 10; //h11_im_59, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap39_u;

/*REG_RF_EMULATOR_TAP_40 0xA0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h11Re60 : 10; //h11_re_60, reset value: 0x0, access type: RW
		uint32 h11Im60 : 10; //h11_im_60, reset value: 0x0, access type: RW
		uint32 h11Re61 : 10; //h11_re_61, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap40_u;

/*REG_RF_EMULATOR_TAP_41 0xA4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h11Im61 : 10; //h11_im_61, reset value: 0x0, access type: RW
		uint32 h11Re62 : 10; //h11_re_62, reset value: 0x0, access type: RW
		uint32 h11Im62 : 10; //h11_im_62, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap41_u;

/*REG_RF_EMULATOR_TAP_42 0xA8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h11Re63 : 10; //h11_re_63, reset value: 0x0, access type: RW
		uint32 h11Im63 : 10; //h11_im_63, reset value: 0x0, access type: RW
		uint32 h11Re64 : 10; //h11_re_64, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap42_u;

/*REG_RF_EMULATOR_TAP_43 0xAC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h11Im64 : 10; //h11_im_64, reset value: 0x0, access type: RW
		uint32 h12Re0 : 10; //h12_re_0, reset value: 0x0, access type: RW
		uint32 h12Im0 : 10; //h12_im_0, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap43_u;

/*REG_RF_EMULATOR_TAP_44 0xB0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h12Re1 : 10; //h12_re_1, reset value: 0x0, access type: RW
		uint32 h12Im1 : 10; //h12_im_1, reset value: 0x0, access type: RW
		uint32 h12Re2 : 10; //h12_re_2, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap44_u;

/*REG_RF_EMULATOR_TAP_45 0xB4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h12Im2 : 10; //h12_im_2, reset value: 0x0, access type: RW
		uint32 h12Re3 : 10; //h12_re_3, reset value: 0x0, access type: RW
		uint32 h12Im3 : 10; //h12_im_3, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap45_u;

/*REG_RF_EMULATOR_TAP_46 0xB8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h12Re4 : 10; //h12_re_4, reset value: 0x0, access type: RW
		uint32 h12Im4 : 10; //h12_im_4, reset value: 0x0, access type: RW
		uint32 h12Re5 : 10; //h12_re_5, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap46_u;

/*REG_RF_EMULATOR_TAP_47 0xBC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h12Im5 : 10; //h12_im_5, reset value: 0x0, access type: RW
		uint32 h12Re6 : 10; //h12_re_6, reset value: 0x0, access type: RW
		uint32 h12Im6 : 10; //h12_im_6, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap47_u;

/*REG_RF_EMULATOR_TAP_48 0xC0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h12Re7 : 10; //h12_re_7, reset value: 0x0, access type: RW
		uint32 h12Im7 : 10; //h12_im_7, reset value: 0x0, access type: RW
		uint32 h12Re8 : 10; //h12_re_8, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap48_u;

/*REG_RF_EMULATOR_TAP_49 0xC4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h12Im8 : 10; //h12_im_8, reset value: 0x0, access type: RW
		uint32 h12Re9 : 10; //h12_re_9, reset value: 0x0, access type: RW
		uint32 h12Im9 : 10; //h12_im_9, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap49_u;

/*REG_RF_EMULATOR_TAP_50 0xC8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h12Re10 : 10; //h12_re_10, reset value: 0x0, access type: RW
		uint32 h12Im10 : 10; //h12_im_10, reset value: 0x0, access type: RW
		uint32 h12Re11 : 10; //h12_re_11, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap50_u;

/*REG_RF_EMULATOR_TAP_51 0xCC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h12Im11 : 10; //h12_im_11, reset value: 0x0, access type: RW
		uint32 h12Re12 : 10; //h12_re_12, reset value: 0x0, access type: RW
		uint32 h12Im12 : 10; //h12_im_12, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap51_u;

/*REG_RF_EMULATOR_TAP_52 0xD0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h12Re13 : 10; //h12_re_13, reset value: 0x0, access type: RW
		uint32 h12Im13 : 10; //h12_im_13, reset value: 0x0, access type: RW
		uint32 h12Re14 : 10; //h12_re_14, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap52_u;

/*REG_RF_EMULATOR_TAP_53 0xD4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h12Im14 : 10; //h12_im_14, reset value: 0x0, access type: RW
		uint32 h12Re15 : 10; //h12_re_15, reset value: 0x0, access type: RW
		uint32 h12Im15 : 10; //h12_im_15, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap53_u;

/*REG_RF_EMULATOR_TAP_54 0xD8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h12Re16 : 10; //h12_re_16, reset value: 0x0, access type: RW
		uint32 h12Im16 : 10; //h12_im_16, reset value: 0x0, access type: RW
		uint32 h12Re17 : 10; //h12_re_17, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap54_u;

/*REG_RF_EMULATOR_TAP_55 0xDC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h12Im17 : 10; //h12_im_17, reset value: 0x0, access type: RW
		uint32 h12Re18 : 10; //h12_re_18, reset value: 0x0, access type: RW
		uint32 h12Im18 : 10; //h12_im_18, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap55_u;

/*REG_RF_EMULATOR_TAP_56 0xE0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h12Re19 : 10; //h12_re_19, reset value: 0x0, access type: RW
		uint32 h12Im19 : 10; //h12_im_19, reset value: 0x0, access type: RW
		uint32 h12Re20 : 10; //h12_re_20, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap56_u;

/*REG_RF_EMULATOR_TAP_57 0xE4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h12Im20 : 10; //h12_im_20, reset value: 0x0, access type: RW
		uint32 h12Re21 : 10; //h12_re_21, reset value: 0x0, access type: RW
		uint32 h12Im21 : 10; //h12_im_21, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap57_u;

/*REG_RF_EMULATOR_TAP_58 0xE8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h12Re22 : 10; //h12_re_22, reset value: 0x0, access type: RW
		uint32 h12Im22 : 10; //h12_im_22, reset value: 0x0, access type: RW
		uint32 h12Re23 : 10; //h12_re_23, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap58_u;

/*REG_RF_EMULATOR_TAP_59 0xEC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h12Im23 : 10; //h12_im_23, reset value: 0x0, access type: RW
		uint32 h12Re24 : 10; //h12_re_24, reset value: 0x0, access type: RW
		uint32 h12Im24 : 10; //h12_im_24, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap59_u;

/*REG_RF_EMULATOR_TAP_60 0xF0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h12Re25 : 10; //h12_re_25, reset value: 0x0, access type: RW
		uint32 h12Im25 : 10; //h12_im_25, reset value: 0x0, access type: RW
		uint32 h12Re26 : 10; //h12_re_26, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap60_u;

/*REG_RF_EMULATOR_TAP_61 0xF4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h12Im26 : 10; //h12_im_26, reset value: 0x0, access type: RW
		uint32 h12Re27 : 10; //h12_re_27, reset value: 0x0, access type: RW
		uint32 h12Im27 : 10; //h12_im_27, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap61_u;

/*REG_RF_EMULATOR_TAP_62 0xF8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h12Re28 : 10; //h12_re_28, reset value: 0x0, access type: RW
		uint32 h12Im28 : 10; //h12_im_28, reset value: 0x0, access type: RW
		uint32 h12Re29 : 10; //h12_re_29, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap62_u;

/*REG_RF_EMULATOR_TAP_63 0xFC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h12Im29 : 10; //h12_im_29, reset value: 0x0, access type: RW
		uint32 h12Re30 : 10; //h12_re_30, reset value: 0x0, access type: RW
		uint32 h12Im30 : 10; //h12_im_30, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap63_u;

/*REG_RF_EMULATOR_TAP_64 0x100 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h12Re31 : 10; //h12_re_31, reset value: 0x0, access type: RW
		uint32 h12Im31 : 10; //h12_im_31, reset value: 0x0, access type: RW
		uint32 h12Re32 : 10; //h12_re_32, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap64_u;

/*REG_RF_EMULATOR_TAP_65 0x104 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h12Im32 : 10; //h12_im_32, reset value: 0x0, access type: RW
		uint32 h12Re33 : 10; //h12_re_33, reset value: 0x0, access type: RW
		uint32 h12Im33 : 10; //h12_im_33, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap65_u;

/*REG_RF_EMULATOR_TAP_66 0x108 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h12Re34 : 10; //h12_re_34, reset value: 0x0, access type: RW
		uint32 h12Im34 : 10; //h12_im_34, reset value: 0x0, access type: RW
		uint32 h12Re35 : 10; //h12_re_35, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap66_u;

/*REG_RF_EMULATOR_TAP_67 0x10C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h12Im35 : 10; //h12_im_35, reset value: 0x0, access type: RW
		uint32 h12Re36 : 10; //h12_re_36, reset value: 0x0, access type: RW
		uint32 h12Im36 : 10; //h12_im_36, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap67_u;

/*REG_RF_EMULATOR_TAP_68 0x110 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h12Re37 : 10; //h12_re_37, reset value: 0x0, access type: RW
		uint32 h12Im37 : 10; //h12_im_37, reset value: 0x0, access type: RW
		uint32 h12Re38 : 10; //h12_re_38, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap68_u;

/*REG_RF_EMULATOR_TAP_69 0x114 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h12Im38 : 10; //h12_im_38, reset value: 0x0, access type: RW
		uint32 h12Re39 : 10; //h12_re_39, reset value: 0x0, access type: RW
		uint32 h12Im39 : 10; //h12_im_39, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap69_u;

/*REG_RF_EMULATOR_TAP_70 0x118 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h12Re40 : 10; //h12_re_40, reset value: 0x0, access type: RW
		uint32 h12Im40 : 10; //h12_im_40, reset value: 0x0, access type: RW
		uint32 h12Re41 : 10; //h12_re_41, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap70_u;

/*REG_RF_EMULATOR_TAP_71 0x11C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h12Im41 : 10; //h12_im_41, reset value: 0x0, access type: RW
		uint32 h12Re42 : 10; //h12_re_42, reset value: 0x0, access type: RW
		uint32 h12Im42 : 10; //h12_im_42, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap71_u;

/*REG_RF_EMULATOR_TAP_72 0x120 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h12Re43 : 10; //h12_re_43, reset value: 0x0, access type: RW
		uint32 h12Im43 : 10; //h12_im_43, reset value: 0x0, access type: RW
		uint32 h12Re44 : 10; //h12_re_44, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap72_u;

/*REG_RF_EMULATOR_TAP_73 0x124 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h12Im44 : 10; //h12_im_44, reset value: 0x0, access type: RW
		uint32 h12Re45 : 10; //h12_re_45, reset value: 0x0, access type: RW
		uint32 h12Im45 : 10; //h12_im_45, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap73_u;

/*REG_RF_EMULATOR_TAP_74 0x128 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h12Re46 : 10; //h12_re_46, reset value: 0x0, access type: RW
		uint32 h12Im46 : 10; //h12_im_46, reset value: 0x0, access type: RW
		uint32 h12Re47 : 10; //h12_re_47, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap74_u;

/*REG_RF_EMULATOR_TAP_75 0x12C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h12Im47 : 10; //h12_im_47, reset value: 0x0, access type: RW
		uint32 h12Re48 : 10; //h12_re_48, reset value: 0x0, access type: RW
		uint32 h12Im48 : 10; //h12_im_48, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap75_u;

/*REG_RF_EMULATOR_TAP_76 0x130 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h12Re49 : 10; //h12_re_49, reset value: 0x0, access type: RW
		uint32 h12Im49 : 10; //h12_im_49, reset value: 0x0, access type: RW
		uint32 h12Re50 : 10; //h12_re_50, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap76_u;

/*REG_RF_EMULATOR_TAP_77 0x134 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h12Im50 : 10; //h12_im_50, reset value: 0x0, access type: RW
		uint32 h12Re51 : 10; //h12_re_51, reset value: 0x0, access type: RW
		uint32 h12Im51 : 10; //h12_im_51, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap77_u;

/*REG_RF_EMULATOR_TAP_78 0x138 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h12Re52 : 10; //h12_re_52, reset value: 0x0, access type: RW
		uint32 h12Im52 : 10; //h12_im_52, reset value: 0x0, access type: RW
		uint32 h12Re53 : 10; //h12_re_53, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap78_u;

/*REG_RF_EMULATOR_TAP_79 0x13C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h12Im53 : 10; //h12_im_53, reset value: 0x0, access type: RW
		uint32 h12Re54 : 10; //h12_re_54, reset value: 0x0, access type: RW
		uint32 h12Im54 : 10; //h12_im_54, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap79_u;

/*REG_RF_EMULATOR_TAP_80 0x140 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h12Re55 : 10; //h12_re_55, reset value: 0x0, access type: RW
		uint32 h12Im55 : 10; //h12_im_55, reset value: 0x0, access type: RW
		uint32 h12Re56 : 10; //h12_re_56, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap80_u;

/*REG_RF_EMULATOR_TAP_81 0x144 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h12Im56 : 10; //h12_im_56, reset value: 0x0, access type: RW
		uint32 h12Re57 : 10; //h12_re_57, reset value: 0x0, access type: RW
		uint32 h12Im57 : 10; //h12_im_57, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap81_u;

/*REG_RF_EMULATOR_TAP_82 0x148 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h12Re58 : 10; //h12_re_58, reset value: 0x0, access type: RW
		uint32 h12Im58 : 10; //h12_im_58, reset value: 0x0, access type: RW
		uint32 h12Re59 : 10; //h12_re_59, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap82_u;

/*REG_RF_EMULATOR_TAP_83 0x14C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h12Im59 : 10; //h12_im_59, reset value: 0x0, access type: RW
		uint32 h12Re60 : 10; //h12_re_60, reset value: 0x0, access type: RW
		uint32 h12Im60 : 10; //h12_im_60, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap83_u;

/*REG_RF_EMULATOR_TAP_84 0x150 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h12Re61 : 10; //h12_re_61, reset value: 0x0, access type: RW
		uint32 h12Im61 : 10; //h12_im_61, reset value: 0x0, access type: RW
		uint32 h12Re62 : 10; //h12_re_62, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap84_u;

/*REG_RF_EMULATOR_TAP_85 0x154 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h12Im62 : 10; //h12_im_62, reset value: 0x0, access type: RW
		uint32 h12Re63 : 10; //h12_re_63, reset value: 0x0, access type: RW
		uint32 h12Im63 : 10; //h12_im_63, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap85_u;

/*REG_RF_EMULATOR_TAP_86 0x158 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h12Re64 : 10; //h12_re_64, reset value: 0x0, access type: RW
		uint32 h12Im64 : 10; //h12_im_64, reset value: 0x0, access type: RW
		uint32 h13Re0 : 10; //h13_re_0, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap86_u;

/*REG_RF_EMULATOR_TAP_87 0x15C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h13Im0 : 10; //h13_im_0, reset value: 0x0, access type: RW
		uint32 h13Re1 : 10; //h13_re_1, reset value: 0x0, access type: RW
		uint32 h13Im1 : 10; //h13_im_1, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap87_u;

/*REG_RF_EMULATOR_TAP_88 0x160 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h13Re2 : 10; //h13_re_2, reset value: 0x0, access type: RW
		uint32 h13Im2 : 10; //h13_im_2, reset value: 0x0, access type: RW
		uint32 h13Re3 : 10; //h13_re_3, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap88_u;

/*REG_RF_EMULATOR_TAP_89 0x164 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h13Im3 : 10; //h13_im_3, reset value: 0x0, access type: RW
		uint32 h13Re4 : 10; //h13_re_4, reset value: 0x0, access type: RW
		uint32 h13Im4 : 10; //h13_im_4, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap89_u;

/*REG_RF_EMULATOR_TAP_90 0x168 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h13Re5 : 10; //h13_re_5, reset value: 0x0, access type: RW
		uint32 h13Im5 : 10; //h13_im_5, reset value: 0x0, access type: RW
		uint32 h13Re6 : 10; //h13_re_6, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap90_u;

/*REG_RF_EMULATOR_TAP_91 0x16C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h13Im6 : 10; //h13_im_6, reset value: 0x0, access type: RW
		uint32 h13Re7 : 10; //h13_re_7, reset value: 0x0, access type: RW
		uint32 h13Im7 : 10; //h13_im_7, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap91_u;

/*REG_RF_EMULATOR_TAP_92 0x170 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h13Re8 : 10; //h13_re_8, reset value: 0x0, access type: RW
		uint32 h13Im8 : 10; //h13_im_8, reset value: 0x0, access type: RW
		uint32 h13Re9 : 10; //h13_re_9, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap92_u;

/*REG_RF_EMULATOR_TAP_93 0x174 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h13Im9 : 10; //h13_im_9, reset value: 0x0, access type: RW
		uint32 h13Re10 : 10; //h13_re_10, reset value: 0x0, access type: RW
		uint32 h13Im10 : 10; //h13_im_10, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap93_u;

/*REG_RF_EMULATOR_TAP_94 0x178 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h13Re11 : 10; //h13_re_11, reset value: 0x0, access type: RW
		uint32 h13Im11 : 10; //h13_im_11, reset value: 0x0, access type: RW
		uint32 h13Re12 : 10; //h13_re_12, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap94_u;

/*REG_RF_EMULATOR_TAP_95 0x17C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h13Im12 : 10; //h13_im_12, reset value: 0x0, access type: RW
		uint32 h13Re13 : 10; //h13_re_13, reset value: 0x0, access type: RW
		uint32 h13Im13 : 10; //h13_im_13, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap95_u;

/*REG_RF_EMULATOR_TAP_96 0x180 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h13Re14 : 10; //h13_re_14, reset value: 0x0, access type: RW
		uint32 h13Im14 : 10; //h13_im_14, reset value: 0x0, access type: RW
		uint32 h13Re15 : 10; //h13_re_15, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap96_u;

/*REG_RF_EMULATOR_TAP_97 0x184 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h13Im15 : 10; //h13_im_15, reset value: 0x0, access type: RW
		uint32 h13Re16 : 10; //h13_re_16, reset value: 0x0, access type: RW
		uint32 h13Im16 : 10; //h13_im_16, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap97_u;

/*REG_RF_EMULATOR_TAP_98 0x188 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h13Re17 : 10; //h13_re_17, reset value: 0x0, access type: RW
		uint32 h13Im17 : 10; //h13_im_17, reset value: 0x0, access type: RW
		uint32 h13Re18 : 10; //h13_re_18, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap98_u;

/*REG_RF_EMULATOR_TAP_99 0x18C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h13Im18 : 10; //h13_im_18, reset value: 0x0, access type: RW
		uint32 h13Re19 : 10; //h13_re_19, reset value: 0x0, access type: RW
		uint32 h13Im19 : 10; //h13_im_19, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap99_u;

/*REG_RF_EMULATOR_TAP_100 0x190 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h13Re20 : 10; //h13_re_20, reset value: 0x0, access type: RW
		uint32 h13Im20 : 10; //h13_im_20, reset value: 0x0, access type: RW
		uint32 h13Re21 : 10; //h13_re_21, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap100_u;

/*REG_RF_EMULATOR_TAP_101 0x194 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h13Im21 : 10; //h13_im_21, reset value: 0x0, access type: RW
		uint32 h13Re22 : 10; //h13_re_22, reset value: 0x0, access type: RW
		uint32 h13Im22 : 10; //h13_im_22, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap101_u;

/*REG_RF_EMULATOR_TAP_102 0x198 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h13Re23 : 10; //h13_re_23, reset value: 0x0, access type: RW
		uint32 h13Im23 : 10; //h13_im_23, reset value: 0x0, access type: RW
		uint32 h13Re24 : 10; //h13_re_24, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap102_u;

/*REG_RF_EMULATOR_TAP_103 0x19C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h13Im24 : 10; //h13_im_24, reset value: 0x0, access type: RW
		uint32 h13Re25 : 10; //h13_re_25, reset value: 0x0, access type: RW
		uint32 h13Im25 : 10; //h13_im_25, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap103_u;

/*REG_RF_EMULATOR_TAP_104 0x1A0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h13Re26 : 10; //h13_re_26, reset value: 0x0, access type: RW
		uint32 h13Im26 : 10; //h13_im_26, reset value: 0x0, access type: RW
		uint32 h13Re27 : 10; //h13_re_27, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap104_u;

/*REG_RF_EMULATOR_TAP_105 0x1A4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h13Im27 : 10; //h13_im_27, reset value: 0x0, access type: RW
		uint32 h13Re28 : 10; //h13_re_28, reset value: 0x0, access type: RW
		uint32 h13Im28 : 10; //h13_im_28, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap105_u;

/*REG_RF_EMULATOR_TAP_106 0x1A8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h13Re29 : 10; //h13_re_29, reset value: 0x0, access type: RW
		uint32 h13Im29 : 10; //h13_im_29, reset value: 0x0, access type: RW
		uint32 h13Re30 : 10; //h13_re_30, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap106_u;

/*REG_RF_EMULATOR_TAP_107 0x1AC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h13Im30 : 10; //h13_im_30, reset value: 0x0, access type: RW
		uint32 h13Re31 : 10; //h13_re_31, reset value: 0x0, access type: RW
		uint32 h13Im31 : 10; //h13_im_31, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap107_u;

/*REG_RF_EMULATOR_TAP_108 0x1B0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h13Re32 : 10; //h13_re_32, reset value: 0x0, access type: RW
		uint32 h13Im32 : 10; //h13_im_32, reset value: 0x0, access type: RW
		uint32 h13Re33 : 10; //h13_re_33, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap108_u;

/*REG_RF_EMULATOR_TAP_109 0x1B4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h13Im33 : 10; //h13_im_33, reset value: 0x0, access type: RW
		uint32 h13Re34 : 10; //h13_re_34, reset value: 0x0, access type: RW
		uint32 h13Im34 : 10; //h13_im_34, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap109_u;

/*REG_RF_EMULATOR_TAP_110 0x1B8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h13Re35 : 10; //h13_re_35, reset value: 0x0, access type: RW
		uint32 h13Im35 : 10; //h13_im_35, reset value: 0x0, access type: RW
		uint32 h13Re36 : 10; //h13_re_36, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap110_u;

/*REG_RF_EMULATOR_TAP_111 0x1BC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h13Im36 : 10; //h13_im_36, reset value: 0x0, access type: RW
		uint32 h13Re37 : 10; //h13_re_37, reset value: 0x0, access type: RW
		uint32 h13Im37 : 10; //h13_im_37, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap111_u;

/*REG_RF_EMULATOR_TAP_112 0x1C0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h13Re38 : 10; //h13_re_38, reset value: 0x0, access type: RW
		uint32 h13Im38 : 10; //h13_im_38, reset value: 0x0, access type: RW
		uint32 h13Re39 : 10; //h13_re_39, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap112_u;

/*REG_RF_EMULATOR_TAP_113 0x1C4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h13Im39 : 10; //h13_im_39, reset value: 0x0, access type: RW
		uint32 h13Re40 : 10; //h13_re_40, reset value: 0x0, access type: RW
		uint32 h13Im40 : 10; //h13_im_40, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap113_u;

/*REG_RF_EMULATOR_TAP_114 0x1C8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h13Re41 : 10; //h13_re_41, reset value: 0x0, access type: RW
		uint32 h13Im41 : 10; //h13_im_41, reset value: 0x0, access type: RW
		uint32 h13Re42 : 10; //h13_re_42, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap114_u;

/*REG_RF_EMULATOR_TAP_115 0x1CC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h13Im42 : 10; //h13_im_42, reset value: 0x0, access type: RW
		uint32 h13Re43 : 10; //h13_re_43, reset value: 0x0, access type: RW
		uint32 h13Im43 : 10; //h13_im_43, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap115_u;

/*REG_RF_EMULATOR_TAP_116 0x1D0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h13Re44 : 10; //h13_re_44, reset value: 0x0, access type: RW
		uint32 h13Im44 : 10; //h13_im_44, reset value: 0x0, access type: RW
		uint32 h13Re45 : 10; //h13_re_45, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap116_u;

/*REG_RF_EMULATOR_TAP_117 0x1D4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h13Im45 : 10; //h13_im_45, reset value: 0x0, access type: RW
		uint32 h13Re46 : 10; //h13_re_46, reset value: 0x0, access type: RW
		uint32 h13Im46 : 10; //h13_im_46, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap117_u;

/*REG_RF_EMULATOR_TAP_118 0x1D8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h13Re47 : 10; //h13_re_47, reset value: 0x0, access type: RW
		uint32 h13Im47 : 10; //h13_im_47, reset value: 0x0, access type: RW
		uint32 h13Re48 : 10; //h13_re_48, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap118_u;

/*REG_RF_EMULATOR_TAP_119 0x1DC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h13Im48 : 10; //h13_im_48, reset value: 0x0, access type: RW
		uint32 h13Re49 : 10; //h13_re_49, reset value: 0x0, access type: RW
		uint32 h13Im49 : 10; //h13_im_49, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap119_u;

/*REG_RF_EMULATOR_TAP_120 0x1E0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h13Re50 : 10; //h13_re_50, reset value: 0x0, access type: RW
		uint32 h13Im50 : 10; //h13_im_50, reset value: 0x0, access type: RW
		uint32 h13Re51 : 10; //h13_re_51, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap120_u;

/*REG_RF_EMULATOR_TAP_121 0x1E4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h13Im51 : 10; //h13_im_51, reset value: 0x0, access type: RW
		uint32 h13Re52 : 10; //h13_re_52, reset value: 0x0, access type: RW
		uint32 h13Im52 : 10; //h13_im_52, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap121_u;

/*REG_RF_EMULATOR_TAP_122 0x1E8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h13Re53 : 10; //h13_re_53, reset value: 0x0, access type: RW
		uint32 h13Im53 : 10; //h13_im_53, reset value: 0x0, access type: RW
		uint32 h13Re54 : 10; //h13_re_54, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap122_u;

/*REG_RF_EMULATOR_TAP_123 0x1EC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h13Im54 : 10; //h13_im_54, reset value: 0x0, access type: RW
		uint32 h13Re55 : 10; //h13_re_55, reset value: 0x0, access type: RW
		uint32 h13Im55 : 10; //h13_im_55, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap123_u;

/*REG_RF_EMULATOR_TAP_124 0x1F0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h13Re56 : 10; //h13_re_56, reset value: 0x0, access type: RW
		uint32 h13Im56 : 10; //h13_im_56, reset value: 0x0, access type: RW
		uint32 h13Re57 : 10; //h13_re_57, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap124_u;

/*REG_RF_EMULATOR_TAP_125 0x1F4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h13Im57 : 10; //h13_im_57, reset value: 0x0, access type: RW
		uint32 h13Re58 : 10; //h13_re_58, reset value: 0x0, access type: RW
		uint32 h13Im58 : 10; //h13_im_58, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap125_u;

/*REG_RF_EMULATOR_TAP_126 0x1F8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h13Re59 : 10; //h13_re_59, reset value: 0x0, access type: RW
		uint32 h13Im59 : 10; //h13_im_59, reset value: 0x0, access type: RW
		uint32 h13Re60 : 10; //h13_re_60, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap126_u;

/*REG_RF_EMULATOR_TAP_127 0x1FC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h13Im60 : 10; //h13_im_60, reset value: 0x0, access type: RW
		uint32 h13Re61 : 10; //h13_re_61, reset value: 0x0, access type: RW
		uint32 h13Im61 : 10; //h13_im_61, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap127_u;

/*REG_RF_EMULATOR_TAP_128 0x200 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h13Re62 : 10; //h13_re_62, reset value: 0x0, access type: RW
		uint32 h13Im62 : 10; //h13_im_62, reset value: 0x0, access type: RW
		uint32 h13Re63 : 10; //h13_re_63, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap128_u;

/*REG_RF_EMULATOR_TAP_129 0x204 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h13Im63 : 10; //h13_im_63, reset value: 0x0, access type: RW
		uint32 h13Re64 : 10; //h13_re_64, reset value: 0x0, access type: RW
		uint32 h13Im64 : 10; //h13_im_64, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap129_u;

/*REG_RF_EMULATOR_TAP_130 0x208 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h14Re0 : 10; //h14_re_0, reset value: 0x0, access type: RW
		uint32 h14Im0 : 10; //h14_im_0, reset value: 0x0, access type: RW
		uint32 h14Re1 : 10; //h14_re_1, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap130_u;

/*REG_RF_EMULATOR_TAP_131 0x20C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h14Im1 : 10; //h14_im_1, reset value: 0x0, access type: RW
		uint32 h14Re2 : 10; //h14_re_2, reset value: 0x0, access type: RW
		uint32 h14Im2 : 10; //h14_im_2, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap131_u;

/*REG_RF_EMULATOR_TAP_132 0x210 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h14Re3 : 10; //h14_re_3, reset value: 0x0, access type: RW
		uint32 h14Im3 : 10; //h14_im_3, reset value: 0x0, access type: RW
		uint32 h14Re4 : 10; //h14_re_4, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap132_u;

/*REG_RF_EMULATOR_TAP_133 0x214 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h14Im4 : 10; //h14_im_4, reset value: 0x0, access type: RW
		uint32 h14Re5 : 10; //h14_re_5, reset value: 0x0, access type: RW
		uint32 h14Im5 : 10; //h14_im_5, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap133_u;

/*REG_RF_EMULATOR_TAP_134 0x218 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h14Re6 : 10; //h14_re_6, reset value: 0x0, access type: RW
		uint32 h14Im6 : 10; //h14_im_6, reset value: 0x0, access type: RW
		uint32 h14Re7 : 10; //h14_re_7, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap134_u;

/*REG_RF_EMULATOR_TAP_135 0x21C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h14Im7 : 10; //h14_im_7, reset value: 0x0, access type: RW
		uint32 h14Re8 : 10; //h14_re_8, reset value: 0x0, access type: RW
		uint32 h14Im8 : 10; //h14_im_8, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap135_u;

/*REG_RF_EMULATOR_TAP_136 0x220 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h14Re9 : 10; //h14_re_9, reset value: 0x0, access type: RW
		uint32 h14Im9 : 10; //h14_im_9, reset value: 0x0, access type: RW
		uint32 h14Re10 : 10; //h14_re_10, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap136_u;

/*REG_RF_EMULATOR_TAP_137 0x224 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h14Im10 : 10; //h14_im_10, reset value: 0x0, access type: RW
		uint32 h14Re11 : 10; //h14_re_11, reset value: 0x0, access type: RW
		uint32 h14Im11 : 10; //h14_im_11, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap137_u;

/*REG_RF_EMULATOR_TAP_138 0x228 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h14Re12 : 10; //h14_re_12, reset value: 0x0, access type: RW
		uint32 h14Im12 : 10; //h14_im_12, reset value: 0x0, access type: RW
		uint32 h14Re13 : 10; //h14_re_13, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap138_u;

/*REG_RF_EMULATOR_TAP_139 0x22C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h14Im13 : 10; //h14_im_13, reset value: 0x0, access type: RW
		uint32 h14Re14 : 10; //h14_re_14, reset value: 0x0, access type: RW
		uint32 h14Im14 : 10; //h14_im_14, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap139_u;

/*REG_RF_EMULATOR_TAP_140 0x230 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h14Re15 : 10; //h14_re_15, reset value: 0x0, access type: RW
		uint32 h14Im15 : 10; //h14_im_15, reset value: 0x0, access type: RW
		uint32 h14Re16 : 10; //h14_re_16, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap140_u;

/*REG_RF_EMULATOR_TAP_141 0x234 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h14Im16 : 10; //h14_im_16, reset value: 0x0, access type: RW
		uint32 h14Re17 : 10; //h14_re_17, reset value: 0x0, access type: RW
		uint32 h14Im17 : 10; //h14_im_17, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap141_u;

/*REG_RF_EMULATOR_TAP_142 0x238 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h14Re18 : 10; //h14_re_18, reset value: 0x0, access type: RW
		uint32 h14Im18 : 10; //h14_im_18, reset value: 0x0, access type: RW
		uint32 h14Re19 : 10; //h14_re_19, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap142_u;

/*REG_RF_EMULATOR_TAP_143 0x23C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h14Im19 : 10; //h14_im_19, reset value: 0x0, access type: RW
		uint32 h14Re20 : 10; //h14_re_20, reset value: 0x0, access type: RW
		uint32 h14Im20 : 10; //h14_im_20, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap143_u;

/*REG_RF_EMULATOR_TAP_144 0x240 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h14Re21 : 10; //h14_re_21, reset value: 0x0, access type: RW
		uint32 h14Im21 : 10; //h14_im_21, reset value: 0x0, access type: RW
		uint32 h14Re22 : 10; //h14_re_22, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap144_u;

/*REG_RF_EMULATOR_TAP_145 0x244 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h14Im22 : 10; //h14_im_22, reset value: 0x0, access type: RW
		uint32 h14Re23 : 10; //h14_re_23, reset value: 0x0, access type: RW
		uint32 h14Im23 : 10; //h14_im_23, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap145_u;

/*REG_RF_EMULATOR_TAP_146 0x248 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h14Re24 : 10; //h14_re_24, reset value: 0x0, access type: RW
		uint32 h14Im24 : 10; //h14_im_24, reset value: 0x0, access type: RW
		uint32 h14Re25 : 10; //h14_re_25, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap146_u;

/*REG_RF_EMULATOR_TAP_147 0x24C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h14Im25 : 10; //h14_im_25, reset value: 0x0, access type: RW
		uint32 h14Re26 : 10; //h14_re_26, reset value: 0x0, access type: RW
		uint32 h14Im26 : 10; //h14_im_26, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap147_u;

/*REG_RF_EMULATOR_TAP_148 0x250 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h14Re27 : 10; //h14_re_27, reset value: 0x0, access type: RW
		uint32 h14Im27 : 10; //h14_im_27, reset value: 0x0, access type: RW
		uint32 h14Re28 : 10; //h14_re_28, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap148_u;

/*REG_RF_EMULATOR_TAP_149 0x254 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h14Im28 : 10; //h14_im_28, reset value: 0x0, access type: RW
		uint32 h14Re29 : 10; //h14_re_29, reset value: 0x0, access type: RW
		uint32 h14Im29 : 10; //h14_im_29, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap149_u;

/*REG_RF_EMULATOR_TAP_150 0x258 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h14Re30 : 10; //h14_re_30, reset value: 0x0, access type: RW
		uint32 h14Im30 : 10; //h14_im_30, reset value: 0x0, access type: RW
		uint32 h14Re31 : 10; //h14_re_31, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap150_u;

/*REG_RF_EMULATOR_TAP_151 0x25C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h14Im31 : 10; //h14_im_31, reset value: 0x0, access type: RW
		uint32 h14Re32 : 10; //h14_re_32, reset value: 0x0, access type: RW
		uint32 h14Im32 : 10; //h14_im_32, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap151_u;

/*REG_RF_EMULATOR_TAP_152 0x260 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h14Re33 : 10; //h14_re_33, reset value: 0x0, access type: RW
		uint32 h14Im33 : 10; //h14_im_33, reset value: 0x0, access type: RW
		uint32 h14Re34 : 10; //h14_re_34, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap152_u;

/*REG_RF_EMULATOR_TAP_153 0x264 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h14Im34 : 10; //h14_im_34, reset value: 0x0, access type: RW
		uint32 h14Re35 : 10; //h14_re_35, reset value: 0x0, access type: RW
		uint32 h14Im35 : 10; //h14_im_35, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap153_u;

/*REG_RF_EMULATOR_TAP_154 0x268 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h14Re36 : 10; //h14_re_36, reset value: 0x0, access type: RW
		uint32 h14Im36 : 10; //h14_im_36, reset value: 0x0, access type: RW
		uint32 h14Re37 : 10; //h14_re_37, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap154_u;

/*REG_RF_EMULATOR_TAP_155 0x26C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h14Im37 : 10; //h14_im_37, reset value: 0x0, access type: RW
		uint32 h14Re38 : 10; //h14_re_38, reset value: 0x0, access type: RW
		uint32 h14Im38 : 10; //h14_im_38, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap155_u;

/*REG_RF_EMULATOR_TAP_156 0x270 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h14Re39 : 10; //h14_re_39, reset value: 0x0, access type: RW
		uint32 h14Im39 : 10; //h14_im_39, reset value: 0x0, access type: RW
		uint32 h14Re40 : 10; //h14_re_40, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap156_u;

/*REG_RF_EMULATOR_TAP_157 0x274 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h14Im40 : 10; //h14_im_40, reset value: 0x0, access type: RW
		uint32 h14Re41 : 10; //h14_re_41, reset value: 0x0, access type: RW
		uint32 h14Im41 : 10; //h14_im_41, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap157_u;

/*REG_RF_EMULATOR_TAP_158 0x278 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h14Re42 : 10; //h14_re_42, reset value: 0x0, access type: RW
		uint32 h14Im42 : 10; //h14_im_42, reset value: 0x0, access type: RW
		uint32 h14Re43 : 10; //h14_re_43, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap158_u;

/*REG_RF_EMULATOR_TAP_159 0x27C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h14Im43 : 10; //h14_im_43, reset value: 0x0, access type: RW
		uint32 h14Re44 : 10; //h14_re_44, reset value: 0x0, access type: RW
		uint32 h14Im44 : 10; //h14_im_44, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap159_u;

/*REG_RF_EMULATOR_TAP_160 0x280 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h14Re45 : 10; //h14_re_45, reset value: 0x0, access type: RW
		uint32 h14Im45 : 10; //h14_im_45, reset value: 0x0, access type: RW
		uint32 h14Re46 : 10; //h14_re_46, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap160_u;

/*REG_RF_EMULATOR_TAP_161 0x284 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h14Im46 : 10; //h14_im_46, reset value: 0x0, access type: RW
		uint32 h14Re47 : 10; //h14_re_47, reset value: 0x0, access type: RW
		uint32 h14Im47 : 10; //h14_im_47, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap161_u;

/*REG_RF_EMULATOR_TAP_162 0x288 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h14Re48 : 10; //h14_re_48, reset value: 0x0, access type: RW
		uint32 h14Im48 : 10; //h14_im_48, reset value: 0x0, access type: RW
		uint32 h14Re49 : 10; //h14_re_49, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap162_u;

/*REG_RF_EMULATOR_TAP_163 0x28C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h14Im49 : 10; //h14_im_49, reset value: 0x0, access type: RW
		uint32 h14Re50 : 10; //h14_re_50, reset value: 0x0, access type: RW
		uint32 h14Im50 : 10; //h14_im_50, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap163_u;

/*REG_RF_EMULATOR_TAP_164 0x290 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h14Re51 : 10; //h14_re_51, reset value: 0x0, access type: RW
		uint32 h14Im51 : 10; //h14_im_51, reset value: 0x0, access type: RW
		uint32 h14Re52 : 10; //h14_re_52, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap164_u;

/*REG_RF_EMULATOR_TAP_165 0x294 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h14Im52 : 10; //h14_im_52, reset value: 0x0, access type: RW
		uint32 h14Re53 : 10; //h14_re_53, reset value: 0x0, access type: RW
		uint32 h14Im53 : 10; //h14_im_53, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap165_u;

/*REG_RF_EMULATOR_TAP_166 0x298 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h14Re54 : 10; //h14_re_54, reset value: 0x0, access type: RW
		uint32 h14Im54 : 10; //h14_im_54, reset value: 0x0, access type: RW
		uint32 h14Re55 : 10; //h14_re_55, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap166_u;

/*REG_RF_EMULATOR_TAP_167 0x29C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h14Im55 : 10; //h14_im_55, reset value: 0x0, access type: RW
		uint32 h14Re56 : 10; //h14_re_56, reset value: 0x0, access type: RW
		uint32 h14Im56 : 10; //h14_im_56, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap167_u;

/*REG_RF_EMULATOR_TAP_168 0x2A0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h14Re57 : 10; //h14_re_57, reset value: 0x0, access type: RW
		uint32 h14Im57 : 10; //h14_im_57, reset value: 0x0, access type: RW
		uint32 h14Re58 : 10; //h14_re_58, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap168_u;

/*REG_RF_EMULATOR_TAP_169 0x2A4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h14Im58 : 10; //h14_im_58, reset value: 0x0, access type: RW
		uint32 h14Re59 : 10; //h14_re_59, reset value: 0x0, access type: RW
		uint32 h14Im59 : 10; //h14_im_59, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap169_u;

/*REG_RF_EMULATOR_TAP_170 0x2A8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h14Re60 : 10; //h14_re_60, reset value: 0x0, access type: RW
		uint32 h14Im60 : 10; //h14_im_60, reset value: 0x0, access type: RW
		uint32 h14Re61 : 10; //h14_re_61, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap170_u;

/*REG_RF_EMULATOR_TAP_171 0x2AC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h14Im61 : 10; //h14_im_61, reset value: 0x0, access type: RW
		uint32 h14Re62 : 10; //h14_re_62, reset value: 0x0, access type: RW
		uint32 h14Im62 : 10; //h14_im_62, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap171_u;

/*REG_RF_EMULATOR_TAP_172 0x2B0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h14Re63 : 10; //h14_re_63, reset value: 0x0, access type: RW
		uint32 h14Im63 : 10; //h14_im_63, reset value: 0x0, access type: RW
		uint32 h14Re64 : 10; //h14_re_64, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap172_u;

/*REG_RF_EMULATOR_TAP_173 0x2B4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h14Im64 : 10; //h14_im_64, reset value: 0x0, access type: RW
		uint32 h21Re0 : 10; //h21_re_0, reset value: 0x0, access type: RW
		uint32 h21Im0 : 10; //h21_im_0, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap173_u;

/*REG_RF_EMULATOR_TAP_174 0x2B8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h21Re1 : 10; //h21_re_1, reset value: 0x0, access type: RW
		uint32 h21Im1 : 10; //h21_im_1, reset value: 0x0, access type: RW
		uint32 h21Re2 : 10; //h21_re_2, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap174_u;

/*REG_RF_EMULATOR_TAP_175 0x2BC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h21Im2 : 10; //h21_im_2, reset value: 0x0, access type: RW
		uint32 h21Re3 : 10; //h21_re_3, reset value: 0x0, access type: RW
		uint32 h21Im3 : 10; //h21_im_3, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap175_u;

/*REG_RF_EMULATOR_TAP_176 0x2C0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h21Re4 : 10; //h21_re_4, reset value: 0x0, access type: RW
		uint32 h21Im4 : 10; //h21_im_4, reset value: 0x0, access type: RW
		uint32 h21Re5 : 10; //h21_re_5, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap176_u;

/*REG_RF_EMULATOR_TAP_177 0x2C4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h21Im5 : 10; //h21_im_5, reset value: 0x0, access type: RW
		uint32 h21Re6 : 10; //h21_re_6, reset value: 0x0, access type: RW
		uint32 h21Im6 : 10; //h21_im_6, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap177_u;

/*REG_RF_EMULATOR_TAP_178 0x2C8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h21Re7 : 10; //h21_re_7, reset value: 0x0, access type: RW
		uint32 h21Im7 : 10; //h21_im_7, reset value: 0x0, access type: RW
		uint32 h21Re8 : 10; //h21_re_8, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap178_u;

/*REG_RF_EMULATOR_TAP_179 0x2CC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h21Im8 : 10; //h21_im_8, reset value: 0x0, access type: RW
		uint32 h21Re9 : 10; //h21_re_9, reset value: 0x0, access type: RW
		uint32 h21Im9 : 10; //h21_im_9, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap179_u;

/*REG_RF_EMULATOR_TAP_180 0x2D0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h21Re10 : 10; //h21_re_10, reset value: 0x0, access type: RW
		uint32 h21Im10 : 10; //h21_im_10, reset value: 0x0, access type: RW
		uint32 h21Re11 : 10; //h21_re_11, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap180_u;

/*REG_RF_EMULATOR_TAP_181 0x2D4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h21Im11 : 10; //h21_im_11, reset value: 0x0, access type: RW
		uint32 h21Re12 : 10; //h21_re_12, reset value: 0x0, access type: RW
		uint32 h21Im12 : 10; //h21_im_12, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap181_u;

/*REG_RF_EMULATOR_TAP_182 0x2D8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h21Re13 : 10; //h21_re_13, reset value: 0x0, access type: RW
		uint32 h21Im13 : 10; //h21_im_13, reset value: 0x0, access type: RW
		uint32 h21Re14 : 10; //h21_re_14, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap182_u;

/*REG_RF_EMULATOR_TAP_183 0x2DC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h21Im14 : 10; //h21_im_14, reset value: 0x0, access type: RW
		uint32 h21Re15 : 10; //h21_re_15, reset value: 0x0, access type: RW
		uint32 h21Im15 : 10; //h21_im_15, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap183_u;

/*REG_RF_EMULATOR_TAP_184 0x2E0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h21Re16 : 10; //h21_re_16, reset value: 0x0, access type: RW
		uint32 h21Im16 : 10; //h21_im_16, reset value: 0x0, access type: RW
		uint32 h21Re17 : 10; //h21_re_17, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap184_u;

/*REG_RF_EMULATOR_TAP_185 0x2E4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h21Im17 : 10; //h21_im_17, reset value: 0x0, access type: RW
		uint32 h21Re18 : 10; //h21_re_18, reset value: 0x0, access type: RW
		uint32 h21Im18 : 10; //h21_im_18, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap185_u;

/*REG_RF_EMULATOR_TAP_186 0x2E8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h21Re19 : 10; //h21_re_19, reset value: 0x0, access type: RW
		uint32 h21Im19 : 10; //h21_im_19, reset value: 0x0, access type: RW
		uint32 h21Re20 : 10; //h21_re_20, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap186_u;

/*REG_RF_EMULATOR_TAP_187 0x2EC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h21Im20 : 10; //h21_im_20, reset value: 0x0, access type: RW
		uint32 h21Re21 : 10; //h21_re_21, reset value: 0x0, access type: RW
		uint32 h21Im21 : 10; //h21_im_21, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap187_u;

/*REG_RF_EMULATOR_TAP_188 0x2F0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h21Re22 : 10; //h21_re_22, reset value: 0x0, access type: RW
		uint32 h21Im22 : 10; //h21_im_22, reset value: 0x0, access type: RW
		uint32 h21Re23 : 10; //h21_re_23, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap188_u;

/*REG_RF_EMULATOR_TAP_189 0x2F4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h21Im23 : 10; //h21_im_23, reset value: 0x0, access type: RW
		uint32 h21Re24 : 10; //h21_re_24, reset value: 0x0, access type: RW
		uint32 h21Im24 : 10; //h21_im_24, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap189_u;

/*REG_RF_EMULATOR_TAP_190 0x2F8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h21Re25 : 10; //h21_re_25, reset value: 0x0, access type: RW
		uint32 h21Im25 : 10; //h21_im_25, reset value: 0x0, access type: RW
		uint32 h21Re26 : 10; //h21_re_26, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap190_u;

/*REG_RF_EMULATOR_TAP_191 0x2FC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h21Im26 : 10; //h21_im_26, reset value: 0x0, access type: RW
		uint32 h21Re27 : 10; //h21_re_27, reset value: 0x0, access type: RW
		uint32 h21Im27 : 10; //h21_im_27, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap191_u;

/*REG_RF_EMULATOR_TAP_192 0x300 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h21Re28 : 10; //h21_re_28, reset value: 0x0, access type: RW
		uint32 h21Im28 : 10; //h21_im_28, reset value: 0x0, access type: RW
		uint32 h21Re29 : 10; //h21_re_29, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap192_u;

/*REG_RF_EMULATOR_TAP_193 0x304 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h21Im29 : 10; //h21_im_29, reset value: 0x0, access type: RW
		uint32 h21Re30 : 10; //h21_re_30, reset value: 0x0, access type: RW
		uint32 h21Im30 : 10; //h21_im_30, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap193_u;

/*REG_RF_EMULATOR_TAP_194 0x308 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h21Re31 : 10; //h21_re_31, reset value: 0x0, access type: RW
		uint32 h21Im31 : 10; //h21_im_31, reset value: 0x0, access type: RW
		uint32 h21Re32 : 10; //h21_re_32, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap194_u;

/*REG_RF_EMULATOR_TAP_195 0x30C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h21Im32 : 10; //h21_im_32, reset value: 0x0, access type: RW
		uint32 h21Re33 : 10; //h21_re_33, reset value: 0x0, access type: RW
		uint32 h21Im33 : 10; //h21_im_33, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap195_u;

/*REG_RF_EMULATOR_TAP_196 0x310 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h21Re34 : 10; //h21_re_34, reset value: 0x0, access type: RW
		uint32 h21Im34 : 10; //h21_im_34, reset value: 0x0, access type: RW
		uint32 h21Re35 : 10; //h21_re_35, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap196_u;

/*REG_RF_EMULATOR_TAP_197 0x314 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h21Im35 : 10; //h21_im_35, reset value: 0x0, access type: RW
		uint32 h21Re36 : 10; //h21_re_36, reset value: 0x0, access type: RW
		uint32 h21Im36 : 10; //h21_im_36, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap197_u;

/*REG_RF_EMULATOR_TAP_198 0x318 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h21Re37 : 10; //h21_re_37, reset value: 0x0, access type: RW
		uint32 h21Im37 : 10; //h21_im_37, reset value: 0x0, access type: RW
		uint32 h21Re38 : 10; //h21_re_38, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap198_u;

/*REG_RF_EMULATOR_TAP_199 0x31C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h21Im38 : 10; //h21_im_38, reset value: 0x0, access type: RW
		uint32 h21Re39 : 10; //h21_re_39, reset value: 0x0, access type: RW
		uint32 h21Im39 : 10; //h21_im_39, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap199_u;

/*REG_RF_EMULATOR_TAP_200 0x320 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h21Re40 : 10; //h21_re_40, reset value: 0x0, access type: RW
		uint32 h21Im40 : 10; //h21_im_40, reset value: 0x0, access type: RW
		uint32 h21Re41 : 10; //h21_re_41, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap200_u;

/*REG_RF_EMULATOR_TAP_201 0x324 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h21Im41 : 10; //h21_im_41, reset value: 0x0, access type: RW
		uint32 h21Re42 : 10; //h21_re_42, reset value: 0x0, access type: RW
		uint32 h21Im42 : 10; //h21_im_42, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap201_u;

/*REG_RF_EMULATOR_TAP_202 0x328 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h21Re43 : 10; //h21_re_43, reset value: 0x0, access type: RW
		uint32 h21Im43 : 10; //h21_im_43, reset value: 0x0, access type: RW
		uint32 h21Re44 : 10; //h21_re_44, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap202_u;

/*REG_RF_EMULATOR_TAP_203 0x32C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h21Im44 : 10; //h21_im_44, reset value: 0x0, access type: RW
		uint32 h21Re45 : 10; //h21_re_45, reset value: 0x0, access type: RW
		uint32 h21Im45 : 10; //h21_im_45, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap203_u;

/*REG_RF_EMULATOR_TAP_204 0x330 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h21Re46 : 10; //h21_re_46, reset value: 0x0, access type: RW
		uint32 h21Im46 : 10; //h21_im_46, reset value: 0x0, access type: RW
		uint32 h21Re47 : 10; //h21_re_47, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap204_u;

/*REG_RF_EMULATOR_TAP_205 0x334 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h21Im47 : 10; //h21_im_47, reset value: 0x0, access type: RW
		uint32 h21Re48 : 10; //h21_re_48, reset value: 0x0, access type: RW
		uint32 h21Im48 : 10; //h21_im_48, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap205_u;

/*REG_RF_EMULATOR_TAP_206 0x338 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h21Re49 : 10; //h21_re_49, reset value: 0x0, access type: RW
		uint32 h21Im49 : 10; //h21_im_49, reset value: 0x0, access type: RW
		uint32 h21Re50 : 10; //h21_re_50, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap206_u;

/*REG_RF_EMULATOR_TAP_207 0x33C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h21Im50 : 10; //h21_im_50, reset value: 0x0, access type: RW
		uint32 h21Re51 : 10; //h21_re_51, reset value: 0x0, access type: RW
		uint32 h21Im51 : 10; //h21_im_51, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap207_u;

/*REG_RF_EMULATOR_TAP_208 0x340 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h21Re52 : 10; //h21_re_52, reset value: 0x0, access type: RW
		uint32 h21Im52 : 10; //h21_im_52, reset value: 0x0, access type: RW
		uint32 h21Re53 : 10; //h21_re_53, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap208_u;

/*REG_RF_EMULATOR_TAP_209 0x344 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h21Im53 : 10; //h21_im_53, reset value: 0x0, access type: RW
		uint32 h21Re54 : 10; //h21_re_54, reset value: 0x0, access type: RW
		uint32 h21Im54 : 10; //h21_im_54, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap209_u;

/*REG_RF_EMULATOR_TAP_210 0x348 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h21Re55 : 10; //h21_re_55, reset value: 0x0, access type: RW
		uint32 h21Im55 : 10; //h21_im_55, reset value: 0x0, access type: RW
		uint32 h21Re56 : 10; //h21_re_56, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap210_u;

/*REG_RF_EMULATOR_TAP_211 0x34C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h21Im56 : 10; //h21_im_56, reset value: 0x0, access type: RW
		uint32 h21Re57 : 10; //h21_re_57, reset value: 0x0, access type: RW
		uint32 h21Im57 : 10; //h21_im_57, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap211_u;

/*REG_RF_EMULATOR_TAP_212 0x350 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h21Re58 : 10; //h21_re_58, reset value: 0x0, access type: RW
		uint32 h21Im58 : 10; //h21_im_58, reset value: 0x0, access type: RW
		uint32 h21Re59 : 10; //h21_re_59, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap212_u;

/*REG_RF_EMULATOR_TAP_213 0x354 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h21Im59 : 10; //h21_im_59, reset value: 0x0, access type: RW
		uint32 h21Re60 : 10; //h21_re_60, reset value: 0x0, access type: RW
		uint32 h21Im60 : 10; //h21_im_60, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap213_u;

/*REG_RF_EMULATOR_TAP_214 0x358 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h21Re61 : 10; //h21_re_61, reset value: 0x0, access type: RW
		uint32 h21Im61 : 10; //h21_im_61, reset value: 0x0, access type: RW
		uint32 h21Re62 : 10; //h21_re_62, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap214_u;

/*REG_RF_EMULATOR_TAP_215 0x35C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h21Im62 : 10; //h21_im_62, reset value: 0x0, access type: RW
		uint32 h21Re63 : 10; //h21_re_63, reset value: 0x0, access type: RW
		uint32 h21Im63 : 10; //h21_im_63, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap215_u;

/*REG_RF_EMULATOR_TAP_216 0x360 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h21Re64 : 10; //h21_re_64, reset value: 0x0, access type: RW
		uint32 h21Im64 : 10; //h21_im_64, reset value: 0x0, access type: RW
		uint32 h22Re0 : 10; //h22_re_0, reset value: 0x1ff, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap216_u;

/*REG_RF_EMULATOR_TAP_217 0x364 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h22Im0 : 10; //h22_im_0, reset value: 0x0, access type: RW
		uint32 h22Re1 : 10; //h22_re_1, reset value: 0x0, access type: RW
		uint32 h22Im1 : 10; //h22_im_1, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap217_u;

/*REG_RF_EMULATOR_TAP_218 0x368 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h22Re2 : 10; //h22_re_2, reset value: 0x0, access type: RW
		uint32 h22Im2 : 10; //h22_im_2, reset value: 0x0, access type: RW
		uint32 h22Re3 : 10; //h22_re_3, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap218_u;

/*REG_RF_EMULATOR_TAP_219 0x36C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h22Im3 : 10; //h22_im_3, reset value: 0x0, access type: RW
		uint32 h22Re4 : 10; //h22_re_4, reset value: 0x0, access type: RW
		uint32 h22Im4 : 10; //h22_im_4, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap219_u;

/*REG_RF_EMULATOR_TAP_220 0x370 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h22Re5 : 10; //h22_re_5, reset value: 0x0, access type: RW
		uint32 h22Im5 : 10; //h22_im_5, reset value: 0x0, access type: RW
		uint32 h22Re6 : 10; //h22_re_6, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap220_u;

/*REG_RF_EMULATOR_TAP_221 0x374 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h22Im6 : 10; //h22_im_6, reset value: 0x0, access type: RW
		uint32 h22Re7 : 10; //h22_re_7, reset value: 0x0, access type: RW
		uint32 h22Im7 : 10; //h22_im_7, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap221_u;

/*REG_RF_EMULATOR_TAP_222 0x378 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h22Re8 : 10; //h22_re_8, reset value: 0x0, access type: RW
		uint32 h22Im8 : 10; //h22_im_8, reset value: 0x0, access type: RW
		uint32 h22Re9 : 10; //h22_re_9, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap222_u;

/*REG_RF_EMULATOR_TAP_223 0x37C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h22Im9 : 10; //h22_im_9, reset value: 0x0, access type: RW
		uint32 h22Re10 : 10; //h22_re_10, reset value: 0x0, access type: RW
		uint32 h22Im10 : 10; //h22_im_10, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap223_u;

/*REG_RF_EMULATOR_TAP_224 0x380 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h22Re11 : 10; //h22_re_11, reset value: 0x0, access type: RW
		uint32 h22Im11 : 10; //h22_im_11, reset value: 0x0, access type: RW
		uint32 h22Re12 : 10; //h22_re_12, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap224_u;

/*REG_RF_EMULATOR_TAP_225 0x384 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h22Im12 : 10; //h22_im_12, reset value: 0x0, access type: RW
		uint32 h22Re13 : 10; //h22_re_13, reset value: 0x0, access type: RW
		uint32 h22Im13 : 10; //h22_im_13, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap225_u;

/*REG_RF_EMULATOR_TAP_226 0x388 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h22Re14 : 10; //h22_re_14, reset value: 0x0, access type: RW
		uint32 h22Im14 : 10; //h22_im_14, reset value: 0x0, access type: RW
		uint32 h22Re15 : 10; //h22_re_15, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap226_u;

/*REG_RF_EMULATOR_TAP_227 0x38C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h22Im15 : 10; //h22_im_15, reset value: 0x0, access type: RW
		uint32 h22Re16 : 10; //h22_re_16, reset value: 0x0, access type: RW
		uint32 h22Im16 : 10; //h22_im_16, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap227_u;

/*REG_RF_EMULATOR_TAP_228 0x390 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h22Re17 : 10; //h22_re_17, reset value: 0x0, access type: RW
		uint32 h22Im17 : 10; //h22_im_17, reset value: 0x0, access type: RW
		uint32 h22Re18 : 10; //h22_re_18, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap228_u;

/*REG_RF_EMULATOR_TAP_229 0x394 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h22Im18 : 10; //h22_im_18, reset value: 0x0, access type: RW
		uint32 h22Re19 : 10; //h22_re_19, reset value: 0x0, access type: RW
		uint32 h22Im19 : 10; //h22_im_19, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap229_u;

/*REG_RF_EMULATOR_TAP_230 0x398 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h22Re20 : 10; //h22_re_20, reset value: 0x0, access type: RW
		uint32 h22Im20 : 10; //h22_im_20, reset value: 0x0, access type: RW
		uint32 h22Re21 : 10; //h22_re_21, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap230_u;

/*REG_RF_EMULATOR_TAP_231 0x39C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h22Im21 : 10; //h22_im_21, reset value: 0x0, access type: RW
		uint32 h22Re22 : 10; //h22_re_22, reset value: 0x0, access type: RW
		uint32 h22Im22 : 10; //h22_im_22, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap231_u;

/*REG_RF_EMULATOR_TAP_232 0x3A0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h22Re23 : 10; //h22_re_23, reset value: 0x0, access type: RW
		uint32 h22Im23 : 10; //h22_im_23, reset value: 0x0, access type: RW
		uint32 h22Re24 : 10; //h22_re_24, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap232_u;

/*REG_RF_EMULATOR_TAP_233 0x3A4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h22Im24 : 10; //h22_im_24, reset value: 0x0, access type: RW
		uint32 h22Re25 : 10; //h22_re_25, reset value: 0x0, access type: RW
		uint32 h22Im25 : 10; //h22_im_25, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap233_u;

/*REG_RF_EMULATOR_TAP_234 0x3A8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h22Re26 : 10; //h22_re_26, reset value: 0x0, access type: RW
		uint32 h22Im26 : 10; //h22_im_26, reset value: 0x0, access type: RW
		uint32 h22Re27 : 10; //h22_re_27, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap234_u;

/*REG_RF_EMULATOR_TAP_235 0x3AC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h22Im27 : 10; //h22_im_27, reset value: 0x0, access type: RW
		uint32 h22Re28 : 10; //h22_re_28, reset value: 0x0, access type: RW
		uint32 h22Im28 : 10; //h22_im_28, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap235_u;

/*REG_RF_EMULATOR_TAP_236 0x3B0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h22Re29 : 10; //h22_re_29, reset value: 0x0, access type: RW
		uint32 h22Im29 : 10; //h22_im_29, reset value: 0x0, access type: RW
		uint32 h22Re30 : 10; //h22_re_30, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap236_u;

/*REG_RF_EMULATOR_TAP_237 0x3B4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h22Im30 : 10; //h22_im_30, reset value: 0x0, access type: RW
		uint32 h22Re31 : 10; //h22_re_31, reset value: 0x0, access type: RW
		uint32 h22Im31 : 10; //h22_im_31, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap237_u;

/*REG_RF_EMULATOR_TAP_238 0x3B8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h22Re32 : 10; //h22_re_32, reset value: 0x0, access type: RW
		uint32 h22Im32 : 10; //h22_im_32, reset value: 0x0, access type: RW
		uint32 h22Re33 : 10; //h22_re_33, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap238_u;

/*REG_RF_EMULATOR_TAP_239 0x3BC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h22Im33 : 10; //h22_im_33, reset value: 0x0, access type: RW
		uint32 h22Re34 : 10; //h22_re_34, reset value: 0x0, access type: RW
		uint32 h22Im34 : 10; //h22_im_34, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap239_u;

/*REG_RF_EMULATOR_TAP_240 0x3C0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h22Re35 : 10; //h22_re_35, reset value: 0x0, access type: RW
		uint32 h22Im35 : 10; //h22_im_35, reset value: 0x0, access type: RW
		uint32 h22Re36 : 10; //h22_re_36, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap240_u;

/*REG_RF_EMULATOR_TAP_241 0x3C4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h22Im36 : 10; //h22_im_36, reset value: 0x0, access type: RW
		uint32 h22Re37 : 10; //h22_re_37, reset value: 0x0, access type: RW
		uint32 h22Im37 : 10; //h22_im_37, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap241_u;

/*REG_RF_EMULATOR_TAP_242 0x3C8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h22Re38 : 10; //h22_re_38, reset value: 0x0, access type: RW
		uint32 h22Im38 : 10; //h22_im_38, reset value: 0x0, access type: RW
		uint32 h22Re39 : 10; //h22_re_39, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap242_u;

/*REG_RF_EMULATOR_TAP_243 0x3CC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h22Im39 : 10; //h22_im_39, reset value: 0x0, access type: RW
		uint32 h22Re40 : 10; //h22_re_40, reset value: 0x0, access type: RW
		uint32 h22Im40 : 10; //h22_im_40, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap243_u;

/*REG_RF_EMULATOR_TAP_244 0x3D0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h22Re41 : 10; //h22_re_41, reset value: 0x0, access type: RW
		uint32 h22Im41 : 10; //h22_im_41, reset value: 0x0, access type: RW
		uint32 h22Re42 : 10; //h22_re_42, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap244_u;

/*REG_RF_EMULATOR_TAP_245 0x3D4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h22Im42 : 10; //h22_im_42, reset value: 0x0, access type: RW
		uint32 h22Re43 : 10; //h22_re_43, reset value: 0x0, access type: RW
		uint32 h22Im43 : 10; //h22_im_43, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap245_u;

/*REG_RF_EMULATOR_TAP_246 0x3D8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h22Re44 : 10; //h22_re_44, reset value: 0x0, access type: RW
		uint32 h22Im44 : 10; //h22_im_44, reset value: 0x0, access type: RW
		uint32 h22Re45 : 10; //h22_re_45, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap246_u;

/*REG_RF_EMULATOR_TAP_247 0x3DC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h22Im45 : 10; //h22_im_45, reset value: 0x0, access type: RW
		uint32 h22Re46 : 10; //h22_re_46, reset value: 0x0, access type: RW
		uint32 h22Im46 : 10; //h22_im_46, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap247_u;

/*REG_RF_EMULATOR_TAP_248 0x3E0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h22Re47 : 10; //h22_re_47, reset value: 0x0, access type: RW
		uint32 h22Im47 : 10; //h22_im_47, reset value: 0x0, access type: RW
		uint32 h22Re48 : 10; //h22_re_48, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap248_u;

/*REG_RF_EMULATOR_TAP_249 0x3E4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h22Im48 : 10; //h22_im_48, reset value: 0x0, access type: RW
		uint32 h22Re49 : 10; //h22_re_49, reset value: 0x0, access type: RW
		uint32 h22Im49 : 10; //h22_im_49, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap249_u;

/*REG_RF_EMULATOR_TAP_250 0x3E8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h22Re50 : 10; //h22_re_50, reset value: 0x0, access type: RW
		uint32 h22Im50 : 10; //h22_im_50, reset value: 0x0, access type: RW
		uint32 h22Re51 : 10; //h22_re_51, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap250_u;

/*REG_RF_EMULATOR_TAP_251 0x3EC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h22Im51 : 10; //h22_im_51, reset value: 0x0, access type: RW
		uint32 h22Re52 : 10; //h22_re_52, reset value: 0x0, access type: RW
		uint32 h22Im52 : 10; //h22_im_52, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap251_u;

/*REG_RF_EMULATOR_TAP_252 0x3F0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h22Re53 : 10; //h22_re_53, reset value: 0x0, access type: RW
		uint32 h22Im53 : 10; //h22_im_53, reset value: 0x0, access type: RW
		uint32 h22Re54 : 10; //h22_re_54, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap252_u;

/*REG_RF_EMULATOR_TAP_253 0x3F4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h22Im54 : 10; //h22_im_54, reset value: 0x0, access type: RW
		uint32 h22Re55 : 10; //h22_re_55, reset value: 0x0, access type: RW
		uint32 h22Im55 : 10; //h22_im_55, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap253_u;

/*REG_RF_EMULATOR_TAP_254 0x3F8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h22Re56 : 10; //h22_re_56, reset value: 0x0, access type: RW
		uint32 h22Im56 : 10; //h22_im_56, reset value: 0x0, access type: RW
		uint32 h22Re57 : 10; //h22_re_57, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap254_u;

/*REG_RF_EMULATOR_TAP_255 0x3FC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h22Im57 : 10; //h22_im_57, reset value: 0x0, access type: RW
		uint32 h22Re58 : 10; //h22_re_58, reset value: 0x0, access type: RW
		uint32 h22Im58 : 10; //h22_im_58, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap255_u;

/*REG_RF_EMULATOR_TAP_256 0x400 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h22Re59 : 10; //h22_re_59, reset value: 0x0, access type: RW
		uint32 h22Im59 : 10; //h22_im_59, reset value: 0x0, access type: RW
		uint32 h22Re60 : 10; //h22_re_60, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap256_u;

/*REG_RF_EMULATOR_TAP_257 0x404 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h22Im60 : 10; //h22_im_60, reset value: 0x0, access type: RW
		uint32 h22Re61 : 10; //h22_re_61, reset value: 0x0, access type: RW
		uint32 h22Im61 : 10; //h22_im_61, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap257_u;

/*REG_RF_EMULATOR_TAP_258 0x408 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h22Re62 : 10; //h22_re_62, reset value: 0x0, access type: RW
		uint32 h22Im62 : 10; //h22_im_62, reset value: 0x0, access type: RW
		uint32 h22Re63 : 10; //h22_re_63, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap258_u;

/*REG_RF_EMULATOR_TAP_259 0x40C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h22Im63 : 10; //h22_im_63, reset value: 0x0, access type: RW
		uint32 h22Re64 : 10; //h22_re_64, reset value: 0x0, access type: RW
		uint32 h22Im64 : 10; //h22_im_64, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap259_u;

/*REG_RF_EMULATOR_TAP_260 0x410 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h23Re0 : 10; //h23_re_0, reset value: 0x0, access type: RW
		uint32 h23Im0 : 10; //h23_im_0, reset value: 0x0, access type: RW
		uint32 h23Re1 : 10; //h23_re_1, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap260_u;

/*REG_RF_EMULATOR_TAP_261 0x414 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h23Im1 : 10; //h23_im_1, reset value: 0x0, access type: RW
		uint32 h23Re2 : 10; //h23_re_2, reset value: 0x0, access type: RW
		uint32 h23Im2 : 10; //h23_im_2, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap261_u;

/*REG_RF_EMULATOR_TAP_262 0x418 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h23Re3 : 10; //h23_re_3, reset value: 0x0, access type: RW
		uint32 h23Im3 : 10; //h23_im_3, reset value: 0x0, access type: RW
		uint32 h23Re4 : 10; //h23_re_4, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap262_u;

/*REG_RF_EMULATOR_TAP_263 0x41C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h23Im4 : 10; //h23_im_4, reset value: 0x0, access type: RW
		uint32 h23Re5 : 10; //h23_re_5, reset value: 0x0, access type: RW
		uint32 h23Im5 : 10; //h23_im_5, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap263_u;

/*REG_RF_EMULATOR_TAP_264 0x420 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h23Re6 : 10; //h23_re_6, reset value: 0x0, access type: RW
		uint32 h23Im6 : 10; //h23_im_6, reset value: 0x0, access type: RW
		uint32 h23Re7 : 10; //h23_re_7, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap264_u;

/*REG_RF_EMULATOR_TAP_265 0x424 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h23Im7 : 10; //h23_im_7, reset value: 0x0, access type: RW
		uint32 h23Re8 : 10; //h23_re_8, reset value: 0x0, access type: RW
		uint32 h23Im8 : 10; //h23_im_8, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap265_u;

/*REG_RF_EMULATOR_TAP_266 0x428 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h23Re9 : 10; //h23_re_9, reset value: 0x0, access type: RW
		uint32 h23Im9 : 10; //h23_im_9, reset value: 0x0, access type: RW
		uint32 h23Re10 : 10; //h23_re_10, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap266_u;

/*REG_RF_EMULATOR_TAP_267 0x42C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h23Im10 : 10; //h23_im_10, reset value: 0x0, access type: RW
		uint32 h23Re11 : 10; //h23_re_11, reset value: 0x0, access type: RW
		uint32 h23Im11 : 10; //h23_im_11, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap267_u;

/*REG_RF_EMULATOR_TAP_268 0x430 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h23Re12 : 10; //h23_re_12, reset value: 0x0, access type: RW
		uint32 h23Im12 : 10; //h23_im_12, reset value: 0x0, access type: RW
		uint32 h23Re13 : 10; //h23_re_13, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap268_u;

/*REG_RF_EMULATOR_TAP_269 0x434 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h23Im13 : 10; //h23_im_13, reset value: 0x0, access type: RW
		uint32 h23Re14 : 10; //h23_re_14, reset value: 0x0, access type: RW
		uint32 h23Im14 : 10; //h23_im_14, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap269_u;

/*REG_RF_EMULATOR_TAP_270 0x438 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h23Re15 : 10; //h23_re_15, reset value: 0x0, access type: RW
		uint32 h23Im15 : 10; //h23_im_15, reset value: 0x0, access type: RW
		uint32 h23Re16 : 10; //h23_re_16, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap270_u;

/*REG_RF_EMULATOR_TAP_271 0x43C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h23Im16 : 10; //h23_im_16, reset value: 0x0, access type: RW
		uint32 h23Re17 : 10; //h23_re_17, reset value: 0x0, access type: RW
		uint32 h23Im17 : 10; //h23_im_17, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap271_u;

/*REG_RF_EMULATOR_TAP_272 0x440 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h23Re18 : 10; //h23_re_18, reset value: 0x0, access type: RW
		uint32 h23Im18 : 10; //h23_im_18, reset value: 0x0, access type: RW
		uint32 h23Re19 : 10; //h23_re_19, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap272_u;

/*REG_RF_EMULATOR_TAP_273 0x444 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h23Im19 : 10; //h23_im_19, reset value: 0x0, access type: RW
		uint32 h23Re20 : 10; //h23_re_20, reset value: 0x0, access type: RW
		uint32 h23Im20 : 10; //h23_im_20, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap273_u;

/*REG_RF_EMULATOR_TAP_274 0x448 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h23Re21 : 10; //h23_re_21, reset value: 0x0, access type: RW
		uint32 h23Im21 : 10; //h23_im_21, reset value: 0x0, access type: RW
		uint32 h23Re22 : 10; //h23_re_22, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap274_u;

/*REG_RF_EMULATOR_TAP_275 0x44C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h23Im22 : 10; //h23_im_22, reset value: 0x0, access type: RW
		uint32 h23Re23 : 10; //h23_re_23, reset value: 0x0, access type: RW
		uint32 h23Im23 : 10; //h23_im_23, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap275_u;

/*REG_RF_EMULATOR_TAP_276 0x450 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h23Re24 : 10; //h23_re_24, reset value: 0x0, access type: RW
		uint32 h23Im24 : 10; //h23_im_24, reset value: 0x0, access type: RW
		uint32 h23Re25 : 10; //h23_re_25, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap276_u;

/*REG_RF_EMULATOR_TAP_277 0x454 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h23Im25 : 10; //h23_im_25, reset value: 0x0, access type: RW
		uint32 h23Re26 : 10; //h23_re_26, reset value: 0x0, access type: RW
		uint32 h23Im26 : 10; //h23_im_26, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap277_u;

/*REG_RF_EMULATOR_TAP_278 0x458 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h23Re27 : 10; //h23_re_27, reset value: 0x0, access type: RW
		uint32 h23Im27 : 10; //h23_im_27, reset value: 0x0, access type: RW
		uint32 h23Re28 : 10; //h23_re_28, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap278_u;

/*REG_RF_EMULATOR_TAP_279 0x45C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h23Im28 : 10; //h23_im_28, reset value: 0x0, access type: RW
		uint32 h23Re29 : 10; //h23_re_29, reset value: 0x0, access type: RW
		uint32 h23Im29 : 10; //h23_im_29, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap279_u;

/*REG_RF_EMULATOR_TAP_280 0x460 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h23Re30 : 10; //h23_re_30, reset value: 0x0, access type: RW
		uint32 h23Im30 : 10; //h23_im_30, reset value: 0x0, access type: RW
		uint32 h23Re31 : 10; //h23_re_31, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap280_u;

/*REG_RF_EMULATOR_TAP_281 0x464 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h23Im31 : 10; //h23_im_31, reset value: 0x0, access type: RW
		uint32 h23Re32 : 10; //h23_re_32, reset value: 0x0, access type: RW
		uint32 h23Im32 : 10; //h23_im_32, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap281_u;

/*REG_RF_EMULATOR_TAP_282 0x468 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h23Re33 : 10; //h23_re_33, reset value: 0x0, access type: RW
		uint32 h23Im33 : 10; //h23_im_33, reset value: 0x0, access type: RW
		uint32 h23Re34 : 10; //h23_re_34, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap282_u;

/*REG_RF_EMULATOR_TAP_283 0x46C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h23Im34 : 10; //h23_im_34, reset value: 0x0, access type: RW
		uint32 h23Re35 : 10; //h23_re_35, reset value: 0x0, access type: RW
		uint32 h23Im35 : 10; //h23_im_35, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap283_u;

/*REG_RF_EMULATOR_TAP_284 0x470 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h23Re36 : 10; //h23_re_36, reset value: 0x0, access type: RW
		uint32 h23Im36 : 10; //h23_im_36, reset value: 0x0, access type: RW
		uint32 h23Re37 : 10; //h23_re_37, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap284_u;

/*REG_RF_EMULATOR_TAP_285 0x474 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h23Im37 : 10; //h23_im_37, reset value: 0x0, access type: RW
		uint32 h23Re38 : 10; //h23_re_38, reset value: 0x0, access type: RW
		uint32 h23Im38 : 10; //h23_im_38, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap285_u;

/*REG_RF_EMULATOR_TAP_286 0x478 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h23Re39 : 10; //h23_re_39, reset value: 0x0, access type: RW
		uint32 h23Im39 : 10; //h23_im_39, reset value: 0x0, access type: RW
		uint32 h23Re40 : 10; //h23_re_40, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap286_u;

/*REG_RF_EMULATOR_TAP_287 0x47C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h23Im40 : 10; //h23_im_40, reset value: 0x0, access type: RW
		uint32 h23Re41 : 10; //h23_re_41, reset value: 0x0, access type: RW
		uint32 h23Im41 : 10; //h23_im_41, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap287_u;

/*REG_RF_EMULATOR_TAP_288 0x480 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h23Re42 : 10; //h23_re_42, reset value: 0x0, access type: RW
		uint32 h23Im42 : 10; //h23_im_42, reset value: 0x0, access type: RW
		uint32 h23Re43 : 10; //h23_re_43, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap288_u;

/*REG_RF_EMULATOR_TAP_289 0x484 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h23Im43 : 10; //h23_im_43, reset value: 0x0, access type: RW
		uint32 h23Re44 : 10; //h23_re_44, reset value: 0x0, access type: RW
		uint32 h23Im44 : 10; //h23_im_44, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap289_u;

/*REG_RF_EMULATOR_TAP_290 0x488 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h23Re45 : 10; //h23_re_45, reset value: 0x0, access type: RW
		uint32 h23Im45 : 10; //h23_im_45, reset value: 0x0, access type: RW
		uint32 h23Re46 : 10; //h23_re_46, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap290_u;

/*REG_RF_EMULATOR_TAP_291 0x48C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h23Im46 : 10; //h23_im_46, reset value: 0x0, access type: RW
		uint32 h23Re47 : 10; //h23_re_47, reset value: 0x0, access type: RW
		uint32 h23Im47 : 10; //h23_im_47, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap291_u;

/*REG_RF_EMULATOR_TAP_292 0x490 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h23Re48 : 10; //h23_re_48, reset value: 0x0, access type: RW
		uint32 h23Im48 : 10; //h23_im_48, reset value: 0x0, access type: RW
		uint32 h23Re49 : 10; //h23_re_49, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap292_u;

/*REG_RF_EMULATOR_TAP_293 0x494 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h23Im49 : 10; //h23_im_49, reset value: 0x0, access type: RW
		uint32 h23Re50 : 10; //h23_re_50, reset value: 0x0, access type: RW
		uint32 h23Im50 : 10; //h23_im_50, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap293_u;

/*REG_RF_EMULATOR_TAP_294 0x498 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h23Re51 : 10; //h23_re_51, reset value: 0x0, access type: RW
		uint32 h23Im51 : 10; //h23_im_51, reset value: 0x0, access type: RW
		uint32 h23Re52 : 10; //h23_re_52, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap294_u;

/*REG_RF_EMULATOR_TAP_295 0x49C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h23Im52 : 10; //h23_im_52, reset value: 0x0, access type: RW
		uint32 h23Re53 : 10; //h23_re_53, reset value: 0x0, access type: RW
		uint32 h23Im53 : 10; //h23_im_53, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap295_u;

/*REG_RF_EMULATOR_TAP_296 0x4A0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h23Re54 : 10; //h23_re_54, reset value: 0x0, access type: RW
		uint32 h23Im54 : 10; //h23_im_54, reset value: 0x0, access type: RW
		uint32 h23Re55 : 10; //h23_re_55, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap296_u;

/*REG_RF_EMULATOR_TAP_297 0x4A4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h23Im55 : 10; //h23_im_55, reset value: 0x0, access type: RW
		uint32 h23Re56 : 10; //h23_re_56, reset value: 0x0, access type: RW
		uint32 h23Im56 : 10; //h23_im_56, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap297_u;

/*REG_RF_EMULATOR_TAP_298 0x4A8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h23Re57 : 10; //h23_re_57, reset value: 0x0, access type: RW
		uint32 h23Im57 : 10; //h23_im_57, reset value: 0x0, access type: RW
		uint32 h23Re58 : 10; //h23_re_58, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap298_u;

/*REG_RF_EMULATOR_TAP_299 0x4AC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h23Im58 : 10; //h23_im_58, reset value: 0x0, access type: RW
		uint32 h23Re59 : 10; //h23_re_59, reset value: 0x0, access type: RW
		uint32 h23Im59 : 10; //h23_im_59, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap299_u;

/*REG_RF_EMULATOR_TAP_300 0x4B0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h23Re60 : 10; //h23_re_60, reset value: 0x0, access type: RW
		uint32 h23Im60 : 10; //h23_im_60, reset value: 0x0, access type: RW
		uint32 h23Re61 : 10; //h23_re_61, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap300_u;

/*REG_RF_EMULATOR_TAP_301 0x4B4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h23Im61 : 10; //h23_im_61, reset value: 0x0, access type: RW
		uint32 h23Re62 : 10; //h23_re_62, reset value: 0x0, access type: RW
		uint32 h23Im62 : 10; //h23_im_62, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap301_u;

/*REG_RF_EMULATOR_TAP_302 0x4B8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h23Re63 : 10; //h23_re_63, reset value: 0x0, access type: RW
		uint32 h23Im63 : 10; //h23_im_63, reset value: 0x0, access type: RW
		uint32 h23Re64 : 10; //h23_re_64, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap302_u;

/*REG_RF_EMULATOR_TAP_303 0x4BC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h23Im64 : 10; //h23_im_64, reset value: 0x0, access type: RW
		uint32 h24Re0 : 10; //h24_re_0, reset value: 0x0, access type: RW
		uint32 h24Im0 : 10; //h24_im_0, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap303_u;

/*REG_RF_EMULATOR_TAP_304 0x4C0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h24Re1 : 10; //h24_re_1, reset value: 0x0, access type: RW
		uint32 h24Im1 : 10; //h24_im_1, reset value: 0x0, access type: RW
		uint32 h24Re2 : 10; //h24_re_2, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap304_u;

/*REG_RF_EMULATOR_TAP_305 0x4C4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h24Im2 : 10; //h24_im_2, reset value: 0x0, access type: RW
		uint32 h24Re3 : 10; //h24_re_3, reset value: 0x0, access type: RW
		uint32 h24Im3 : 10; //h24_im_3, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap305_u;

/*REG_RF_EMULATOR_TAP_306 0x4C8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h24Re4 : 10; //h24_re_4, reset value: 0x0, access type: RW
		uint32 h24Im4 : 10; //h24_im_4, reset value: 0x0, access type: RW
		uint32 h24Re5 : 10; //h24_re_5, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap306_u;

/*REG_RF_EMULATOR_TAP_307 0x4CC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h24Im5 : 10; //h24_im_5, reset value: 0x0, access type: RW
		uint32 h24Re6 : 10; //h24_re_6, reset value: 0x0, access type: RW
		uint32 h24Im6 : 10; //h24_im_6, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap307_u;

/*REG_RF_EMULATOR_TAP_308 0x4D0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h24Re7 : 10; //h24_re_7, reset value: 0x0, access type: RW
		uint32 h24Im7 : 10; //h24_im_7, reset value: 0x0, access type: RW
		uint32 h24Re8 : 10; //h24_re_8, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap308_u;

/*REG_RF_EMULATOR_TAP_309 0x4D4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h24Im8 : 10; //h24_im_8, reset value: 0x0, access type: RW
		uint32 h24Re9 : 10; //h24_re_9, reset value: 0x0, access type: RW
		uint32 h24Im9 : 10; //h24_im_9, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap309_u;

/*REG_RF_EMULATOR_TAP_310 0x4D8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h24Re10 : 10; //h24_re_10, reset value: 0x0, access type: RW
		uint32 h24Im10 : 10; //h24_im_10, reset value: 0x0, access type: RW
		uint32 h24Re11 : 10; //h24_re_11, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap310_u;

/*REG_RF_EMULATOR_TAP_311 0x4DC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h24Im11 : 10; //h24_im_11, reset value: 0x0, access type: RW
		uint32 h24Re12 : 10; //h24_re_12, reset value: 0x0, access type: RW
		uint32 h24Im12 : 10; //h24_im_12, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap311_u;

/*REG_RF_EMULATOR_TAP_312 0x4E0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h24Re13 : 10; //h24_re_13, reset value: 0x0, access type: RW
		uint32 h24Im13 : 10; //h24_im_13, reset value: 0x0, access type: RW
		uint32 h24Re14 : 10; //h24_re_14, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap312_u;

/*REG_RF_EMULATOR_TAP_313 0x4E4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h24Im14 : 10; //h24_im_14, reset value: 0x0, access type: RW
		uint32 h24Re15 : 10; //h24_re_15, reset value: 0x0, access type: RW
		uint32 h24Im15 : 10; //h24_im_15, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap313_u;

/*REG_RF_EMULATOR_TAP_314 0x4E8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h24Re16 : 10; //h24_re_16, reset value: 0x0, access type: RW
		uint32 h24Im16 : 10; //h24_im_16, reset value: 0x0, access type: RW
		uint32 h24Re17 : 10; //h24_re_17, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap314_u;

/*REG_RF_EMULATOR_TAP_315 0x4EC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h24Im17 : 10; //h24_im_17, reset value: 0x0, access type: RW
		uint32 h24Re18 : 10; //h24_re_18, reset value: 0x0, access type: RW
		uint32 h24Im18 : 10; //h24_im_18, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap315_u;

/*REG_RF_EMULATOR_TAP_316 0x4F0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h24Re19 : 10; //h24_re_19, reset value: 0x0, access type: RW
		uint32 h24Im19 : 10; //h24_im_19, reset value: 0x0, access type: RW
		uint32 h24Re20 : 10; //h24_re_20, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap316_u;

/*REG_RF_EMULATOR_TAP_317 0x4F4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h24Im20 : 10; //h24_im_20, reset value: 0x0, access type: RW
		uint32 h24Re21 : 10; //h24_re_21, reset value: 0x0, access type: RW
		uint32 h24Im21 : 10; //h24_im_21, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap317_u;

/*REG_RF_EMULATOR_TAP_318 0x4F8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h24Re22 : 10; //h24_re_22, reset value: 0x0, access type: RW
		uint32 h24Im22 : 10; //h24_im_22, reset value: 0x0, access type: RW
		uint32 h24Re23 : 10; //h24_re_23, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap318_u;

/*REG_RF_EMULATOR_TAP_319 0x4FC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h24Im23 : 10; //h24_im_23, reset value: 0x0, access type: RW
		uint32 h24Re24 : 10; //h24_re_24, reset value: 0x0, access type: RW
		uint32 h24Im24 : 10; //h24_im_24, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap319_u;

/*REG_RF_EMULATOR_TAP_320 0x500 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h24Re25 : 10; //h24_re_25, reset value: 0x0, access type: RW
		uint32 h24Im25 : 10; //h24_im_25, reset value: 0x0, access type: RW
		uint32 h24Re26 : 10; //h24_re_26, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap320_u;

/*REG_RF_EMULATOR_TAP_321 0x504 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h24Im26 : 10; //h24_im_26, reset value: 0x0, access type: RW
		uint32 h24Re27 : 10; //h24_re_27, reset value: 0x0, access type: RW
		uint32 h24Im27 : 10; //h24_im_27, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap321_u;

/*REG_RF_EMULATOR_TAP_322 0x508 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h24Re28 : 10; //h24_re_28, reset value: 0x0, access type: RW
		uint32 h24Im28 : 10; //h24_im_28, reset value: 0x0, access type: RW
		uint32 h24Re29 : 10; //h24_re_29, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap322_u;

/*REG_RF_EMULATOR_TAP_323 0x50C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h24Im29 : 10; //h24_im_29, reset value: 0x0, access type: RW
		uint32 h24Re30 : 10; //h24_re_30, reset value: 0x0, access type: RW
		uint32 h24Im30 : 10; //h24_im_30, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap323_u;

/*REG_RF_EMULATOR_TAP_324 0x510 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h24Re31 : 10; //h24_re_31, reset value: 0x0, access type: RW
		uint32 h24Im31 : 10; //h24_im_31, reset value: 0x0, access type: RW
		uint32 h24Re32 : 10; //h24_re_32, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap324_u;

/*REG_RF_EMULATOR_TAP_325 0x514 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h24Im32 : 10; //h24_im_32, reset value: 0x0, access type: RW
		uint32 h24Re33 : 10; //h24_re_33, reset value: 0x0, access type: RW
		uint32 h24Im33 : 10; //h24_im_33, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap325_u;

/*REG_RF_EMULATOR_TAP_326 0x518 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h24Re34 : 10; //h24_re_34, reset value: 0x0, access type: RW
		uint32 h24Im34 : 10; //h24_im_34, reset value: 0x0, access type: RW
		uint32 h24Re35 : 10; //h24_re_35, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap326_u;

/*REG_RF_EMULATOR_TAP_327 0x51C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h24Im35 : 10; //h24_im_35, reset value: 0x0, access type: RW
		uint32 h24Re36 : 10; //h24_re_36, reset value: 0x0, access type: RW
		uint32 h24Im36 : 10; //h24_im_36, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap327_u;

/*REG_RF_EMULATOR_TAP_328 0x520 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h24Re37 : 10; //h24_re_37, reset value: 0x0, access type: RW
		uint32 h24Im37 : 10; //h24_im_37, reset value: 0x0, access type: RW
		uint32 h24Re38 : 10; //h24_re_38, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap328_u;

/*REG_RF_EMULATOR_TAP_329 0x524 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h24Im38 : 10; //h24_im_38, reset value: 0x0, access type: RW
		uint32 h24Re39 : 10; //h24_re_39, reset value: 0x0, access type: RW
		uint32 h24Im39 : 10; //h24_im_39, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap329_u;

/*REG_RF_EMULATOR_TAP_330 0x528 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h24Re40 : 10; //h24_re_40, reset value: 0x0, access type: RW
		uint32 h24Im40 : 10; //h24_im_40, reset value: 0x0, access type: RW
		uint32 h24Re41 : 10; //h24_re_41, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap330_u;

/*REG_RF_EMULATOR_TAP_331 0x52C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h24Im41 : 10; //h24_im_41, reset value: 0x0, access type: RW
		uint32 h24Re42 : 10; //h24_re_42, reset value: 0x0, access type: RW
		uint32 h24Im42 : 10; //h24_im_42, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap331_u;

/*REG_RF_EMULATOR_TAP_332 0x530 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h24Re43 : 10; //h24_re_43, reset value: 0x0, access type: RW
		uint32 h24Im43 : 10; //h24_im_43, reset value: 0x0, access type: RW
		uint32 h24Re44 : 10; //h24_re_44, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap332_u;

/*REG_RF_EMULATOR_TAP_333 0x534 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h24Im44 : 10; //h24_im_44, reset value: 0x0, access type: RW
		uint32 h24Re45 : 10; //h24_re_45, reset value: 0x0, access type: RW
		uint32 h24Im45 : 10; //h24_im_45, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap333_u;

/*REG_RF_EMULATOR_TAP_334 0x538 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h24Re46 : 10; //h24_re_46, reset value: 0x0, access type: RW
		uint32 h24Im46 : 10; //h24_im_46, reset value: 0x0, access type: RW
		uint32 h24Re47 : 10; //h24_re_47, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap334_u;

/*REG_RF_EMULATOR_TAP_335 0x53C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h24Im47 : 10; //h24_im_47, reset value: 0x0, access type: RW
		uint32 h24Re48 : 10; //h24_re_48, reset value: 0x0, access type: RW
		uint32 h24Im48 : 10; //h24_im_48, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap335_u;

/*REG_RF_EMULATOR_TAP_336 0x540 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h24Re49 : 10; //h24_re_49, reset value: 0x0, access type: RW
		uint32 h24Im49 : 10; //h24_im_49, reset value: 0x0, access type: RW
		uint32 h24Re50 : 10; //h24_re_50, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap336_u;

/*REG_RF_EMULATOR_TAP_337 0x544 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h24Im50 : 10; //h24_im_50, reset value: 0x0, access type: RW
		uint32 h24Re51 : 10; //h24_re_51, reset value: 0x0, access type: RW
		uint32 h24Im51 : 10; //h24_im_51, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap337_u;

/*REG_RF_EMULATOR_TAP_338 0x548 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h24Re52 : 10; //h24_re_52, reset value: 0x0, access type: RW
		uint32 h24Im52 : 10; //h24_im_52, reset value: 0x0, access type: RW
		uint32 h24Re53 : 10; //h24_re_53, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap338_u;

/*REG_RF_EMULATOR_TAP_339 0x54C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h24Im53 : 10; //h24_im_53, reset value: 0x0, access type: RW
		uint32 h24Re54 : 10; //h24_re_54, reset value: 0x0, access type: RW
		uint32 h24Im54 : 10; //h24_im_54, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap339_u;

/*REG_RF_EMULATOR_TAP_340 0x550 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h24Re55 : 10; //h24_re_55, reset value: 0x0, access type: RW
		uint32 h24Im55 : 10; //h24_im_55, reset value: 0x0, access type: RW
		uint32 h24Re56 : 10; //h24_re_56, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap340_u;

/*REG_RF_EMULATOR_TAP_341 0x554 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h24Im56 : 10; //h24_im_56, reset value: 0x0, access type: RW
		uint32 h24Re57 : 10; //h24_re_57, reset value: 0x0, access type: RW
		uint32 h24Im57 : 10; //h24_im_57, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap341_u;

/*REG_RF_EMULATOR_TAP_342 0x558 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h24Re58 : 10; //h24_re_58, reset value: 0x0, access type: RW
		uint32 h24Im58 : 10; //h24_im_58, reset value: 0x0, access type: RW
		uint32 h24Re59 : 10; //h24_re_59, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap342_u;

/*REG_RF_EMULATOR_TAP_343 0x55C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h24Im59 : 10; //h24_im_59, reset value: 0x0, access type: RW
		uint32 h24Re60 : 10; //h24_re_60, reset value: 0x0, access type: RW
		uint32 h24Im60 : 10; //h24_im_60, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap343_u;

/*REG_RF_EMULATOR_TAP_344 0x560 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h24Re61 : 10; //h24_re_61, reset value: 0x0, access type: RW
		uint32 h24Im61 : 10; //h24_im_61, reset value: 0x0, access type: RW
		uint32 h24Re62 : 10; //h24_re_62, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap344_u;

/*REG_RF_EMULATOR_TAP_345 0x564 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h24Im62 : 10; //h24_im_62, reset value: 0x0, access type: RW
		uint32 h24Re63 : 10; //h24_re_63, reset value: 0x0, access type: RW
		uint32 h24Im63 : 10; //h24_im_63, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap345_u;

/*REG_RF_EMULATOR_TAP_346 0x568 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h24Re64 : 10; //h24_re_64, reset value: 0x0, access type: RW
		uint32 h24Im64 : 10; //h24_im_64, reset value: 0x0, access type: RW
		uint32 h31Re0 : 10; //h31_re_0, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap346_u;

/*REG_RF_EMULATOR_TAP_347 0x56C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h31Im0 : 10; //h31_im_0, reset value: 0x0, access type: RW
		uint32 h31Re1 : 10; //h31_re_1, reset value: 0x0, access type: RW
		uint32 h31Im1 : 10; //h31_im_1, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap347_u;

/*REG_RF_EMULATOR_TAP_348 0x570 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h31Re2 : 10; //h31_re_2, reset value: 0x0, access type: RW
		uint32 h31Im2 : 10; //h31_im_2, reset value: 0x0, access type: RW
		uint32 h31Re3 : 10; //h31_re_3, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap348_u;

/*REG_RF_EMULATOR_TAP_349 0x574 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h31Im3 : 10; //h31_im_3, reset value: 0x0, access type: RW
		uint32 h31Re4 : 10; //h31_re_4, reset value: 0x0, access type: RW
		uint32 h31Im4 : 10; //h31_im_4, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap349_u;

/*REG_RF_EMULATOR_TAP_350 0x578 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h31Re5 : 10; //h31_re_5, reset value: 0x0, access type: RW
		uint32 h31Im5 : 10; //h31_im_5, reset value: 0x0, access type: RW
		uint32 h31Re6 : 10; //h31_re_6, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap350_u;

/*REG_RF_EMULATOR_TAP_351 0x57C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h31Im6 : 10; //h31_im_6, reset value: 0x0, access type: RW
		uint32 h31Re7 : 10; //h31_re_7, reset value: 0x0, access type: RW
		uint32 h31Im7 : 10; //h31_im_7, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap351_u;

/*REG_RF_EMULATOR_TAP_352 0x580 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h31Re8 : 10; //h31_re_8, reset value: 0x0, access type: RW
		uint32 h31Im8 : 10; //h31_im_8, reset value: 0x0, access type: RW
		uint32 h31Re9 : 10; //h31_re_9, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap352_u;

/*REG_RF_EMULATOR_TAP_353 0x584 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h31Im9 : 10; //h31_im_9, reset value: 0x0, access type: RW
		uint32 h31Re10 : 10; //h31_re_10, reset value: 0x0, access type: RW
		uint32 h31Im10 : 10; //h31_im_10, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap353_u;

/*REG_RF_EMULATOR_TAP_354 0x588 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h31Re11 : 10; //h31_re_11, reset value: 0x0, access type: RW
		uint32 h31Im11 : 10; //h31_im_11, reset value: 0x0, access type: RW
		uint32 h31Re12 : 10; //h31_re_12, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap354_u;

/*REG_RF_EMULATOR_TAP_355 0x58C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h31Im12 : 10; //h31_im_12, reset value: 0x0, access type: RW
		uint32 h31Re13 : 10; //h31_re_13, reset value: 0x0, access type: RW
		uint32 h31Im13 : 10; //h31_im_13, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap355_u;

/*REG_RF_EMULATOR_TAP_356 0x590 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h31Re14 : 10; //h31_re_14, reset value: 0x0, access type: RW
		uint32 h31Im14 : 10; //h31_im_14, reset value: 0x0, access type: RW
		uint32 h31Re15 : 10; //h31_re_15, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap356_u;

/*REG_RF_EMULATOR_TAP_357 0x594 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h31Im15 : 10; //h31_im_15, reset value: 0x0, access type: RW
		uint32 h31Re16 : 10; //h31_re_16, reset value: 0x0, access type: RW
		uint32 h31Im16 : 10; //h31_im_16, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap357_u;

/*REG_RF_EMULATOR_TAP_358 0x598 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h31Re17 : 10; //h31_re_17, reset value: 0x0, access type: RW
		uint32 h31Im17 : 10; //h31_im_17, reset value: 0x0, access type: RW
		uint32 h31Re18 : 10; //h31_re_18, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap358_u;

/*REG_RF_EMULATOR_TAP_359 0x59C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h31Im18 : 10; //h31_im_18, reset value: 0x0, access type: RW
		uint32 h31Re19 : 10; //h31_re_19, reset value: 0x0, access type: RW
		uint32 h31Im19 : 10; //h31_im_19, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap359_u;

/*REG_RF_EMULATOR_TAP_360 0x5A0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h31Re20 : 10; //h31_re_20, reset value: 0x0, access type: RW
		uint32 h31Im20 : 10; //h31_im_20, reset value: 0x0, access type: RW
		uint32 h31Re21 : 10; //h31_re_21, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap360_u;

/*REG_RF_EMULATOR_TAP_361 0x5A4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h31Im21 : 10; //h31_im_21, reset value: 0x0, access type: RW
		uint32 h31Re22 : 10; //h31_re_22, reset value: 0x0, access type: RW
		uint32 h31Im22 : 10; //h31_im_22, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap361_u;

/*REG_RF_EMULATOR_TAP_362 0x5A8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h31Re23 : 10; //h31_re_23, reset value: 0x0, access type: RW
		uint32 h31Im23 : 10; //h31_im_23, reset value: 0x0, access type: RW
		uint32 h31Re24 : 10; //h31_re_24, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap362_u;

/*REG_RF_EMULATOR_TAP_363 0x5AC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h31Im24 : 10; //h31_im_24, reset value: 0x0, access type: RW
		uint32 h31Re25 : 10; //h31_re_25, reset value: 0x0, access type: RW
		uint32 h31Im25 : 10; //h31_im_25, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap363_u;

/*REG_RF_EMULATOR_TAP_364 0x5B0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h31Re26 : 10; //h31_re_26, reset value: 0x0, access type: RW
		uint32 h31Im26 : 10; //h31_im_26, reset value: 0x0, access type: RW
		uint32 h31Re27 : 10; //h31_re_27, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap364_u;

/*REG_RF_EMULATOR_TAP_365 0x5B4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h31Im27 : 10; //h31_im_27, reset value: 0x0, access type: RW
		uint32 h31Re28 : 10; //h31_re_28, reset value: 0x0, access type: RW
		uint32 h31Im28 : 10; //h31_im_28, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap365_u;

/*REG_RF_EMULATOR_TAP_366 0x5B8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h31Re29 : 10; //h31_re_29, reset value: 0x0, access type: RW
		uint32 h31Im29 : 10; //h31_im_29, reset value: 0x0, access type: RW
		uint32 h31Re30 : 10; //h31_re_30, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap366_u;

/*REG_RF_EMULATOR_TAP_367 0x5BC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h31Im30 : 10; //h31_im_30, reset value: 0x0, access type: RW
		uint32 h31Re31 : 10; //h31_re_31, reset value: 0x0, access type: RW
		uint32 h31Im31 : 10; //h31_im_31, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap367_u;

/*REG_RF_EMULATOR_TAP_368 0x5C0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h31Re32 : 10; //h31_re_32, reset value: 0x0, access type: RW
		uint32 h31Im32 : 10; //h31_im_32, reset value: 0x0, access type: RW
		uint32 h31Re33 : 10; //h31_re_33, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap368_u;

/*REG_RF_EMULATOR_TAP_369 0x5C4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h31Im33 : 10; //h31_im_33, reset value: 0x0, access type: RW
		uint32 h31Re34 : 10; //h31_re_34, reset value: 0x0, access type: RW
		uint32 h31Im34 : 10; //h31_im_34, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap369_u;

/*REG_RF_EMULATOR_TAP_370 0x5C8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h31Re35 : 10; //h31_re_35, reset value: 0x0, access type: RW
		uint32 h31Im35 : 10; //h31_im_35, reset value: 0x0, access type: RW
		uint32 h31Re36 : 10; //h31_re_36, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap370_u;

/*REG_RF_EMULATOR_TAP_371 0x5CC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h31Im36 : 10; //h31_im_36, reset value: 0x0, access type: RW
		uint32 h31Re37 : 10; //h31_re_37, reset value: 0x0, access type: RW
		uint32 h31Im37 : 10; //h31_im_37, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap371_u;

/*REG_RF_EMULATOR_TAP_372 0x5D0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h31Re38 : 10; //h31_re_38, reset value: 0x0, access type: RW
		uint32 h31Im38 : 10; //h31_im_38, reset value: 0x0, access type: RW
		uint32 h31Re39 : 10; //h31_re_39, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap372_u;

/*REG_RF_EMULATOR_TAP_373 0x5D4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h31Im39 : 10; //h31_im_39, reset value: 0x0, access type: RW
		uint32 h31Re40 : 10; //h31_re_40, reset value: 0x0, access type: RW
		uint32 h31Im40 : 10; //h31_im_40, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap373_u;

/*REG_RF_EMULATOR_TAP_374 0x5D8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h31Re41 : 10; //h31_re_41, reset value: 0x0, access type: RW
		uint32 h31Im41 : 10; //h31_im_41, reset value: 0x0, access type: RW
		uint32 h31Re42 : 10; //h31_re_42, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap374_u;

/*REG_RF_EMULATOR_TAP_375 0x5DC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h31Im42 : 10; //h31_im_42, reset value: 0x0, access type: RW
		uint32 h31Re43 : 10; //h31_re_43, reset value: 0x0, access type: RW
		uint32 h31Im43 : 10; //h31_im_43, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap375_u;

/*REG_RF_EMULATOR_TAP_376 0x5E0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h31Re44 : 10; //h31_re_44, reset value: 0x0, access type: RW
		uint32 h31Im44 : 10; //h31_im_44, reset value: 0x0, access type: RW
		uint32 h31Re45 : 10; //h31_re_45, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap376_u;

/*REG_RF_EMULATOR_TAP_377 0x5E4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h31Im45 : 10; //h31_im_45, reset value: 0x0, access type: RW
		uint32 h31Re46 : 10; //h31_re_46, reset value: 0x0, access type: RW
		uint32 h31Im46 : 10; //h31_im_46, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap377_u;

/*REG_RF_EMULATOR_TAP_378 0x5E8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h31Re47 : 10; //h31_re_47, reset value: 0x0, access type: RW
		uint32 h31Im47 : 10; //h31_im_47, reset value: 0x0, access type: RW
		uint32 h31Re48 : 10; //h31_re_48, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap378_u;

/*REG_RF_EMULATOR_TAP_379 0x5EC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h31Im48 : 10; //h31_im_48, reset value: 0x0, access type: RW
		uint32 h31Re49 : 10; //h31_re_49, reset value: 0x0, access type: RW
		uint32 h31Im49 : 10; //h31_im_49, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap379_u;

/*REG_RF_EMULATOR_TAP_380 0x5F0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h31Re50 : 10; //h31_re_50, reset value: 0x0, access type: RW
		uint32 h31Im50 : 10; //h31_im_50, reset value: 0x0, access type: RW
		uint32 h31Re51 : 10; //h31_re_51, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap380_u;

/*REG_RF_EMULATOR_TAP_381 0x5F4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h31Im51 : 10; //h31_im_51, reset value: 0x0, access type: RW
		uint32 h31Re52 : 10; //h31_re_52, reset value: 0x0, access type: RW
		uint32 h31Im52 : 10; //h31_im_52, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap381_u;

/*REG_RF_EMULATOR_TAP_382 0x5F8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h31Re53 : 10; //h31_re_53, reset value: 0x0, access type: RW
		uint32 h31Im53 : 10; //h31_im_53, reset value: 0x0, access type: RW
		uint32 h31Re54 : 10; //h31_re_54, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap382_u;

/*REG_RF_EMULATOR_TAP_383 0x5FC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h31Im54 : 10; //h31_im_54, reset value: 0x0, access type: RW
		uint32 h31Re55 : 10; //h31_re_55, reset value: 0x0, access type: RW
		uint32 h31Im55 : 10; //h31_im_55, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap383_u;

/*REG_RF_EMULATOR_TAP_384 0x600 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h31Re56 : 10; //h31_re_56, reset value: 0x0, access type: RW
		uint32 h31Im56 : 10; //h31_im_56, reset value: 0x0, access type: RW
		uint32 h31Re57 : 10; //h31_re_57, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap384_u;

/*REG_RF_EMULATOR_TAP_385 0x604 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h31Im57 : 10; //h31_im_57, reset value: 0x0, access type: RW
		uint32 h31Re58 : 10; //h31_re_58, reset value: 0x0, access type: RW
		uint32 h31Im58 : 10; //h31_im_58, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap385_u;

/*REG_RF_EMULATOR_TAP_386 0x608 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h31Re59 : 10; //h31_re_59, reset value: 0x0, access type: RW
		uint32 h31Im59 : 10; //h31_im_59, reset value: 0x0, access type: RW
		uint32 h31Re60 : 10; //h31_re_60, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap386_u;

/*REG_RF_EMULATOR_TAP_387 0x60C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h31Im60 : 10; //h31_im_60, reset value: 0x0, access type: RW
		uint32 h31Re61 : 10; //h31_re_61, reset value: 0x0, access type: RW
		uint32 h31Im61 : 10; //h31_im_61, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap387_u;

/*REG_RF_EMULATOR_TAP_388 0x610 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h31Re62 : 10; //h31_re_62, reset value: 0x0, access type: RW
		uint32 h31Im62 : 10; //h31_im_62, reset value: 0x0, access type: RW
		uint32 h31Re63 : 10; //h31_re_63, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap388_u;

/*REG_RF_EMULATOR_TAP_389 0x614 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h31Im63 : 10; //h31_im_63, reset value: 0x0, access type: RW
		uint32 h31Re64 : 10; //h31_re_64, reset value: 0x0, access type: RW
		uint32 h31Im64 : 10; //h31_im_64, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap389_u;

/*REG_RF_EMULATOR_TAP_390 0x618 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h32Re0 : 10; //h32_re_0, reset value: 0x0, access type: RW
		uint32 h32Im0 : 10; //h32_im_0, reset value: 0x0, access type: RW
		uint32 h32Re1 : 10; //h32_re_1, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap390_u;

/*REG_RF_EMULATOR_TAP_391 0x61C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h32Im1 : 10; //h32_im_1, reset value: 0x0, access type: RW
		uint32 h32Re2 : 10; //h32_re_2, reset value: 0x0, access type: RW
		uint32 h32Im2 : 10; //h32_im_2, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap391_u;

/*REG_RF_EMULATOR_TAP_392 0x620 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h32Re3 : 10; //h32_re_3, reset value: 0x0, access type: RW
		uint32 h32Im3 : 10; //h32_im_3, reset value: 0x0, access type: RW
		uint32 h32Re4 : 10; //h32_re_4, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap392_u;

/*REG_RF_EMULATOR_TAP_393 0x624 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h32Im4 : 10; //h32_im_4, reset value: 0x0, access type: RW
		uint32 h32Re5 : 10; //h32_re_5, reset value: 0x0, access type: RW
		uint32 h32Im5 : 10; //h32_im_5, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap393_u;

/*REG_RF_EMULATOR_TAP_394 0x628 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h32Re6 : 10; //h32_re_6, reset value: 0x0, access type: RW
		uint32 h32Im6 : 10; //h32_im_6, reset value: 0x0, access type: RW
		uint32 h32Re7 : 10; //h32_re_7, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap394_u;

/*REG_RF_EMULATOR_TAP_395 0x62C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h32Im7 : 10; //h32_im_7, reset value: 0x0, access type: RW
		uint32 h32Re8 : 10; //h32_re_8, reset value: 0x0, access type: RW
		uint32 h32Im8 : 10; //h32_im_8, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap395_u;

/*REG_RF_EMULATOR_TAP_396 0x630 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h32Re9 : 10; //h32_re_9, reset value: 0x0, access type: RW
		uint32 h32Im9 : 10; //h32_im_9, reset value: 0x0, access type: RW
		uint32 h32Re10 : 10; //h32_re_10, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap396_u;

/*REG_RF_EMULATOR_TAP_397 0x634 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h32Im10 : 10; //h32_im_10, reset value: 0x0, access type: RW
		uint32 h32Re11 : 10; //h32_re_11, reset value: 0x0, access type: RW
		uint32 h32Im11 : 10; //h32_im_11, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap397_u;

/*REG_RF_EMULATOR_TAP_398 0x638 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h32Re12 : 10; //h32_re_12, reset value: 0x0, access type: RW
		uint32 h32Im12 : 10; //h32_im_12, reset value: 0x0, access type: RW
		uint32 h32Re13 : 10; //h32_re_13, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap398_u;

/*REG_RF_EMULATOR_TAP_399 0x63C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h32Im13 : 10; //h32_im_13, reset value: 0x0, access type: RW
		uint32 h32Re14 : 10; //h32_re_14, reset value: 0x0, access type: RW
		uint32 h32Im14 : 10; //h32_im_14, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap399_u;

/*REG_RF_EMULATOR_TAP_400 0x640 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h32Re15 : 10; //h32_re_15, reset value: 0x0, access type: RW
		uint32 h32Im15 : 10; //h32_im_15, reset value: 0x0, access type: RW
		uint32 h32Re16 : 10; //h32_re_16, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap400_u;

/*REG_RF_EMULATOR_TAP_401 0x644 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h32Im16 : 10; //h32_im_16, reset value: 0x0, access type: RW
		uint32 h32Re17 : 10; //h32_re_17, reset value: 0x0, access type: RW
		uint32 h32Im17 : 10; //h32_im_17, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap401_u;

/*REG_RF_EMULATOR_TAP_402 0x648 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h32Re18 : 10; //h32_re_18, reset value: 0x0, access type: RW
		uint32 h32Im18 : 10; //h32_im_18, reset value: 0x0, access type: RW
		uint32 h32Re19 : 10; //h32_re_19, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap402_u;

/*REG_RF_EMULATOR_TAP_403 0x64C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h32Im19 : 10; //h32_im_19, reset value: 0x0, access type: RW
		uint32 h32Re20 : 10; //h32_re_20, reset value: 0x0, access type: RW
		uint32 h32Im20 : 10; //h32_im_20, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap403_u;

/*REG_RF_EMULATOR_TAP_404 0x650 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h32Re21 : 10; //h32_re_21, reset value: 0x0, access type: RW
		uint32 h32Im21 : 10; //h32_im_21, reset value: 0x0, access type: RW
		uint32 h32Re22 : 10; //h32_re_22, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap404_u;

/*REG_RF_EMULATOR_TAP_405 0x654 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h32Im22 : 10; //h32_im_22, reset value: 0x0, access type: RW
		uint32 h32Re23 : 10; //h32_re_23, reset value: 0x0, access type: RW
		uint32 h32Im23 : 10; //h32_im_23, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap405_u;

/*REG_RF_EMULATOR_TAP_406 0x658 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h32Re24 : 10; //h32_re_24, reset value: 0x0, access type: RW
		uint32 h32Im24 : 10; //h32_im_24, reset value: 0x0, access type: RW
		uint32 h32Re25 : 10; //h32_re_25, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap406_u;

/*REG_RF_EMULATOR_TAP_407 0x65C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h32Im25 : 10; //h32_im_25, reset value: 0x0, access type: RW
		uint32 h32Re26 : 10; //h32_re_26, reset value: 0x0, access type: RW
		uint32 h32Im26 : 10; //h32_im_26, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap407_u;

/*REG_RF_EMULATOR_TAP_408 0x660 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h32Re27 : 10; //h32_re_27, reset value: 0x0, access type: RW
		uint32 h32Im27 : 10; //h32_im_27, reset value: 0x0, access type: RW
		uint32 h32Re28 : 10; //h32_re_28, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap408_u;

/*REG_RF_EMULATOR_TAP_409 0x664 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h32Im28 : 10; //h32_im_28, reset value: 0x0, access type: RW
		uint32 h32Re29 : 10; //h32_re_29, reset value: 0x0, access type: RW
		uint32 h32Im29 : 10; //h32_im_29, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap409_u;

/*REG_RF_EMULATOR_TAP_410 0x668 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h32Re30 : 10; //h32_re_30, reset value: 0x0, access type: RW
		uint32 h32Im30 : 10; //h32_im_30, reset value: 0x0, access type: RW
		uint32 h32Re31 : 10; //h32_re_31, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap410_u;

/*REG_RF_EMULATOR_TAP_411 0x66C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h32Im31 : 10; //h32_im_31, reset value: 0x0, access type: RW
		uint32 h32Re32 : 10; //h32_re_32, reset value: 0x0, access type: RW
		uint32 h32Im32 : 10; //h32_im_32, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap411_u;

/*REG_RF_EMULATOR_TAP_412 0x670 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h32Re33 : 10; //h32_re_33, reset value: 0x0, access type: RW
		uint32 h32Im33 : 10; //h32_im_33, reset value: 0x0, access type: RW
		uint32 h32Re34 : 10; //h32_re_34, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap412_u;

/*REG_RF_EMULATOR_TAP_413 0x674 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h32Im34 : 10; //h32_im_34, reset value: 0x0, access type: RW
		uint32 h32Re35 : 10; //h32_re_35, reset value: 0x0, access type: RW
		uint32 h32Im35 : 10; //h32_im_35, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap413_u;

/*REG_RF_EMULATOR_TAP_414 0x678 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h32Re36 : 10; //h32_re_36, reset value: 0x0, access type: RW
		uint32 h32Im36 : 10; //h32_im_36, reset value: 0x0, access type: RW
		uint32 h32Re37 : 10; //h32_re_37, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap414_u;

/*REG_RF_EMULATOR_TAP_415 0x67C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h32Im37 : 10; //h32_im_37, reset value: 0x0, access type: RW
		uint32 h32Re38 : 10; //h32_re_38, reset value: 0x0, access type: RW
		uint32 h32Im38 : 10; //h32_im_38, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap415_u;

/*REG_RF_EMULATOR_TAP_416 0x680 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h32Re39 : 10; //h32_re_39, reset value: 0x0, access type: RW
		uint32 h32Im39 : 10; //h32_im_39, reset value: 0x0, access type: RW
		uint32 h32Re40 : 10; //h32_re_40, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap416_u;

/*REG_RF_EMULATOR_TAP_417 0x684 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h32Im40 : 10; //h32_im_40, reset value: 0x0, access type: RW
		uint32 h32Re41 : 10; //h32_re_41, reset value: 0x0, access type: RW
		uint32 h32Im41 : 10; //h32_im_41, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap417_u;

/*REG_RF_EMULATOR_TAP_418 0x688 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h32Re42 : 10; //h32_re_42, reset value: 0x0, access type: RW
		uint32 h32Im42 : 10; //h32_im_42, reset value: 0x0, access type: RW
		uint32 h32Re43 : 10; //h32_re_43, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap418_u;

/*REG_RF_EMULATOR_TAP_419 0x68C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h32Im43 : 10; //h32_im_43, reset value: 0x0, access type: RW
		uint32 h32Re44 : 10; //h32_re_44, reset value: 0x0, access type: RW
		uint32 h32Im44 : 10; //h32_im_44, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap419_u;

/*REG_RF_EMULATOR_TAP_420 0x690 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h32Re45 : 10; //h32_re_45, reset value: 0x0, access type: RW
		uint32 h32Im45 : 10; //h32_im_45, reset value: 0x0, access type: RW
		uint32 h32Re46 : 10; //h32_re_46, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap420_u;

/*REG_RF_EMULATOR_TAP_421 0x694 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h32Im46 : 10; //h32_im_46, reset value: 0x0, access type: RW
		uint32 h32Re47 : 10; //h32_re_47, reset value: 0x0, access type: RW
		uint32 h32Im47 : 10; //h32_im_47, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap421_u;

/*REG_RF_EMULATOR_TAP_422 0x698 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h32Re48 : 10; //h32_re_48, reset value: 0x0, access type: RW
		uint32 h32Im48 : 10; //h32_im_48, reset value: 0x0, access type: RW
		uint32 h32Re49 : 10; //h32_re_49, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap422_u;

/*REG_RF_EMULATOR_TAP_423 0x69C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h32Im49 : 10; //h32_im_49, reset value: 0x0, access type: RW
		uint32 h32Re50 : 10; //h32_re_50, reset value: 0x0, access type: RW
		uint32 h32Im50 : 10; //h32_im_50, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap423_u;

/*REG_RF_EMULATOR_TAP_424 0x6A0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h32Re51 : 10; //h32_re_51, reset value: 0x0, access type: RW
		uint32 h32Im51 : 10; //h32_im_51, reset value: 0x0, access type: RW
		uint32 h32Re52 : 10; //h32_re_52, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap424_u;

/*REG_RF_EMULATOR_TAP_425 0x6A4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h32Im52 : 10; //h32_im_52, reset value: 0x0, access type: RW
		uint32 h32Re53 : 10; //h32_re_53, reset value: 0x0, access type: RW
		uint32 h32Im53 : 10; //h32_im_53, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap425_u;

/*REG_RF_EMULATOR_TAP_426 0x6A8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h32Re54 : 10; //h32_re_54, reset value: 0x0, access type: RW
		uint32 h32Im54 : 10; //h32_im_54, reset value: 0x0, access type: RW
		uint32 h32Re55 : 10; //h32_re_55, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap426_u;

/*REG_RF_EMULATOR_TAP_427 0x6AC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h32Im55 : 10; //h32_im_55, reset value: 0x0, access type: RW
		uint32 h32Re56 : 10; //h32_re_56, reset value: 0x0, access type: RW
		uint32 h32Im56 : 10; //h32_im_56, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap427_u;

/*REG_RF_EMULATOR_TAP_428 0x6B0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h32Re57 : 10; //h32_re_57, reset value: 0x0, access type: RW
		uint32 h32Im57 : 10; //h32_im_57, reset value: 0x0, access type: RW
		uint32 h32Re58 : 10; //h32_re_58, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap428_u;

/*REG_RF_EMULATOR_TAP_429 0x6B4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h32Im58 : 10; //h32_im_58, reset value: 0x0, access type: RW
		uint32 h32Re59 : 10; //h32_re_59, reset value: 0x0, access type: RW
		uint32 h32Im59 : 10; //h32_im_59, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap429_u;

/*REG_RF_EMULATOR_TAP_430 0x6B8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h32Re60 : 10; //h32_re_60, reset value: 0x0, access type: RW
		uint32 h32Im60 : 10; //h32_im_60, reset value: 0x0, access type: RW
		uint32 h32Re61 : 10; //h32_re_61, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap430_u;

/*REG_RF_EMULATOR_TAP_431 0x6BC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h32Im61 : 10; //h32_im_61, reset value: 0x0, access type: RW
		uint32 h32Re62 : 10; //h32_re_62, reset value: 0x0, access type: RW
		uint32 h32Im62 : 10; //h32_im_62, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap431_u;

/*REG_RF_EMULATOR_TAP_432 0x6C0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h32Re63 : 10; //h32_re_63, reset value: 0x0, access type: RW
		uint32 h32Im63 : 10; //h32_im_63, reset value: 0x0, access type: RW
		uint32 h32Re64 : 10; //h32_re_64, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap432_u;

/*REG_RF_EMULATOR_TAP_433 0x6C4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h32Im64 : 10; //h32_im_64, reset value: 0x0, access type: RW
		uint32 h33Re0 : 10; //h33_re_0, reset value: 0x1ff, access type: RW
		uint32 h33Im0 : 10; //h33_im_0, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap433_u;

/*REG_RF_EMULATOR_TAP_434 0x6C8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h33Re1 : 10; //h33_re_1, reset value: 0x0, access type: RW
		uint32 h33Im1 : 10; //h33_im_1, reset value: 0x0, access type: RW
		uint32 h33Re2 : 10; //h33_re_2, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap434_u;

/*REG_RF_EMULATOR_TAP_435 0x6CC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h33Im2 : 10; //h33_im_2, reset value: 0x0, access type: RW
		uint32 h33Re3 : 10; //h33_re_3, reset value: 0x0, access type: RW
		uint32 h33Im3 : 10; //h33_im_3, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap435_u;

/*REG_RF_EMULATOR_TAP_436 0x6D0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h33Re4 : 10; //h33_re_4, reset value: 0x0, access type: RW
		uint32 h33Im4 : 10; //h33_im_4, reset value: 0x0, access type: RW
		uint32 h33Re5 : 10; //h33_re_5, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap436_u;

/*REG_RF_EMULATOR_TAP_437 0x6D4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h33Im5 : 10; //h33_im_5, reset value: 0x0, access type: RW
		uint32 h33Re6 : 10; //h33_re_6, reset value: 0x0, access type: RW
		uint32 h33Im6 : 10; //h33_im_6, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap437_u;

/*REG_RF_EMULATOR_TAP_438 0x6D8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h33Re7 : 10; //h33_re_7, reset value: 0x0, access type: RW
		uint32 h33Im7 : 10; //h33_im_7, reset value: 0x0, access type: RW
		uint32 h33Re8 : 10; //h33_re_8, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap438_u;

/*REG_RF_EMULATOR_TAP_439 0x6DC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h33Im8 : 10; //h33_im_8, reset value: 0x0, access type: RW
		uint32 h33Re9 : 10; //h33_re_9, reset value: 0x0, access type: RW
		uint32 h33Im9 : 10; //h33_im_9, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap439_u;

/*REG_RF_EMULATOR_TAP_440 0x6E0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h33Re10 : 10; //h33_re_10, reset value: 0x0, access type: RW
		uint32 h33Im10 : 10; //h33_im_10, reset value: 0x0, access type: RW
		uint32 h33Re11 : 10; //h33_re_11, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap440_u;

/*REG_RF_EMULATOR_TAP_441 0x6E4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h33Im11 : 10; //h33_im_11, reset value: 0x0, access type: RW
		uint32 h33Re12 : 10; //h33_re_12, reset value: 0x0, access type: RW
		uint32 h33Im12 : 10; //h33_im_12, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap441_u;

/*REG_RF_EMULATOR_TAP_442 0x6E8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h33Re13 : 10; //h33_re_13, reset value: 0x0, access type: RW
		uint32 h33Im13 : 10; //h33_im_13, reset value: 0x0, access type: RW
		uint32 h33Re14 : 10; //h33_re_14, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap442_u;

/*REG_RF_EMULATOR_TAP_443 0x6EC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h33Im14 : 10; //h33_im_14, reset value: 0x0, access type: RW
		uint32 h33Re15 : 10; //h33_re_15, reset value: 0x0, access type: RW
		uint32 h33Im15 : 10; //h33_im_15, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap443_u;

/*REG_RF_EMULATOR_TAP_444 0x6F0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h33Re16 : 10; //h33_re_16, reset value: 0x0, access type: RW
		uint32 h33Im16 : 10; //h33_im_16, reset value: 0x0, access type: RW
		uint32 h33Re17 : 10; //h33_re_17, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap444_u;

/*REG_RF_EMULATOR_TAP_445 0x6F4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h33Im17 : 10; //h33_im_17, reset value: 0x0, access type: RW
		uint32 h33Re18 : 10; //h33_re_18, reset value: 0x0, access type: RW
		uint32 h33Im18 : 10; //h33_im_18, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap445_u;

/*REG_RF_EMULATOR_TAP_446 0x6F8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h33Re19 : 10; //h33_re_19, reset value: 0x0, access type: RW
		uint32 h33Im19 : 10; //h33_im_19, reset value: 0x0, access type: RW
		uint32 h33Re20 : 10; //h33_re_20, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap446_u;

/*REG_RF_EMULATOR_TAP_447 0x6FC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h33Im20 : 10; //h33_im_20, reset value: 0x0, access type: RW
		uint32 h33Re21 : 10; //h33_re_21, reset value: 0x0, access type: RW
		uint32 h33Im21 : 10; //h33_im_21, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap447_u;

/*REG_RF_EMULATOR_TAP_448 0x700 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h33Re22 : 10; //h33_re_22, reset value: 0x0, access type: RW
		uint32 h33Im22 : 10; //h33_im_22, reset value: 0x0, access type: RW
		uint32 h33Re23 : 10; //h33_re_23, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap448_u;

/*REG_RF_EMULATOR_TAP_449 0x704 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h33Im23 : 10; //h33_im_23, reset value: 0x0, access type: RW
		uint32 h33Re24 : 10; //h33_re_24, reset value: 0x0, access type: RW
		uint32 h33Im24 : 10; //h33_im_24, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap449_u;

/*REG_RF_EMULATOR_TAP_450 0x708 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h33Re25 : 10; //h33_re_25, reset value: 0x0, access type: RW
		uint32 h33Im25 : 10; //h33_im_25, reset value: 0x0, access type: RW
		uint32 h33Re26 : 10; //h33_re_26, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap450_u;

/*REG_RF_EMULATOR_TAP_451 0x70C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h33Im26 : 10; //h33_im_26, reset value: 0x0, access type: RW
		uint32 h33Re27 : 10; //h33_re_27, reset value: 0x0, access type: RW
		uint32 h33Im27 : 10; //h33_im_27, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap451_u;

/*REG_RF_EMULATOR_TAP_452 0x710 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h33Re28 : 10; //h33_re_28, reset value: 0x0, access type: RW
		uint32 h33Im28 : 10; //h33_im_28, reset value: 0x0, access type: RW
		uint32 h33Re29 : 10; //h33_re_29, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap452_u;

/*REG_RF_EMULATOR_TAP_453 0x714 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h33Im29 : 10; //h33_im_29, reset value: 0x0, access type: RW
		uint32 h33Re30 : 10; //h33_re_30, reset value: 0x0, access type: RW
		uint32 h33Im30 : 10; //h33_im_30, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap453_u;

/*REG_RF_EMULATOR_TAP_454 0x718 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h33Re31 : 10; //h33_re_31, reset value: 0x0, access type: RW
		uint32 h33Im31 : 10; //h33_im_31, reset value: 0x0, access type: RW
		uint32 h33Re32 : 10; //h33_re_32, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap454_u;

/*REG_RF_EMULATOR_TAP_455 0x71C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h33Im32 : 10; //h33_im_32, reset value: 0x0, access type: RW
		uint32 h33Re33 : 10; //h33_re_33, reset value: 0x0, access type: RW
		uint32 h33Im33 : 10; //h33_im_33, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap455_u;

/*REG_RF_EMULATOR_TAP_456 0x720 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h33Re34 : 10; //h33_re_34, reset value: 0x0, access type: RW
		uint32 h33Im34 : 10; //h33_im_34, reset value: 0x0, access type: RW
		uint32 h33Re35 : 10; //h33_re_35, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap456_u;

/*REG_RF_EMULATOR_TAP_457 0x724 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h33Im35 : 10; //h33_im_35, reset value: 0x0, access type: RW
		uint32 h33Re36 : 10; //h33_re_36, reset value: 0x0, access type: RW
		uint32 h33Im36 : 10; //h33_im_36, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap457_u;

/*REG_RF_EMULATOR_TAP_458 0x728 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h33Re37 : 10; //h33_re_37, reset value: 0x0, access type: RW
		uint32 h33Im37 : 10; //h33_im_37, reset value: 0x0, access type: RW
		uint32 h33Re38 : 10; //h33_re_38, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap458_u;

/*REG_RF_EMULATOR_TAP_459 0x72C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h33Im38 : 10; //h33_im_38, reset value: 0x0, access type: RW
		uint32 h33Re39 : 10; //h33_re_39, reset value: 0x0, access type: RW
		uint32 h33Im39 : 10; //h33_im_39, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap459_u;

/*REG_RF_EMULATOR_TAP_460 0x730 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h33Re40 : 10; //h33_re_40, reset value: 0x0, access type: RW
		uint32 h33Im40 : 10; //h33_im_40, reset value: 0x0, access type: RW
		uint32 h33Re41 : 10; //h33_re_41, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap460_u;

/*REG_RF_EMULATOR_TAP_461 0x734 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h33Im41 : 10; //h33_im_41, reset value: 0x0, access type: RW
		uint32 h33Re42 : 10; //h33_re_42, reset value: 0x0, access type: RW
		uint32 h33Im42 : 10; //h33_im_42, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap461_u;

/*REG_RF_EMULATOR_TAP_462 0x738 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h33Re43 : 10; //h33_re_43, reset value: 0x0, access type: RW
		uint32 h33Im43 : 10; //h33_im_43, reset value: 0x0, access type: RW
		uint32 h33Re44 : 10; //h33_re_44, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap462_u;

/*REG_RF_EMULATOR_TAP_463 0x73C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h33Im44 : 10; //h33_im_44, reset value: 0x0, access type: RW
		uint32 h33Re45 : 10; //h33_re_45, reset value: 0x0, access type: RW
		uint32 h33Im45 : 10; //h33_im_45, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap463_u;

/*REG_RF_EMULATOR_TAP_464 0x740 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h33Re46 : 10; //h33_re_46, reset value: 0x0, access type: RW
		uint32 h33Im46 : 10; //h33_im_46, reset value: 0x0, access type: RW
		uint32 h33Re47 : 10; //h33_re_47, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap464_u;

/*REG_RF_EMULATOR_TAP_465 0x744 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h33Im47 : 10; //h33_im_47, reset value: 0x0, access type: RW
		uint32 h33Re48 : 10; //h33_re_48, reset value: 0x0, access type: RW
		uint32 h33Im48 : 10; //h33_im_48, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap465_u;

/*REG_RF_EMULATOR_TAP_466 0x748 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h33Re49 : 10; //h33_re_49, reset value: 0x0, access type: RW
		uint32 h33Im49 : 10; //h33_im_49, reset value: 0x0, access type: RW
		uint32 h33Re50 : 10; //h33_re_50, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap466_u;

/*REG_RF_EMULATOR_TAP_467 0x74C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h33Im50 : 10; //h33_im_50, reset value: 0x0, access type: RW
		uint32 h33Re51 : 10; //h33_re_51, reset value: 0x0, access type: RW
		uint32 h33Im51 : 10; //h33_im_51, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap467_u;

/*REG_RF_EMULATOR_TAP_468 0x750 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h33Re52 : 10; //h33_re_52, reset value: 0x0, access type: RW
		uint32 h33Im52 : 10; //h33_im_52, reset value: 0x0, access type: RW
		uint32 h33Re53 : 10; //h33_re_53, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap468_u;

/*REG_RF_EMULATOR_TAP_469 0x754 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h33Im53 : 10; //h33_im_53, reset value: 0x0, access type: RW
		uint32 h33Re54 : 10; //h33_re_54, reset value: 0x0, access type: RW
		uint32 h33Im54 : 10; //h33_im_54, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap469_u;

/*REG_RF_EMULATOR_TAP_470 0x758 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h33Re55 : 10; //h33_re_55, reset value: 0x0, access type: RW
		uint32 h33Im55 : 10; //h33_im_55, reset value: 0x0, access type: RW
		uint32 h33Re56 : 10; //h33_re_56, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap470_u;

/*REG_RF_EMULATOR_TAP_471 0x75C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h33Im56 : 10; //h33_im_56, reset value: 0x0, access type: RW
		uint32 h33Re57 : 10; //h33_re_57, reset value: 0x0, access type: RW
		uint32 h33Im57 : 10; //h33_im_57, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap471_u;

/*REG_RF_EMULATOR_TAP_472 0x760 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h33Re58 : 10; //h33_re_58, reset value: 0x0, access type: RW
		uint32 h33Im58 : 10; //h33_im_58, reset value: 0x0, access type: RW
		uint32 h33Re59 : 10; //h33_re_59, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap472_u;

/*REG_RF_EMULATOR_TAP_473 0x764 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h33Im59 : 10; //h33_im_59, reset value: 0x0, access type: RW
		uint32 h33Re60 : 10; //h33_re_60, reset value: 0x0, access type: RW
		uint32 h33Im60 : 10; //h33_im_60, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap473_u;

/*REG_RF_EMULATOR_TAP_474 0x768 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h33Re61 : 10; //h33_re_61, reset value: 0x0, access type: RW
		uint32 h33Im61 : 10; //h33_im_61, reset value: 0x0, access type: RW
		uint32 h33Re62 : 10; //h33_re_62, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap474_u;

/*REG_RF_EMULATOR_TAP_475 0x76C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h33Im62 : 10; //h33_im_62, reset value: 0x0, access type: RW
		uint32 h33Re63 : 10; //h33_re_63, reset value: 0x0, access type: RW
		uint32 h33Im63 : 10; //h33_im_63, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap475_u;

/*REG_RF_EMULATOR_TAP_476 0x770 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h33Re64 : 10; //h33_re_64, reset value: 0x0, access type: RW
		uint32 h33Im64 : 10; //h33_im_64, reset value: 0x0, access type: RW
		uint32 h34Re0 : 10; //h34_re_0, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap476_u;

/*REG_RF_EMULATOR_TAP_477 0x774 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h34Im0 : 10; //h34_im_0, reset value: 0x0, access type: RW
		uint32 h34Re1 : 10; //h34_re_1, reset value: 0x0, access type: RW
		uint32 h34Im1 : 10; //h34_im_1, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap477_u;

/*REG_RF_EMULATOR_TAP_478 0x778 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h34Re2 : 10; //h34_re_2, reset value: 0x0, access type: RW
		uint32 h34Im2 : 10; //h34_im_2, reset value: 0x0, access type: RW
		uint32 h34Re3 : 10; //h34_re_3, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap478_u;

/*REG_RF_EMULATOR_TAP_479 0x77C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h34Im3 : 10; //h34_im_3, reset value: 0x0, access type: RW
		uint32 h34Re4 : 10; //h34_re_4, reset value: 0x0, access type: RW
		uint32 h34Im4 : 10; //h34_im_4, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap479_u;

/*REG_RF_EMULATOR_TAP_480 0x780 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h34Re5 : 10; //h34_re_5, reset value: 0x0, access type: RW
		uint32 h34Im5 : 10; //h34_im_5, reset value: 0x0, access type: RW
		uint32 h34Re6 : 10; //h34_re_6, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap480_u;

/*REG_RF_EMULATOR_TAP_481 0x784 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h34Im6 : 10; //h34_im_6, reset value: 0x0, access type: RW
		uint32 h34Re7 : 10; //h34_re_7, reset value: 0x0, access type: RW
		uint32 h34Im7 : 10; //h34_im_7, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap481_u;

/*REG_RF_EMULATOR_TAP_482 0x788 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h34Re8 : 10; //h34_re_8, reset value: 0x0, access type: RW
		uint32 h34Im8 : 10; //h34_im_8, reset value: 0x0, access type: RW
		uint32 h34Re9 : 10; //h34_re_9, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap482_u;

/*REG_RF_EMULATOR_TAP_483 0x78C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h34Im9 : 10; //h34_im_9, reset value: 0x0, access type: RW
		uint32 h34Re10 : 10; //h34_re_10, reset value: 0x0, access type: RW
		uint32 h34Im10 : 10; //h34_im_10, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap483_u;

/*REG_RF_EMULATOR_TAP_484 0x790 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h34Re11 : 10; //h34_re_11, reset value: 0x0, access type: RW
		uint32 h34Im11 : 10; //h34_im_11, reset value: 0x0, access type: RW
		uint32 h34Re12 : 10; //h34_re_12, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap484_u;

/*REG_RF_EMULATOR_TAP_485 0x794 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h34Im12 : 10; //h34_im_12, reset value: 0x0, access type: RW
		uint32 h34Re13 : 10; //h34_re_13, reset value: 0x0, access type: RW
		uint32 h34Im13 : 10; //h34_im_13, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap485_u;

/*REG_RF_EMULATOR_TAP_486 0x798 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h34Re14 : 10; //h34_re_14, reset value: 0x0, access type: RW
		uint32 h34Im14 : 10; //h34_im_14, reset value: 0x0, access type: RW
		uint32 h34Re15 : 10; //h34_re_15, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap486_u;

/*REG_RF_EMULATOR_TAP_487 0x79C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h34Im15 : 10; //h34_im_15, reset value: 0x0, access type: RW
		uint32 h34Re16 : 10; //h34_re_16, reset value: 0x0, access type: RW
		uint32 h34Im16 : 10; //h34_im_16, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap487_u;

/*REG_RF_EMULATOR_TAP_488 0x7A0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h34Re17 : 10; //h34_re_17, reset value: 0x0, access type: RW
		uint32 h34Im17 : 10; //h34_im_17, reset value: 0x0, access type: RW
		uint32 h34Re18 : 10; //h34_re_18, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap488_u;

/*REG_RF_EMULATOR_TAP_489 0x7A4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h34Im18 : 10; //h34_im_18, reset value: 0x0, access type: RW
		uint32 h34Re19 : 10; //h34_re_19, reset value: 0x0, access type: RW
		uint32 h34Im19 : 10; //h34_im_19, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap489_u;

/*REG_RF_EMULATOR_TAP_490 0x7A8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h34Re20 : 10; //h34_re_20, reset value: 0x0, access type: RW
		uint32 h34Im20 : 10; //h34_im_20, reset value: 0x0, access type: RW
		uint32 h34Re21 : 10; //h34_re_21, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap490_u;

/*REG_RF_EMULATOR_TAP_491 0x7AC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h34Im21 : 10; //h34_im_21, reset value: 0x0, access type: RW
		uint32 h34Re22 : 10; //h34_re_22, reset value: 0x0, access type: RW
		uint32 h34Im22 : 10; //h34_im_22, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap491_u;

/*REG_RF_EMULATOR_TAP_492 0x7B0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h34Re23 : 10; //h34_re_23, reset value: 0x0, access type: RW
		uint32 h34Im23 : 10; //h34_im_23, reset value: 0x0, access type: RW
		uint32 h34Re24 : 10; //h34_re_24, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap492_u;

/*REG_RF_EMULATOR_TAP_493 0x7B4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h34Im24 : 10; //h34_im_24, reset value: 0x0, access type: RW
		uint32 h34Re25 : 10; //h34_re_25, reset value: 0x0, access type: RW
		uint32 h34Im25 : 10; //h34_im_25, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap493_u;

/*REG_RF_EMULATOR_TAP_494 0x7B8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h34Re26 : 10; //h34_re_26, reset value: 0x0, access type: RW
		uint32 h34Im26 : 10; //h34_im_26, reset value: 0x0, access type: RW
		uint32 h34Re27 : 10; //h34_re_27, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap494_u;

/*REG_RF_EMULATOR_TAP_495 0x7BC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h34Im27 : 10; //h34_im_27, reset value: 0x0, access type: RW
		uint32 h34Re28 : 10; //h34_re_28, reset value: 0x0, access type: RW
		uint32 h34Im28 : 10; //h34_im_28, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap495_u;

/*REG_RF_EMULATOR_TAP_496 0x7C0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h34Re29 : 10; //h34_re_29, reset value: 0x0, access type: RW
		uint32 h34Im29 : 10; //h34_im_29, reset value: 0x0, access type: RW
		uint32 h34Re30 : 10; //h34_re_30, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap496_u;

/*REG_RF_EMULATOR_TAP_497 0x7C4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h34Im30 : 10; //h34_im_30, reset value: 0x0, access type: RW
		uint32 h34Re31 : 10; //h34_re_31, reset value: 0x0, access type: RW
		uint32 h34Im31 : 10; //h34_im_31, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap497_u;

/*REG_RF_EMULATOR_TAP_498 0x7C8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h34Re32 : 10; //h34_re_32, reset value: 0x0, access type: RW
		uint32 h34Im32 : 10; //h34_im_32, reset value: 0x0, access type: RW
		uint32 h34Re33 : 10; //h34_re_33, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap498_u;

/*REG_RF_EMULATOR_TAP_499 0x7CC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h34Im33 : 10; //h34_im_33, reset value: 0x0, access type: RW
		uint32 h34Re34 : 10; //h34_re_34, reset value: 0x0, access type: RW
		uint32 h34Im34 : 10; //h34_im_34, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap499_u;

/*REG_RF_EMULATOR_TAP_500 0x7D0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h34Re35 : 10; //h34_re_35, reset value: 0x0, access type: RW
		uint32 h34Im35 : 10; //h34_im_35, reset value: 0x0, access type: RW
		uint32 h34Re36 : 10; //h34_re_36, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap500_u;

/*REG_RF_EMULATOR_TAP_501 0x7D4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h34Im36 : 10; //h34_im_36, reset value: 0x0, access type: RW
		uint32 h34Re37 : 10; //h34_re_37, reset value: 0x0, access type: RW
		uint32 h34Im37 : 10; //h34_im_37, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap501_u;

/*REG_RF_EMULATOR_TAP_502 0x7D8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h34Re38 : 10; //h34_re_38, reset value: 0x0, access type: RW
		uint32 h34Im38 : 10; //h34_im_38, reset value: 0x0, access type: RW
		uint32 h34Re39 : 10; //h34_re_39, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap502_u;

/*REG_RF_EMULATOR_TAP_503 0x7DC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h34Im39 : 10; //h34_im_39, reset value: 0x0, access type: RW
		uint32 h34Re40 : 10; //h34_re_40, reset value: 0x0, access type: RW
		uint32 h34Im40 : 10; //h34_im_40, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap503_u;

/*REG_RF_EMULATOR_TAP_504 0x7E0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h34Re41 : 10; //h34_re_41, reset value: 0x0, access type: RW
		uint32 h34Im41 : 10; //h34_im_41, reset value: 0x0, access type: RW
		uint32 h34Re42 : 10; //h34_re_42, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap504_u;

/*REG_RF_EMULATOR_TAP_505 0x7E4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h34Im42 : 10; //h34_im_42, reset value: 0x0, access type: RW
		uint32 h34Re43 : 10; //h34_re_43, reset value: 0x0, access type: RW
		uint32 h34Im43 : 10; //h34_im_43, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap505_u;

/*REG_RF_EMULATOR_TAP_506 0x7E8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h34Re44 : 10; //h34_re_44, reset value: 0x0, access type: RW
		uint32 h34Im44 : 10; //h34_im_44, reset value: 0x0, access type: RW
		uint32 h34Re45 : 10; //h34_re_45, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap506_u;

/*REG_RF_EMULATOR_TAP_507 0x7EC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h34Im45 : 10; //h34_im_45, reset value: 0x0, access type: RW
		uint32 h34Re46 : 10; //h34_re_46, reset value: 0x0, access type: RW
		uint32 h34Im46 : 10; //h34_im_46, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap507_u;

/*REG_RF_EMULATOR_TAP_508 0x7F0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h34Re47 : 10; //h34_re_47, reset value: 0x0, access type: RW
		uint32 h34Im47 : 10; //h34_im_47, reset value: 0x0, access type: RW
		uint32 h34Re48 : 10; //h34_re_48, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap508_u;

/*REG_RF_EMULATOR_TAP_509 0x7F4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h34Im48 : 10; //h34_im_48, reset value: 0x0, access type: RW
		uint32 h34Re49 : 10; //h34_re_49, reset value: 0x0, access type: RW
		uint32 h34Im49 : 10; //h34_im_49, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap509_u;

/*REG_RF_EMULATOR_TAP_510 0x7F8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h34Re50 : 10; //h34_re_50, reset value: 0x0, access type: RW
		uint32 h34Im50 : 10; //h34_im_50, reset value: 0x0, access type: RW
		uint32 h34Re51 : 10; //h34_re_51, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap510_u;

/*REG_RF_EMULATOR_TAP_511 0x7FC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h34Im51 : 10; //h34_im_51, reset value: 0x0, access type: RW
		uint32 h34Re52 : 10; //h34_re_52, reset value: 0x0, access type: RW
		uint32 h34Im52 : 10; //h34_im_52, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap511_u;

/*REG_RF_EMULATOR_TAP_512 0x800 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h34Re53 : 10; //h34_re_53, reset value: 0x0, access type: RW
		uint32 h34Im53 : 10; //h34_im_53, reset value: 0x0, access type: RW
		uint32 h34Re54 : 10; //h34_re_54, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap512_u;

/*REG_RF_EMULATOR_TAP_513 0x804 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h34Im54 : 10; //h34_im_54, reset value: 0x0, access type: RW
		uint32 h34Re55 : 10; //h34_re_55, reset value: 0x0, access type: RW
		uint32 h34Im55 : 10; //h34_im_55, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap513_u;

/*REG_RF_EMULATOR_TAP_514 0x808 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h34Re56 : 10; //h34_re_56, reset value: 0x0, access type: RW
		uint32 h34Im56 : 10; //h34_im_56, reset value: 0x0, access type: RW
		uint32 h34Re57 : 10; //h34_re_57, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap514_u;

/*REG_RF_EMULATOR_TAP_515 0x80C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h34Im57 : 10; //h34_im_57, reset value: 0x0, access type: RW
		uint32 h34Re58 : 10; //h34_re_58, reset value: 0x0, access type: RW
		uint32 h34Im58 : 10; //h34_im_58, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap515_u;

/*REG_RF_EMULATOR_TAP_516 0x810 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h34Re59 : 10; //h34_re_59, reset value: 0x0, access type: RW
		uint32 h34Im59 : 10; //h34_im_59, reset value: 0x0, access type: RW
		uint32 h34Re60 : 10; //h34_re_60, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap516_u;

/*REG_RF_EMULATOR_TAP_517 0x814 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h34Im60 : 10; //h34_im_60, reset value: 0x0, access type: RW
		uint32 h34Re61 : 10; //h34_re_61, reset value: 0x0, access type: RW
		uint32 h34Im61 : 10; //h34_im_61, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap517_u;

/*REG_RF_EMULATOR_TAP_518 0x818 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h34Re62 : 10; //h34_re_62, reset value: 0x0, access type: RW
		uint32 h34Im62 : 10; //h34_im_62, reset value: 0x0, access type: RW
		uint32 h34Re63 : 10; //h34_re_63, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap518_u;

/*REG_RF_EMULATOR_TAP_519 0x81C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h34Im63 : 10; //h34_im_63, reset value: 0x0, access type: RW
		uint32 h34Re64 : 10; //h34_re_64, reset value: 0x0, access type: RW
		uint32 h34Im64 : 10; //h34_im_64, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap519_u;

/*REG_RF_EMULATOR_TAP_520 0x820 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h41Re0 : 10; //h41_re_0, reset value: 0x0, access type: RW
		uint32 h41Im0 : 10; //h41_im_0, reset value: 0x0, access type: RW
		uint32 h41Re1 : 10; //h41_re_1, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap520_u;

/*REG_RF_EMULATOR_TAP_521 0x824 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h41Im1 : 10; //h41_im_1, reset value: 0x0, access type: RW
		uint32 h41Re2 : 10; //h41_re_2, reset value: 0x0, access type: RW
		uint32 h41Im2 : 10; //h41_im_2, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap521_u;

/*REG_RF_EMULATOR_TAP_522 0x828 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h41Re3 : 10; //h41_re_3, reset value: 0x0, access type: RW
		uint32 h41Im3 : 10; //h41_im_3, reset value: 0x0, access type: RW
		uint32 h41Re4 : 10; //h41_re_4, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap522_u;

/*REG_RF_EMULATOR_TAP_523 0x82C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h41Im4 : 10; //h41_im_4, reset value: 0x0, access type: RW
		uint32 h41Re5 : 10; //h41_re_5, reset value: 0x0, access type: RW
		uint32 h41Im5 : 10; //h41_im_5, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap523_u;

/*REG_RF_EMULATOR_TAP_524 0x830 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h41Re6 : 10; //h41_re_6, reset value: 0x0, access type: RW
		uint32 h41Im6 : 10; //h41_im_6, reset value: 0x0, access type: RW
		uint32 h41Re7 : 10; //h41_re_7, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap524_u;

/*REG_RF_EMULATOR_TAP_525 0x834 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h41Im7 : 10; //h41_im_7, reset value: 0x0, access type: RW
		uint32 h41Re8 : 10; //h41_re_8, reset value: 0x0, access type: RW
		uint32 h41Im8 : 10; //h41_im_8, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap525_u;

/*REG_RF_EMULATOR_TAP_526 0x838 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h41Re9 : 10; //h41_re_9, reset value: 0x0, access type: RW
		uint32 h41Im9 : 10; //h41_im_9, reset value: 0x0, access type: RW
		uint32 h41Re10 : 10; //h41_re_10, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap526_u;

/*REG_RF_EMULATOR_TAP_527 0x83C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h41Im10 : 10; //h41_im_10, reset value: 0x0, access type: RW
		uint32 h41Re11 : 10; //h41_re_11, reset value: 0x0, access type: RW
		uint32 h41Im11 : 10; //h41_im_11, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap527_u;

/*REG_RF_EMULATOR_TAP_528 0x840 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h41Re12 : 10; //h41_re_12, reset value: 0x0, access type: RW
		uint32 h41Im12 : 10; //h41_im_12, reset value: 0x0, access type: RW
		uint32 h41Re13 : 10; //h41_re_13, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap528_u;

/*REG_RF_EMULATOR_TAP_529 0x844 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h41Im13 : 10; //h41_im_13, reset value: 0x0, access type: RW
		uint32 h41Re14 : 10; //h41_re_14, reset value: 0x0, access type: RW
		uint32 h41Im14 : 10; //h41_im_14, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap529_u;

/*REG_RF_EMULATOR_TAP_530 0x848 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h41Re15 : 10; //h41_re_15, reset value: 0x0, access type: RW
		uint32 h41Im15 : 10; //h41_im_15, reset value: 0x0, access type: RW
		uint32 h41Re16 : 10; //h41_re_16, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap530_u;

/*REG_RF_EMULATOR_TAP_531 0x84C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h41Im16 : 10; //h41_im_16, reset value: 0x0, access type: RW
		uint32 h41Re17 : 10; //h41_re_17, reset value: 0x0, access type: RW
		uint32 h41Im17 : 10; //h41_im_17, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap531_u;

/*REG_RF_EMULATOR_TAP_532 0x850 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h41Re18 : 10; //h41_re_18, reset value: 0x0, access type: RW
		uint32 h41Im18 : 10; //h41_im_18, reset value: 0x0, access type: RW
		uint32 h41Re19 : 10; //h41_re_19, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap532_u;

/*REG_RF_EMULATOR_TAP_533 0x854 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h41Im19 : 10; //h41_im_19, reset value: 0x0, access type: RW
		uint32 h41Re20 : 10; //h41_re_20, reset value: 0x0, access type: RW
		uint32 h41Im20 : 10; //h41_im_20, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap533_u;

/*REG_RF_EMULATOR_TAP_534 0x858 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h41Re21 : 10; //h41_re_21, reset value: 0x0, access type: RW
		uint32 h41Im21 : 10; //h41_im_21, reset value: 0x0, access type: RW
		uint32 h41Re22 : 10; //h41_re_22, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap534_u;

/*REG_RF_EMULATOR_TAP_535 0x85C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h41Im22 : 10; //h41_im_22, reset value: 0x0, access type: RW
		uint32 h41Re23 : 10; //h41_re_23, reset value: 0x0, access type: RW
		uint32 h41Im23 : 10; //h41_im_23, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap535_u;

/*REG_RF_EMULATOR_TAP_536 0x860 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h41Re24 : 10; //h41_re_24, reset value: 0x0, access type: RW
		uint32 h41Im24 : 10; //h41_im_24, reset value: 0x0, access type: RW
		uint32 h41Re25 : 10; //h41_re_25, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap536_u;

/*REG_RF_EMULATOR_TAP_537 0x864 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h41Im25 : 10; //h41_im_25, reset value: 0x0, access type: RW
		uint32 h41Re26 : 10; //h41_re_26, reset value: 0x0, access type: RW
		uint32 h41Im26 : 10; //h41_im_26, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap537_u;

/*REG_RF_EMULATOR_TAP_538 0x868 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h41Re27 : 10; //h41_re_27, reset value: 0x0, access type: RW
		uint32 h41Im27 : 10; //h41_im_27, reset value: 0x0, access type: RW
		uint32 h41Re28 : 10; //h41_re_28, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap538_u;

/*REG_RF_EMULATOR_TAP_539 0x86C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h41Im28 : 10; //h41_im_28, reset value: 0x0, access type: RW
		uint32 h41Re29 : 10; //h41_re_29, reset value: 0x0, access type: RW
		uint32 h41Im29 : 10; //h41_im_29, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap539_u;

/*REG_RF_EMULATOR_TAP_540 0x870 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h41Re30 : 10; //h41_re_30, reset value: 0x0, access type: RW
		uint32 h41Im30 : 10; //h41_im_30, reset value: 0x0, access type: RW
		uint32 h41Re31 : 10; //h41_re_31, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap540_u;

/*REG_RF_EMULATOR_TAP_541 0x874 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h41Im31 : 10; //h41_im_31, reset value: 0x0, access type: RW
		uint32 h41Re32 : 10; //h41_re_32, reset value: 0x0, access type: RW
		uint32 h41Im32 : 10; //h41_im_32, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap541_u;

/*REG_RF_EMULATOR_TAP_542 0x878 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h41Re33 : 10; //h41_re_33, reset value: 0x0, access type: RW
		uint32 h41Im33 : 10; //h41_im_33, reset value: 0x0, access type: RW
		uint32 h41Re34 : 10; //h41_re_34, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap542_u;

/*REG_RF_EMULATOR_TAP_543 0x87C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h41Im34 : 10; //h41_im_34, reset value: 0x0, access type: RW
		uint32 h41Re35 : 10; //h41_re_35, reset value: 0x0, access type: RW
		uint32 h41Im35 : 10; //h41_im_35, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap543_u;

/*REG_RF_EMULATOR_TAP_544 0x880 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h41Re36 : 10; //h41_re_36, reset value: 0x0, access type: RW
		uint32 h41Im36 : 10; //h41_im_36, reset value: 0x0, access type: RW
		uint32 h41Re37 : 10; //h41_re_37, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap544_u;

/*REG_RF_EMULATOR_TAP_545 0x884 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h41Im37 : 10; //h41_im_37, reset value: 0x0, access type: RW
		uint32 h41Re38 : 10; //h41_re_38, reset value: 0x0, access type: RW
		uint32 h41Im38 : 10; //h41_im_38, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap545_u;

/*REG_RF_EMULATOR_TAP_546 0x888 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h41Re39 : 10; //h41_re_39, reset value: 0x0, access type: RW
		uint32 h41Im39 : 10; //h41_im_39, reset value: 0x0, access type: RW
		uint32 h41Re40 : 10; //h41_re_40, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap546_u;

/*REG_RF_EMULATOR_TAP_547 0x88C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h41Im40 : 10; //h41_im_40, reset value: 0x0, access type: RW
		uint32 h41Re41 : 10; //h41_re_41, reset value: 0x0, access type: RW
		uint32 h41Im41 : 10; //h41_im_41, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap547_u;

/*REG_RF_EMULATOR_TAP_548 0x890 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h41Re42 : 10; //h41_re_42, reset value: 0x0, access type: RW
		uint32 h41Im42 : 10; //h41_im_42, reset value: 0x0, access type: RW
		uint32 h41Re43 : 10; //h41_re_43, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap548_u;

/*REG_RF_EMULATOR_TAP_549 0x894 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h41Im43 : 10; //h41_im_43, reset value: 0x0, access type: RW
		uint32 h41Re44 : 10; //h41_re_44, reset value: 0x0, access type: RW
		uint32 h41Im44 : 10; //h41_im_44, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap549_u;

/*REG_RF_EMULATOR_TAP_550 0x898 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h41Re45 : 10; //h41_re_45, reset value: 0x0, access type: RW
		uint32 h41Im45 : 10; //h41_im_45, reset value: 0x0, access type: RW
		uint32 h41Re46 : 10; //h41_re_46, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap550_u;

/*REG_RF_EMULATOR_TAP_551 0x89C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h41Im46 : 10; //h41_im_46, reset value: 0x0, access type: RW
		uint32 h41Re47 : 10; //h41_re_47, reset value: 0x0, access type: RW
		uint32 h41Im47 : 10; //h41_im_47, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap551_u;

/*REG_RF_EMULATOR_TAP_552 0x8A0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h41Re48 : 10; //h41_re_48, reset value: 0x0, access type: RW
		uint32 h41Im48 : 10; //h41_im_48, reset value: 0x0, access type: RW
		uint32 h41Re49 : 10; //h41_re_49, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap552_u;

/*REG_RF_EMULATOR_TAP_553 0x8A4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h41Im49 : 10; //h41_im_49, reset value: 0x0, access type: RW
		uint32 h41Re50 : 10; //h41_re_50, reset value: 0x0, access type: RW
		uint32 h41Im50 : 10; //h41_im_50, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap553_u;

/*REG_RF_EMULATOR_TAP_554 0x8A8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h41Re51 : 10; //h41_re_51, reset value: 0x0, access type: RW
		uint32 h41Im51 : 10; //h41_im_51, reset value: 0x0, access type: RW
		uint32 h41Re52 : 10; //h41_re_52, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap554_u;

/*REG_RF_EMULATOR_TAP_555 0x8AC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h41Im52 : 10; //h41_im_52, reset value: 0x0, access type: RW
		uint32 h41Re53 : 10; //h41_re_53, reset value: 0x0, access type: RW
		uint32 h41Im53 : 10; //h41_im_53, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap555_u;

/*REG_RF_EMULATOR_TAP_556 0x8B0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h41Re54 : 10; //h41_re_54, reset value: 0x0, access type: RW
		uint32 h41Im54 : 10; //h41_im_54, reset value: 0x0, access type: RW
		uint32 h41Re55 : 10; //h41_re_55, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap556_u;

/*REG_RF_EMULATOR_TAP_557 0x8B4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h41Im55 : 10; //h41_im_55, reset value: 0x0, access type: RW
		uint32 h41Re56 : 10; //h41_re_56, reset value: 0x0, access type: RW
		uint32 h41Im56 : 10; //h41_im_56, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap557_u;

/*REG_RF_EMULATOR_TAP_558 0x8B8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h41Re57 : 10; //h41_re_57, reset value: 0x0, access type: RW
		uint32 h41Im57 : 10; //h41_im_57, reset value: 0x0, access type: RW
		uint32 h41Re58 : 10; //h41_re_58, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap558_u;

/*REG_RF_EMULATOR_TAP_559 0x8BC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h41Im58 : 10; //h41_im_58, reset value: 0x0, access type: RW
		uint32 h41Re59 : 10; //h41_re_59, reset value: 0x0, access type: RW
		uint32 h41Im59 : 10; //h41_im_59, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap559_u;

/*REG_RF_EMULATOR_TAP_560 0x8C0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h41Re60 : 10; //h41_re_60, reset value: 0x0, access type: RW
		uint32 h41Im60 : 10; //h41_im_60, reset value: 0x0, access type: RW
		uint32 h41Re61 : 10; //h41_re_61, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap560_u;

/*REG_RF_EMULATOR_TAP_561 0x8C4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h41Im61 : 10; //h41_im_61, reset value: 0x0, access type: RW
		uint32 h41Re62 : 10; //h41_re_62, reset value: 0x0, access type: RW
		uint32 h41Im62 : 10; //h41_im_62, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap561_u;

/*REG_RF_EMULATOR_TAP_562 0x8C8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h41Re63 : 10; //h41_re_63, reset value: 0x0, access type: RW
		uint32 h41Im63 : 10; //h41_im_63, reset value: 0x0, access type: RW
		uint32 h41Re64 : 10; //h41_re_64, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap562_u;

/*REG_RF_EMULATOR_TAP_563 0x8CC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h41Im64 : 10; //h41_im_64, reset value: 0x0, access type: RW
		uint32 h42Re0 : 10; //h42_re_0, reset value: 0x0, access type: RW
		uint32 h42Im0 : 10; //h42_im_0, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap563_u;

/*REG_RF_EMULATOR_TAP_564 0x8D0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h42Re1 : 10; //h42_re_1, reset value: 0x0, access type: RW
		uint32 h42Im1 : 10; //h42_im_1, reset value: 0x0, access type: RW
		uint32 h42Re2 : 10; //h42_re_2, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap564_u;

/*REG_RF_EMULATOR_TAP_565 0x8D4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h42Im2 : 10; //h42_im_2, reset value: 0x0, access type: RW
		uint32 h42Re3 : 10; //h42_re_3, reset value: 0x0, access type: RW
		uint32 h42Im3 : 10; //h42_im_3, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap565_u;

/*REG_RF_EMULATOR_TAP_566 0x8D8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h42Re4 : 10; //h42_re_4, reset value: 0x0, access type: RW
		uint32 h42Im4 : 10; //h42_im_4, reset value: 0x0, access type: RW
		uint32 h42Re5 : 10; //h42_re_5, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap566_u;

/*REG_RF_EMULATOR_TAP_567 0x8DC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h42Im5 : 10; //h42_im_5, reset value: 0x0, access type: RW
		uint32 h42Re6 : 10; //h42_re_6, reset value: 0x0, access type: RW
		uint32 h42Im6 : 10; //h42_im_6, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap567_u;

/*REG_RF_EMULATOR_TAP_568 0x8E0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h42Re7 : 10; //h42_re_7, reset value: 0x0, access type: RW
		uint32 h42Im7 : 10; //h42_im_7, reset value: 0x0, access type: RW
		uint32 h42Re8 : 10; //h42_re_8, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap568_u;

/*REG_RF_EMULATOR_TAP_569 0x8E4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h42Im8 : 10; //h42_im_8, reset value: 0x0, access type: RW
		uint32 h42Re9 : 10; //h42_re_9, reset value: 0x0, access type: RW
		uint32 h42Im9 : 10; //h42_im_9, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap569_u;

/*REG_RF_EMULATOR_TAP_570 0x8E8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h42Re10 : 10; //h42_re_10, reset value: 0x0, access type: RW
		uint32 h42Im10 : 10; //h42_im_10, reset value: 0x0, access type: RW
		uint32 h42Re11 : 10; //h42_re_11, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap570_u;

/*REG_RF_EMULATOR_TAP_571 0x8EC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h42Im11 : 10; //h42_im_11, reset value: 0x0, access type: RW
		uint32 h42Re12 : 10; //h42_re_12, reset value: 0x0, access type: RW
		uint32 h42Im12 : 10; //h42_im_12, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap571_u;

/*REG_RF_EMULATOR_TAP_572 0x8F0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h42Re13 : 10; //h42_re_13, reset value: 0x0, access type: RW
		uint32 h42Im13 : 10; //h42_im_13, reset value: 0x0, access type: RW
		uint32 h42Re14 : 10; //h42_re_14, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap572_u;

/*REG_RF_EMULATOR_TAP_573 0x8F4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h42Im14 : 10; //h42_im_14, reset value: 0x0, access type: RW
		uint32 h42Re15 : 10; //h42_re_15, reset value: 0x0, access type: RW
		uint32 h42Im15 : 10; //h42_im_15, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap573_u;

/*REG_RF_EMULATOR_TAP_574 0x8F8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h42Re16 : 10; //h42_re_16, reset value: 0x0, access type: RW
		uint32 h42Im16 : 10; //h42_im_16, reset value: 0x0, access type: RW
		uint32 h42Re17 : 10; //h42_re_17, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap574_u;

/*REG_RF_EMULATOR_TAP_575 0x8FC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h42Im17 : 10; //h42_im_17, reset value: 0x0, access type: RW
		uint32 h42Re18 : 10; //h42_re_18, reset value: 0x0, access type: RW
		uint32 h42Im18 : 10; //h42_im_18, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap575_u;

/*REG_RF_EMULATOR_TAP_576 0x900 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h42Re19 : 10; //h42_re_19, reset value: 0x0, access type: RW
		uint32 h42Im19 : 10; //h42_im_19, reset value: 0x0, access type: RW
		uint32 h42Re20 : 10; //h42_re_20, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap576_u;

/*REG_RF_EMULATOR_TAP_577 0x904 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h42Im20 : 10; //h42_im_20, reset value: 0x0, access type: RW
		uint32 h42Re21 : 10; //h42_re_21, reset value: 0x0, access type: RW
		uint32 h42Im21 : 10; //h42_im_21, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap577_u;

/*REG_RF_EMULATOR_TAP_578 0x908 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h42Re22 : 10; //h42_re_22, reset value: 0x0, access type: RW
		uint32 h42Im22 : 10; //h42_im_22, reset value: 0x0, access type: RW
		uint32 h42Re23 : 10; //h42_re_23, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap578_u;

/*REG_RF_EMULATOR_TAP_579 0x90C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h42Im23 : 10; //h42_im_23, reset value: 0x0, access type: RW
		uint32 h42Re24 : 10; //h42_re_24, reset value: 0x0, access type: RW
		uint32 h42Im24 : 10; //h42_im_24, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap579_u;

/*REG_RF_EMULATOR_TAP_580 0x910 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h42Re25 : 10; //h42_re_25, reset value: 0x0, access type: RW
		uint32 h42Im25 : 10; //h42_im_25, reset value: 0x0, access type: RW
		uint32 h42Re26 : 10; //h42_re_26, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap580_u;

/*REG_RF_EMULATOR_TAP_581 0x914 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h42Im26 : 10; //h42_im_26, reset value: 0x0, access type: RW
		uint32 h42Re27 : 10; //h42_re_27, reset value: 0x0, access type: RW
		uint32 h42Im27 : 10; //h42_im_27, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap581_u;

/*REG_RF_EMULATOR_TAP_582 0x918 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h42Re28 : 10; //h42_re_28, reset value: 0x0, access type: RW
		uint32 h42Im28 : 10; //h42_im_28, reset value: 0x0, access type: RW
		uint32 h42Re29 : 10; //h42_re_29, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap582_u;

/*REG_RF_EMULATOR_TAP_583 0x91C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h42Im29 : 10; //h42_im_29, reset value: 0x0, access type: RW
		uint32 h42Re30 : 10; //h42_re_30, reset value: 0x0, access type: RW
		uint32 h42Im30 : 10; //h42_im_30, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap583_u;

/*REG_RF_EMULATOR_TAP_584 0x920 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h42Re31 : 10; //h42_re_31, reset value: 0x0, access type: RW
		uint32 h42Im31 : 10; //h42_im_31, reset value: 0x0, access type: RW
		uint32 h42Re32 : 10; //h42_re_32, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap584_u;

/*REG_RF_EMULATOR_TAP_585 0x924 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h42Im32 : 10; //h42_im_32, reset value: 0x0, access type: RW
		uint32 h42Re33 : 10; //h42_re_33, reset value: 0x0, access type: RW
		uint32 h42Im33 : 10; //h42_im_33, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap585_u;

/*REG_RF_EMULATOR_TAP_586 0x928 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h42Re34 : 10; //h42_re_34, reset value: 0x0, access type: RW
		uint32 h42Im34 : 10; //h42_im_34, reset value: 0x0, access type: RW
		uint32 h42Re35 : 10; //h42_re_35, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap586_u;

/*REG_RF_EMULATOR_TAP_587 0x92C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h42Im35 : 10; //h42_im_35, reset value: 0x0, access type: RW
		uint32 h42Re36 : 10; //h42_re_36, reset value: 0x0, access type: RW
		uint32 h42Im36 : 10; //h42_im_36, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap587_u;

/*REG_RF_EMULATOR_TAP_588 0x930 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h42Re37 : 10; //h42_re_37, reset value: 0x0, access type: RW
		uint32 h42Im37 : 10; //h42_im_37, reset value: 0x0, access type: RW
		uint32 h42Re38 : 10; //h42_re_38, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap588_u;

/*REG_RF_EMULATOR_TAP_589 0x934 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h42Im38 : 10; //h42_im_38, reset value: 0x0, access type: RW
		uint32 h42Re39 : 10; //h42_re_39, reset value: 0x0, access type: RW
		uint32 h42Im39 : 10; //h42_im_39, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap589_u;

/*REG_RF_EMULATOR_TAP_590 0x938 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h42Re40 : 10; //h42_re_40, reset value: 0x0, access type: RW
		uint32 h42Im40 : 10; //h42_im_40, reset value: 0x0, access type: RW
		uint32 h42Re41 : 10; //h42_re_41, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap590_u;

/*REG_RF_EMULATOR_TAP_591 0x93C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h42Im41 : 10; //h42_im_41, reset value: 0x0, access type: RW
		uint32 h42Re42 : 10; //h42_re_42, reset value: 0x0, access type: RW
		uint32 h42Im42 : 10; //h42_im_42, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap591_u;

/*REG_RF_EMULATOR_TAP_592 0x940 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h42Re43 : 10; //h42_re_43, reset value: 0x0, access type: RW
		uint32 h42Im43 : 10; //h42_im_43, reset value: 0x0, access type: RW
		uint32 h42Re44 : 10; //h42_re_44, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap592_u;

/*REG_RF_EMULATOR_TAP_593 0x944 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h42Im44 : 10; //h42_im_44, reset value: 0x0, access type: RW
		uint32 h42Re45 : 10; //h42_re_45, reset value: 0x0, access type: RW
		uint32 h42Im45 : 10; //h42_im_45, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap593_u;

/*REG_RF_EMULATOR_TAP_594 0x948 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h42Re46 : 10; //h42_re_46, reset value: 0x0, access type: RW
		uint32 h42Im46 : 10; //h42_im_46, reset value: 0x0, access type: RW
		uint32 h42Re47 : 10; //h42_re_47, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap594_u;

/*REG_RF_EMULATOR_TAP_595 0x94C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h42Im47 : 10; //h42_im_47, reset value: 0x0, access type: RW
		uint32 h42Re48 : 10; //h42_re_48, reset value: 0x0, access type: RW
		uint32 h42Im48 : 10; //h42_im_48, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap595_u;

/*REG_RF_EMULATOR_TAP_596 0x950 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h42Re49 : 10; //h42_re_49, reset value: 0x0, access type: RW
		uint32 h42Im49 : 10; //h42_im_49, reset value: 0x0, access type: RW
		uint32 h42Re50 : 10; //h42_re_50, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap596_u;

/*REG_RF_EMULATOR_TAP_597 0x954 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h42Im50 : 10; //h42_im_50, reset value: 0x0, access type: RW
		uint32 h42Re51 : 10; //h42_re_51, reset value: 0x0, access type: RW
		uint32 h42Im51 : 10; //h42_im_51, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap597_u;

/*REG_RF_EMULATOR_TAP_598 0x958 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h42Re52 : 10; //h42_re_52, reset value: 0x0, access type: RW
		uint32 h42Im52 : 10; //h42_im_52, reset value: 0x0, access type: RW
		uint32 h42Re53 : 10; //h42_re_53, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap598_u;

/*REG_RF_EMULATOR_TAP_599 0x95C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h42Im53 : 10; //h42_im_53, reset value: 0x0, access type: RW
		uint32 h42Re54 : 10; //h42_re_54, reset value: 0x0, access type: RW
		uint32 h42Im54 : 10; //h42_im_54, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap599_u;

/*REG_RF_EMULATOR_TAP_600 0x960 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h42Re55 : 10; //h42_re_55, reset value: 0x0, access type: RW
		uint32 h42Im55 : 10; //h42_im_55, reset value: 0x0, access type: RW
		uint32 h42Re56 : 10; //h42_re_56, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap600_u;

/*REG_RF_EMULATOR_TAP_601 0x964 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h42Im56 : 10; //h42_im_56, reset value: 0x0, access type: RW
		uint32 h42Re57 : 10; //h42_re_57, reset value: 0x0, access type: RW
		uint32 h42Im57 : 10; //h42_im_57, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap601_u;

/*REG_RF_EMULATOR_TAP_602 0x968 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h42Re58 : 10; //h42_re_58, reset value: 0x0, access type: RW
		uint32 h42Im58 : 10; //h42_im_58, reset value: 0x0, access type: RW
		uint32 h42Re59 : 10; //h42_re_59, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap602_u;

/*REG_RF_EMULATOR_TAP_603 0x96C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h42Im59 : 10; //h42_im_59, reset value: 0x0, access type: RW
		uint32 h42Re60 : 10; //h42_re_60, reset value: 0x0, access type: RW
		uint32 h42Im60 : 10; //h42_im_60, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap603_u;

/*REG_RF_EMULATOR_TAP_604 0x970 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h42Re61 : 10; //h42_re_61, reset value: 0x0, access type: RW
		uint32 h42Im61 : 10; //h42_im_61, reset value: 0x0, access type: RW
		uint32 h42Re62 : 10; //h42_re_62, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap604_u;

/*REG_RF_EMULATOR_TAP_605 0x974 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h42Im62 : 10; //h42_im_62, reset value: 0x0, access type: RW
		uint32 h42Re63 : 10; //h42_re_63, reset value: 0x0, access type: RW
		uint32 h42Im63 : 10; //h42_im_63, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap605_u;

/*REG_RF_EMULATOR_TAP_606 0x978 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h42Re64 : 10; //h42_re_64, reset value: 0x0, access type: RW
		uint32 h42Im64 : 10; //h42_im_64, reset value: 0x0, access type: RW
		uint32 h43Re0 : 10; //h43_re_0, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap606_u;

/*REG_RF_EMULATOR_TAP_607 0x97C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h43Im0 : 10; //h43_im_0, reset value: 0x0, access type: RW
		uint32 h43Re1 : 10; //h43_re_1, reset value: 0x0, access type: RW
		uint32 h43Im1 : 10; //h43_im_1, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap607_u;

/*REG_RF_EMULATOR_TAP_608 0x980 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h43Re2 : 10; //h43_re_2, reset value: 0x0, access type: RW
		uint32 h43Im2 : 10; //h43_im_2, reset value: 0x0, access type: RW
		uint32 h43Re3 : 10; //h43_re_3, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap608_u;

/*REG_RF_EMULATOR_TAP_609 0x984 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h43Im3 : 10; //h43_im_3, reset value: 0x0, access type: RW
		uint32 h43Re4 : 10; //h43_re_4, reset value: 0x0, access type: RW
		uint32 h43Im4 : 10; //h43_im_4, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap609_u;

/*REG_RF_EMULATOR_TAP_610 0x988 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h43Re5 : 10; //h43_re_5, reset value: 0x0, access type: RW
		uint32 h43Im5 : 10; //h43_im_5, reset value: 0x0, access type: RW
		uint32 h43Re6 : 10; //h43_re_6, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap610_u;

/*REG_RF_EMULATOR_TAP_611 0x98C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h43Im6 : 10; //h43_im_6, reset value: 0x0, access type: RW
		uint32 h43Re7 : 10; //h43_re_7, reset value: 0x0, access type: RW
		uint32 h43Im7 : 10; //h43_im_7, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap611_u;

/*REG_RF_EMULATOR_TAP_612 0x990 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h43Re8 : 10; //h43_re_8, reset value: 0x0, access type: RW
		uint32 h43Im8 : 10; //h43_im_8, reset value: 0x0, access type: RW
		uint32 h43Re9 : 10; //h43_re_9, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap612_u;

/*REG_RF_EMULATOR_TAP_613 0x994 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h43Im9 : 10; //h43_im_9, reset value: 0x0, access type: RW
		uint32 h43Re10 : 10; //h43_re_10, reset value: 0x0, access type: RW
		uint32 h43Im10 : 10; //h43_im_10, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap613_u;

/*REG_RF_EMULATOR_TAP_614 0x998 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h43Re11 : 10; //h43_re_11, reset value: 0x0, access type: RW
		uint32 h43Im11 : 10; //h43_im_11, reset value: 0x0, access type: RW
		uint32 h43Re12 : 10; //h43_re_12, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap614_u;

/*REG_RF_EMULATOR_TAP_615 0x99C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h43Im12 : 10; //h43_im_12, reset value: 0x0, access type: RW
		uint32 h43Re13 : 10; //h43_re_13, reset value: 0x0, access type: RW
		uint32 h43Im13 : 10; //h43_im_13, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap615_u;

/*REG_RF_EMULATOR_TAP_616 0x9A0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h43Re14 : 10; //h43_re_14, reset value: 0x0, access type: RW
		uint32 h43Im14 : 10; //h43_im_14, reset value: 0x0, access type: RW
		uint32 h43Re15 : 10; //h43_re_15, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap616_u;

/*REG_RF_EMULATOR_TAP_617 0x9A4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h43Im15 : 10; //h43_im_15, reset value: 0x0, access type: RW
		uint32 h43Re16 : 10; //h43_re_16, reset value: 0x0, access type: RW
		uint32 h43Im16 : 10; //h43_im_16, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap617_u;

/*REG_RF_EMULATOR_TAP_618 0x9A8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h43Re17 : 10; //h43_re_17, reset value: 0x0, access type: RW
		uint32 h43Im17 : 10; //h43_im_17, reset value: 0x0, access type: RW
		uint32 h43Re18 : 10; //h43_re_18, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap618_u;

/*REG_RF_EMULATOR_TAP_619 0x9AC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h43Im18 : 10; //h43_im_18, reset value: 0x0, access type: RW
		uint32 h43Re19 : 10; //h43_re_19, reset value: 0x0, access type: RW
		uint32 h43Im19 : 10; //h43_im_19, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap619_u;

/*REG_RF_EMULATOR_TAP_620 0x9B0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h43Re20 : 10; //h43_re_20, reset value: 0x0, access type: RW
		uint32 h43Im20 : 10; //h43_im_20, reset value: 0x0, access type: RW
		uint32 h43Re21 : 10; //h43_re_21, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap620_u;

/*REG_RF_EMULATOR_TAP_621 0x9B4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h43Im21 : 10; //h43_im_21, reset value: 0x0, access type: RW
		uint32 h43Re22 : 10; //h43_re_22, reset value: 0x0, access type: RW
		uint32 h43Im22 : 10; //h43_im_22, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap621_u;

/*REG_RF_EMULATOR_TAP_622 0x9B8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h43Re23 : 10; //h43_re_23, reset value: 0x0, access type: RW
		uint32 h43Im23 : 10; //h43_im_23, reset value: 0x0, access type: RW
		uint32 h43Re24 : 10; //h43_re_24, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap622_u;

/*REG_RF_EMULATOR_TAP_623 0x9BC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h43Im24 : 10; //h43_im_24, reset value: 0x0, access type: RW
		uint32 h43Re25 : 10; //h43_re_25, reset value: 0x0, access type: RW
		uint32 h43Im25 : 10; //h43_im_25, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap623_u;

/*REG_RF_EMULATOR_TAP_624 0x9C0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h43Re26 : 10; //h43_re_26, reset value: 0x0, access type: RW
		uint32 h43Im26 : 10; //h43_im_26, reset value: 0x0, access type: RW
		uint32 h43Re27 : 10; //h43_re_27, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap624_u;

/*REG_RF_EMULATOR_TAP_625 0x9C4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h43Im27 : 10; //h43_im_27, reset value: 0x0, access type: RW
		uint32 h43Re28 : 10; //h43_re_28, reset value: 0x0, access type: RW
		uint32 h43Im28 : 10; //h43_im_28, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap625_u;

/*REG_RF_EMULATOR_TAP_626 0x9C8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h43Re29 : 10; //h43_re_29, reset value: 0x0, access type: RW
		uint32 h43Im29 : 10; //h43_im_29, reset value: 0x0, access type: RW
		uint32 h43Re30 : 10; //h43_re_30, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap626_u;

/*REG_RF_EMULATOR_TAP_627 0x9CC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h43Im30 : 10; //h43_im_30, reset value: 0x0, access type: RW
		uint32 h43Re31 : 10; //h43_re_31, reset value: 0x0, access type: RW
		uint32 h43Im31 : 10; //h43_im_31, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap627_u;

/*REG_RF_EMULATOR_TAP_628 0x9D0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h43Re32 : 10; //h43_re_32, reset value: 0x0, access type: RW
		uint32 h43Im32 : 10; //h43_im_32, reset value: 0x0, access type: RW
		uint32 h43Re33 : 10; //h43_re_33, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap628_u;

/*REG_RF_EMULATOR_TAP_629 0x9D4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h43Im33 : 10; //h43_im_33, reset value: 0x0, access type: RW
		uint32 h43Re34 : 10; //h43_re_34, reset value: 0x0, access type: RW
		uint32 h43Im34 : 10; //h43_im_34, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap629_u;

/*REG_RF_EMULATOR_TAP_630 0x9D8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h43Re35 : 10; //h43_re_35, reset value: 0x0, access type: RW
		uint32 h43Im35 : 10; //h43_im_35, reset value: 0x0, access type: RW
		uint32 h43Re36 : 10; //h43_re_36, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap630_u;

/*REG_RF_EMULATOR_TAP_631 0x9DC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h43Im36 : 10; //h43_im_36, reset value: 0x0, access type: RW
		uint32 h43Re37 : 10; //h43_re_37, reset value: 0x0, access type: RW
		uint32 h43Im37 : 10; //h43_im_37, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap631_u;

/*REG_RF_EMULATOR_TAP_632 0x9E0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h43Re38 : 10; //h43_re_38, reset value: 0x0, access type: RW
		uint32 h43Im38 : 10; //h43_im_38, reset value: 0x0, access type: RW
		uint32 h43Re39 : 10; //h43_re_39, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap632_u;

/*REG_RF_EMULATOR_TAP_633 0x9E4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h43Im39 : 10; //h43_im_39, reset value: 0x0, access type: RW
		uint32 h43Re40 : 10; //h43_re_40, reset value: 0x0, access type: RW
		uint32 h43Im40 : 10; //h43_im_40, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap633_u;

/*REG_RF_EMULATOR_TAP_634 0x9E8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h43Re41 : 10; //h43_re_41, reset value: 0x0, access type: RW
		uint32 h43Im41 : 10; //h43_im_41, reset value: 0x0, access type: RW
		uint32 h43Re42 : 10; //h43_re_42, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap634_u;

/*REG_RF_EMULATOR_TAP_635 0x9EC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h43Im42 : 10; //h43_im_42, reset value: 0x0, access type: RW
		uint32 h43Re43 : 10; //h43_re_43, reset value: 0x0, access type: RW
		uint32 h43Im43 : 10; //h43_im_43, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap635_u;

/*REG_RF_EMULATOR_TAP_636 0x9F0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h43Re44 : 10; //h43_re_44, reset value: 0x0, access type: RW
		uint32 h43Im44 : 10; //h43_im_44, reset value: 0x0, access type: RW
		uint32 h43Re45 : 10; //h43_re_45, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap636_u;

/*REG_RF_EMULATOR_TAP_637 0x9F4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h43Im45 : 10; //h43_im_45, reset value: 0x0, access type: RW
		uint32 h43Re46 : 10; //h43_re_46, reset value: 0x0, access type: RW
		uint32 h43Im46 : 10; //h43_im_46, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap637_u;

/*REG_RF_EMULATOR_TAP_638 0x9F8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h43Re47 : 10; //h43_re_47, reset value: 0x0, access type: RW
		uint32 h43Im47 : 10; //h43_im_47, reset value: 0x0, access type: RW
		uint32 h43Re48 : 10; //h43_re_48, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap638_u;

/*REG_RF_EMULATOR_TAP_639 0x9FC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h43Im48 : 10; //h43_im_48, reset value: 0x0, access type: RW
		uint32 h43Re49 : 10; //h43_re_49, reset value: 0x0, access type: RW
		uint32 h43Im49 : 10; //h43_im_49, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap639_u;

/*REG_RF_EMULATOR_TAP_640 0xA00 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h43Re50 : 10; //h43_re_50, reset value: 0x0, access type: RW
		uint32 h43Im50 : 10; //h43_im_50, reset value: 0x0, access type: RW
		uint32 h43Re51 : 10; //h43_re_51, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap640_u;

/*REG_RF_EMULATOR_TAP_641 0xA04 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h43Im51 : 10; //h43_im_51, reset value: 0x0, access type: RW
		uint32 h43Re52 : 10; //h43_re_52, reset value: 0x0, access type: RW
		uint32 h43Im52 : 10; //h43_im_52, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap641_u;

/*REG_RF_EMULATOR_TAP_642 0xA08 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h43Re53 : 10; //h43_re_53, reset value: 0x0, access type: RW
		uint32 h43Im53 : 10; //h43_im_53, reset value: 0x0, access type: RW
		uint32 h43Re54 : 10; //h43_re_54, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap642_u;

/*REG_RF_EMULATOR_TAP_643 0xA0C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h43Im54 : 10; //h43_im_54, reset value: 0x0, access type: RW
		uint32 h43Re55 : 10; //h43_re_55, reset value: 0x0, access type: RW
		uint32 h43Im55 : 10; //h43_im_55, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap643_u;

/*REG_RF_EMULATOR_TAP_644 0xA10 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h43Re56 : 10; //h43_re_56, reset value: 0x0, access type: RW
		uint32 h43Im56 : 10; //h43_im_56, reset value: 0x0, access type: RW
		uint32 h43Re57 : 10; //h43_re_57, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap644_u;

/*REG_RF_EMULATOR_TAP_645 0xA14 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h43Im57 : 10; //h43_im_57, reset value: 0x0, access type: RW
		uint32 h43Re58 : 10; //h43_re_58, reset value: 0x0, access type: RW
		uint32 h43Im58 : 10; //h43_im_58, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap645_u;

/*REG_RF_EMULATOR_TAP_646 0xA18 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h43Re59 : 10; //h43_re_59, reset value: 0x0, access type: RW
		uint32 h43Im59 : 10; //h43_im_59, reset value: 0x0, access type: RW
		uint32 h43Re60 : 10; //h43_re_60, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap646_u;

/*REG_RF_EMULATOR_TAP_647 0xA1C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h43Im60 : 10; //h43_im_60, reset value: 0x0, access type: RW
		uint32 h43Re61 : 10; //h43_re_61, reset value: 0x0, access type: RW
		uint32 h43Im61 : 10; //h43_im_61, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap647_u;

/*REG_RF_EMULATOR_TAP_648 0xA20 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h43Re62 : 10; //h43_re_62, reset value: 0x0, access type: RW
		uint32 h43Im62 : 10; //h43_im_62, reset value: 0x0, access type: RW
		uint32 h43Re63 : 10; //h43_re_63, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap648_u;

/*REG_RF_EMULATOR_TAP_649 0xA24 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h43Im63 : 10; //h43_im_63, reset value: 0x0, access type: RW
		uint32 h43Re64 : 10; //h43_re_64, reset value: 0x0, access type: RW
		uint32 h43Im64 : 10; //h43_im_64, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap649_u;

/*REG_RF_EMULATOR_TAP_650 0xA28 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h44Re0 : 10; //h44_re_0, reset value: 0x1ff, access type: RW
		uint32 h44Im0 : 10; //h44_im_0, reset value: 0x0, access type: RW
		uint32 h44Re1 : 10; //h44_re_1, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap650_u;

/*REG_RF_EMULATOR_TAP_651 0xA2C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h44Im1 : 10; //h44_im_1, reset value: 0x0, access type: RW
		uint32 h44Re2 : 10; //h44_re_2, reset value: 0x0, access type: RW
		uint32 h44Im2 : 10; //h44_im_2, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap651_u;

/*REG_RF_EMULATOR_TAP_652 0xA30 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h44Re3 : 10; //h44_re_3, reset value: 0x0, access type: RW
		uint32 h44Im3 : 10; //h44_im_3, reset value: 0x0, access type: RW
		uint32 h44Re4 : 10; //h44_re_4, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap652_u;

/*REG_RF_EMULATOR_TAP_653 0xA34 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h44Im4 : 10; //h44_im_4, reset value: 0x0, access type: RW
		uint32 h44Re5 : 10; //h44_re_5, reset value: 0x0, access type: RW
		uint32 h44Im5 : 10; //h44_im_5, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap653_u;

/*REG_RF_EMULATOR_TAP_654 0xA38 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h44Re6 : 10; //h44_re_6, reset value: 0x0, access type: RW
		uint32 h44Im6 : 10; //h44_im_6, reset value: 0x0, access type: RW
		uint32 h44Re7 : 10; //h44_re_7, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap654_u;

/*REG_RF_EMULATOR_TAP_655 0xA3C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h44Im7 : 10; //h44_im_7, reset value: 0x0, access type: RW
		uint32 h44Re8 : 10; //h44_re_8, reset value: 0x0, access type: RW
		uint32 h44Im8 : 10; //h44_im_8, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap655_u;

/*REG_RF_EMULATOR_TAP_656 0xA40 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h44Re9 : 10; //h44_re_9, reset value: 0x0, access type: RW
		uint32 h44Im9 : 10; //h44_im_9, reset value: 0x0, access type: RW
		uint32 h44Re10 : 10; //h44_re_10, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap656_u;

/*REG_RF_EMULATOR_TAP_657 0xA44 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h44Im10 : 10; //h44_im_10, reset value: 0x0, access type: RW
		uint32 h44Re11 : 10; //h44_re_11, reset value: 0x0, access type: RW
		uint32 h44Im11 : 10; //h44_im_11, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap657_u;

/*REG_RF_EMULATOR_TAP_658 0xA48 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h44Re12 : 10; //h44_re_12, reset value: 0x0, access type: RW
		uint32 h44Im12 : 10; //h44_im_12, reset value: 0x0, access type: RW
		uint32 h44Re13 : 10; //h44_re_13, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap658_u;

/*REG_RF_EMULATOR_TAP_659 0xA4C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h44Im13 : 10; //h44_im_13, reset value: 0x0, access type: RW
		uint32 h44Re14 : 10; //h44_re_14, reset value: 0x0, access type: RW
		uint32 h44Im14 : 10; //h44_im_14, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap659_u;

/*REG_RF_EMULATOR_TAP_660 0xA50 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h44Re15 : 10; //h44_re_15, reset value: 0x0, access type: RW
		uint32 h44Im15 : 10; //h44_im_15, reset value: 0x0, access type: RW
		uint32 h44Re16 : 10; //h44_re_16, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap660_u;

/*REG_RF_EMULATOR_TAP_661 0xA54 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h44Im16 : 10; //h44_im_16, reset value: 0x0, access type: RW
		uint32 h44Re17 : 10; //h44_re_17, reset value: 0x0, access type: RW
		uint32 h44Im17 : 10; //h44_im_17, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap661_u;

/*REG_RF_EMULATOR_TAP_662 0xA58 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h44Re18 : 10; //h44_re_18, reset value: 0x0, access type: RW
		uint32 h44Im18 : 10; //h44_im_18, reset value: 0x0, access type: RW
		uint32 h44Re19 : 10; //h44_re_19, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap662_u;

/*REG_RF_EMULATOR_TAP_663 0xA5C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h44Im19 : 10; //h44_im_19, reset value: 0x0, access type: RW
		uint32 h44Re20 : 10; //h44_re_20, reset value: 0x0, access type: RW
		uint32 h44Im20 : 10; //h44_im_20, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap663_u;

/*REG_RF_EMULATOR_TAP_664 0xA60 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h44Re21 : 10; //h44_re_21, reset value: 0x0, access type: RW
		uint32 h44Im21 : 10; //h44_im_21, reset value: 0x0, access type: RW
		uint32 h44Re22 : 10; //h44_re_22, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap664_u;

/*REG_RF_EMULATOR_TAP_665 0xA64 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h44Im22 : 10; //h44_im_22, reset value: 0x0, access type: RW
		uint32 h44Re23 : 10; //h44_re_23, reset value: 0x0, access type: RW
		uint32 h44Im23 : 10; //h44_im_23, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap665_u;

/*REG_RF_EMULATOR_TAP_666 0xA68 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h44Re24 : 10; //h44_re_24, reset value: 0x0, access type: RW
		uint32 h44Im24 : 10; //h44_im_24, reset value: 0x0, access type: RW
		uint32 h44Re25 : 10; //h44_re_25, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap666_u;

/*REG_RF_EMULATOR_TAP_667 0xA6C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h44Im25 : 10; //h44_im_25, reset value: 0x0, access type: RW
		uint32 h44Re26 : 10; //h44_re_26, reset value: 0x0, access type: RW
		uint32 h44Im26 : 10; //h44_im_26, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap667_u;

/*REG_RF_EMULATOR_TAP_668 0xA70 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h44Re27 : 10; //h44_re_27, reset value: 0x0, access type: RW
		uint32 h44Im27 : 10; //h44_im_27, reset value: 0x0, access type: RW
		uint32 h44Re28 : 10; //h44_re_28, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap668_u;

/*REG_RF_EMULATOR_TAP_669 0xA74 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h44Im28 : 10; //h44_im_28, reset value: 0x0, access type: RW
		uint32 h44Re29 : 10; //h44_re_29, reset value: 0x0, access type: RW
		uint32 h44Im29 : 10; //h44_im_29, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap669_u;

/*REG_RF_EMULATOR_TAP_670 0xA78 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h44Re30 : 10; //h44_re_30, reset value: 0x0, access type: RW
		uint32 h44Im30 : 10; //h44_im_30, reset value: 0x0, access type: RW
		uint32 h44Re31 : 10; //h44_re_31, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap670_u;

/*REG_RF_EMULATOR_TAP_671 0xA7C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h44Im31 : 10; //h44_im_31, reset value: 0x0, access type: RW
		uint32 h44Re32 : 10; //h44_re_32, reset value: 0x0, access type: RW
		uint32 h44Im32 : 10; //h44_im_32, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap671_u;

/*REG_RF_EMULATOR_TAP_672 0xA80 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h44Re33 : 10; //h44_re_33, reset value: 0x0, access type: RW
		uint32 h44Im33 : 10; //h44_im_33, reset value: 0x0, access type: RW
		uint32 h44Re34 : 10; //h44_re_34, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap672_u;

/*REG_RF_EMULATOR_TAP_673 0xA84 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h44Im34 : 10; //h44_im_34, reset value: 0x0, access type: RW
		uint32 h44Re35 : 10; //h44_re_35, reset value: 0x0, access type: RW
		uint32 h44Im35 : 10; //h44_im_35, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap673_u;

/*REG_RF_EMULATOR_TAP_674 0xA88 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h44Re36 : 10; //h44_re_36, reset value: 0x0, access type: RW
		uint32 h44Im36 : 10; //h44_im_36, reset value: 0x0, access type: RW
		uint32 h44Re37 : 10; //h44_re_37, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap674_u;

/*REG_RF_EMULATOR_TAP_675 0xA8C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h44Im37 : 10; //h44_im_37, reset value: 0x0, access type: RW
		uint32 h44Re38 : 10; //h44_re_38, reset value: 0x0, access type: RW
		uint32 h44Im38 : 10; //h44_im_38, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap675_u;

/*REG_RF_EMULATOR_TAP_676 0xA90 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h44Re39 : 10; //h44_re_39, reset value: 0x0, access type: RW
		uint32 h44Im39 : 10; //h44_im_39, reset value: 0x0, access type: RW
		uint32 h44Re40 : 10; //h44_re_40, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap676_u;

/*REG_RF_EMULATOR_TAP_677 0xA94 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h44Im40 : 10; //h44_im_40, reset value: 0x0, access type: RW
		uint32 h44Re41 : 10; //h44_re_41, reset value: 0x0, access type: RW
		uint32 h44Im41 : 10; //h44_im_41, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap677_u;

/*REG_RF_EMULATOR_TAP_678 0xA98 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h44Re42 : 10; //h44_re_42, reset value: 0x0, access type: RW
		uint32 h44Im42 : 10; //h44_im_42, reset value: 0x0, access type: RW
		uint32 h44Re43 : 10; //h44_re_43, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap678_u;

/*REG_RF_EMULATOR_TAP_679 0xA9C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h44Im43 : 10; //h44_im_43, reset value: 0x0, access type: RW
		uint32 h44Re44 : 10; //h44_re_44, reset value: 0x0, access type: RW
		uint32 h44Im44 : 10; //h44_im_44, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap679_u;

/*REG_RF_EMULATOR_TAP_680 0xAA0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h44Re45 : 10; //h44_re_45, reset value: 0x0, access type: RW
		uint32 h44Im45 : 10; //h44_im_45, reset value: 0x0, access type: RW
		uint32 h44Re46 : 10; //h44_re_46, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap680_u;

/*REG_RF_EMULATOR_TAP_681 0xAA4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h44Im46 : 10; //h44_im_46, reset value: 0x0, access type: RW
		uint32 h44Re47 : 10; //h44_re_47, reset value: 0x0, access type: RW
		uint32 h44Im47 : 10; //h44_im_47, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap681_u;

/*REG_RF_EMULATOR_TAP_682 0xAA8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h44Re48 : 10; //h44_re_48, reset value: 0x0, access type: RW
		uint32 h44Im48 : 10; //h44_im_48, reset value: 0x0, access type: RW
		uint32 h44Re49 : 10; //h44_re_49, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap682_u;

/*REG_RF_EMULATOR_TAP_683 0xAAC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h44Im49 : 10; //h44_im_49, reset value: 0x0, access type: RW
		uint32 h44Re50 : 10; //h44_re_50, reset value: 0x0, access type: RW
		uint32 h44Im50 : 10; //h44_im_50, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap683_u;

/*REG_RF_EMULATOR_TAP_684 0xAB0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h44Re51 : 10; //h44_re_51, reset value: 0x0, access type: RW
		uint32 h44Im51 : 10; //h44_im_51, reset value: 0x0, access type: RW
		uint32 h44Re52 : 10; //h44_re_52, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap684_u;

/*REG_RF_EMULATOR_TAP_685 0xAB4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h44Im52 : 10; //h44_im_52, reset value: 0x0, access type: RW
		uint32 h44Re53 : 10; //h44_re_53, reset value: 0x0, access type: RW
		uint32 h44Im53 : 10; //h44_im_53, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap685_u;

/*REG_RF_EMULATOR_TAP_686 0xAB8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h44Re54 : 10; //h44_re_54, reset value: 0x0, access type: RW
		uint32 h44Im54 : 10; //h44_im_54, reset value: 0x0, access type: RW
		uint32 h44Re55 : 10; //h44_re_55, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap686_u;

/*REG_RF_EMULATOR_TAP_687 0xABC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h44Im55 : 10; //h44_im_55, reset value: 0x0, access type: RW
		uint32 h44Re56 : 10; //h44_re_56, reset value: 0x0, access type: RW
		uint32 h44Im56 : 10; //h44_im_56, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap687_u;

/*REG_RF_EMULATOR_TAP_688 0xAC0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h44Re57 : 10; //h44_re_57, reset value: 0x0, access type: RW
		uint32 h44Im57 : 10; //h44_im_57, reset value: 0x0, access type: RW
		uint32 h44Re58 : 10; //h44_re_58, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap688_u;

/*REG_RF_EMULATOR_TAP_689 0xAC4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h44Im58 : 10; //h44_im_58, reset value: 0x0, access type: RW
		uint32 h44Re59 : 10; //h44_re_59, reset value: 0x0, access type: RW
		uint32 h44Im59 : 10; //h44_im_59, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap689_u;

/*REG_RF_EMULATOR_TAP_690 0xAC8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h44Re60 : 10; //h44_re_60, reset value: 0x0, access type: RW
		uint32 h44Im60 : 10; //h44_im_60, reset value: 0x0, access type: RW
		uint32 h44Re61 : 10; //h44_re_61, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap690_u;

/*REG_RF_EMULATOR_TAP_691 0xACC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h44Im61 : 10; //h44_im_61, reset value: 0x0, access type: RW
		uint32 h44Re62 : 10; //h44_re_62, reset value: 0x0, access type: RW
		uint32 h44Im62 : 10; //h44_im_62, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap691_u;

/*REG_RF_EMULATOR_TAP_692 0xAD0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h44Re63 : 10; //h44_re_63, reset value: 0x0, access type: RW
		uint32 h44Im63 : 10; //h44_im_63, reset value: 0x0, access type: RW
		uint32 h44Re64 : 10; //h44_re_64, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegRfEmulatorTap692_u;

/*REG_RF_EMULATOR_TAP_693 0xAD4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 h44Im64 : 10; //h44_im_64, reset value: 0x0, access type: RW
		uint32 reserved0 : 22;
	} bitFields;
} RegRfEmulatorTap693_u;

/*REG_RF_EMULATOR_RX_SNR 0xB00 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 rxSnrAnt0 : 8; //SNR value for the RX, reset value: 0x1, access type: RW
		uint32 rxSnrAnt1 : 8; //SNR value for the RX, reset value: 0x1, access type: RW
		uint32 rxSnrAnt2 : 8; //SNR value for the RX, reset value: 0x1, access type: RW
		uint32 rxSnrAnt3 : 8; //SNR value for the RX, reset value: 0x1, access type: RW
	} bitFields;
} RegRfEmulatorRxSnr_u;

/*REG_RF_EMULATOR_TX_SNR 0xB04 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txSnrAnt0 : 8; //SNR value for the TX, reset value: 0x1, access type: RW
		uint32 txSnrAnt1 : 8; //SNR value for the TX, reset value: 0x1, access type: RW
		uint32 txSnrAnt2 : 8; //SNR value for the TX, reset value: 0x1, access type: RW
		uint32 txSnrAnt3 : 8; //SNR value for the TX, reset value: 0x1, access type: RW
	} bitFields;
} RegRfEmulatorTxSnr_u;

/*REG_RF_EMULATOR_RX_ANT0_SEED0 0xB08 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 rxBaseSeed0Ant0 : 32; //seed value, reset value: 0x12, access type: RW
	} bitFields;
} RegRfEmulatorRxAnt0Seed0_u;

/*REG_RF_EMULATOR_RX_ANT0_SEED1 0xB0C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 rxBaseSeed1Ant0 : 32; //seed value, reset value: 0x12, access type: RW
	} bitFields;
} RegRfEmulatorRxAnt0Seed1_u;

/*REG_RF_EMULATOR_RX_ANT0_SEED2 0xB10 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 rxBaseSeed2Ant0 : 32; //seed value, reset value: 0x12, access type: RW
	} bitFields;
} RegRfEmulatorRxAnt0Seed2_u;

/*REG_RF_EMULATOR_RX_ANT1_SEED0 0xB14 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 rxBaseSeed0Ant1 : 32; //seed value, reset value: 0x12, access type: RW
	} bitFields;
} RegRfEmulatorRxAnt1Seed0_u;

/*REG_RF_EMULATOR_RX_ANT1_SEED1 0xB18 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 rxBaseSeed1Ant1 : 32; //seed value, reset value: 0x12, access type: RW
	} bitFields;
} RegRfEmulatorRxAnt1Seed1_u;

/*REG_RF_EMULATOR_RX_ANT1_SEED2 0xB1C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 rxBaseSeed2Ant1 : 32; //seed value, reset value: 0x12, access type: RW
	} bitFields;
} RegRfEmulatorRxAnt1Seed2_u;

/*REG_RF_EMULATOR_RX_ANT2_SEED0 0xB20 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 rxBaseSeed0Ant2 : 32; //seed value, reset value: 0x12, access type: RW
	} bitFields;
} RegRfEmulatorRxAnt2Seed0_u;

/*REG_RF_EMULATOR_RX_ANT2_SEED1 0xB24 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 rxBaseSeed1Ant2 : 32; //seed value, reset value: 0x12, access type: RW
	} bitFields;
} RegRfEmulatorRxAnt2Seed1_u;

/*REG_RF_EMULATOR_RX_ANT2_SEED2 0xB28 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 rxBaseSeed2Ant2 : 32; //seed value, reset value: 0x12, access type: RW
	} bitFields;
} RegRfEmulatorRxAnt2Seed2_u;

/*REG_RF_EMULATOR_RX_ANT3_SEED0 0xB2C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 rxBaseSeed0Ant3 : 32; //seed value, reset value: 0x12, access type: RW
	} bitFields;
} RegRfEmulatorRxAnt3Seed0_u;

/*REG_RF_EMULATOR_RX_ANT3_SEED1 0xB30 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 rxBaseSeed1Ant3 : 32; //seed value, reset value: 0x12, access type: RW
	} bitFields;
} RegRfEmulatorRxAnt3Seed1_u;

/*REG_RF_EMULATOR_RX_ANT3_SEED2 0xB34 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 rxBaseSeed2Ant3 : 32; //seed value, reset value: 0x12, access type: RW
	} bitFields;
} RegRfEmulatorRxAnt3Seed2_u;

/*REG_RF_EMULATOR_TX_ANT0_SEED0 0xB38 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txBaseSeed0Ant0 : 32; //seed value, reset value: 0x12, access type: RW
	} bitFields;
} RegRfEmulatorTxAnt0Seed0_u;

/*REG_RF_EMULATOR_TX_ANT0_SEED1 0xB3C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txBaseSeed1Ant0 : 32; //seed value, reset value: 0x12, access type: RW
	} bitFields;
} RegRfEmulatorTxAnt0Seed1_u;

/*REG_RF_EMULATOR_TX_ANT0_SEED2 0xB40 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txBaseSeed2Ant0 : 32; //seed value, reset value: 0x12, access type: RW
	} bitFields;
} RegRfEmulatorTxAnt0Seed2_u;

/*REG_RF_EMULATOR_TX_ANT1_SEED0 0xB44 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txBaseSeed0Ant1 : 32; //seed value, reset value: 0x12, access type: RW
	} bitFields;
} RegRfEmulatorTxAnt1Seed0_u;

/*REG_RF_EMULATOR_TX_ANT1_SEED1 0xB48 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txBaseSeed1Ant1 : 32; //seed value, reset value: 0x12, access type: RW
	} bitFields;
} RegRfEmulatorTxAnt1Seed1_u;

/*REG_RF_EMULATOR_TX_ANT1_SEED2 0xB4C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txBaseSeed2Ant1 : 32; //seed value, reset value: 0x12, access type: RW
	} bitFields;
} RegRfEmulatorTxAnt1Seed2_u;

/*REG_RF_EMULATOR_TX_ANT2_SEED0 0xB50 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txBaseSeed0Ant2 : 32; //seed value, reset value: 0x12, access type: RW
	} bitFields;
} RegRfEmulatorTxAnt2Seed0_u;

/*REG_RF_EMULATOR_TX_ANT2_SEED1 0xB54 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txBaseSeed1Ant2 : 32; //seed value, reset value: 0x12, access type: RW
	} bitFields;
} RegRfEmulatorTxAnt2Seed1_u;

/*REG_RF_EMULATOR_TX_ANT2_SEED2 0xB58 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txBaseSeed2Ant2 : 32; //seed value, reset value: 0x12, access type: RW
	} bitFields;
} RegRfEmulatorTxAnt2Seed2_u;

/*REG_RF_EMULATOR_TX_ANT3_SEED0 0xB5C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txBaseSeed0Ant3 : 32; //seed value, reset value: 0x12, access type: RW
	} bitFields;
} RegRfEmulatorTxAnt3Seed0_u;

/*REG_RF_EMULATOR_TX_ANT3_SEED1 0xB60 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txBaseSeed1Ant3 : 32; //seed value, reset value: 0x12, access type: RW
	} bitFields;
} RegRfEmulatorTxAnt3Seed1_u;

/*REG_RF_EMULATOR_TX_ANT3_SEED2 0xB64 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txBaseSeed2Ant3 : 32; //seed value, reset value: 0x12, access type: RW
	} bitFields;
} RegRfEmulatorTxAnt3Seed2_u;

/*REG_RF_EMULATOR_LOSS 0xB68 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 lossValue : 8; //loss value for local FIRs, reset value: 0x7F, access type: RW
		uint32 reserved0 : 24;
	} bitFields;
} RegRfEmulatorLoss_u;

/*REG_RF_EMULATOR_RF_CONTROL 0xB70 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 selectHbLb : 4; //select between 2.4GHz antenna (1'b0) and 5GHz antenna (1'b1), reset value: 0xF, access type: RW
		uint32 enPgaBfilter : 4; //enable per antenna the PGA in the RF, reset value: 0xF, access type: RW
		uint32 rfRxDelay : 8; //640MHz cycle delay of RF module RX latency, reset value: 0x0, access type: RW
		uint32 rfTxDelay : 8; //640MHz cycle delay of RF module TX latency, reset value: 0x0, access type: RW
		uint32 channelAdd1Smp : 1; //Add 640MHz cycle delay before the channel emulator on the valid signal, reset value: 0x0, access type: RW
		uint32 channelRxStrbCntrl : 1; //Add 640MHz cycle delay for the RX strb in the PHY, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
		uint32 addNoiseFromRfGain : 4; //per antenna bit to add the noise gain from the RF gain, reset value: 0x0, access type: RW
	} bitFields;
} RegRfEmulatorRfControl_u;

/*REG_RF_EMULATOR_TSSI2_ANT0 0xB74 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 swTssi2Ant0 : 24; //Set TSSI value, reset value: 0x1, access type: RW
		uint32 reserved0 : 8;
	} bitFields;
} RegRfEmulatorTssi2Ant0_u;

/*REG_RF_EMULATOR_TSSI2_ANT1 0xB78 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 swTssi2Ant1 : 24; //Set TSSI value, reset value: 0x1, access type: RW
		uint32 reserved0 : 8;
	} bitFields;
} RegRfEmulatorTssi2Ant1_u;

/*REG_RF_EMULATOR_TSSI2_ANT2 0xB7C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 swTssi2Ant2 : 24; //Set TSSI value, reset value: 0x1, access type: RW
		uint32 reserved0 : 8;
	} bitFields;
} RegRfEmulatorTssi2Ant2_u;

/*REG_RF_EMULATOR_TSSI2_ANT3 0xB80 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 swTssi2Ant3 : 24; //Set TSSI value, reset value: 0x1, access type: RW
		uint32 reserved0 : 8;
	} bitFields;
} RegRfEmulatorTssi2Ant3_u;

/*REG_RF_EMULATOR_TSSI5_ANT0 0xB84 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 swTssi5Ant0 : 24; //Set TSSI value, reset value: 0x1, access type: RW
		uint32 reserved0 : 8;
	} bitFields;
} RegRfEmulatorTssi5Ant0_u;

/*REG_RF_EMULATOR_TSSI5_ANT1 0xB88 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 swTssi5Ant1 : 24; //Set TSSI value, reset value: 0x1, access type: RW
		uint32 reserved0 : 8;
	} bitFields;
} RegRfEmulatorTssi5Ant1_u;

/*REG_RF_EMULATOR_TSSI5_ANT2 0xB8C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 swTssi5Ant2 : 24; //Set TSSI value, reset value: 0x1, access type: RW
		uint32 reserved0 : 8;
	} bitFields;
} RegRfEmulatorTssi5Ant2_u;

/*REG_RF_EMULATOR_TSSI5_ANT3 0xB90 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 swTssi5Ant3 : 24; //Set TSSI value, reset value: 0x1, access type: RW
		uint32 reserved0 : 8;
	} bitFields;
} RegRfEmulatorTssi5Ant3_u;

/*REG_RF_EMULATOR_FECTRL_ANT 0xB94 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 fectrlAnt0 : 6; //no description, reset value: 0x0, access type: RO
		uint32 fectrlAnt1 : 6; //no description, reset value: 0x0, access type: RO
		uint32 fectrlAnt2 : 6; //no description, reset value: 0x0, access type: RO
		uint32 fectrlAnt3 : 6; //no description, reset value: 0x0, access type: RO
		uint32 reserved0 : 8;
	} bitFields;
} RegRfEmulatorFectrlAnt_u;

/*REG_RF_EMULATOR_FECTRL_C 0xB98 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 fectrlC : 8; //no description, reset value: 0x0, access type: RO
		uint32 reserved0 : 24;
	} bitFields;
} RegRfEmulatorFectrlC_u;

/*REG_RF_EMULATOR_IMP_ANT0_PGA_IQ_GMM 0xBA0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 impAnt0PgaIqGmm : 24; //RX I/Q gain mismatch , reset value: 0x0, access type: RW
		uint32 reserved0 : 8;
	} bitFields;
} RegRfEmulatorImpAnt0PgaIqGmm_u;

/*REG_RF_EMULATOR_IMP_ANT0_PGA_IQ_PMM 0xBA4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 impAnt0PgaIqPmm : 24; //RX I/Q phase mismatch, reset value: 0x0, access type: RW
		uint32 reserved0 : 8;
	} bitFields;
} RegRfEmulatorImpAnt0PgaIqPmm_u;

/*REG_RF_EMULATOR_IMP_ANT1_PGA_IQ_GMM 0xBA8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 impAnt1PgaIqGmm : 24; //RX I/Q gain mismatch , reset value: 0x0, access type: RW
		uint32 reserved0 : 8;
	} bitFields;
} RegRfEmulatorImpAnt1PgaIqGmm_u;

/*REG_RF_EMULATOR_IMP_ANT1_PGA_IQ_PMM 0xBAC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 impAnt1PgaIqPmm : 24; //RX I/Q phase mismatch, reset value: 0x0, access type: RW
		uint32 reserved0 : 8;
	} bitFields;
} RegRfEmulatorImpAnt1PgaIqPmm_u;

/*REG_RF_EMULATOR_IMP_ANT2_PGA_IQ_GMM 0xBB0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 impAnt2PgaIqGmm : 24; //RX I/Q gain mismatch , reset value: 0x0, access type: RW
		uint32 reserved0 : 8;
	} bitFields;
} RegRfEmulatorImpAnt2PgaIqGmm_u;

/*REG_RF_EMULATOR_IMP_ANT2_PGA_IQ_PMM 0xBB4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 impAnt2PgaIqPmm : 24; //RX I/Q phase mismatch, reset value: 0x0, access type: RW
		uint32 reserved0 : 8;
	} bitFields;
} RegRfEmulatorImpAnt2PgaIqPmm_u;

/*REG_RF_EMULATOR_IMP_ANT3_PGA_IQ_GMM 0xBB8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 impAnt3PgaIqGmm : 24; //RX I/Q gain mismatch , reset value: 0x0, access type: RW
		uint32 reserved0 : 8;
	} bitFields;
} RegRfEmulatorImpAnt3PgaIqGmm_u;

/*REG_RF_EMULATOR_IMP_ANT3_PGA_IQ_PMM 0xBBC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 impAnt3PgaIqPmm : 24; //RX I/Q phase mismatch, reset value: 0x0, access type: RW
		uint32 reserved0 : 8;
	} bitFields;
} RegRfEmulatorImpAnt3PgaIqPmm_u;

/*REG_RF_EMULATOR_IMP_ANT0_TPC_DC_I 0xBC0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 impAnt0TpcDcI : 24; //TX DC impairments, reset value: 0x0, access type: RW
		uint32 reserved0 : 8;
	} bitFields;
} RegRfEmulatorImpAnt0TpcDcI_u;

/*REG_RF_EMULATOR_IMP_ANT0_TPC_DC_Q 0xBC4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 impAnt0TpcDcQ : 24; //TX DC impairments, reset value: 0x0, access type: RW
		uint32 reserved0 : 8;
	} bitFields;
} RegRfEmulatorImpAnt0TpcDcQ_u;

/*REG_RF_EMULATOR_IMP_ANT1_TPC_DC_I 0xBC8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 impAnt1TpcDcI : 24; //TX DC impairments, reset value: 0x0, access type: RW
		uint32 reserved0 : 8;
	} bitFields;
} RegRfEmulatorImpAnt1TpcDcI_u;

/*REG_RF_EMULATOR_IMP_ANT1_TPC_DC_Q 0xBCC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 impAnt1TpcDcQ : 24; //TX DC impairments, reset value: 0x0, access type: RW
		uint32 reserved0 : 8;
	} bitFields;
} RegRfEmulatorImpAnt1TpcDcQ_u;

/*REG_RF_EMULATOR_IMP_ANT2_TPC_DC_I 0xBD0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 impAnt2TpcDcI : 24; //TX DC impairments, reset value: 0x0, access type: RW
		uint32 reserved0 : 8;
	} bitFields;
} RegRfEmulatorImpAnt2TpcDcI_u;

/*REG_RF_EMULATOR_IMP_ANT2_TPC_DC_Q 0xBD4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 impAnt2TpcDcQ : 24; //TX DC impairments, reset value: 0x0, access type: RW
		uint32 reserved0 : 8;
	} bitFields;
} RegRfEmulatorImpAnt2TpcDcQ_u;

/*REG_RF_EMULATOR_IMP_ANT3_TPC_DC_I 0xBD8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 impAnt3TpcDcI : 24; //TX DC impairments, reset value: 0x0, access type: RW
		uint32 reserved0 : 8;
	} bitFields;
} RegRfEmulatorImpAnt3TpcDcI_u;

/*REG_RF_EMULATOR_IMP_ANT3_TPC_DC_Q 0xBDC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 impAnt3TpcDcQ : 24; //TX DC impairments, reset value: 0x0, access type: RW
		uint32 reserved0 : 8;
	} bitFields;
} RegRfEmulatorImpAnt3TpcDcQ_u;

/*REG_RF_EMULATOR_IMP_ANT0_PGA_DC_OFFSET_I 0xBE0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 impAnt0PgaDcOffsetI : 24; //RX DC imparmaents, reset value: 0x0, access type: RW
		uint32 reserved0 : 8;
	} bitFields;
} RegRfEmulatorImpAnt0PgaDcOffsetI_u;

/*REG_RF_EMULATOR_IMP_ANT0_PASSIVEMIXER_IQ_GMM 0xBE0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 impAnt0PassivemixerIqGmm : 24; //TX I/Q gain mismatch , reset value: 0x0, access type: RW
		uint32 reserved0 : 8;
	} bitFields;
} RegRfEmulatorImpAnt0PassivemixerIqGmm_u;

/*REG_RF_EMULATOR_IMP_ANT0_PGA_DC_OFFSET_Q 0xBE4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 impAnt0PgaDcOffsetQ : 24; //RX DC imparmaents, reset value: 0x0, access type: RW
		uint32 reserved0 : 8;
	} bitFields;
} RegRfEmulatorImpAnt0PgaDcOffsetQ_u;

/*REG_RF_EMULATOR_IMP_ANT0_PASSIVEMIXER_IQ_PMM 0xBE4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 impAnt0PassivemixerIqPmm : 24; //TX I/Q phase mismatch, reset value: 0x0, access type: RW
		uint32 reserved0 : 8;
	} bitFields;
} RegRfEmulatorImpAnt0PassivemixerIqPmm_u;

/*REG_RF_EMULATOR_IMP_ANT1_PASSIVEMIXER_IQ_GMM 0xBE8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 impAnt1PassivemixerIqGmm : 24; //TX I/Q gain mismatch , reset value: 0x0, access type: RW
		uint32 reserved0 : 8;
	} bitFields;
} RegRfEmulatorImpAnt1PassivemixerIqGmm_u;

/*REG_RF_EMULATOR_IMP_ANT1_PGA_DC_OFFSET_I 0xBE8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 impAnt1PgaDcOffsetI : 24; //RX DC imparmaents, reset value: 0x0, access type: RW
		uint32 reserved0 : 8;
	} bitFields;
} RegRfEmulatorImpAnt1PgaDcOffsetI_u;

/*REG_RF_EMULATOR_IMP_ANT1_PASSIVEMIXER_IQ_PMM 0xBEC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 impAnt1PassivemixerIqPmm : 24; //TX I/Q phase mismatch, reset value: 0x0, access type: RW
		uint32 reserved0 : 8;
	} bitFields;
} RegRfEmulatorImpAnt1PassivemixerIqPmm_u;

/*REG_RF_EMULATOR_IMP_ANT1_PGA_DC_OFFSET_Q 0xBEC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 impAnt1PgaDcOffsetQ : 24; //RX DC imparmaents, reset value: 0x0, access type: RW
		uint32 reserved0 : 8;
	} bitFields;
} RegRfEmulatorImpAnt1PgaDcOffsetQ_u;

/*REG_RF_EMULATOR_IMP_ANT2_PGA_DC_OFFSET_I 0xBF0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 impAnt2PgaDcOffsetI : 24; //RX DC imparmaents, reset value: 0x0, access type: RW
		uint32 reserved0 : 8;
	} bitFields;
} RegRfEmulatorImpAnt2PgaDcOffsetI_u;

/*REG_RF_EMULATOR_IMP_ANT2_PASSIVEMIXER_IQ_GMM 0xBF0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 impAnt2PassivemixerIqGmm : 24; //TX I/Q gain mismatch , reset value: 0x0, access type: RW
		uint32 reserved0 : 8;
	} bitFields;
} RegRfEmulatorImpAnt2PassivemixerIqGmm_u;

/*REG_RF_EMULATOR_IMP_ANT2_PGA_DC_OFFSET_Q 0xBF4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 impAnt2PgaDcOffsetQ : 24; //RX DC imparmaents, reset value: 0x0, access type: RW
		uint32 reserved0 : 8;
	} bitFields;
} RegRfEmulatorImpAnt2PgaDcOffsetQ_u;

/*REG_RF_EMULATOR_IMP_ANT2_PASSIVEMIXER_IQ_PMM 0xBF4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 impAnt2PassivemixerIqPmm : 24; //TX I/Q phase mismatch, reset value: 0x0, access type: RW
		uint32 reserved0 : 8;
	} bitFields;
} RegRfEmulatorImpAnt2PassivemixerIqPmm_u;

/*REG_RF_EMULATOR_IMP_ANT3_PASSIVEMIXER_IQ_GMM 0xBF8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 impAnt3PassivemixerIqGmm : 24; //TX I/Q gain mismatch , reset value: 0x0, access type: RW
		uint32 reserved0 : 8;
	} bitFields;
} RegRfEmulatorImpAnt3PassivemixerIqGmm_u;

/*REG_RF_EMULATOR_IMP_ANT3_PGA_DC_OFFSET_I 0xBF8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 impAnt3PgaDcOffsetI : 24; //RX DC imparmaents, reset value: 0x0, access type: RW
		uint32 reserved0 : 8;
	} bitFields;
} RegRfEmulatorImpAnt3PgaDcOffsetI_u;

/*REG_RF_EMULATOR_IMP_ANT3_PGA_DC_OFFSET_Q 0xBFC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 impAnt3PgaDcOffsetQ : 24; //RX DC imparmaents, reset value: 0x0, access type: RW
		uint32 reserved0 : 8;
	} bitFields;
} RegRfEmulatorImpAnt3PgaDcOffsetQ_u;

/*REG_RF_EMULATOR_IMP_ANT3_PASSIVEMIXER_IQ_PMM 0xBFC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 impAnt3PassivemixerIqPmm : 24; //TX I/Q phase mismatch, reset value: 0x0, access type: RW
		uint32 reserved0 : 8;
	} bitFields;
} RegRfEmulatorImpAnt3PassivemixerIqPmm_u;

/*REG_RF_EMULATOR_SPARE_REG 0xC00 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 spareField : 24; //spare RW reg, reset value: 0x1234, access type: RW
		uint32 reserved0 : 8;
	} bitFields;
} RegRfEmulatorSpareReg_u;

/*REG_RF_EMULATOR_RF_CONTROL_EXT 0xC04 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 channelSkipRxAnt : 4; //skip the rx path through the channel and loss per antenna, reset value: 0x0, access type: RW
		uint32 reserved0 : 28;
	} bitFields;
} RegRfEmulatorRfControlExt_u;

/*REG_RF_EMULATOR_EXTERNAL_PSEL 0xD00 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 externalPsels : 8; //Hit value for configuring all registers. This field must be equal to the exact instance id in order to read/write , reset value: 0x0, access type: RW
		uint32 reserved0 : 24;
	} bitFields;
} RegRfEmulatorExternalPsel_u;



#endif // _RF_EMULATOR_REGS_H_
