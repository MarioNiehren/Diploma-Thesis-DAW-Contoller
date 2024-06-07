/***************************************************************************//**
 * @defgroup        TSCButton_Source  Source
 * @brief           Study this part for details about this module.
 *
 * @addtogroup      TSCButton
 * @{
 *
 * @addtogroup      TSCButton_Source
 * @{
 *
 * @file            tscFader.c
 *
 * @date            Sep 6, 2023
 * @author          Mario Niehren
 */

#include <tscButton.h>

/**
 * @brief     Structure for internal use. All initialized buttons will be linked
 *            here.
 */
typedef struct
{
  TSCButton_structTd* InitializedButtons[MAX_TSC_BUTTONS]; /**< Array with
                                         pointer to all initialized buttons */
  uint8_t     CurrentButtonIndex;   /**< Active Index of the pointer array*/
  uint8_t     NumButtons;           /**< Number of initialized buttons */

  timer_setup_structTd  timer;      /**< timer used to give the sampling
                                         capacitor time to discharge */
  uint8_t     CapacitorDischargeTime; /**< Time the capacitors need to
                                         discharge */
  bool      interrupted;
  bool      TSCBlocked;
  bool      TSCStarted;
}TSCButton_internal_structTd;

/**
 * @brief     Object of TSCButton_internal_structTd type to save internal data.
 */
TSCButton_internal_structTd TSC_Internal = {
    .CurrentButtonIndex = 0,
    .NumButtons = 0,
    .interrupted = false,
    .TSCBlocked = false,
    .TSCStarted = false,
};

/***************************************************************************//**
 * @name      Initialize
 * @brief     Use these functions to initialize the touch sense controller
 *            based button.
 * @{
 ******************************************************************************/

/** @cond *//* Function Prototypes */
void link_ButtonToInternalStructure(TSCButton_structTd* tsc);
uint32_t extract_GroupNumberFromIOCCRMask(uint32_t Bitmask);
/** @endcond *//* Function Prototypes */

/* Description in .h */
void TSCButton_init_TSC(TSCButton_structTd* tsc, TSC_HandleTypeDef* htsc, uint32_t IOChannel)
{
  /** @internal     1.  link HAL generated tsc handler (htsc) */
  tsc->htsc = htsc;

  /** @internal     2.  Copy configuration settings from htsc to
   *                    TSC_IOConfigTypeDef ioConfigTsc. This is necessary
   *                    for the use of some HAL TSC functions. */
  tsc->ioConfigTsc.ChannelIOs = IOChannel;
  tsc->ioConfigTsc.ShieldIOs = tsc->htsc->Init.ShieldIOs;
  tsc->ioConfigTsc.SamplingIOs = tsc->htsc->Init.SamplingIOs;

  /** @internal     3.  Extract group number from IOChannel and save it */
  uint32_t GroupNumber = extract_GroupNumberFromIOCCRMask(IOChannel);
  tsc->TSCGroup = GroupNumber;
  /** @internal     4.  link button structure to the internal structure */
  link_ButtonToInternalStructure(tsc);
}

/**
 * @brief     Link the pointer to a TSCButton to the internal structure
 *            used for the acquisition management.
 * @param     tsc       pointer to the users tsc structure
 * @return    none
 */
void link_ButtonToInternalStructure(TSCButton_structTd* tsc)
{
  uint8_t Index = TSC_Internal.CurrentButtonIndex;

  TSC_Internal.InitializedButtons[Index] = tsc;
  TSC_Internal.CurrentButtonIndex++;
  TSC_Internal.NumButtons++;
}

/**
 * @brief     Extract the group number of a IO-Bitmask. This Bitmask has
 *            to match the TSC_IOCCR_Gx_IOx masks.
 * @param     Bitmask   to be extracted
 * @return    TSC-Channel Number that will be used for HAL_TSC_IOConfig()
 */
uint32_t extract_GroupNumberFromIOCCRMask(uint32_t Bitmask)
{
  /** @internal     1.  Declare Variables for internal use */
  uint32_t ReturnGroupNumber = 0;
  uint32_t GroupNumberCnt = 0;
  uint32_t IOCCR_GroupShift = 4;  /* Each Group has 4 Bits inside the
                                     TSC_IOCCR register */

  /** @internal     2.  Loop through all available groups. */
  for(GroupNumberCnt = 0; GroupNumberCnt < NUM_TSC_GROUPS; GroupNumberCnt++)
  {
    /** @internal     3.  Calculate a compare bitmask. */
    uint32_t IOCCR_Shift = GroupNumberCnt * IOCCR_GroupShift;
    uint32_t CompareMask = 0x0F << (IOCCR_Shift);

    /** @internal     4.  Use the compare bitmask to check it a bit of the
     *                    group is set.
     *                    If yes: Set the return value to the current group
     *                    number of the loop.  */
    if((Bitmask & CompareMask) != 0)
    {
      ReturnGroupNumber = GroupNumberCnt;
    }
  }

  return ReturnGroupNumber;
}

/* Description in .h */
void TSCButton_init_Threshold(TSCButton_structTd* tsc, uint16_t value)
{
  /** @internal     1.  save threshold value for the tsc to change state */
  tsc->threshold = value;
}

/* Description in .h */
void TSCButton_init_DischargeTimeMsAll(uint8_t value)
{
  /** @internal     1.  save the capacitor discharge time */
  TSC_Internal.CapacitorDischargeTime = value;
}

/** @} ************************************************************************/
/* end of name "Initialize"
 ******************************************************************************/


/***************************************************************************//**
 * @name      Process TSC
 * @{
 ******************************************************************************/

/* Description in .h */
void TSCButton_start_All(void)
{
  /** @internal     1.  set button current button index to 0 */
  TSC_Internal.CurrentButtonIndex = 0;
}

/** @cond *//* Function Prototypes */
void block_TSCForSomeMs(uint32_t BlockingTime);
bool check_TSCBlocked(void);
void store_TSCSample(TSCButton_structTd* tsc, uint32_t Sample);
void calculate_SmoothTSCValue(TSCButton_structTd* tsc);
/** @endcond *//* Function Prototypes */

/* Description in .h */
void TSCButton_update_All(void)
{
  uint8_t NumButtons = TSC_Internal.NumButtons;
  uint8_t CurrentButtonIndex = TSC_Internal.CurrentButtonIndex;
  TSCButton_structTd* ActiveTSCButton = TSC_Internal.InitializedButtons[CurrentButtonIndex];

  /** @internal     1.  Check if TSC is blocked.
   *                    - If yes: leave function*/

  if(check_TSCBlocked() == false)
  {
    /** @internal     2.  Check it TSC is already running
     *                    - If not: start TSC */
    if(TSC_Internal.TSCStarted == false)
    {
      HAL_TSC_IODischarge(ActiveTSCButton->htsc, DISABLE);


      HAL_TSC_IOConfig(ActiveTSCButton->htsc, &ActiveTSCButton->ioConfigTsc);
      HAL_TSC_Start_IT(ActiveTSCButton->htsc);
      TSC_Internal.TSCStarted = true;
    }

    /** @internal     3.  Check if TSC was interrupted.
     *                    - If not: leave function */
    if(TSC_Internal.interrupted == true)
    {
      TSC_Internal.interrupted = false;
      HAL_TSC_Stop_IT(ActiveTSCButton->htsc);
      TSC_Internal.TSCStarted = false;

      uint32_t  TSC_Value = 0;
      TSC_Value = HAL_TSC_GroupGetValue(ActiveTSCButton->htsc, ActiveTSCButton->TSCGroup);

      store_TSCSample(ActiveTSCButton, TSC_Value);
      calculate_SmoothTSCValue(ActiveTSCButton);

      uint32_t  TSC_Smooth = ActiveTSCButton->SmoothValue;

      if(TSC_Smooth >= ActiveTSCButton->threshold)
      {
        ActiveTSCButton->state = TSCBUTTON_RELEASED;
      }
      else if(TSC_Smooth < ActiveTSCButton->threshold)
      {
        ActiveTSCButton->state = TSCBUTTON_TOUCHED;
      }
      HAL_TSC_IODischarge(ActiveTSCButton->htsc, ENABLE);
      block_TSCForSomeMs(TSC_Internal.CapacitorDischargeTime);

      TSC_Internal.CurrentButtonIndex++;
      if(TSC_Internal.CurrentButtonIndex == NumButtons)
      {
        TSC_Internal.CurrentButtonIndex = 0;
      }
    }
  }
}

/**
 * @brief     Function to block the TSC for a specific time in ms. This will
 *            only block the TSC, not the whole program.
 * @param     BlockingTime in milli seconds
 * @return    none
 */
void block_TSCForSomeMs(uint32_t BlockingTime)
{
  timer_set_ThresholdMS(&TSC_Internal.timer, BlockingTime);
  TSC_Internal.TSCBlocked = true;
}

/**
 * @brief     Check if TSC is blocked
 * @param     none
 * @return    true if TSC is blocked, false if not
 */
bool check_TSCBlocked(void)
{
  bool ReturnValue = false;
  if(TSC_Internal.TSCBlocked == true)
  {
    if(timer_check_TimerElapsed(&TSC_Internal.timer) == true)
    {
      ReturnValue = false;
      TSC_Internal.TSCBlocked = false;
    }
    else
    {
      ReturnValue = true;
    }
  }
  return ReturnValue;
}

/**
 * @brief     Store the current TSC sample to the structure.
 * @param     tsc       pointer to the users tsc structure
 * @param     Sample    of the current TSC measurement
 * @return    none
 *
 */
void store_TSCSample(TSCButton_structTd* tsc, uint32_t Sample)
{
  /** @internal     1.  Copy Sample to Sample Array at current
   *                    Array-Index */
  tsc->Samples[tsc->SamplesIndex] = Sample;
  /** @internal     2.  Copy Sample to Value Raw to be able
   *                    to return it quick to the user if needed. */
  tsc->RawValue = Sample;
  /** @internal     3.  Count up the sample array index for the next value. */
  tsc->SamplesIndex++;
  /** @internal     4.  If sample array index reaches the defined
   *                     @ref TSC_NUM_SAMPLES, reset the index. The following
   *                     values will overwrite the old ones. */
  if(tsc->SamplesIndex == TSC_NUM_SAMPLES)
  {
    tsc->SamplesIndex = 0x00;
  }
}

/**
 * @brief     Calculate an average of all available samples
 * @param     tsc       pointer to the users tsc structure
 * @return    none
 */
void calculate_SmoothTSCValue(TSCButton_structTd* tsc)
{
  /** @internal     1.  Declare a variable to store the sum of all samples */
  uint32_t SumOfSamples = 0x00;
  /** @internal     2.  Calculate the sum of all samples */
  for(uint8_t i = 0; i < TSC_NUM_SAMPLES; i++)
  {
    SumOfSamples = SumOfSamples + tsc->Samples[i];
  }
  /** @internal     3.  Divide the sum of all samples by the
   *                    @ref TSC_NUM_SAMPLES to get the smoothed value. */
  tsc->SmoothValue = SumOfSamples / TSC_NUM_SAMPLES;
}

/* Description in .h */
void TSCButton_manage_Interrupt()
{
  TSC_Internal.interrupted = true;
}

/** @} ************************************************************************/
/* end of name "Process TSC"
 ******************************************************************************/


/***************************************************************************//**
 * @name      Get Functions
 * @{
 ******************************************************************************/

/* Description in .h */
TSCButton_State_enumTd TSCButton_get_State(TSCButton_structTd* tsc)
{
  return tsc->state;
}

/* Description in .h */
uint32_t TSCButton_get_RawValue(TSCButton_structTd* tsc)
{
  return tsc->RawValue;
}

/* Description in .h */
uint32_t TSCButton_get_SmoothValue(TSCButton_structTd* tsc)
{
  return tsc->SmoothValue;
}
/** @} ************************************************************************/
/* end of name "Get Functions"
 ******************************************************************************/


/**@}*//* end of defgroup "TSCButton_Source" */
/**@}*//* end of defgroup "TSCButton" */
