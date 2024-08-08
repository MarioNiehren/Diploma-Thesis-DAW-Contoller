/**
 * @defgroup    ButtonMatrix Button Matrix
 * @brief       This module is used to setup and process a button matrix.
 *
 *	<h1> How to use: </h1>
 *	<h2> Software </h2>
 *		1. Setup the Button Matrix according to the \ref SetupProcedure "Setup Procedure" description.
 *		2. Set Interrupt Management to the systems ISR as described \ref InterruptManagement "here".
 *		3. Call update function in while loop as described \ref UpdateMatrix "here"
 *		4. Now you can use the \ref GetStates "Get Button States" Functions to find pushed buttons.
 *
 *		@note	This module also needs \ref timer.c and \ref timer.h for internal use.<br>
 *
 *  <h2> Hardware </h2>
 *  This is a schematic example:
 *
 *  @image html TastmatrixSchaltbild_W1000px.jpg
 *
 * @defgroup		ButtonMatrix_Header Header File
 * @brief				Study this module if you just want to use the button matrix functions.
 *
 * @note				As default, stm32f1xx_hal.h is included in this file.
 * 							If you want to use this module with an other STM32 Microcontroller that does not belong to the
 * 							f1xx line, please replace this include with the file for your model.
 *
 *
 * @addtogroup  ButtonMatrix
 * @{
 *
 * @addtogroup	ButtonMatrix_Header
 * @{
 *
 * @file    buttonMatrix.h
 *
 * @date    Aug 9, 2023
 * @author  Mario
 */

#ifndef INC_BUTTONMATRIX_H_
#define INC_BUTTONMATRIX_H_

#include "stm32f1xx_hal.h"
#include "timer.h"
#include <stdbool.h>

/** Max Number 16, because STM32 M3 Controllers usually have 16 GPIO EXTI-lines. */
#define BUTTON_MATRIX_MAX_INTERRUPT_LINES   16U

/**
 * @name   Enumerations for the user in external use
 *
 * These enumerations are offered to the user to write a better understandable code
 * and to avoid magic numbers when working with this module.
 * - INT stands for Interrupt.
 * - DRV stands for Drive.
 */
typedef enum
{
	BUTTON_MATRIX_INT_0 = 0,			/**<Interrupt Line 0*/
	BUTTON_MATRIX_INT_1 = 1,			/**<Interrupt Line 1*/
	BUTTON_MATRIX_INT_2 = 2,			/**<Interrupt Line 2*/
	BUTTON_MATRIX_INT_3 = 3,			/**<Interrupt Line 3*/
	BUTTON_MATRIX_INT_4 = 4,			/**<Interrupt Line 4*/
	BUTTON_MATRIX_INT_5 = 5,			/**<Interrupt Line 5*/
	BUTTON_MATRIX_INT_6 = 6,			/**<Interrupt Line 6*/
	BUTTON_MATRIX_INT_7 = 7,			/**<Interrupt Line 7*/
	BUTTON_MATRIX_INT_8 = 8,			/**<Interrupt Line 8*/
	BUTTON_MATRIX_INT_9 = 9,			/**<Interrupt Line 9*/
	BUTTON_MATRIX_INT_10 = 10,	  /**<Interrupt Line 10*/
	BUTTON_MATRIX_INT_11 = 11,		/**<Interrupt Line 11*/
	BUTTON_MATRIX_INT_12 = 12,		/**<Interrupt Line 12*/
	BUTTON_MATRIX_INT_13 = 13,		/**<Interrupt Line 13*/
	BUTTON_MATRIX_INT_14 = 14,		/**<Interrupt Line 14*/
	BUTTON_MATRIX_INT_15 = 15,		/**<Interrupt Line 15*/

	BUTTON_MATRIX_DRV_0 = 0,			/**<Drive Line 0*/
	BUTTON_MATRIX_DRV_1 = 1,			/**<Drive Line 1*/
	BUTTON_MATRIX_DRV_2 = 2,			/**<Drive Line 2*/
	BUTTON_MATRIX_DRV_3 = 3,			/**<Drive Line 3*/
	BUTTON_MATRIX_DRV_4 = 4,			/**<Drive Line 4*/
	BUTTON_MATRIX_DRV_5 = 5,			/**<Drive Line 5*/
	BUTTON_MATRIX_DRV_6 = 6,			/**<Drive Line 6*/
	BUTTON_MATRIX_DRV_7 = 7,			/**<Drive Line 7*/
	BUTTON_MATRIX_DRV_8 = 8,			/**<Drive Line 8*/
	BUTTON_MATRIX_DRV_9 = 9,			/**<Drive Line 9*/
	BUTTON_MATRIX_DRV_10 = 10,		/**<Drive Line 10*/
	BUTTON_MATRIX_DRV_11 = 11,		/**<Drive Line 11*/
	BUTTON_MATRIX_DRV_12 = 12,		/**<Drive Line 12*/
	BUTTON_MATRIX_DRV_13 = 13,		/**<Drive Line 13*/
	BUTTON_MATRIX_DRV_14 = 14,		/**<Drive Line 14*/
	BUTTON_MATRIX_DRV_15 = 15		  /**<Drive Line 15*/
}buttonMatrix_lines_enum;


/**
 * @name      Structures to handle the button matrix
 *
 * @{
 */

/**
 * @brief     Structure to store coordinates to a button
 *
 * This structure used to setup buttons on user side. It's needed to get the state of a specific button.
 * It is recommended to setup one object of this type for each button according to the users hardware setup.
 */
typedef struct buttonMatrix_Coordinates_Td
{
  uint8_t PosIntLine;         /**<numeric position on the interrupt line*/
  uint8_t PosDrvLine;         /**<numeric position on the drive line*/
}buttonMatrix_Coordinates_Td;

/**
 * @brief     Structure to store the initialization values
 *
 * @note      GPIO_TypeDef is part of STM's HAL
 */
typedef struct buttonMatrix_init_Td
{
  GPIO_TypeDef* GPIOinterrupt[16];    /**<Place to store up to 16 interrupt ports */
  uint16_t pinInterrupt[16];          /**<Place to store up to 16 interrupt pins */

  GPIO_TypeDef* GPIOdrive[16];        /**<Place to store up to 16 drive ports */
  uint16_t pinDrive[16];              /**<Place to store up to 16 drive pins */

  uint8_t numInterruptLines;          /**<Number of interrupt lines */
  uint8_t numDriveLines;              /**<Number of drive lines */
}buttonMatrix_init_Td;

/**
 * @brief     Structure for Interrupt Management
 */
typedef struct buttonMatrix_InterruptManagement_Td
{
  uint16_t interruptedPins;       /**<16 bit register to set flags for interrupted Pins*/
  uint16_t digitsInAccount;       /**<16 bit register to set a bit for each digit of the GPIO register that is relevant*/
}buttonMatrix_InterruptManagement_Td;

/**
 * @brief     Main Button Matrix Structure
 * @note      The user has to declare an object of this structure to work with it.
 *            It contains all relevant data and states. Initializations will also be stored here.
 */
typedef struct buttonMatrix_structTd
{
  /** Structure for initialization */
  buttonMatrix_init_Td init;
  /** Structure for a \ref Timer used debounce */
  Timer_structTd timer;
  /** Interrupt management */
  buttonMatrix_InterruptManagement_Td interrupt;
  /** Currently active interrupt line */
  buttonMatrix_lines_enum activeInterruptLine;
  /** Currently active drive line */
  buttonMatrix_lines_enum activeDriveLineCounter;
  /** Array of 16 bit registers to store the button states on an interrupt line */
  uint16_t buttonStatesOnInterruptLine[BUTTON_MATRIX_MAX_INTERRUPT_LINES];
  /** Flag that indicates it the matrix is in read mode */
  bool readModeEntered;

}buttonMatrix_structTd;
/** @} *//* end of name "Structures to handle the button matrix" */

/**
 * @name			Setup, initialize and start the Button Matrix
 * @anchor		SetupProcedure
 *
 * Procedure to setup a button matrix:
 * 1. Declare an object of the type buttonMatrix_structTd for the matrix
 * 2. Use ButtonMatrix_init_Structure() to fill the structure with required start values
 * 3. Call ButtonMatrix_init_InterruptLinePin() for each interrupt line in use. If needed, save the returned line number for later use.
 * 4. Call ButtonMatrix_init_DriveLinePin() for each drive line in use.  If needed, save the returned line number for later use.
 * 5. Set the update frequency with ButtonMatrix_init_TimerThresholdInMs(). A reasonable value would be 5/NumberOfDriveLines to get a read interval of about
 * 		5ms for each Key (what is a common debounce time for typical tactile switches). If there is bouncing, increase the value. Be careful that it does not get to slow to detect fast button pushes.
 * 6. Call buttonMatrix_start() once to start the matrix.
 *
 * @note			It is recommended to setup an object of buttonMatrix_Coordinates_Td type for each button with coordinates that fit the individual hardware
 * 						setup. That makes it easy to handle the buttons and get the states of them.
 *
 * @{
 */

/**
 * @brief			Set values of the structure to required start values.
 *
 * @param     buttonMatrix: Pointer to the structure of the button matrix setup by the user
 *
 * @return		none
 */
void ButtonMatrix_init_Structure(buttonMatrix_structTd* buttonMatrix);

/**
 * @brief			Setup a Pin for an interrupt line. This function will return the number of the line.
 * 						The number counts up internal and starts at 0 (for the first initialized Pin).
 * @note			Activate internal pull-ups of the microcontroller if no external ones are used.
 * @warning		The Interrupt has to occure on **falling edge** as we work with LOW-active signals!
 *
 * @param     buttonMatrix: Pointer to the structure of the button matrix setup by the user
 * @param			GPIOint:			Pointer to the interrupt enabled GPIO Port
 * @param			pinInt: 			Register value of the interrupt Pin (as given from reference manual or HAL)
 *
 * @return		Interrupt Line Number
 */
uint8_t ButtonMatrix_init_InterruptLinePin(buttonMatrix_structTd* buttonMatrix, GPIO_TypeDef* GPIOint, uint16_t pinInt);

/**
 * @brief			Setup a Pin for a drive line. This function will return the number of the line.
 * 						The number counts up internal and starts at 0 (for the first initialized Pin).
 *
 * @param     buttonMatrix: Pointer to the structure of the button matrix setup by the user
 * @param			GPIOint:			Pointer to the drive output GPIO Port
 * @param			pinInt: 			Register value of the drive Pin (as given from reference manual or HAL)
 *
 * @return		Drive Line Number
 */
uint8_t ButtonMatrix_init_DriveLinePin(buttonMatrix_structTd* buttonMatrix, GPIO_TypeDef* GPIOdrv, uint16_t pinDrv);

/**
 * @brief     Setup the timer threshold for debouncing
 *
 * @param     buttonMatrix: Pointer to the structure of the button matrix setup by the user
 * @param     timThresh:    Threshold time in milliseconds
 *
 * @return    none
 */
void ButtonMatrix_init_TimerThresholdInMs(buttonMatrix_structTd* buttonMatrix, uint16_t timThresh);

/**
 * @brief     Function to start the button matrix.
 *
 * @param     buttonMatrix: Pointer to the structure of the button matrix setup by the user
 *
 * @return    none
 */
void ButtonMatrix_start(buttonMatrix_structTd* buttonMatrix);

/** @} *//* end of name "Setup and Initialize the Button Matrix" */

/**
 * @name			Interrupt Management
 * @anchor		InterruptManagement
 * @note			This function has to be called in the interrupt service routine. This is an example with STM32 HAL:
 *
 * 						@code
 * 						void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
 * 						{
 * 						  ButtonMatrix_manage_Interrupt(&UserButtonMatrix, GPIO_Pin);
 * 						}
 * 						@endcode
 *
 * @{
 */
/**
 * @brief     Flag interrupted Pins
 * @param     buttonMatrix: Pointer to the structure of the button matrix setup by the user
 * @param			GPIO_Pin: The pin where the interrupt is detected.
 * @return		none
 */
void ButtonMatrix_manage_Interrupt(buttonMatrix_structTd* buttonMatrix, uint16_t GPIO_Pin);
/** @} *//* end of name "Interrupt Management"*/

/**
 * @name			Update Button Matrix
 * @anchor		UpdateMatrix
 *
 * Call this function periodically to pull the Button Matrix. This Function is not blocking, as it uses a non blocking timer and only starts
 * to process, if the timer is elapsed and an interrupt is detected. If this function is not called often enough, the stability may suffer.
 * Try to avoid other blocking program elements.
 *
 * @{
 */
/**
 * @brief			Main function to update the button matrix.
 *
 * @param     buttonMatrix: Pointer to the structure of the button matrix setup by the user
 * @return		none TODO: Add Ret true if matrix is active. This can be used to mark if the user
 * 						should fetch new states.
 */
void ButtonMatrix_update(buttonMatrix_structTd* buttonMatrix);
/** @} *//* end of name "Update Button Matrix" */

/**
 * @name			Get Button States
 * @anchor		GetStates
 * @brief			Use these functions to read states of specific buttons in the matrix.
 * @{
 */

/**
 * @brief			Check if a specific button is pushed
 *
 * @param     buttonMatrix: Pointer to the structure of the button matrix setup by the user
 * @param			buttonName: Object of buttonMatrix_Coordinates_Td type, that stores the coordinates of the specific button. Setup by user.
 * @return		true if the button is pushed, false if not.
 */
bool ButtonMatrix_check_ButtonPushed(buttonMatrix_structTd* buttonMatrix, buttonMatrix_Coordinates_Td* buttonName);

/**
 * @brief			Get a register that describes each buttons stat on a specific interrupt line
 * @param     buttonMatrix: Pointer to the structure of the button matrix setup by the user
 * @param			posIntLine: The number of the relevant interrupt line. Use the number that was returned by ButtonMatrix_init_InterruptLinePin().
 */
uint16_t ButtonMatrix_get_ButtonStatesRegisterOnInterruptLine(buttonMatrix_structTd* buttonMatrix, uint16_t posIntLine);
/** @} *//* end of name "Get Button States" */

/** @} *//* end of defgroup "ButtonMatrix_Header" */
/** @} *//* end of defgroup "ButtonMatrix" */

#endif /* INC_BUTTONMATRIX_H_ */
