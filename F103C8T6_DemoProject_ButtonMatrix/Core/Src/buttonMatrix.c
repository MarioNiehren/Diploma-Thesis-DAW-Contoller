/**
 * @defgroup    ButtonMatrix_SourceCode
 * @brief       Study this module for detailed information what happens inside the button matrix functions.
 *
 * @addtogroup  ButtonMatrix
 * @{
 *
 * @addtogroup  ButtonMatrix_SourceCode Source Code
 * @{
 *
 * @file    buttonMatrix.c
 * @date    Aug 9, 2023
 * @author  Mario Niehren
 */

#include <buttonMatrix.h>

/**
 * @name      Initialization
 * @brief     These  functions are used to setup the button matrix structure.
 *
 * @{
 */

void ButtonMatrix_init_Structure(buttonMatrix_structTd* buttonMatrix)
{
	buttonMatrix->activeDriveLineCounter = 0x00;
	buttonMatrix->activeInterruptLine = 0x00;
}

uint8_t ButtonMatrix_init_InterruptLinePin(buttonMatrix_structTd* buttonMatrix, GPIO_TypeDef* GPIOint, uint16_t pinInt)
{
	uint8_t LineNumber = buttonMatrix->init.numInterruptLines;

	buttonMatrix->init.GPIOinterrupt[LineNumber] = GPIOint;
	buttonMatrix->init.pinInterrupt[LineNumber] = pinInt;
	buttonMatrix->init.numInterruptLines++;

	return LineNumber;
}

uint8_t ButtonMatrix_init_DriveLinePin(buttonMatrix_structTd* buttonMatrix, GPIO_TypeDef* GPIOdrv, uint16_t pinDrv)
{
	uint8_t LineNumber = buttonMatrix->init.numDriveLines;

	buttonMatrix->init.GPIOdrive[LineNumber] = GPIOdrv;
	buttonMatrix->init.pinDrive[LineNumber] = pinDrv;
	buttonMatrix->init.numDriveLines++;

	return LineNumber;
}


void ButtonMatrix_init_TimerThresholdInMs(buttonMatrix_structTd* buttonMatrix, uint16_t timThresh)
{
	Timer_set_ThresholdInMs(&buttonMatrix->timer, timThresh);
}
/** @} *//* end of "Initialization"*/


/**
 * @name      Start button matrix
 * @brief     These functions are used start the button matrix. It is required as it resets everything to default.
 *
 * @{
 */

/** @cond *//* Function Prototypes */
void setup_digitsInAccount(buttonMatrix_structTd* buttonMatrix);
void reset_buttonStatesOnInterruptLine(buttonMatrix_structTd* buttonMatrix);
void reset_DrivePinsToLeaveReadMode(buttonMatrix_structTd* buttonMatrix);
/**@endcond *//* Function Prototypes */


void ButtonMatrix_start(buttonMatrix_structTd* buttonMatrix)
{
  /** @internal   1. Reset all drive pins*/
  reset_DrivePinsToLeaveReadMode(buttonMatrix);
  /** @internal   2. Reset all button states*/
  reset_buttonStatesOnInterruptLine(buttonMatrix);
  /** @internal   3. Setup relevant digits on the interrupt bit register */
  setup_digitsInAccount(buttonMatrix);

}

/**
 * @brief     This function resets the button state register on each interrupt line.
 *
 * @param     buttonMatrix: Pointer to the structure of the button matrix setup by the user
 *
 * @return    none
 */
void reset_buttonStatesOnInterruptLine(buttonMatrix_structTd* buttonMatrix)
{
  /** @internal   loop over all interrupt lines and set them to 0x0000*/
  for(uint8_t i = 0x00; i < buttonMatrix->init.numInterruptLines; i++)
  {
    buttonMatrix->buttonStatesOnInterruptLine[i] = 0x0000;
  }
}

/**
 * @brief     Function to setup a register that stores all relevant interrupt pins.
 *
 * @param     buttonMatrix: Pointer to the structure of the button matrix setup by the user
 *
 * @return    none
 */
void setup_digitsInAccount(buttonMatrix_structTd* buttonMatrix)
{
  /** @internal   1. Reset the "Digits in account" register */
  buttonMatrix->interrupt.digitsInAccount = 0x0000;

  /** @internal   2. Loop over all existing interrupt lines */
  for(uint8_t i = 0x00; i < buttonMatrix->init.numInterruptLines; i++)
  {
    /** @internal   - In the loop: set the relevant digits of the interrupt pin to the register with bitwise OR
     *              - the value in Pin Interrupt has to match STM32F1xx register values. */
    buttonMatrix->interrupt.digitsInAccount |= buttonMatrix->init.pinInterrupt[i];
  }
}
/** @} *//* end of "Start button matrix"*/



/**
 * @name      Interrupt Function
 * @brief			Call this function in the HAL GPIO interrupt handler.
 * @{
 */

void ButtonMatrix_manage_Interrupt(buttonMatrix_structTd* buttonMatrix, uint16_t GPIO_Pin)
{
	/** @internal		Set the register for interrupted GPIOS with bitwise OR.*/
  buttonMatrix->interrupt.interruptedPins |= GPIO_Pin;
}

/** @} *//* end of name "Interrupt Function" */


/**
 * @name      Update Button Matrix
 * @brief     functions to update the button matrix
 *
 * @{
 */

/** @cond *//* Function Prototypes */
bool check_InterruptDetected(buttonMatrix_structTd* buttonMatrix);
void enter_ReadMode(buttonMatrix_structTd* buttonMatrix);
void read_ButtonsOnActiveDriveLine(buttonMatrix_structTd* buttonMatrix);
bool check_DriveLinesProcessed(buttonMatrix_structTd* buttonMatrix);
uint16_t get_InterruptLinesStateRegister(buttonMatrix_structTd* buttonMatrix);
void reset_InactiveInterruptLines(buttonMatrix_structTd* buttonMatrix, uint16_t InterruptStateRegister);
bool check_ButtonsReleased(buttonMatrix_structTd* buttonMatrix);
void countup_ActiveDriveLine(buttonMatrix_structTd* buttonMatrix);
/**@endcond *//* Function Prototypes */

void ButtonMatrix_update(buttonMatrix_structTd* buttonMatrix)
{
  /** @internal    1. check if the debounce timer is elapsed.
   *                  - if not: leave function
   */
	if(Timer_check_TimerElapsed(&buttonMatrix->timer))
	{
	  /** @internal    2. check if the matrix is in read mode
	   *                  - if not: check for interrupt and enter read mode
	   *                    if an interrupt is detected.
	   */
		if(buttonMatrix->readModeEntered == false)
		{
			if(check_InterruptDetected(buttonMatrix) == true)
			{
				enter_ReadMode(buttonMatrix);
			}
		}
	  /** @internal    3. check again if the matrix is in read mode
	   *                  - if not: leave function
	   */
		if(buttonMatrix->readModeEntered == true)
		{
	    /** @internal    4. Read the button states on the active drive line (according to the buttonMatrix_Td structure)*/
		  read_ButtonsOnActiveDriveLine(buttonMatrix);
	    /** @internal    5. check if all drive lines were processed
	     *                  - if not: count active drive line up and leave function.
	     */
			if(check_DriveLinesProcessed(buttonMatrix) == true)
			{
		    /** @internal    6. Manage interrupt lines: get the active interrupt lines and deactivate every line, that is not
		     *                  active anymore.
		     */
				uint16_t InterruptLineStates = get_InterruptLinesStateRegister(buttonMatrix);
				reset_InactiveInterruptLines(buttonMatrix, InterruptLineStates);

		    /** @internal    7. check if any button still is pushed
		     *                  - if not: reset drive pins and leave read mode
		     */
				if(check_ButtonsReleased(buttonMatrix) == true)
				{
					reset_DrivePinsToLeaveReadMode(buttonMatrix);
				}
			}
	    /** @internal    8. Count up the active drive line in buttonMatrix_structTd structure and leave function
	     */
			countup_ActiveDriveLine(buttonMatrix);
		}
	}
}

/**
 * @brief			Checks if any interrupt line is interrupted.
 * @param     buttonMatrix: Pointer to the structure of the button matrix setup by the user
 * @return		true if an interrupt was found
 */
bool check_InterruptDetected(buttonMatrix_structTd* buttonMatrix)
{
	/** @internal		1. Set the return value with default false. */
  bool returnValue = false;

  /** @internal		2.	Use bitwise AND on the register that flags the interrupted GPIOs with the register that flags the digits in account of the other register.
   * 									If a relevant flag is set (!= 0), set the return value to true. */
	if((buttonMatrix->interrupt.interruptedPins & buttonMatrix->interrupt.digitsInAccount) != 0x0000)
	{
		returnValue = true;
	}
	/** @internal		3.	Return the return value.*/
	return returnValue;
}

/**
 * @brief			Start the read mode of the button matrix.
 * @param     buttonMatrix: Pointer to the structure of the button matrix setup by the user
 * @return		none
 */
void enter_ReadMode(buttonMatrix_structTd* buttonMatrix)
{
	/** @internal		1.	Set all drive lines to high, except the first one, as this one has to be low when beginning the read procedure.*/
	for(uint8_t i = 1; i < buttonMatrix->init.numDriveLines; i++) /* i = 1 to exclude the first drive line */
	{
		HAL_GPIO_WritePin(buttonMatrix->init.GPIOdrive[i], buttonMatrix->init.pinDrive[i], GPIO_PIN_SET);
	}

	/** @internal		2.	Set read mode entered flag to true. */
	buttonMatrix->readModeEntered = true;
}

/**
 * @brief			Read all button states on an active drive line and flag them in the corresponding interrupt line register.
 * @param     buttonMatrix: Pointer to the structure of the button matrix setup by the user
 * @return		none
 */

void read_ButtonsOnActiveDriveLine(buttonMatrix_structTd* buttonMatrix)
{
	uint8_t ActiveDriveLine = buttonMatrix->activeDriveLineCounter;
	GPIO_TypeDef* ActiveDrvGPIO = buttonMatrix->init.GPIOdrive[ActiveDriveLine];
	uint16_t ActiveDrvPin = buttonMatrix->init.pinDrive[ActiveDriveLine];
	uint16_t InterruptedPinsRegister = buttonMatrix->interrupt.interruptedPins;
	uint8_t NumInterruptLines = buttonMatrix->init.numInterruptLines;

	/** @internal		1.	Set the currently active drive pin to low */
	HAL_GPIO_WritePin(ActiveDrvGPIO, ActiveDrvPin, GPIO_PIN_RESET);

	/** @internal		2.	Loop through all existing interrupt lines*/
	for(uint8_t i = 0; i < NumInterruptLines; i++)
	{
		/** @internal		3.	In the loop: Check if the currently relevant interrupt pin was interrupted. If yes:
		 * 									- Read the currently relevant interrupt pin
		 * 									- set or reset the Pin Flag in the interrupt line register with bit manipulation
		 * 										according to the previously read state */
		if((InterruptedPinsRegister & buttonMatrix->init.pinInterrupt[i]) != 0x0000)
		{
			GPIO_PinState lineState = HAL_GPIO_ReadPin(buttonMatrix->init.GPIOinterrupt[i], buttonMatrix->init.pinInterrupt[i]);

			switch(lineState)
			{
				case GPIO_PIN_RESET:
					buttonMatrix->buttonStatesOnInterruptLine[i] |= (0x01 << (ActiveDriveLine));
					break;
				case GPIO_PIN_SET:
					buttonMatrix->buttonStatesOnInterruptLine[i] &= ~(0x01 << (ActiveDriveLine));
					break;
			}

		}
	}
	/** @internal		4.	Set the currently active drive pin back to high */
	HAL_GPIO_WritePin(ActiveDrvGPIO, ActiveDrvPin, GPIO_PIN_SET);
}

/**
 * @brief			Check if all drive lines are already processed
 * @param     buttonMatrix: Pointer to the structure of the button matrix setup by the user
 * @return		true if all drive lies are processed
 */
bool check_DriveLinesProcessed(buttonMatrix_structTd* buttonMatrix)
{
	bool returnValue = false;

	/** @internal		1.	Set return value to true if the drive line counter is equal to the initialized number of drive lines */
	if(buttonMatrix->activeDriveLineCounter == (buttonMatrix->init.numDriveLines - 1))	/* -1 because of digital 0 as first number. */
	{
		returnValue = true;
	}
	/** @internal		2.	Return return Value*/
	return returnValue;
}

/**
 * @brief			Get the Register that describes the active interrupt lines
 * @param     buttonMatrix: Pointer to the structure of the button matrix setup by the user
 * @return		uint16_t value that shout be interpreted as a bit register.
 */
uint16_t get_InterruptLinesStateRegister(buttonMatrix_structTd* buttonMatrix)
{
	/** @internal		1.	setup empty register that stores active interrupt lines to be written and returned */
	uint16_t returnValue = 0x0000;

	/** @internal		2.	loop through all interrupt lines */
	for(uint8_t i = 0; i < buttonMatrix->init.numInterruptLines; i++)
	{
		/** @internal		3. 	in loop: check if any button flag is set on the current interrupt line.
		* 									- if yes: Set the active flag for the interrupt line using the interrupt pin.*/
		if(buttonMatrix->buttonStatesOnInterruptLine[i] != 0x0000)
		{
			returnValue |= buttonMatrix->init.pinInterrupt[i];
		}
	}

	/** @internal		4.	Return the register with all active interrupt line flags. If no Line is active, the value will be 0 */
	return returnValue;
}

/**
 * @brief			Reset all interrupt flags for interrupt lines that are not active anymore
 * @param     buttonMatrix: Pointer to the structure of the button matrix setup by the user
 * @param			InterruptStateRegister:	Register that describes the currently valid interrupt lines.
 * 						Use the return value from get_InterruptLinesStateRegister()
 */
void reset_InactiveInterruptLines(buttonMatrix_structTd* buttonMatrix, uint16_t InterruptStateRegister)
{
	/** @internal		1.	loop through all interrupt lines */
	for(uint8_t i = 0; i < buttonMatrix->init.numInterruptLines; i++)
	{
		/** @internal		2. 	in loop: check if the current interrupt pin is flagged as active
		* 									- if no: Reset the interrupt line in the interrupted Pins register .*/
		if((InterruptStateRegister & buttonMatrix->init.pinInterrupt[i]) == 0)
		{
			buttonMatrix->interrupt.interruptedPins &= ~buttonMatrix->init.pinInterrupt[i];
		}
	}
}

/**
 * @brief			Check if all buttons are released
 * @param     buttonMatrix: Pointer to the structure of the button matrix setup by the user
 * @return		true if no button is pushed anymore
 */
bool check_ButtonsReleased(buttonMatrix_structTd* buttonMatrix)
{
	/** @internal		1.	setup return variable as true */
	bool returnValue = true;

	/** @internal		2.	loop through all interrupt lines */
	for(uint8_t i = 0; i < buttonMatrix->init.numInterruptLines; i++)
	{
		/** @internal		3. 	in loop: check if any button flag is set on current interrupt line
		* 									- if yes: Set return variable to false */
		if(buttonMatrix->buttonStatesOnInterruptLine[i] != 0x0000)
		{
			returnValue = false;
			break;
		}
	}

	/** @internal		4.	Return return variable*/
	return returnValue;
}

/**
 * @brief			Set all drive pins to low and leave read mode
 * @param     buttonMatrix: Pointer to the structure of the button matrix setup by the user
 */
void reset_DrivePinsToLeaveReadMode(buttonMatrix_structTd* buttonMatrix)
{
	/** @internal		1.	loop through all drive lines and set each line to low */
	for(uint8_t i = 0U; i < buttonMatrix->init.numDriveLines; i++)
	{
		HAL_GPIO_WritePin(buttonMatrix->init.GPIOdrive[i], buttonMatrix->init.pinDrive[i], GPIO_PIN_RESET);
	}

	/** @internal		2.	Set "Read Mode Entered" to false*/
	buttonMatrix->readModeEntered = false;
}

/**
 * @brief			Count up active drive line counter to go to the next line
 * @param     buttonMatrix: Pointer to the structure of the button matrix setup by the user
 */
void countup_ActiveDriveLine(buttonMatrix_structTd* buttonMatrix)
{
	/** @internal		1.	Check if the counter is smaller then the number of drive lines (what is the max value of the counter)*/
  if(buttonMatrix->activeDriveLineCounter < (buttonMatrix->init.numDriveLines - 1)) /* -1 because of digital 0 as first number. */
  {
  	/** @internal		- if yes: count up */
    buttonMatrix->activeDriveLineCounter++;
  }
  else
  {
    	/** @internal		- if no: reset counter to 0 */
    buttonMatrix->activeDriveLineCounter = 0;
  }
}
/** @} *//* end of "Update Button Matrix"*/

/**
 * @name			Get States from the Button Matrix
 *
 * @brief			Use these functions to read states of specific buttons in the matrix.
 *
 * @{
 */

bool ButtonMatrix_check_ButtonPushed(buttonMatrix_structTd* buttonMatrix, buttonMatrix_Coordinates_Td* buttonName)
{
	uint16_t posIntLine = buttonName->PosIntLine;
	uint16_t posDrvLine = buttonName->PosDrvLine;
	bool returnValue = false;
	if(buttonMatrix->buttonStatesOnInterruptLine[posIntLine] & buttonMatrix->init.pinDrive[posDrvLine])
	{
		returnValue = true;
	}
	return  returnValue;
}

uint16_t ButtonMatrix_get_ButtonStatesRegisterOnInterruptLine(buttonMatrix_structTd* buttonMatrix, uint16_t posIntLine)
{
	return buttonMatrix->buttonStatesOnInterruptLine[posIntLine];
}
/** @} *//* end of name "Get States from the Button Matrix" */

/** @} *//* end of defgroup "ButtonMatrix_SourceCode" */
/** @} *//* end of defgroup "ButtonMatrix" */

