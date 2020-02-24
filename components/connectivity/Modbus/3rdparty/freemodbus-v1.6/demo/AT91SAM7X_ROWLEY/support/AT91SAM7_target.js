/******************************************************************************
  Target Script for Atmel AT91SAM7.

  Copyright (c) 2007 Rowley Associates Limited.

  This file may be distributed under the terms of the License Agreement
  provided with this software.

  THIS FILE IS PROVIDED AS IS WITH NO WARRANTY OF ANY KIND, INCLUDING THE
  WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 ******************************************************************************/

function Reset()
{
  /* Mask All interrupt pAic->AIC_IDCR = 0xFFFFFFFF; */
  TargetInterface.pokeWord(0xffffffff,0xFFFFF124);
  TargetInterface.pokeWord(0xffffffff,0xFFFFF128);
  /* disable peripheral clock  Peripheral Clock Disable Register */
  TargetInterface.pokeWord(0xffffffff,0xFFFFFC14);

  /* Reset and stop target */
  TargetInterface.stopAndReset(1);

  CKGR_MOR_VAL = 0x00004001;
  CKGR_PLLR_VAL = 0x10483F0E; /* Set up the PLL (MCK, UDPCK = 18.432MHz * 73 / 14 / 2 = 48.05486MHz) */
  CKGR_MCKR_VAL1 = 0x00000004;
  CKGR_MCKR_VAL2 = 0x00000007;

  if (TargetInterface.setMaximumJTAGFrequency)
    {
      /* Configure Clock (post 1.6 version) */
      TargetInterface.setMaximumJTAGFrequency(32768);
      TargetInterface.pokeWord(0xFFFFFC20, CKGR_MOR_VAL);
      TargetInterface.pokeWord(0xFFFFFC2C, CKGR_PLLR_VAL);
      TargetInterface.pokeWord(0xFFFFFC30, CKGR_MCKR_VAL1);
      TargetInterface.pokeWord(0xFFFFFC30, CKGR_MCKR_VAL2);
      TargetInterface.setMaximumJTAGFrequency(10000000);
    }
  else
    {
      /* Configure Clock (1.6 version) */
      TargetInterface.pokeWord(0xFFFFFC20, CKGR_MOR_VAL);
      TargetInterface.delay(10);
      TargetInterface.pokeWord(0xFFFFFC2C, CKGR_PLLR_VAL);
      TargetInterface.delay(10);
      TargetInterface.pokeWord(0xFFFFFC30, CKGR_MCKR_VAL1);
      TargetInterface.delay(10);
      TargetInterface.pokeWord(0xFFFFFC30, CKGR_MCKR_VAL2);
      TargetInterface.delay(10);
    }
}

function RAMReset()
{
  Reset();
  /* Remap SRAM to 0x00000000 */
  TargetInterface.pokeWord(0xFFFFFF00, 1); // MC_RCR 
}

function FLASHReset()
{
  Reset();
}

