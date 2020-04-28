 /*
  * FreeModbus Libary: A portable Modbus implementation for Modbus ASCII/RTU.
  * Copyright (C) 2006 Christian Walter <wolti@sil.at>
  *
  * This library is free software; you can redistribute it and/or
  * modify it under the terms of the GNU Lesser General Public
  * License as published by the Free Software Foundation; either
  * version 2.1 of the License, or (at your option) any later version.
  *
  * This library is distributed in the hope that it will be useful,
  * but WITHOUT ANY WARRANTY; without even the implied warranty of
  * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  * Lesser General Public License for more details.
  *
  * You should have received a copy of the GNU Lesser General Public
  * License along with this library; if not, write to the Free Software
  * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
  *
  * File: $Id$
  */

#ifndef _MB_CONFIG_H
#define _MB_CONFIG_H

#ifdef __cplusplus
PR_BEGIN_EXTERN_C
#endif
/* ----------------------- Defines ------------------------------------------*/
/*! \defgroup modbus_cfg Modbus Configuration
 *
 * Most modules in the protocol stack are completly optional and can be
 * excluded. This is specially important if target resources are very small
 * and program memory space should be saved.<br>
 *
 * All of these settings are available in the file <code>mbconfig.h</code>
 */
/*! \addtogroup modbus_cfg
 *  @{
 */
/*! \brief If Modbus ASCII support is enabled. */
#define MB_ASCII_ENABLED                        (  0 )
/*! \brief If Modbus RTU support is enabled. */
#define MB_RTU_ENABLED                          (  0 )
/*! \brief If Modbus TCP support is enabled. */
#define MB_TCP_ENABLED                          (  1 )
/*! \brief The character timeout value for Modbus ASCII.
 *
 * The character timeout value is not fixed for Modbus ASCII and is therefore
 * a configuration option. It should be set to the maximum expected delay
 * time of the network.
 */
#define MB_ASCII_TIMEOUT_SEC                    (  1 )
/*! \brief Maximum number of Modbus functions codes the protocol stack
 *    should support.
 *
 * The maximum number of supported Modbus functions must be greater than
 * the sum of all enabled functions in this file and custom function
 * handlers. If set to small adding more functions will fail.
 */
#define MB_FUNC_HANDLERS_MAX                    ( 16 )
/*! \brief Number of bytes which should be allocated for the <em>Report Slave ID
 *    </em>command.
 *
 * This number limits the maximum size of the additional segment in the
 * report slave id function. See eMBSetSlaveID(  ) for more information on
 * how to set this value. It is only used if MB_FUNC_OTHER_REP_SLAVEID_ENABLED
 * is set to <code>1</code>.
 */
#define MB_FUNC_OTHER_REP_SLAVEID_BUF           ( 32 )
/*! \brief If the <em>Report Slave ID</em> function should be enabled. */
#define MB_FUNC_OTHER_REP_SLAVEID_ENABLED       (  1 )
/*! \brief If the <em>Read Input Registers</em> function should be enabled. */
#define MB_FUNC_READ_INPUT_ENABLED              (  1 )
/*! \brief If the <em>Read Holding Registers</em> function should be enabled. */
#define MB_FUNC_READ_HOLDING_ENABLED            (  1 )
/*! \brief If the <em>Write Single Register</em> function should be enabled. */
#define MB_FUNC_WRITE_HOLDING_ENABLED           (  1 )
/*! \brief If the <em>Write Multiple registers</em> function should be enabled. */
#define MB_FUNC_WRITE_MULTIPLE_HOLDING_ENABLED  (  1 )
/*! \brief If the <em>Read Coils</em> function should be enabled. */
#define MB_FUNC_READ_COILS_ENABLED              (  1 )
/*! \brief If the <em>Write Coils</em> function should be enabled. */
#define MB_FUNC_WRITE_COIL_ENABLED              (  1 )
/*! \brief If the <em>Write Multiple Coils</em> function should be enabled. */
#define MB_FUNC_WRITE_MULTIPLE_COILS_ENABLED    (  1 )
/*! \brief If the <em>Read Discrete Inputs</em> function should be enabled. */
#define MB_FUNC_READ_DISCRETE_INPUTS_ENABLED    (  1 )
/*! @} */
#ifdef __cplusplus
    PR_END_EXTERN_C
#endif
#endif
