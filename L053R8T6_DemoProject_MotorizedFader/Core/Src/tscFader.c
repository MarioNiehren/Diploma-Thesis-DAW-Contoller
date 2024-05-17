/*
 * tscFader.c
 *
 *  Created on: Sep 6, 2023
 *      Author: Mario Niehren
 */

#include "tscFader.h"

void tscFader_initMuxAddress(tscFader_TypeDef* tsc, uint8_t muxAddress)
{
  tsc->muxAddress = muxAddress;

}

void tscFader_initThreshold(tscFader_TypeDef* tsc, uint16_t value)
{
  tsc->threshold = value;
}

void tscFader_Discharge()
{
  HAL_TSC_IODischarge(&htsc, ENABLE);
  HAL_Delay(1);
}

void tscFader_start_IT()
{
  HAL_TSC_IODischarge(&htsc, DISABLE);


  /*TODO: in init packen */
  TSC_IOConfigTypeDef ioConfigTsc;
  ioConfigTsc.ChannelIOs = htsc.Init.ChannelIOs;
  ioConfigTsc.ShieldIOs = htsc.Init.ShieldIOs;
  ioConfigTsc.SamplingIOs = htsc.Init.SamplingIOs;


  HAL_TSC_IOConfig(&htsc, &ioConfigTsc);
  HAL_TSC_Start_IT(&htsc);
}

void tscFader_stop_IT()
{
  HAL_TSC_Stop_IT(&htsc);
}

uint32_t test;
void tscFader_updateState(tscFader_TypeDef* tsc)
{
  uint32_t tscValue = 0x00;
  tscValue = HAL_TSC_GroupGetValue(&htsc, 2);
  test = tscValue;
  if(tscValue >= tsc->threshold)
  {
    tsc->state = tsl_released;
  }
  else if(tscValue < tsc->threshold)
  {
    tsc->state = tsl_touched;
  }
}

tslFader_State_TypeDef tscFader_getState(tscFader_TypeDef* tsc)
{
  return tsc->state;
}

bool tscFader_checkIfInterrupted()
{
#if 0 /* redesign Interrupt management */
  bool returnValue = false;
  if(tsc_interruptConvCplt == true)
  {
    returnValue = true;
  }
  return returnValue;
#endif
}

void tscFader_resetInterrupt()
{
#if 0 /* redesign Interrupt management */
  tsc_interruptConvCplt = false;
#endif
}
