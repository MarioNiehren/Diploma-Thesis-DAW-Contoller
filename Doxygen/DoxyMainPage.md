@mainpage	Main Page
@brief		This Library contains software that can be used to build a DAW-Controller with STM32-Microcontrollers.

# Included Software Parts
- @ref ButtonMatrix		"Button Matrix based on interrupts to handle many buttons efficient." <br>
- @ref EPaperSSD1681	"E-Paper B/W with SSD1681 Driver" <br>
- @ref MotorFader		"Motorized Fader (ALPS) with TB6612FNG Motor Driver for DC-Motors, STM32 Touch Sense Control and 12Bit ADC."
- @ref MAX7219			"LED Driver MAX7219 to handle a large number of LEDs"
- @ref RotaryEncoder	"Read rotary encoders based on interrupts"
- @ref MIDI_UART		"Send MIDI over UART (This module shows the principle but is no complete MIDI implementation)"

# How to use:

Each Softwarepart contains at least a Header module and a Source module as an own Topic. The Header module is the part of the software, that is actually used by the user.
All thr rest ist working in the backrgound. Make sure to include all required .h and .c files for the software you want to use. Even if you only work with the header file in the end,
all parts of a module are needed.
If a Softwarepart uses seperate modules, the user has to know (e. g. Motor fader), the modules ducumentation can be found directly on the Pafe of the Softwarepart. If the Softwarepart 
uses seperate modules, that are not directly relevant for the user, the modules documentation is "hidden" in the Source page.

# General structure

All modules are based on STM32 HAL. Usually the user has to declare an object of a specific typedef structure that is given in the header file. Next, a set of initialize functions
has to be called. After that a start function has to be called sometimes. If a module is interrupt based, a manage_Interrupt function has to be called in the assosiated interrupt
handler.
In the while loop, an update function has to be called periodically. Some do not have an update function, that means, that the processing runs on a DMA channel or in the 
background on the MCU (e.g. The PWM of the motor driver).
Set/Get functions allow to interact with the module in the code.
- For peripherals, HAL generates handlers. These handlers have to be linked with the initialize functions, so the modul can use it.
- For pins, that are not part of a HAL handler (usually GPIOs), the pins that are relevant for a module have to be linked with an init function.
- The microcontroller used to write the module is included in the header (e.g. #include "stm32l0xx_hal.h"). Often it should work to replace this, if an other STM32 model is used.
- If calibration functions exist: these functions can be used to improve the performansce. If they are not used, the corresponing parameters will work with default vales.

# Repository with the Code
@todo add link too github here

Each Softwarepart is set up as an individual STM32 Project. The relevant files can be found ProjectName->Core->Inc/Src. In main.c is a use example, that is not documented in Doxygen.
Please look at the main.c to the how a Softwarepart can be used.

# Disclaimer

Everything here is software, that has the intention to show, how the hardware works. There is no guarantee, that the code will work as expected and there are no Error-mechansims yet!
So do not use the code in critical environments.
All demo projects are testet and should work under the given conditions. If there are problems, please study the documentation and code first. Most problems are caused in the Cube MX settings,
wrong hardware wiring or typos.

# Contact

E-Mail: mario.niehren@tonmeister.de