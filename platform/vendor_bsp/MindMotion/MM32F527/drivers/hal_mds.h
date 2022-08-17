/*
 * Copyright 2022 MindMotion Microelectronics Co., Ltd.
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef __HAL_MDS_H__
#define __HAL_MDS_H__

#include "hal_common.h"
#include "hal_mds_remap.h"
/*!
 * @addtogroup MDS
 * @{
 */

/*!
 * @brief MDS driver version number.
 */
#define MDS_DRIVER_VERSION 0u /*!< mds_0. */

/*!
 * @brief MDS TriggerInMode type.
 *
 * Use the Trigger in mode, selecting the edge detection mode for the trigger source.
 */
typedef enum
{
    MDS_TriggerInMode_RisingEdge = 0u, /*!< Select Rising edge detection. */
    MDS_TriggerInMode_FallingEdge = 1u, /*!< Select Falling edge detection. */
    MDS_TriggerInMode_BothEdges = 2u, /*!< Select Rising and Falling edge detection. */
}MDS_TriggerInMode_Type;

/*!
 * @brief MDS CluLogic type.
 *
 *  Select the operation logic of the Clu.
 */
typedef enum
{
    MDS_CluLogic_OR_OR   = 0u, /*!< The operation logic selects the OR-OR logic, and the four input logics are in0|in1|in2|in3. */
    MDS_CluLogic_AND_OR  = 1u, /*!< The operation logic selects the AND-OR logic, and the four input logics are (in0&in1)|(in2&in3). */
    MDS_CluLogic_OR_XOR  = 2u, /*!< The operation logic selects the OR-XOR logic, and the four input logics are (in0|in1)^(in2|in3). */
    MDS_CluLogic_AND_AND = 3u, /*!< The operation logic selects the OR-OR logic, and the four input logics are in0&in1&in2&in3. */
}MDS_CluLogic_Type;

/*!
 * @brief This type of structure instance is used to keep the settings
 *  when calling the @ref MDS_EnableTriggerClu() or @ref MDS_EnableConnClu() to configure using the Clu out as the trigger source.
 */

typedef struct
{
    uint32_t   CluInSource[MDS_CLU_IN_LEN]; /*!< Select the trigger and connection source for each of Clu in, TRIGGER_IN_SOURCE_LEN is the max input length. */
    uint32_t   CluInSourceInvertMask; /*!< Select trigger or connect the source after taking the inverse as input to the clu. */
    uint32_t   CluInSourceTriggerEdgeMask; /*!< Select the clu in source trigger edge detection mode. */
    MDS_CluLogic_Type CluLogic; /*!< Select the Clu operation logic. */
} MDS_CluConf_Type;

/*!
 * @brief Enable the trigger mode without clu output as connection source in.
 * the hal_mds_remap.h define the mds(mindswitch) in or out, the trigger in correspond to trigger out.
 *
 * @param MDSx MDS instance.
 * @param out the trigger source, see to hal_mds_remap.h.
 * @param in the trigger target, see to hal_mds_remap.h.
 * @param trigger_mode select the trigger in edge detection mode.
 * @return None.
 */
void       MDS_EnableTrigger(MDS_Type * MDSx, uint32_t out, uint32_t in, MDS_TriggerInMode_Type trigger_mode);

/*!
 * @brief Enable software trigger as the trigger source in.
 * when using the software trigger as the trigger source, enable it and it can only finish once trigger.
 *
 * @param MDSx MDS instance.
 * @param enable true is enable the software trigger as the trigger source, false is not enable.
 * @return None.
 */
void       MDS_DoSwTrigger(MDS_Type * MDSx);

/*!
 * @brief Enable the connection mode without clu output as connection source in.
 * the hal_mds_remap.h define the mds(mindswitch) in or out, the connection in correspond to conntion out.
 *
 * @param MDSx MDS instance.
 * @param out the connection target, see to hal_mds_remap.h.
 * @param in the input of connection, see to hal_mds_remap.h.
 * @return None.
 */
void       MDS_EnableConn(MDS_Type * MDSx, uint32_t out, uint32_t in);

/*!
 * @brief Enable the clu output as the trigger source in.
 * the hal_mds_remap.h define the mds(mindswitch) in or out, the connection in correspond to conntion out.
 *
 * @param MDSx MDS instance.
 * @param conf configure the clu parameter, @ref MDS_CluConf_Type.
 * @param out select the trigger out, see to hal_mds_remap.h.
 * @param in select which clu in, see to hal_mds_remap.h.
 * @return None.
 */
void       MDS_EnableTriggerClu(MDS_Type * MDSx, uint32_t out, uint32_t in, MDS_CluConf_Type * conf);

/*!
 * @brief Enable the clu output as the connection source in.
 * the hal_mds_remap.h define the mds(mindswitch) in or out, the connection in correspond to conntion out.
 *
 * @param MDSx MDS instance.
 * @param conf configure the clu parameter, @ref MDS_CluConf_Type.
 * @param out select the connection out, see to hal_mds_remap.h.
 * @param in select which clu in, see to hal_mds_remap.h.
 * @return None.
 */
void       MDS_EnableConnClu(MDS_Type * MDSx, uint32_t out, uint32_t in, MDS_CluConf_Type * conf);


/*!
 *@}
 */

#endif /*__HAL_MDS_H__. */
