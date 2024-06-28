/***************************************************************************//**
 * @defgroup				Wiper_Source		Source Code
 * @brief						Study this part for details about the function of wipers.
 *
 * @addtogroup			WiperMotorFader
 * @{
 *
 * @addtogroup			Wiper_Source
 * @{
 *
 * @file						wiper.c
 *
 * @date						Sep 4, 2023
 * @author					Mario Niehren
 ******************************************************************************/

#include <wiper.h>

/**
 * @brief     Structure that contains all data for an ADC.
 */
typedef struct
{
  ADC_HandleTypeDef*  hadc;         /**<  HAL-handle of the ADC */
  bool            hadcInterrupted;  /**<  Flag to mark an interrupt */
  uint16_t        BufferDMA[NUM_ALL_ADC_CHANNELS];  /**< DMA will use this
                                          buffer to store the ADC data of each
                                          channel. */
  Wiper_structTd* InitializedWipers[NUM_ALL_ADC_CHANNELS];  /**< Pointers to all
                                          initialized wipers on this ADC */
  uint8_t         NumUsedChannels;  /**<  Number of used channels of this ADC.
                                          This is equal to the number of
                                          initialized wipers on this ADC */

}Wiper_ADCdescriptor_structTd;

/**
 * @brief     Structure that contains all active ADCs. This is for internal
 *            use only.
 */
typedef struct
{
  Wiper_ADCdescriptor_structTd ADCs[NUM_ALL_ADC_ON_MCU];  /**<  Pointer to all
                                           available ADCs on the used STM32
                                           microcontroller */
  uint8_t   NumADCs;                  /**< Number of actually used ADCs*/
}Wiper_Internal_StructTd;

/**
 * @brief     Object of Wiper_Internal_StructTd type to store data needed inside
 *            this module.
 */
Wiper_Internal_StructTd WiperInternal = {0};

/***************************************************************************//**
 * @name			Initialize
 * @brief			Use these functions to initialize the wiper of the fader
 * @{
 ******************************************************************************/

/** @cond *//* Function Prototypes */
bool check_ADCAlreadyInUse(ADC_HandleTypeDef* hadc);
uint8_t get_ValidADCIndex(ADC_HandleTypeDef* hadc);
uint8_t get_NextFreeADCIndex(ADC_HandleTypeDef* hadc);
void link_hadc(ADC_HandleTypeDef* hadc, uint8_t IndexADC);
void link_WiperToInternalStructure(Wiper_structTd* Wiper, uint8_t IndexADC);
/** @endcond *//* Function Prototypes */

/* Description in .h */
void Wiper_init_ADC(Wiper_structTd* Wiper, ADC_HandleTypeDef* hadc)
{
  uint8_t IndexValidADC = 0;

  /** @internal     1.  Check if the ADC-handler is already linked.
   *                    - if yes: find the valid ADC index of the internal
   *                              structure.
   *                    - if no:  find the next free ADC index and link the
   *                              ADC handle to the structure */
  if(check_ADCAlreadyInUse(hadc) == true)
  {
    IndexValidADC = get_ValidADCIndex(hadc);
  }
  else
  {
    IndexValidADC = get_NextFreeADCIndex(hadc);
    link_hadc(hadc, IndexValidADC);
  }
  /** @internal     2.  Link the ADC relevant parts of the Wiper structure to
   *                    the internal structure at the valid ADC index. */
  link_WiperToInternalStructure(Wiper, IndexValidADC);
}

/**
 * @brief     Check if the ADC-handler is already linked to the internal
 *            structure.
 * @param     hadc        pointer to the HAL generated ADC-handle of the ADC
 *                        that is used for the wiper
 * @return    "true" if the ADC-handler is already linked, "false" it not
 */
bool check_ADCAlreadyInUse(ADC_HandleTypeDef* hadc)
{
  bool hadcInUse = false;

  /** @internal     1.  Loop trough all available ADCs of the microcontroller */
  for(uint8_t IndexLoop = 0; IndexLoop < NUM_ALL_ADC_ON_MCU; IndexLoop++)
  {
    /** @internal     2.  Check if the current ADC matches with the ADC-handler.
     *                    if yes: Return true. */
    if(WiperInternal.ADCs[IndexLoop].hadc == hadc)
    {
      hadcInUse = true;
    }
  }

  return hadcInUse;
}

/**
 * @brief     Get the index of an ADC that is already linked to the internal
 *            structure
 * @param     hadc        pointer to the HAL generated ADC-handle of the ADC
 *                        that is used for the wiper
 * @return    valid ADC index. If the ADC is not linked yet, the function will
 *            return 0xFF.
 */
uint8_t get_ValidADCIndex(ADC_HandleTypeDef* hadc)
{
  uint8_t IndexValidADC = 0xFF;

  /** @internal     1.  Loop trough all available ADCs of the microcontroller */
  for(uint8_t IndexLoop = 0; IndexLoop < NUM_ALL_ADC_ON_MCU; IndexLoop++)
  {
    if(WiperInternal.ADCs[IndexLoop].hadc == hadc)
    {
      /** @internal     2.  Check if the current ADC matches with the
       *                    ADC-handler.
       *                    - if yes: Return the current number of the loop
       *                              counter. */
      IndexValidADC = IndexLoop;
    }
  }
  return IndexValidADC;
}

/**
 * @brief     Get the index number of the next free space in the ADC array of
 *            the internal structure.
 * @param     hadc        pointer to the HAL generated ADC-handle of the ADC
 *                        that is used for the wiper
 * @return    index number of the next free space
 */
uint8_t get_NextFreeADCIndex(ADC_HandleTypeDef* hadc)
{
  /** @internal     1.  Return the number of linked ADCs of the internal
   *                    structure. */
  return WiperInternal.NumADCs;
}

/**
 * @brief     link the hadc to the internal structure.
 * @param     hadc        pointer to the HAL generated ADC-handle of the ADC
 *                        that is used for the wiper
 * @param     IndexADC    Index of the internal structure, where the ADC should
 *                        be linked.
 * @return    none
 */
void link_hadc(ADC_HandleTypeDef* hadc, uint8_t IndexADC)
{
  /** @internal     1.  link hadc to the internal structure */
  WiperInternal.ADCs[IndexADC].hadc = hadc;
  /** @internal     2.  Count up the number of linked ADCs */
  WiperInternal.NumADCs++;
}

/**
 * @brief     link ADC relevant parts of the wiper structure to the internal
 *            structure
 * @param     hadc        pointer to the HAL generated ADC-handle of the ADC
 *                        that is used for the wiper
 * @param     IndexADC    Index of the internal structure, where the ADC is
 *                        linked.
 */
void link_WiperToInternalStructure(Wiper_structTd* Wiper, uint8_t IndexADC)
{
  /** @internal     1.  Get the Wiper Index */
  uint8_t WiperIndex = WiperInternal.ADCs[IndexADC].NumUsedChannels;
  /** @internal     2.  Store the Wiper Index to the Wiper structure to find it
   *                    quick later. */
  Wiper->IndexBufferDMA = WiperIndex;
  /** @internal     3.  Link the Wiper structure to the internal ADC structure
   *                    at the Wiper Index*/
  WiperInternal.ADCs[IndexADC].InitializedWipers[WiperIndex] = Wiper;
  /** @internal     4.  Count up the number of used channels of the ADC */
  WiperInternal.ADCs[IndexADC].NumUsedChannels++;
}

/* Description in .h */
void Wiper_init_Hysteresis(Wiper_structTd* Wiper)
{
  uint8_t   ThresholdDefault = 15;
  uint16_t  NumSmallerValues = 0;
  uint16_t  NumBiggerValues = 0;
  uint16_t  NumDeviationThresholdDefault = 1000;

  /** @internal     1.  Store default hysteresis values to wiper structure. */
  Wiper->Hyst_Threshold = ThresholdDefault;
  Wiper->Hyst_NumSmallerValues = NumSmallerValues;
  Wiper->Hyst_NumBiggerValues = NumBiggerValues;
  Wiper->Hyst_DeviationThreshold = NumDeviationThresholdDefault;
}

/** @} ************************************************************************/
/* end of name "Initialize"
 ******************************************************************************/


/***************************************************************************//**
 * @name      Calibrate
 * @brief     Use this function to calibrate the wiper
 * @note      These functions are not essential, but may be helpful to improve
 *            the performance.
 * @{
 ******************************************************************************/

/* Description in .h */
void Wiper_calibrate_HysteresisThreshold(Wiper_structTd* Wiper, uint8_t Threshold)
{
  /** @internal     1.  Copy Threshold to Hyst_Threshold of the wiper structure */
  Wiper->Hyst_Threshold = Threshold;
}

/* Description in .h */
void Wiper_calibrate_HysteresisDeviationThreshold(Wiper_structTd* Wiper, uint8_t Threshold)
{
  /** @internal     1.  Copy Threshold to Hyst_DeviationThreshold of the wiper
   *                    structure */
  Wiper->Hyst_DeviationThreshold = Threshold;
}

/** @} ************************************************************************/
/* end of name "Calibrate"
 ******************************************************************************/

/***************************************************************************//**
 * @name			Process ADC
 * @brief			Use these functions to update the ADC values
 * @{
 ******************************************************************************/

/* Description in .h */
void Wiper_start_All(void)
{
  uint8_t IndexLoop = 0;
  uint8_t NumHadcs = WiperInternal.NumADCs;

  /** @internal     1.  Loop through all active ADCs */
  for(IndexLoop = 0; IndexLoop < NumHadcs; IndexLoop++)
  {
    ADC_HandleTypeDef*  handle = WiperInternal.ADCs[IndexLoop].hadc;
    uint16_t* Buffer = WiperInternal.ADCs[IndexLoop].BufferDMA;
    uint8_t UsedChannels = WiperInternal.ADCs[IndexLoop].NumUsedChannels;

    /** @internal     2. Start ADC on DMA for each active ADC */
    HAL_ADC_Start_DMA(handle, (uint32_t*) Buffer, UsedChannels);
  }
}

/** @cond *//* Function Prototypes */
void store_SampleFromDMABuffer(Wiper_structTd* Wiper, uint16_t Buffer);
void calculate_SmoothADCValue(Wiper_structTd* Wiper);
/** @endcond *//* Function Prototypes */

/* Description in .h */
void Wiper_update_All(void)
{
  uint8_t IndexADCs = 0;
  uint8_t NumHadcs = WiperInternal.NumADCs;

  /** @internal     1.  Loop through all active ADCs. Inside the loop: */
  for(IndexADCs = 0; IndexADCs < NumHadcs; IndexADCs++)
  {
    Wiper_ADCdescriptor_structTd* ActiveADC = &WiperInternal.ADCs[IndexADCs];
    /** @internal     2.  Check if the current ADC was interrupted.
     *                    - If no: leave function. */
    if(ActiveADC->hadcInterrupted == true)
    {
      /** @internal      3.  Reset Interrupt Flag of current ADC */
      ActiveADC->hadcInterrupted = false;

      uint8_t IndexChannels = 0;
      uint8_t NumChannels = ActiveADC->NumUsedChannels;

      /** @internal     4.  loop trough all active channels of the current ADC */
      for(IndexChannels = 0; IndexChannels < NumChannels; IndexChannels++)
      {
        Wiper_structTd* Wiper = ActiveADC->InitializedWipers[IndexChannels];
        uint16_t Buffer = ActiveADC->BufferDMA[IndexChannels];
        /** @internal     5.  Store the data of the DMA buffer to the wiper
         *                    that belongs to the current channel */
        store_SampleFromDMABuffer(Wiper, Buffer);
        /** @internal     6.  Calculate the smoothed value for the wiper
         *                    that belongs to the current channel */
        calculate_SmoothADCValue(Wiper);
      }

      ADC_HandleTypeDef*  handle = ActiveADC->hadc;
      uint16_t* Buffer = ActiveADC->BufferDMA;
      uint8_t UsedChannels = ActiveADC->NumUsedChannels;

      /** @internal     7.  Restart the ADC-DMA of the currently interrupted ADC
       *                    as we are in single conservation mode. */
      HAL_ADC_Start_DMA(handle, (uint32_t*) Buffer, UsedChannels);
    }
  }
}

/**
 * @brief			Copy the current value from the DMA buffer to the Sample Array.
 * @param			Wiper			pointer to the users wiper structure
 * @return		none
 *
 */
void store_SampleFromDMABuffer(Wiper_structTd* Wiper, uint16_t Buffer)
{
	/** @internal			1.	Copy Value from DMA buffer to Sample Array at current
	 * 										Array-Index */
	Wiper->Samples[Wiper->SamplesIndex] = Buffer;
  /** @internal     2.  Copy Value from DMA buffer to Value Raw to be able
   *                    to return it quick to the user if needed. */
	Wiper->ValueRaw = Buffer;
	/** @internal			3.	Count up the sample array index for the next value. */
	Wiper->SamplesIndex++;
	/** @internal			4.	If sample array index reaches the defined
	 * 										 @ref NUMBER_OF_SAMPLES, reset the index. The following
	 * 										 values will overwrite the old ones. */
	if(Wiper->SamplesIndex == NUMBER_OF_SAMPLES)
	{
		Wiper->SamplesIndex = 0x00;
	}
}

/** @cond *//* Function Prototypes */
uint16_t get_SmoothedVlaueWithHysteresis(Wiper_structTd* Wiper, uint16_t SamplesAverage);
/** @endcond *//* Function Prototypes */

/**
 * @brief			Calculate an average of all available samples
 * @param			Wiper			pointer to the users wiper structure
 * @return		none
 */
void calculate_SmoothADCValue(Wiper_structTd* Wiper)
{
	/** @internal			1.	Declare a variable to store the sum of all samples */
  uint32_t SumOfSamples = 0x00;
  uint16_t SamplesAverage = 0x00;
  /** @internal			2.	Calculate the sum of all samples */
  for(uint8_t i = 0; i < NUMBER_OF_SAMPLES; i++)
  {
    SumOfSamples = SumOfSamples + Wiper->Samples[i];
  }
  /** @internal			3.	Divide the sum of all samples by the
   * 										@ref NUMBER_OF_SAMPLES to get the smoothed value. */
  SamplesAverage = SumOfSamples / NUMBER_OF_SAMPLES;
  /** @internal     4.  Calculate hysteresis */
  Wiper->ValueSmooth = get_SmoothedVlaueWithHysteresis(Wiper, SamplesAverage);
}

/** @cond *//* Function Prototypes */
uint16_t get_ValueApproximatedToAccuracy(Wiper_structTd* Wiper, uint16_t SamplesAverage, uint16_t CompareValue);
void reset_ValueDeviationCounters(Wiper_structTd* Wiper);
/** @endcond *//* Function Prototypes */

/**
 * @brief			Check if the average of samples is in a hysteresis area around
 * 						the current value. It reduce noise a lot but the hysteresis
 * 						threshold should be as low as possible to avoid big value jumps
 * 						around the current value.
 * @param			Wiper			pointer to the users wiper structure
 * @param			SamplesAverage	average value calculated from existing samples
 * @return		valid smoothed value. If the hysteresis is inactive, it will be
 * 						equal to Sampled Average.
 */
uint16_t get_SmoothedVlaueWithHysteresis(Wiper_structTd* Wiper, uint16_t SamplesAverage)
{
  /** @internal     1.  Setup variables used inside this function */
	uint16_t ReturnValue = 0;
	uint8_t HystThreshold = Wiper->Hyst_Threshold;
	uint16_t CompareValue = Wiper->ValueSmooth;

	/** @internal     2.  Calculate Limits (Min/Max) of hysteresis range */
	uint16_t HystMin = CompareValue - HystThreshold;
	uint16_t HystMax = CompareValue + HystThreshold;

	/** @internal     3.  Check if average value of the current samples is in the
   *                    hysteresis range
	 *                    - If yes: check if the samples average is accurate
	 *                      enough, correct it if necessary and save the value for
	 *                      return
	 *                    - If not: Save the samples average as return value and
	 *                      reset the value deviation counters witch are required
	 *                      for the accuracy approximation. */
	if((SamplesAverage > HystMin) && (SamplesAverage < HystMax))
  {
	  ReturnValue = get_ValueApproximatedToAccuracy(Wiper, SamplesAverage, CompareValue);
  }
	else
  {
	  ReturnValue = SamplesAverage;
	  reset_ValueDeviationCounters(Wiper);
	}

	/** @internal     4.  Return the result */
	return ReturnValue;
}

/**
 * @brief     Check if the average of samples drifts away from the current
 *            smooth value inside the hysteresis range. It adjusts the value if
 *            necessary.
 * @param     Wiper     pointer to the users wiper structure
 * @param     SamplesAverage  average value calculated from existing samples
 * @param     CompareValue    value the samples average will be compared to
 */
uint16_t get_ValueApproximatedToAccuracy(Wiper_structTd* Wiper, uint16_t SamplesAverage, uint16_t CompareValue)
{
  uint16_t ReturnValue = 0;

  uint16_t NumSmallerValuesInHyst = Wiper->Hyst_NumSmallerValues;
  uint16_t NumBiggerValuesInHyst = Wiper->Hyst_NumBiggerValues;
  uint16_t Threshold = Wiper->Hyst_DeviationThreshold;

  /** @internal     1.  If the samples average is bigger then the compare value,
   *                    add the difference to the bigger values counter */
  if(SamplesAverage > CompareValue)
  {
    uint16_t Difference = SamplesAverage + CompareValue;
    NumBiggerValuesInHyst = NumBiggerValuesInHyst + Difference;
  }
  /** @internal     2.  If the samples average is smaller then the compare value,
   *                    add the difference to the smaller values counter */
  if(SamplesAverage < CompareValue)
  {
    uint16_t Difference = CompareValue - SamplesAverage;
    NumSmallerValuesInHyst = NumSmallerValuesInHyst + Difference;
  }

  /** @internal     3.  If the bigger value counter overflows the threshold
   *                    in reference to the smaller value counter, add +1 to the
   *                    return value and reset counters. */
  if(NumBiggerValuesInHyst > (NumSmallerValuesInHyst + Threshold))
  {
    ReturnValue = CompareValue + 1;
    NumBiggerValuesInHyst = 0;
    NumSmallerValuesInHyst = 0;
  }
  /** @internal     4.  If the smaller value counter overflows the threshold
   *                    in reference to the bigger value counter, add -1 to the
   *                    return value and reset counters. */
  else if(NumSmallerValuesInHyst > (NumBiggerValuesInHyst + Threshold))
  {
    ReturnValue = CompareValue - 1;
    NumBiggerValuesInHyst = 0;
    NumSmallerValuesInHyst = 0;
  }
  else
  {
    /** @internal     5.  If the Threshold does not overflow, return the
     *                    original value. */
    ReturnValue = CompareValue;
  }

  Wiper->Hyst_NumSmallerValues = NumSmallerValuesInHyst;
  Wiper->Hyst_NumBiggerValues = NumBiggerValuesInHyst;

  return ReturnValue;
}

/**
 * @brief     Reset the hysteresis deviation counters
 * @param     Wiper     pointer to the users wiper structure
 * @return    none
 */
void reset_ValueDeviationCounters(Wiper_structTd* Wiper)
{
  Wiper->Hyst_NumSmallerValues = 0;
  Wiper->Hyst_NumBiggerValues = 0;
}

/* Description in .h */
void Wiper_manage_Interrupt(ADC_HandleTypeDef* hadc)
{
  uint8_t index = 0;
  /** @internal     1.  Loop through all active ADCs*/
  for(index = 0; index < WiperInternal.NumADCs; index++)
  {
    /** @internal     2.  If current ADC matches hadc, set interrupt flag*/
    if(WiperInternal.ADCs[index].hadc == hadc)
    {
      WiperInternal.ADCs[index].hadcInterrupted = true;
    }
  }
}

/** @} ************************************************************************/
/* end of name "Process ADC"
 ******************************************************************************/


/***************************************************************************//**
 * @name			Get Functions
 * @brief			Use these functions to get values from the Whiper
 * @{
 ******************************************************************************/

/* Description in .h */
uint16_t Wiper_get_RawValue(Wiper_structTd* Wiper)
{
	/** @internal			1.	return the value of DMA butter for the ADC*/
  return Wiper->ValueRaw;
}

/* Description in .h */
uint16_t Wiper_get_SmoothValue(Wiper_structTd* Wiper)
{
	/** @internal			1.	return the value of the wiper */
  return Wiper->ValueSmooth;
}

/** @} ************************************************************************/
/* end of name "Get Functions"
 ******************************************************************************/


/**@}*//* end of defgroup "Wiper_Source" */
/**@}*//* end of defgroup "WiperMotorFader" */
