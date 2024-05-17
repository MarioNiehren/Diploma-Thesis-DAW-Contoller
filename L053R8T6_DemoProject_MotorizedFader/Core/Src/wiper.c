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

/***************************************************************************//**
 * @name			Initialize
 * @brief			Use these functions to initialize the wiper of the fader
 * @{
 ******************************************************************************/
/* Description in .h */
void Wiper_init_ADC(Wiper_structTd* Wiper, ADC_HandleTypeDef* hadc)
{
	/** @internal				Link HAL-ADC-Handle to the Wiper Structure. */
  Wiper->handle = hadc;
}

/* Description in .h */
void Wiper_init_Hysteresis(Wiper_structTd* Wiper)
{
  Wiper->Hyst_Threshold = 0x04;
  Wiper->Hyst_NumSmallerValues = 0x00;
  Wiper->Hyst_NumBiggerValues = 0x00;
  Wiper->Hyst_DeviationThreshold = 200;
}

/** @} ************************************************************************/
/* end of name "Initialize"
 ******************************************************************************/


/***************************************************************************//**
 * @name      Calibrate
 * @brief     Use this function to calibrate the wiper
 * @note      There functions are not essential, but may be helpful to improve
 *            the performance.
 * @{
 ******************************************************************************/

/** @} ************************************************************************/
/* end of name "Calibrate"
 ******************************************************************************/

/***************************************************************************//**
 * @name			Process ADC
 * @brief			Use these functions to control the ADC
 * @{
 ******************************************************************************/

/* Description in .h */
void Wiper_start(Wiper_structTd* Wiper)
{
	/** @internal			1.	Declare variable with the length of bytes to be buffered
	 * 										by DMA. We have 12Bit ADC, so we need 2 bytes. */
	uint32_t		NumOfBytes = 1;
	/** @internal			2.	Start ADC in DMA mode and link wiper->value to DMA
	 * 										to buffer data from Peripheral to Memory */
  HAL_ADC_Start_DMA(Wiper->handle, &Wiper->BufferDMA, NumOfBytes);
}

/** @cond *//* Function Prototypes */
void store_SampleFromDMABuffer(Wiper_structTd* Wiper);
void calculate_SmoothValue(Wiper_structTd* Wiper);
/** @endcond *//* Function Prototypes */

/* Description in .h */
void Wiper_update(Wiper_structTd* Wiper)
{
	/** @internal			1.	Check if a new ADC Value is available.
												if not -> leave function. */
	if(Wiper->Interrupted == true)
  {
		/** @internal			2.	Reset Interrupt flag */
		Wiper->Interrupted = false;
		/** @internal			3.	Copy the new value to the sample array */
		store_SampleFromDMABuffer(Wiper);
		/** @internal			4.	Calculate the new smoothed value */
		calculate_SmoothValue(Wiper);
		/** @internal			5.	Restart ADC conversion */
		Wiper_start(Wiper);
  }
}

/**
 * @brief			Copy the current value from the DMA buffer to the Sample Array.
 * @param			Wiper			pointer to the users wiper structure
 * @return		none
 *
 */
void store_SampleFromDMABuffer(Wiper_structTd* Wiper)
{
	/** @internal			1.	Copy Value from DMA buffer to Sample Array at current
	 * 										Array-Index */
	Wiper->Samples[Wiper->SamplesIndex] = Wiper->BufferDMA;
	/** @internal			2.	Count up the sample array index for the next value. */
	Wiper->SamplesIndex++;
	/** @internal			3.	If sample array index reaches the defined
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
 * @param			Whiper			pointer to the users whiper structure
 * @return		none
 */
void calculate_SmoothValue(Wiper_structTd* Wiper)
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
	uint16_t ReturnValue = 0;
	uint8_t HystThreshold = 4;
	uint16_t CompareValue = Wiper->ValueSmooth;
	uint16_t HystMin = CompareValue - HystThreshold;
	uint16_t HystMax = CompareValue + HystThreshold;

	if((SamplesAverage > HystMin) && (SamplesAverage < HystMax))
  {
	  ReturnValue = get_ValueApproximatedToAccuracy(Wiper, SamplesAverage, CompareValue);
  }
	else
  {
	  ReturnValue = SamplesAverage;
	  reset_ValueDeviationCounters(Wiper);
	}

	return ReturnValue;
}

uint8_t NumBiggerValuesInHyst = 0;
uint8_t NumSmallerValuesInHyst = 0;
uint16_t get_ValueApproximatedToAccuracy(Wiper_structTd* Wiper, uint16_t SamplesAverage, uint16_t CompareValue)
{
  uint16_t ReturnValue = 0;

  if(SamplesAverage > CompareValue)
  {
   NumBiggerValuesInHyst++;
  }

  if(SamplesAverage < CompareValue)
  {
   NumSmallerValuesInHyst++;
  }

  if(NumBiggerValuesInHyst > (NumSmallerValuesInHyst + 200))
  {
   ReturnValue = CompareValue + 1;
   NumBiggerValuesInHyst = 0;
   NumSmallerValuesInHyst = 0;
  }
  else if(NumSmallerValuesInHyst > (NumBiggerValuesInHyst + 200))
  {
   ReturnValue = CompareValue - 1;
   NumBiggerValuesInHyst = 0;
   NumSmallerValuesInHyst = 0;
  }
  else
  {
   ReturnValue = CompareValue;
  }

  return ReturnValue;
}

void reset_ValueDeviationCounters(Wiper_structTd* Wiper)
{
  NumBiggerValuesInHyst = 0;
  NumSmallerValuesInHyst = 0;
}
/* Description in .h */
void Wiper_manage_Interrupt(Wiper_structTd* Wiper, ADC_HandleTypeDef* hadc)
{
	/** @internal			1.	Check if interrupt occurred on the wipers ADC.
	 * 										If not -> leave function. */
	if(hadc == Wiper->handle)
  {
			/** @internal			2.	Set interrupt flag */
	  Wiper->Interrupted = true;
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
  return Wiper->BufferDMA;
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
