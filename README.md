# Welcome to the DAW-Controller development library

This Project contains several software modules for some components that are required to
develop a custom DAW-Controller.

## Currently available modules:

| Name													| Description										| MCU used in example				|
| ----------------------------------------------------- | ------------------------------------------------- | --------------------------------- |
| [Button Matrix](F103C8T6_DemoProject_ButtonMatrix)	| Setup and read an interrupt based button matrix 	| STM32F103C8T6 ("Blue-Pill" board)	|
| [E-Paper](F103C8T6_DemoProject_E-Paper_SSD1681)		| Setup and write / draw to an E-Paper Display		| STM32F103C8T6 ("Blue-Pill" board) |
| [Motorized Fader](L053R8T6_DemoProject_MotorizedFader)| Setup a and use a Motorized fader with:			| STM32L053R8T6 (Nucleo Board)		|
														| TB6612FNG Motor Driver, 							|
														| ADC, 												|
| [Motorized Fader](L053R8T6_DemoProject_MotorizedFader)| Setup a and use a Motorized fader with:			| STM32L053R8T6 (Nucleo Board)		|

## Documentation
To read the Doxygen documentation, please copy the [Doxygen HTML-folder](Doxygen/HTML/html) to your local system 
and open index.html inside this folder with any browser.