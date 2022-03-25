//-----------------------------------------------------------------------------
// LSD Generator
//-----------------------------------------------------------------------------
// Perl Package        : LSD::generator::targetC (v1.1)
// LSD Source          : C:/Users/huchunfe/Perforce/huchunfe_huchunfe-MOBL1_dev.FalcONT/ipg_lsd/lsd_sys/source/xml/reg_files/qos_wred.xml
// Register File Name  : QOS_WRED
// Register File Title : QoS WRED Registers
// Register Width      : 32
// Note                : Doxygen compliant comments
//-----------------------------------------------------------------------------

#ifndef _QOS_WRED_H
#define _QOS_WRED_H

//! \defgroup QOS_WRED Register File QOS_WRED - QoS WRED Registers
//! @{

//! Base Address of QOS_WRED
#define QOS_WRED_MODULE_BASE 0x18820000u

//! \defgroup QOS_WRED_QOS_WRED_PRIORITIZE_POP Register QOS_WRED_QOS_WRED_PRIORITIZE_POP - qos_wred_prioritize_pop
//! @{

//! Register Offset (relative)
#define QOS_WRED_QOS_WRED_PRIORITIZE_POP 0x0
//! Register Offset (absolute) for 1st Instance QOS_WRED
#define QOS_WRED_QOS_WRED_QOS_WRED_PRIORITIZE_POP 0x18820000u

//! Register Reset Value
#define QOS_WRED_QOS_WRED_PRIORITIZE_POP_RST 0x00000000u

//! Field PRIORITIZE_POP - prioritize_pop
#define QOS_WRED_QOS_WRED_PRIORITIZE_POP_PRIORITIZE_POP_POS 0
//! Field PRIORITIZE_POP - prioritize_pop
#define QOS_WRED_QOS_WRED_PRIORITIZE_POP_PRIORITIZE_POP_MASK 0x1u

//! @}

//! \defgroup QOS_WRED_QOS_WRED_TOTAL_RESOURCES_INIT_VAL Register QOS_WRED_QOS_WRED_TOTAL_RESOURCES_INIT_VAL - qos_wred_total_resources_init_val
//! @{

//! Register Offset (relative)
#define QOS_WRED_QOS_WRED_TOTAL_RESOURCES_INIT_VAL 0x4
//! Register Offset (absolute) for 1st Instance QOS_WRED
#define QOS_WRED_QOS_WRED_QOS_WRED_TOTAL_RESOURCES_INIT_VAL 0x18820004u

//! Register Reset Value
#define QOS_WRED_QOS_WRED_TOTAL_RESOURCES_INIT_VAL_RST 0x00000000u

//! Field TOTAL_RESOURCES_INIT_VAL - total_resources_init_val
#define QOS_WRED_QOS_WRED_TOTAL_RESOURCES_INIT_VAL_TOTAL_RESOURCES_INIT_VAL_POS 0
//! Field TOTAL_RESOURCES_INIT_VAL - total_resources_init_val
#define QOS_WRED_QOS_WRED_TOTAL_RESOURCES_INIT_VAL_TOTAL_RESOURCES_INIT_VAL_MASK 0xFFFFFFFFu

//! @}

//! \defgroup QOS_WRED_QOS_WRED_AVG_Q_WEIGHT Register QOS_WRED_QOS_WRED_AVG_Q_WEIGHT - qos_wred_avg_q_weight
//! @{

//! Register Offset (relative)
#define QOS_WRED_QOS_WRED_AVG_Q_WEIGHT 0x8
//! Register Offset (absolute) for 1st Instance QOS_WRED
#define QOS_WRED_QOS_WRED_QOS_WRED_AVG_Q_WEIGHT 0x18820008u

//! Register Reset Value
#define QOS_WRED_QOS_WRED_AVG_Q_WEIGHT_RST 0x00000000u

//! Field WEIGHT - weight
#define QOS_WRED_QOS_WRED_AVG_Q_WEIGHT_WEIGHT_POS 0
//! Field WEIGHT - weight
#define QOS_WRED_QOS_WRED_AVG_Q_WEIGHT_WEIGHT_MASK 0x3FFu

//! @}

//! \defgroup QOS_WRED_QOS_WRED_SHIFT_SIZE_COUNTER Register QOS_WRED_QOS_WRED_SHIFT_SIZE_COUNTER - qos_wred_shift_size_counter
//! @{

//! Register Offset (relative)
#define QOS_WRED_QOS_WRED_SHIFT_SIZE_COUNTER 0xC
//! Register Offset (absolute) for 1st Instance QOS_WRED
#define QOS_WRED_QOS_WRED_QOS_WRED_SHIFT_SIZE_COUNTER 0x1882000Cu

//! Register Reset Value
#define QOS_WRED_QOS_WRED_SHIFT_SIZE_COUNTER_RST 0x00000000u

//! Field SHIFT_SIZE_COUNTER - shift_size_counter
#define QOS_WRED_QOS_WRED_SHIFT_SIZE_COUNTER_SHIFT_SIZE_COUNTER_POS 0
//! Field SHIFT_SIZE_COUNTER - shift_size_counter
#define QOS_WRED_QOS_WRED_SHIFT_SIZE_COUNTER_SHIFT_SIZE_COUNTER_MASK 0x7u

//! @}

//! \defgroup QOS_WRED_QOS_WRED_SIGMA_UNUSED_FORCE_VAL Register QOS_WRED_QOS_WRED_SIGMA_UNUSED_FORCE_VAL - qos_wred_sigma_unused_force_val
//! @{

//! Register Offset (relative)
#define QOS_WRED_QOS_WRED_SIGMA_UNUSED_FORCE_VAL 0x10
//! Register Offset (absolute) for 1st Instance QOS_WRED
#define QOS_WRED_QOS_WRED_QOS_WRED_SIGMA_UNUSED_FORCE_VAL 0x18820010u

//! Register Reset Value
#define QOS_WRED_QOS_WRED_SIGMA_UNUSED_FORCE_VAL_RST 0x00000000u

//! Field SIGMA_UNUSED_FORCE_VAL - sigma_unused_force_val
#define QOS_WRED_QOS_WRED_SIGMA_UNUSED_FORCE_VAL_SIGMA_UNUSED_FORCE_VAL_POS 0
//! Field SIGMA_UNUSED_FORCE_VAL - sigma_unused_force_val
#define QOS_WRED_QOS_WRED_SIGMA_UNUSED_FORCE_VAL_SIGMA_UNUSED_FORCE_VAL_MASK 0xFFFFFFFFu

//! @}

//! \defgroup QOS_WRED_QOS_WRED_Q_ID_MAX_OCCUPANCY Register QOS_WRED_QOS_WRED_Q_ID_MAX_OCCUPANCY - qos_wred_q_id_max_occupancy
//! @{

//! Register Offset (relative)
#define QOS_WRED_QOS_WRED_Q_ID_MAX_OCCUPANCY 0x14
//! Register Offset (absolute) for 1st Instance QOS_WRED
#define QOS_WRED_QOS_WRED_QOS_WRED_Q_ID_MAX_OCCUPANCY 0x18820014u

//! Register Reset Value
#define QOS_WRED_QOS_WRED_Q_ID_MAX_OCCUPANCY_RST 0x00000000u

//! Field Q_ID - q_id
#define QOS_WRED_QOS_WRED_Q_ID_MAX_OCCUPANCY_Q_ID_POS 0
//! Field Q_ID - q_id
#define QOS_WRED_QOS_WRED_Q_ID_MAX_OCCUPANCY_Q_ID_MASK 0xFFFFFFFFu

//! @}

//! \defgroup QOS_WRED_QOS_WRED_MAX_OCCUPANCY_R Register QOS_WRED_QOS_WRED_MAX_OCCUPANCY_R - qos_wred_max_occupancy_r
//! @{

//! Register Offset (relative)
#define QOS_WRED_QOS_WRED_MAX_OCCUPANCY_R 0x34
//! Register Offset (absolute) for 1st Instance QOS_WRED
#define QOS_WRED_QOS_WRED_QOS_WRED_MAX_OCCUPANCY_R 0x18820034u

//! Register Reset Value
#define QOS_WRED_QOS_WRED_MAX_OCCUPANCY_R_RST 0x00000000u

//! Field MAX_OCCUPANCY_R - max_occupancy_r
#define QOS_WRED_QOS_WRED_MAX_OCCUPANCY_R_MAX_OCCUPANCY_R_POS 0
//! Field MAX_OCCUPANCY_R - max_occupancy_r
#define QOS_WRED_QOS_WRED_MAX_OCCUPANCY_R_MAX_OCCUPANCY_R_MASK 0xFFFFFFFFu

//! @}

//! \defgroup QOS_WRED_QOS_WRED_MAX_OCCUPANCY_RC Register QOS_WRED_QOS_WRED_MAX_OCCUPANCY_RC - qos_wred_max_occupancy_rc
//! @{

//! Register Offset (relative)
#define QOS_WRED_QOS_WRED_MAX_OCCUPANCY_RC 0x54
//! Register Offset (absolute) for 1st Instance QOS_WRED
#define QOS_WRED_QOS_WRED_QOS_WRED_MAX_OCCUPANCY_RC 0x18820054u

//! Register Reset Value
#define QOS_WRED_QOS_WRED_MAX_OCCUPANCY_RC_RST 0x00000000u

//! Field MAX_OCCUPANCY_RC - max_occupancy_rc
#define QOS_WRED_QOS_WRED_MAX_OCCUPANCY_RC_MAX_OCCUPANCY_RC_POS 0
//! Field MAX_OCCUPANCY_RC - max_occupancy_rc
#define QOS_WRED_QOS_WRED_MAX_OCCUPANCY_RC_MAX_OCCUPANCY_RC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup QOS_WRED_QOS_WRED_INACTIVE_Q_ACCESS_COUNT Register QOS_WRED_QOS_WRED_INACTIVE_Q_ACCESS_COUNT - qos_wred_inactive_q_access_count
//! @{

//! Register Offset (relative)
#define QOS_WRED_QOS_WRED_INACTIVE_Q_ACCESS_COUNT 0x74
//! Register Offset (absolute) for 1st Instance QOS_WRED
#define QOS_WRED_QOS_WRED_QOS_WRED_INACTIVE_Q_ACCESS_COUNT 0x18820074u

//! Register Reset Value
#define QOS_WRED_QOS_WRED_INACTIVE_Q_ACCESS_COUNT_RST 0x00000000u

//! Field INACTIVE_Q_ACCESS_COUNT - inactive_q_access_count
#define QOS_WRED_QOS_WRED_INACTIVE_Q_ACCESS_COUNT_INACTIVE_Q_ACCESS_COUNT_POS 0
//! Field INACTIVE_Q_ACCESS_COUNT - inactive_q_access_count
#define QOS_WRED_QOS_WRED_INACTIVE_Q_ACCESS_COUNT_INACTIVE_Q_ACCESS_COUNT_MASK 0xFFFFFFFFu

//! @}

//! \defgroup QOS_WRED_QOS_WRED_POP_UNDERFLOW_COUNT Register QOS_WRED_QOS_WRED_POP_UNDERFLOW_COUNT - qos_wred_pop_underflow_count
//! @{

//! Register Offset (relative)
#define QOS_WRED_QOS_WRED_POP_UNDERFLOW_COUNT 0x78
//! Register Offset (absolute) for 1st Instance QOS_WRED
#define QOS_WRED_QOS_WRED_QOS_WRED_POP_UNDERFLOW_COUNT 0x18820078u

//! Register Reset Value
#define QOS_WRED_QOS_WRED_POP_UNDERFLOW_COUNT_RST 0x00000000u

//! Field POP_UNDERFLOW_COUNT - pop_underflow_count
#define QOS_WRED_QOS_WRED_POP_UNDERFLOW_COUNT_POP_UNDERFLOW_COUNT_POS 0
//! Field POP_UNDERFLOW_COUNT - pop_underflow_count
#define QOS_WRED_QOS_WRED_POP_UNDERFLOW_COUNT_POP_UNDERFLOW_COUNT_MASK 0xFFFFFFFFu

//! @}

//! \defgroup QOS_WRED_QOS_WRED_Q_CONFIGS_UPDATE Register QOS_WRED_QOS_WRED_Q_CONFIGS_UPDATE - qos_wred_q_configs_update
//! @{

//! Register Offset (relative)
#define QOS_WRED_QOS_WRED_Q_CONFIGS_UPDATE 0x7C
//! Register Offset (absolute) for 1st Instance QOS_WRED
#define QOS_WRED_QOS_WRED_QOS_WRED_Q_CONFIGS_UPDATE 0x1882007Cu

//! Register Reset Value
#define QOS_WRED_QOS_WRED_Q_CONFIGS_UPDATE_RST 0x00000000u

//! Field UPDATE_ACTIVE_Q - update_active_q
#define QOS_WRED_QOS_WRED_Q_CONFIGS_UPDATE_UPDATE_ACTIVE_Q_POS 0
//! Field UPDATE_ACTIVE_Q - update_active_q
#define QOS_WRED_QOS_WRED_Q_CONFIGS_UPDATE_UPDATE_ACTIVE_Q_MASK 0x1u

//! Field UPDATE_DISABLE - update_disable
#define QOS_WRED_QOS_WRED_Q_CONFIGS_UPDATE_UPDATE_DISABLE_POS 1
//! Field UPDATE_DISABLE - update_disable
#define QOS_WRED_QOS_WRED_Q_CONFIGS_UPDATE_UPDATE_DISABLE_MASK 0x2u

//! Field UPDATE_FIXED_GREEN_DROP_P - update_fixed_green_drop_p
#define QOS_WRED_QOS_WRED_Q_CONFIGS_UPDATE_UPDATE_FIXED_GREEN_DROP_P_POS 2
//! Field UPDATE_FIXED_GREEN_DROP_P - update_fixed_green_drop_p
#define QOS_WRED_QOS_WRED_Q_CONFIGS_UPDATE_UPDATE_FIXED_GREEN_DROP_P_MASK 0x4u

//! Field UPDATE_FIXED_YELLOW_DROP_P - update_fixed_yellow_drop_p
#define QOS_WRED_QOS_WRED_Q_CONFIGS_UPDATE_UPDATE_FIXED_YELLOW_DROP_P_POS 3
//! Field UPDATE_FIXED_YELLOW_DROP_P - update_fixed_yellow_drop_p
#define QOS_WRED_QOS_WRED_Q_CONFIGS_UPDATE_UPDATE_FIXED_YELLOW_DROP_P_MASK 0x8u

//! Field UPDATE_MIN_AVG_YELLOW - update_min_avg_yellow
#define QOS_WRED_QOS_WRED_Q_CONFIGS_UPDATE_UPDATE_MIN_AVG_YELLOW_POS 4
//! Field UPDATE_MIN_AVG_YELLOW - update_min_avg_yellow
#define QOS_WRED_QOS_WRED_Q_CONFIGS_UPDATE_UPDATE_MIN_AVG_YELLOW_MASK 0x10u

//! Field UPDATE_MAX_AVG_YELLOW - update_max_avg_yellow
#define QOS_WRED_QOS_WRED_Q_CONFIGS_UPDATE_UPDATE_MAX_AVG_YELLOW_POS 5
//! Field UPDATE_MAX_AVG_YELLOW - update_max_avg_yellow
#define QOS_WRED_QOS_WRED_Q_CONFIGS_UPDATE_UPDATE_MAX_AVG_YELLOW_MASK 0x20u

//! Field UPDATE_SLOPE_YELLOW - update_slope_yellow
#define QOS_WRED_QOS_WRED_Q_CONFIGS_UPDATE_UPDATE_SLOPE_YELLOW_POS 6
//! Field UPDATE_SLOPE_YELLOW - update_slope_yellow
#define QOS_WRED_QOS_WRED_Q_CONFIGS_UPDATE_UPDATE_SLOPE_YELLOW_MASK 0x40u

//! Field UPDATE_SHIFT_AVG_YELLOW - update_shift_avg_yellow
#define QOS_WRED_QOS_WRED_Q_CONFIGS_UPDATE_UPDATE_SHIFT_AVG_YELLOW_POS 7
//! Field UPDATE_SHIFT_AVG_YELLOW - update_shift_avg_yellow
#define QOS_WRED_QOS_WRED_Q_CONFIGS_UPDATE_UPDATE_SHIFT_AVG_YELLOW_MASK 0x80u

//! Field UPDATE_MIN_AVG_GREEN - update_min_avg_green
#define QOS_WRED_QOS_WRED_Q_CONFIGS_UPDATE_UPDATE_MIN_AVG_GREEN_POS 8
//! Field UPDATE_MIN_AVG_GREEN - update_min_avg_green
#define QOS_WRED_QOS_WRED_Q_CONFIGS_UPDATE_UPDATE_MIN_AVG_GREEN_MASK 0x100u

//! Field UPDATE_MAX_AVG_GREEN - update_max_avg_green
#define QOS_WRED_QOS_WRED_Q_CONFIGS_UPDATE_UPDATE_MAX_AVG_GREEN_POS 9
//! Field UPDATE_MAX_AVG_GREEN - update_max_avg_green
#define QOS_WRED_QOS_WRED_Q_CONFIGS_UPDATE_UPDATE_MAX_AVG_GREEN_MASK 0x200u

//! Field UPDATE_SLOPE_GREEN - update_slope_green
#define QOS_WRED_QOS_WRED_Q_CONFIGS_UPDATE_UPDATE_SLOPE_GREEN_POS 10
//! Field UPDATE_SLOPE_GREEN - update_slope_green
#define QOS_WRED_QOS_WRED_Q_CONFIGS_UPDATE_UPDATE_SLOPE_GREEN_MASK 0x400u

//! Field UPDATE_SHIFT_AVG_GREEN - update_shift_avg_green
#define QOS_WRED_QOS_WRED_Q_CONFIGS_UPDATE_UPDATE_SHIFT_AVG_GREEN_POS 11
//! Field UPDATE_SHIFT_AVG_GREEN - update_shift_avg_green
#define QOS_WRED_QOS_WRED_Q_CONFIGS_UPDATE_UPDATE_SHIFT_AVG_GREEN_MASK 0x800u

//! Field UPDATE_MIN_GUARANTEED - update_min_guaranteed
#define QOS_WRED_QOS_WRED_Q_CONFIGS_UPDATE_UPDATE_MIN_GUARANTEED_POS 12
//! Field UPDATE_MIN_GUARANTEED - update_min_guaranteed
#define QOS_WRED_QOS_WRED_Q_CONFIGS_UPDATE_UPDATE_MIN_GUARANTEED_MASK 0x1000u

//! Field UPDATE_MAX_ALLOWED - update_max_allowed
#define QOS_WRED_QOS_WRED_Q_CONFIGS_UPDATE_UPDATE_MAX_ALLOWED_POS 13
//! Field UPDATE_MAX_ALLOWED - update_max_allowed
#define QOS_WRED_QOS_WRED_Q_CONFIGS_UPDATE_UPDATE_MAX_ALLOWED_MASK 0x2000u

//! @}

//! \defgroup QOS_WRED_QOS_WRED_ACTIVE_Q Register QOS_WRED_QOS_WRED_ACTIVE_Q - qos_wred_active_q
//! @{

//! Register Offset (relative)
#define QOS_WRED_QOS_WRED_ACTIVE_Q 0x80
//! Register Offset (absolute) for 1st Instance QOS_WRED
#define QOS_WRED_QOS_WRED_QOS_WRED_ACTIVE_Q 0x18820080u

//! Register Reset Value
#define QOS_WRED_QOS_WRED_ACTIVE_Q_RST 0x00000000u

//! Field ACTIVE_Q - active_q
#define QOS_WRED_QOS_WRED_ACTIVE_Q_ACTIVE_Q_POS 0
//! Field ACTIVE_Q - active_q
#define QOS_WRED_QOS_WRED_ACTIVE_Q_ACTIVE_Q_MASK 0x1u

//! @}

//! \defgroup QOS_WRED_QOS_WRED_DISABLE Register QOS_WRED_QOS_WRED_DISABLE - qos_wred_disable
//! @{

//! Register Offset (relative)
#define QOS_WRED_QOS_WRED_DISABLE 0x84
//! Register Offset (absolute) for 1st Instance QOS_WRED
#define QOS_WRED_QOS_WRED_QOS_WRED_DISABLE 0x18820084u

//! Register Reset Value
#define QOS_WRED_QOS_WRED_DISABLE_RST 0x00000000u

//! Field DISABLE - disable
#define QOS_WRED_QOS_WRED_DISABLE_DISABLE_POS 0
//! Field DISABLE - disable
#define QOS_WRED_QOS_WRED_DISABLE_DISABLE_MASK 0xFu

//! @}

//! \defgroup QOS_WRED_QOS_WRED_FIXED_GREEN_DROP_P Register QOS_WRED_QOS_WRED_FIXED_GREEN_DROP_P - qos_wred_fixed_green_drop_p
//! @{

//! Register Offset (relative)
#define QOS_WRED_QOS_WRED_FIXED_GREEN_DROP_P 0x88
//! Register Offset (absolute) for 1st Instance QOS_WRED
#define QOS_WRED_QOS_WRED_QOS_WRED_FIXED_GREEN_DROP_P 0x18820088u

//! Register Reset Value
#define QOS_WRED_QOS_WRED_FIXED_GREEN_DROP_P_RST 0x00000000u

//! Field FIXED_GREEN_DROP_P - fixed_green_drop_p
#define QOS_WRED_QOS_WRED_FIXED_GREEN_DROP_P_FIXED_GREEN_DROP_P_POS 0
//! Field FIXED_GREEN_DROP_P - fixed_green_drop_p
#define QOS_WRED_QOS_WRED_FIXED_GREEN_DROP_P_FIXED_GREEN_DROP_P_MASK 0xFFFFu

//! @}

//! \defgroup QOS_WRED_QOS_WRED_FIXED_YELLOW_DROP_P Register QOS_WRED_QOS_WRED_FIXED_YELLOW_DROP_P - qos_wred_fixed_yellow_drop_p
//! @{

//! Register Offset (relative)
#define QOS_WRED_QOS_WRED_FIXED_YELLOW_DROP_P 0x8C
//! Register Offset (absolute) for 1st Instance QOS_WRED
#define QOS_WRED_QOS_WRED_QOS_WRED_FIXED_YELLOW_DROP_P 0x1882008Cu

//! Register Reset Value
#define QOS_WRED_QOS_WRED_FIXED_YELLOW_DROP_P_RST 0x00000000u

//! Field FIXED_YELLOW_DROP_P - fixed_yellow_drop_p
#define QOS_WRED_QOS_WRED_FIXED_YELLOW_DROP_P_FIXED_YELLOW_DROP_P_POS 0
//! Field FIXED_YELLOW_DROP_P - fixed_yellow_drop_p
#define QOS_WRED_QOS_WRED_FIXED_YELLOW_DROP_P_FIXED_YELLOW_DROP_P_MASK 0xFFFFu

//! @}

//! \defgroup QOS_WRED_QOS_WRED_MIN_AVG_YELLOW Register QOS_WRED_QOS_WRED_MIN_AVG_YELLOW - qos_wred_min_avg_yellow
//! @{

//! Register Offset (relative)
#define QOS_WRED_QOS_WRED_MIN_AVG_YELLOW 0x90
//! Register Offset (absolute) for 1st Instance QOS_WRED
#define QOS_WRED_QOS_WRED_QOS_WRED_MIN_AVG_YELLOW 0x18820090u

//! Register Reset Value
#define QOS_WRED_QOS_WRED_MIN_AVG_YELLOW_RST 0x00000000u

//! Field MIN_AVG_YELLOW - min_avg_yellow
#define QOS_WRED_QOS_WRED_MIN_AVG_YELLOW_MIN_AVG_YELLOW_POS 0
//! Field MIN_AVG_YELLOW - min_avg_yellow
#define QOS_WRED_QOS_WRED_MIN_AVG_YELLOW_MIN_AVG_YELLOW_MASK 0xFFFFFu

//! @}

//! \defgroup QOS_WRED_QOS_WRED_MAX_AVG_YELLOW Register QOS_WRED_QOS_WRED_MAX_AVG_YELLOW - qos_wred_max_avg_yellow
//! @{

//! Register Offset (relative)
#define QOS_WRED_QOS_WRED_MAX_AVG_YELLOW 0x94
//! Register Offset (absolute) for 1st Instance QOS_WRED
#define QOS_WRED_QOS_WRED_QOS_WRED_MAX_AVG_YELLOW 0x18820094u

//! Register Reset Value
#define QOS_WRED_QOS_WRED_MAX_AVG_YELLOW_RST 0x00000000u

//! Field MAX_AVG_YELLOW - max_avg_yellow
#define QOS_WRED_QOS_WRED_MAX_AVG_YELLOW_MAX_AVG_YELLOW_POS 0
//! Field MAX_AVG_YELLOW - max_avg_yellow
#define QOS_WRED_QOS_WRED_MAX_AVG_YELLOW_MAX_AVG_YELLOW_MASK 0xFFFFFu

//! @}

//! \defgroup QOS_WRED_QOS_WRED_SLOPE_YELLOW Register QOS_WRED_QOS_WRED_SLOPE_YELLOW - qos_wred_slope_yellow
//! @{

//! Register Offset (relative)
#define QOS_WRED_QOS_WRED_SLOPE_YELLOW 0x98
//! Register Offset (absolute) for 1st Instance QOS_WRED
#define QOS_WRED_QOS_WRED_QOS_WRED_SLOPE_YELLOW 0x18820098u

//! Register Reset Value
#define QOS_WRED_QOS_WRED_SLOPE_YELLOW_RST 0x00000000u

//! Field SLOPE_YELLOW - slope_yellow
#define QOS_WRED_QOS_WRED_SLOPE_YELLOW_SLOPE_YELLOW_POS 0
//! Field SLOPE_YELLOW - slope_yellow
#define QOS_WRED_QOS_WRED_SLOPE_YELLOW_SLOPE_YELLOW_MASK 0x1Fu

//! @}

//! \defgroup QOS_WRED_QOS_WRED_SHIFT_AVG_YELLOW Register QOS_WRED_QOS_WRED_SHIFT_AVG_YELLOW - qos_wred_shift_avg_yellow
//! @{

//! Register Offset (relative)
#define QOS_WRED_QOS_WRED_SHIFT_AVG_YELLOW 0x9C
//! Register Offset (absolute) for 1st Instance QOS_WRED
#define QOS_WRED_QOS_WRED_QOS_WRED_SHIFT_AVG_YELLOW 0x1882009Cu

//! Register Reset Value
#define QOS_WRED_QOS_WRED_SHIFT_AVG_YELLOW_RST 0x00000000u

//! Field SHIFT_AVG_YELLOW - shift_avg_yellow
#define QOS_WRED_QOS_WRED_SHIFT_AVG_YELLOW_SHIFT_AVG_YELLOW_POS 0
//! Field SHIFT_AVG_YELLOW - shift_avg_yellow
#define QOS_WRED_QOS_WRED_SHIFT_AVG_YELLOW_SHIFT_AVG_YELLOW_MASK 0x1Fu

//! @}

//! \defgroup QOS_WRED_QOS_WRED_MIN_AVG_GREEN Register QOS_WRED_QOS_WRED_MIN_AVG_GREEN - qos_wred_min_avg_green
//! @{

//! Register Offset (relative)
#define QOS_WRED_QOS_WRED_MIN_AVG_GREEN 0xA0
//! Register Offset (absolute) for 1st Instance QOS_WRED
#define QOS_WRED_QOS_WRED_QOS_WRED_MIN_AVG_GREEN 0x188200A0u

//! Register Reset Value
#define QOS_WRED_QOS_WRED_MIN_AVG_GREEN_RST 0x00000000u

//! Field MIN_AVG_GREEN - min_avg_green
#define QOS_WRED_QOS_WRED_MIN_AVG_GREEN_MIN_AVG_GREEN_POS 0
//! Field MIN_AVG_GREEN - min_avg_green
#define QOS_WRED_QOS_WRED_MIN_AVG_GREEN_MIN_AVG_GREEN_MASK 0xFFFFFu

//! @}

//! \defgroup QOS_WRED_QOS_WRED_MAX_AVG_GREEN Register QOS_WRED_QOS_WRED_MAX_AVG_GREEN - qos_wred_max_avg_green
//! @{

//! Register Offset (relative)
#define QOS_WRED_QOS_WRED_MAX_AVG_GREEN 0xA4
//! Register Offset (absolute) for 1st Instance QOS_WRED
#define QOS_WRED_QOS_WRED_QOS_WRED_MAX_AVG_GREEN 0x188200A4u

//! Register Reset Value
#define QOS_WRED_QOS_WRED_MAX_AVG_GREEN_RST 0x00000000u

//! Field MAX_AVG_GREEN - max_avg_green
#define QOS_WRED_QOS_WRED_MAX_AVG_GREEN_MAX_AVG_GREEN_POS 0
//! Field MAX_AVG_GREEN - max_avg_green
#define QOS_WRED_QOS_WRED_MAX_AVG_GREEN_MAX_AVG_GREEN_MASK 0xFFFFFu

//! @}

//! \defgroup QOS_WRED_QOS_WRED_SLOPE_GREEN Register QOS_WRED_QOS_WRED_SLOPE_GREEN - qos_wred_slope_green
//! @{

//! Register Offset (relative)
#define QOS_WRED_QOS_WRED_SLOPE_GREEN 0xA8
//! Register Offset (absolute) for 1st Instance QOS_WRED
#define QOS_WRED_QOS_WRED_QOS_WRED_SLOPE_GREEN 0x188200A8u

//! Register Reset Value
#define QOS_WRED_QOS_WRED_SLOPE_GREEN_RST 0x00000000u

//! Field SLOPE_GREEN - slope_green
#define QOS_WRED_QOS_WRED_SLOPE_GREEN_SLOPE_GREEN_POS 0
//! Field SLOPE_GREEN - slope_green
#define QOS_WRED_QOS_WRED_SLOPE_GREEN_SLOPE_GREEN_MASK 0x1Fu

//! @}

//! \defgroup QOS_WRED_QOS_WRED_SHIFT_AVG_GREEN Register QOS_WRED_QOS_WRED_SHIFT_AVG_GREEN - qos_wred_shift_avg_green
//! @{

//! Register Offset (relative)
#define QOS_WRED_QOS_WRED_SHIFT_AVG_GREEN 0xAC
//! Register Offset (absolute) for 1st Instance QOS_WRED
#define QOS_WRED_QOS_WRED_QOS_WRED_SHIFT_AVG_GREEN 0x188200ACu

//! Register Reset Value
#define QOS_WRED_QOS_WRED_SHIFT_AVG_GREEN_RST 0x00000000u

//! Field SHIFT_AVG_GREEN - shift_avg_green
#define QOS_WRED_QOS_WRED_SHIFT_AVG_GREEN_SHIFT_AVG_GREEN_POS 0
//! Field SHIFT_AVG_GREEN - shift_avg_green
#define QOS_WRED_QOS_WRED_SHIFT_AVG_GREEN_SHIFT_AVG_GREEN_MASK 0x1Fu

//! @}

//! \defgroup QOS_WRED_QOS_WRED_MIN_GUARANTEED Register QOS_WRED_QOS_WRED_MIN_GUARANTEED - qos_wred_min_guaranteed
//! @{

//! Register Offset (relative)
#define QOS_WRED_QOS_WRED_MIN_GUARANTEED 0xB0
//! Register Offset (absolute) for 1st Instance QOS_WRED
#define QOS_WRED_QOS_WRED_QOS_WRED_MIN_GUARANTEED 0x188200B0u

//! Register Reset Value
#define QOS_WRED_QOS_WRED_MIN_GUARANTEED_RST 0x00000000u

//! Field MIN_GUARANTEED - min_guaranteed
#define QOS_WRED_QOS_WRED_MIN_GUARANTEED_MIN_GUARANTEED_POS 0
//! Field MIN_GUARANTEED - min_guaranteed
#define QOS_WRED_QOS_WRED_MIN_GUARANTEED_MIN_GUARANTEED_MASK 0xFFFFu

//! @}

//! \defgroup QOS_WRED_QOS_WRED_MAX_ALLOWED Register QOS_WRED_QOS_WRED_MAX_ALLOWED - qos_wred_max_allowed
//! @{

//! Register Offset (relative)
#define QOS_WRED_QOS_WRED_MAX_ALLOWED 0xB4
//! Register Offset (absolute) for 1st Instance QOS_WRED
#define QOS_WRED_QOS_WRED_QOS_WRED_MAX_ALLOWED 0x188200B4u

//! Register Reset Value
#define QOS_WRED_QOS_WRED_MAX_ALLOWED_RST 0x00000000u

//! Field MAX_ALLOWED - max_allowed
#define QOS_WRED_QOS_WRED_MAX_ALLOWED_MAX_ALLOWED_POS 0
//! Field MAX_ALLOWED - max_allowed
#define QOS_WRED_QOS_WRED_MAX_ALLOWED_MAX_ALLOWED_MASK 0xFFFFu

//! @}

//! \defgroup QOS_WRED_QOS_WRED_Q_COUNTERS_CLEAR Register QOS_WRED_QOS_WRED_Q_COUNTERS_CLEAR - qos_wred_q_counters_clear
//! @{

//! Register Offset (relative)
#define QOS_WRED_QOS_WRED_Q_COUNTERS_CLEAR 0xB8
//! Register Offset (absolute) for 1st Instance QOS_WRED
#define QOS_WRED_QOS_WRED_QOS_WRED_Q_COUNTERS_CLEAR 0x188200B8u

//! Register Reset Value
#define QOS_WRED_QOS_WRED_Q_COUNTERS_CLEAR_RST 0x00000000u

//! Field CLEAR_Q_SIZE_BYTES - clear_q_size_bytes
#define QOS_WRED_QOS_WRED_Q_COUNTERS_CLEAR_CLEAR_Q_SIZE_BYTES_POS 0
//! Field CLEAR_Q_SIZE_BYTES - clear_q_size_bytes
#define QOS_WRED_QOS_WRED_Q_COUNTERS_CLEAR_CLEAR_Q_SIZE_BYTES_MASK 0x1u

//! Field CLEAR_Q_AVG_SIZE_BYTES - clear_q_avg_size_bytes
#define QOS_WRED_QOS_WRED_Q_COUNTERS_CLEAR_CLEAR_Q_AVG_SIZE_BYTES_POS 1
//! Field CLEAR_Q_AVG_SIZE_BYTES - clear_q_avg_size_bytes
#define QOS_WRED_QOS_WRED_Q_COUNTERS_CLEAR_CLEAR_Q_AVG_SIZE_BYTES_MASK 0x2u

//! Field CLEAR_Q_SIZE_ENTRIES - clear_q_size_entries
#define QOS_WRED_QOS_WRED_Q_COUNTERS_CLEAR_CLEAR_Q_SIZE_ENTRIES_POS 2
//! Field CLEAR_Q_SIZE_ENTRIES - clear_q_size_entries
#define QOS_WRED_QOS_WRED_Q_COUNTERS_CLEAR_CLEAR_Q_SIZE_ENTRIES_MASK 0x4u

//! Field CLEAR_DROP_P_YELLOW - clear_drop_p_yellow
#define QOS_WRED_QOS_WRED_Q_COUNTERS_CLEAR_CLEAR_DROP_P_YELLOW_POS 3
//! Field CLEAR_DROP_P_YELLOW - clear_drop_p_yellow
#define QOS_WRED_QOS_WRED_Q_COUNTERS_CLEAR_CLEAR_DROP_P_YELLOW_MASK 0x8u

//! Field CLEAR_DROP_P_GREEN - clear_drop_p_green
#define QOS_WRED_QOS_WRED_Q_COUNTERS_CLEAR_CLEAR_DROP_P_GREEN_POS 4
//! Field CLEAR_DROP_P_GREEN - clear_drop_p_green
#define QOS_WRED_QOS_WRED_Q_COUNTERS_CLEAR_CLEAR_DROP_P_GREEN_MASK 0x10u

//! Field CLEAR_TOTAL_BYTES_ADDED - clear_total_bytes_added
#define QOS_WRED_QOS_WRED_Q_COUNTERS_CLEAR_CLEAR_TOTAL_BYTES_ADDED_POS 5
//! Field CLEAR_TOTAL_BYTES_ADDED - clear_total_bytes_added
#define QOS_WRED_QOS_WRED_Q_COUNTERS_CLEAR_CLEAR_TOTAL_BYTES_ADDED_MASK 0x20u

//! Field CLEAR_TOTAL_ACCEPTS - clear_total_accepts
#define QOS_WRED_QOS_WRED_Q_COUNTERS_CLEAR_CLEAR_TOTAL_ACCEPTS_POS 6
//! Field CLEAR_TOTAL_ACCEPTS - clear_total_accepts
#define QOS_WRED_QOS_WRED_Q_COUNTERS_CLEAR_CLEAR_TOTAL_ACCEPTS_MASK 0x40u

//! Field CLEAR_TOTAL_DROPS - clear_total_drops
#define QOS_WRED_QOS_WRED_Q_COUNTERS_CLEAR_CLEAR_TOTAL_DROPS_POS 7
//! Field CLEAR_TOTAL_DROPS - clear_total_drops
#define QOS_WRED_QOS_WRED_Q_COUNTERS_CLEAR_CLEAR_TOTAL_DROPS_MASK 0x80u

//! Field CLEAR_TOTAL_DROPPED_BYTES - clear_total_dropped_bytes
#define QOS_WRED_QOS_WRED_Q_COUNTERS_CLEAR_CLEAR_TOTAL_DROPPED_BYTES_POS 8
//! Field CLEAR_TOTAL_DROPPED_BYTES - clear_total_dropped_bytes
#define QOS_WRED_QOS_WRED_Q_COUNTERS_CLEAR_CLEAR_TOTAL_DROPPED_BYTES_MASK 0x100u

//! Field CLEAR_TOTAL_RED_DROPS - clear_total_red_drops
#define QOS_WRED_QOS_WRED_Q_COUNTERS_CLEAR_CLEAR_TOTAL_RED_DROPS_POS 9
//! Field CLEAR_TOTAL_RED_DROPS - clear_total_red_drops
#define QOS_WRED_QOS_WRED_Q_COUNTERS_CLEAR_CLEAR_TOTAL_RED_DROPS_MASK 0x200u

//! @}

//! \defgroup QOS_WRED_QOS_WRED_Q_ADDR Register QOS_WRED_QOS_WRED_Q_ADDR - qos_wred_q_addr
//! @{

//! Register Offset (relative)
#define QOS_WRED_QOS_WRED_Q_ADDR 0xBC
//! Register Offset (absolute) for 1st Instance QOS_WRED
#define QOS_WRED_QOS_WRED_QOS_WRED_Q_ADDR 0x188200BCu

//! Register Reset Value
#define QOS_WRED_QOS_WRED_Q_ADDR_RST 0x00000000u

//! Field Q_ADDR - q_addr
#define QOS_WRED_QOS_WRED_Q_ADDR_Q_ADDR_POS 0
//! Field Q_ADDR - q_addr
#define QOS_WRED_QOS_WRED_Q_ADDR_Q_ADDR_MASK 0xFFFFFFFFu

//! @}

//! \defgroup QOS_WRED_QOS_WRED_MEM_CMD Register QOS_WRED_QOS_WRED_MEM_CMD - qos_wred_mem_cmd
//! @{

//! Register Offset (relative)
#define QOS_WRED_QOS_WRED_MEM_CMD 0xC0
//! Register Offset (absolute) for 1st Instance QOS_WRED
#define QOS_WRED_QOS_WRED_QOS_WRED_MEM_CMD 0x188200C0u

//! Register Reset Value
#define QOS_WRED_QOS_WRED_MEM_CMD_RST 0x00000000u

//! Field MEM_CMD - mem_cmd
#define QOS_WRED_QOS_WRED_MEM_CMD_MEM_CMD_POS 0
//! Field MEM_CMD - mem_cmd
#define QOS_WRED_QOS_WRED_MEM_CMD_MEM_CMD_MASK 0x7u

//! @}

//! \defgroup QOS_WRED_QOS_WRED_MEM_ACCESS_BUSY Register QOS_WRED_QOS_WRED_MEM_ACCESS_BUSY - qos_wred_mem_access_busy
//! @{

//! Register Offset (relative)
#define QOS_WRED_QOS_WRED_MEM_ACCESS_BUSY 0xC4
//! Register Offset (absolute) for 1st Instance QOS_WRED
#define QOS_WRED_QOS_WRED_QOS_WRED_MEM_ACCESS_BUSY 0x188200C4u

//! Register Reset Value
#define QOS_WRED_QOS_WRED_MEM_ACCESS_BUSY_RST 0x00000000u

//! Field MEM_ACCESS_BUSY - mem_access_busy
#define QOS_WRED_QOS_WRED_MEM_ACCESS_BUSY_MEM_ACCESS_BUSY_POS 0
//! Field MEM_ACCESS_BUSY - mem_access_busy
#define QOS_WRED_QOS_WRED_MEM_ACCESS_BUSY_MEM_ACCESS_BUSY_MASK 0x1u

//! @}

//! \defgroup QOS_WRED_QOS_WRED_Q_SIZE_BYTES Register QOS_WRED_QOS_WRED_Q_SIZE_BYTES - qos_wred_q_size_bytes
//! @{

//! Register Offset (relative)
#define QOS_WRED_QOS_WRED_Q_SIZE_BYTES 0xC8
//! Register Offset (absolute) for 1st Instance QOS_WRED
#define QOS_WRED_QOS_WRED_QOS_WRED_Q_SIZE_BYTES 0x188200C8u

//! Register Reset Value
#define QOS_WRED_QOS_WRED_Q_SIZE_BYTES_RST 0x00000000u

//! Field Q_SIZE_BYTES - q_size_bytes
#define QOS_WRED_QOS_WRED_Q_SIZE_BYTES_Q_SIZE_BYTES_POS 0
//! Field Q_SIZE_BYTES - q_size_bytes
#define QOS_WRED_QOS_WRED_Q_SIZE_BYTES_Q_SIZE_BYTES_MASK 0x1FFFFFFu

//! @}

//! \defgroup QOS_WRED_QOS_WRED_Q_AVG_SIZE_BYTES Register QOS_WRED_QOS_WRED_Q_AVG_SIZE_BYTES - qos_wred_q_avg_size_bytes
//! @{

//! Register Offset (relative)
#define QOS_WRED_QOS_WRED_Q_AVG_SIZE_BYTES 0xCC
//! Register Offset (absolute) for 1st Instance QOS_WRED
#define QOS_WRED_QOS_WRED_QOS_WRED_Q_AVG_SIZE_BYTES 0x188200CCu

//! Register Reset Value
#define QOS_WRED_QOS_WRED_Q_AVG_SIZE_BYTES_RST 0x00000000u

//! Field Q_AVG_SIZE_BYTES - q_avg_size_bytes
#define QOS_WRED_QOS_WRED_Q_AVG_SIZE_BYTES_Q_AVG_SIZE_BYTES_POS 0
//! Field Q_AVG_SIZE_BYTES - q_avg_size_bytes
#define QOS_WRED_QOS_WRED_Q_AVG_SIZE_BYTES_Q_AVG_SIZE_BYTES_MASK 0xFFFFFu

//! @}

//! \defgroup QOS_WRED_QOS_WRED_Q_SIZE_ENTRIES Register QOS_WRED_QOS_WRED_Q_SIZE_ENTRIES - qos_wred_q_size_entries
//! @{

//! Register Offset (relative)
#define QOS_WRED_QOS_WRED_Q_SIZE_ENTRIES 0xD0
//! Register Offset (absolute) for 1st Instance QOS_WRED
#define QOS_WRED_QOS_WRED_QOS_WRED_Q_SIZE_ENTRIES 0x188200D0u

//! Register Reset Value
#define QOS_WRED_QOS_WRED_Q_SIZE_ENTRIES_RST 0x00000000u

//! Field Q_SIZE_ENTRIES - q_size_entries
#define QOS_WRED_QOS_WRED_Q_SIZE_ENTRIES_Q_SIZE_ENTRIES_POS 0
//! Field Q_SIZE_ENTRIES - q_size_entries
#define QOS_WRED_QOS_WRED_Q_SIZE_ENTRIES_Q_SIZE_ENTRIES_MASK 0xFFFFu

//! @}

//! \defgroup QOS_WRED_QOS_WRED_DROP_P_YELLOW Register QOS_WRED_QOS_WRED_DROP_P_YELLOW - qos_wred_drop_p_yellow
//! @{

//! Register Offset (relative)
#define QOS_WRED_QOS_WRED_DROP_P_YELLOW 0xD4
//! Register Offset (absolute) for 1st Instance QOS_WRED
#define QOS_WRED_QOS_WRED_QOS_WRED_DROP_P_YELLOW 0x188200D4u

//! Register Reset Value
#define QOS_WRED_QOS_WRED_DROP_P_YELLOW_RST 0x00000000u

//! Field DROP_P_YELLOW - drop_p_yellow
#define QOS_WRED_QOS_WRED_DROP_P_YELLOW_DROP_P_YELLOW_POS 0
//! Field DROP_P_YELLOW - drop_p_yellow
#define QOS_WRED_QOS_WRED_DROP_P_YELLOW_DROP_P_YELLOW_MASK 0xFFFFu

//! @}

//! \defgroup QOS_WRED_QOS_WRED_DROP_P_GREEN Register QOS_WRED_QOS_WRED_DROP_P_GREEN - qos_wred_drop_p_green
//! @{

//! Register Offset (relative)
#define QOS_WRED_QOS_WRED_DROP_P_GREEN 0xD8
//! Register Offset (absolute) for 1st Instance QOS_WRED
#define QOS_WRED_QOS_WRED_QOS_WRED_DROP_P_GREEN 0x188200D8u

//! Register Reset Value
#define QOS_WRED_QOS_WRED_DROP_P_GREEN_RST 0x00000000u

//! Field DROP_P_GREEN - drop_p_green
#define QOS_WRED_QOS_WRED_DROP_P_GREEN_DROP_P_GREEN_POS 0
//! Field DROP_P_GREEN - drop_p_green
#define QOS_WRED_QOS_WRED_DROP_P_GREEN_DROP_P_GREEN_MASK 0xFFFFu

//! @}

//! \defgroup QOS_WRED_QOS_WRED_TOTAL_BYTES_ADDED_LO Register QOS_WRED_QOS_WRED_TOTAL_BYTES_ADDED_LO - qos_wred_total_bytes_added_lo
//! @{

//! Register Offset (relative)
#define QOS_WRED_QOS_WRED_TOTAL_BYTES_ADDED_LO 0xDC
//! Register Offset (absolute) for 1st Instance QOS_WRED
#define QOS_WRED_QOS_WRED_QOS_WRED_TOTAL_BYTES_ADDED_LO 0x188200DCu

//! Register Reset Value
#define QOS_WRED_QOS_WRED_TOTAL_BYTES_ADDED_LO_RST 0x00000000u

//! Field TOTAL_BYTES_ADDED_LO - total_bytes_added_lo
#define QOS_WRED_QOS_WRED_TOTAL_BYTES_ADDED_LO_TOTAL_BYTES_ADDED_LO_POS 0
//! Field TOTAL_BYTES_ADDED_LO - total_bytes_added_lo
#define QOS_WRED_QOS_WRED_TOTAL_BYTES_ADDED_LO_TOTAL_BYTES_ADDED_LO_MASK 0xFFFFFFFFu

//! @}

//! \defgroup QOS_WRED_QOS_WRED_TOTAL_BYTES_ADDED_HI Register QOS_WRED_QOS_WRED_TOTAL_BYTES_ADDED_HI - qos_wred_total_bytes_added_hi
//! @{

//! Register Offset (relative)
#define QOS_WRED_QOS_WRED_TOTAL_BYTES_ADDED_HI 0xE0
//! Register Offset (absolute) for 1st Instance QOS_WRED
#define QOS_WRED_QOS_WRED_QOS_WRED_TOTAL_BYTES_ADDED_HI 0x188200E0u

//! Register Reset Value
#define QOS_WRED_QOS_WRED_TOTAL_BYTES_ADDED_HI_RST 0x00000000u

//! Field TOTAL_BYTES_ADDED_HI - total_bytes_added_hi
#define QOS_WRED_QOS_WRED_TOTAL_BYTES_ADDED_HI_TOTAL_BYTES_ADDED_HI_POS 0
//! Field TOTAL_BYTES_ADDED_HI - total_bytes_added_hi
#define QOS_WRED_QOS_WRED_TOTAL_BYTES_ADDED_HI_TOTAL_BYTES_ADDED_HI_MASK 0xFFFFFFFFu

//! @}

//! \defgroup QOS_WRED_QOS_WRED_TOTAL_BYTES_DROPPED_LO Register QOS_WRED_QOS_WRED_TOTAL_BYTES_DROPPED_LO - qos_wred_total_bytes_dropped_lo
//! @{

//! Register Offset (relative)
#define QOS_WRED_QOS_WRED_TOTAL_BYTES_DROPPED_LO 0xE4
//! Register Offset (absolute) for 1st Instance QOS_WRED
#define QOS_WRED_QOS_WRED_QOS_WRED_TOTAL_BYTES_DROPPED_LO 0x188200E4u

//! Register Reset Value
#define QOS_WRED_QOS_WRED_TOTAL_BYTES_DROPPED_LO_RST 0x00000000u

//! Field TOTAL_BYTES_DROPPED_LO - total_bytes_dropped_lo
#define QOS_WRED_QOS_WRED_TOTAL_BYTES_DROPPED_LO_TOTAL_BYTES_DROPPED_LO_POS 0
//! Field TOTAL_BYTES_DROPPED_LO - total_bytes_dropped_lo
#define QOS_WRED_QOS_WRED_TOTAL_BYTES_DROPPED_LO_TOTAL_BYTES_DROPPED_LO_MASK 0xFFFFFFFFu

//! @}

//! \defgroup QOS_WRED_QOS_WRED_TOTAL_BYTES_DROPPED_HI Register QOS_WRED_QOS_WRED_TOTAL_BYTES_DROPPED_HI - qos_wred_total_bytes_dropped_hi
//! @{

//! Register Offset (relative)
#define QOS_WRED_QOS_WRED_TOTAL_BYTES_DROPPED_HI 0xE8
//! Register Offset (absolute) for 1st Instance QOS_WRED
#define QOS_WRED_QOS_WRED_QOS_WRED_TOTAL_BYTES_DROPPED_HI 0x188200E8u

//! Register Reset Value
#define QOS_WRED_QOS_WRED_TOTAL_BYTES_DROPPED_HI_RST 0x00000000u

//! Field TOTAL_BYTES_DROPPED_HI - total_bytes_dropped_hi
#define QOS_WRED_QOS_WRED_TOTAL_BYTES_DROPPED_HI_TOTAL_BYTES_DROPPED_HI_POS 0
//! Field TOTAL_BYTES_DROPPED_HI - total_bytes_dropped_hi
#define QOS_WRED_QOS_WRED_TOTAL_BYTES_DROPPED_HI_TOTAL_BYTES_DROPPED_HI_MASK 0xFFFFFFFFu

//! @}

//! \defgroup QOS_WRED_QOS_WRED_TOTAL_ACCEPTS Register QOS_WRED_QOS_WRED_TOTAL_ACCEPTS - qos_wred_total_accepts
//! @{

//! Register Offset (relative)
#define QOS_WRED_QOS_WRED_TOTAL_ACCEPTS 0xEC
//! Register Offset (absolute) for 1st Instance QOS_WRED
#define QOS_WRED_QOS_WRED_QOS_WRED_TOTAL_ACCEPTS 0x188200ECu

//! Register Reset Value
#define QOS_WRED_QOS_WRED_TOTAL_ACCEPTS_RST 0x00000000u

//! Field TOTAL_ACCEPTS - total_accepts
#define QOS_WRED_QOS_WRED_TOTAL_ACCEPTS_TOTAL_ACCEPTS_POS 0
//! Field TOTAL_ACCEPTS - total_accepts
#define QOS_WRED_QOS_WRED_TOTAL_ACCEPTS_TOTAL_ACCEPTS_MASK 0xFFFFFFFFu

//! @}

//! \defgroup QOS_WRED_QOS_WRED_TOTAL_DROPS Register QOS_WRED_QOS_WRED_TOTAL_DROPS - qos_wred_total_drops
//! @{

//! Register Offset (relative)
#define QOS_WRED_QOS_WRED_TOTAL_DROPS 0xF0
//! Register Offset (absolute) for 1st Instance QOS_WRED
#define QOS_WRED_QOS_WRED_QOS_WRED_TOTAL_DROPS 0x188200F0u

//! Register Reset Value
#define QOS_WRED_QOS_WRED_TOTAL_DROPS_RST 0x00000000u

//! Field TOTAL_DROPS - total_drops
#define QOS_WRED_QOS_WRED_TOTAL_DROPS_TOTAL_DROPS_POS 0
//! Field TOTAL_DROPS - total_drops
#define QOS_WRED_QOS_WRED_TOTAL_DROPS_TOTAL_DROPS_MASK 0xFFFFFFFFu

//! @}

//! \defgroup QOS_WRED_QOS_WRED_TOTAL_RED_DROPS Register QOS_WRED_QOS_WRED_TOTAL_RED_DROPS - qos_wred_total_red_drops
//! @{

//! Register Offset (relative)
#define QOS_WRED_QOS_WRED_TOTAL_RED_DROPS 0xF4
//! Register Offset (absolute) for 1st Instance QOS_WRED
#define QOS_WRED_QOS_WRED_QOS_WRED_TOTAL_RED_DROPS 0x188200F4u

//! Register Reset Value
#define QOS_WRED_QOS_WRED_TOTAL_RED_DROPS_RST 0x00000000u

//! Field TOTAL_RED_DROPS - total_red_drops
#define QOS_WRED_QOS_WRED_TOTAL_RED_DROPS_TOTAL_RED_DROPS_POS 0
//! Field TOTAL_RED_DROPS - total_red_drops
#define QOS_WRED_QOS_WRED_TOTAL_RED_DROPS_TOTAL_RED_DROPS_MASK 0xFFFFu

//! @}

//! \defgroup QOS_WRED_QOS_WRED_MEM_RD_VALID Register QOS_WRED_QOS_WRED_MEM_RD_VALID - qos_wred_mem_rd_valid
//! @{

//! Register Offset (relative)
#define QOS_WRED_QOS_WRED_MEM_RD_VALID 0xF8
//! Register Offset (absolute) for 1st Instance QOS_WRED
#define QOS_WRED_QOS_WRED_QOS_WRED_MEM_RD_VALID 0x188200F8u

//! Register Reset Value
#define QOS_WRED_QOS_WRED_MEM_RD_VALID_RST 0x00000000u

//! Field MEM_RD_VALID - mem_rd_valid
#define QOS_WRED_QOS_WRED_MEM_RD_VALID_MEM_RD_VALID_POS 0
//! Field MEM_RD_VALID - mem_rd_valid
#define QOS_WRED_QOS_WRED_MEM_RD_VALID_MEM_RD_VALID_MASK 0x1u

//! @}

//! @}

#endif
