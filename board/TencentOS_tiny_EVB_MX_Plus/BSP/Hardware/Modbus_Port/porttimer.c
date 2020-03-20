/*
 * FreeModbus Libary: BARE Port
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

/* ----------------------- Platform includes --------------------------------*/
#include "mod_port.h"
#include "mcu_init.h"
/* ----------------------- Modbus includes ----------------------------------*/
#include "mb.h"
#include "mbport.h"

/* ----------------------- static functions ---------------------------------*/
void prvvTIMERExpiredISR( void );
extern TIM_HandleTypeDef htim6;
uint16_t timeout = 0;
uint16_t downcounter = 0;
/* ----------------------- Start implementation -----------------------------*/
BOOL
xMBPortTimersInit( USHORT usTim1Timerout50us )
{
  TIM_MasterConfigTypeDef sMasterConfig;
  
  htim6.Instance = TIM6;
  htim6.Init.Prescaler = (HAL_RCC_GetPCLK1Freq() / 1000000) - 1;
  htim6.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim6.Init.Period = 50 - 1;
  
  timeout = usTim1Timerout50us;
  
  if (HAL_TIM_Base_Init(&htim6) != HAL_OK)
  {
    return FALSE;
  }
  
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_UPDATE;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim6, &sMasterConfig) != HAL_OK)
  {
    return FALSE;
  }
  
  return TRUE;
}


inline void
vMBPortTimersEnable(  )
{
  /* Enable the timer with the timeout passed to xMBPortTimersInit( ) */
  downcounter = timeout;
  HAL_TIM_Base_Start_IT(&htim6);
	
	
}

inline void
vMBPortTimersDisable(  )
{
  /* Disable any pending timers. */
  HAL_TIM_Base_Stop_IT(&htim6);
}

/* Create an ISR which is called whenever the timer has expired. This function
 * must then call pxMBPortCBTimerExpired( ) to notify the protocol stack that
 * the timer has expired.
 */
//void prvvTIMERExpiredISR( void )
//{
//    ( void )pxMBPortCBTimerExpired(  );
//}

