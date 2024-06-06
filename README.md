# Welcome to the DAW-Controller development library

This Project contains several software modules for some components that are required to
develop a custom DAW-Controller.

## Currently available modules:

| Name													| Description										| MCU used in example				|
| ----------------------------------------------------- | ------------------------------------------------- | --------------------------------- |
| [Button Matrix](F103C8T6_DemoProject_ButtonMatrix)	| Setup and read an interrupt based button matrix 	| STM32F103C8T6 ("Blue-Pill" board)	|
| [E-Paper](F103C8T6_DemoProject_E-Paper_SSD1681)		| Setup and write / draw to an E-Paper Display		| STM32F103C8T6 ("Blue-Pill" board) |
| [Motorized Fader](L053R8T6_DemoProject_MotorizedFader)| Setup a and use a Motorized fader with: <ul><li>ADC using DMA</li><li>TB6612FNG motor driver with interrupt based PWM</li><li>PID with anti wind up and low pass for the D Term</li><li>Touch Sensor using STM32 TSC Control (no STM middleware used here.)</li></ul> 			| STM32L053R8T6 (Nucleo Board)		|

## Documentation
To read the Doxygen documentation, please copy the [Doxygen HTML-folder](Doxygen/HTML/html) to your local system 
and open index.html inside this folder with any browser.

 <table>
  <tr>
    <th>Name</th>
    <th>Description</th>
    <th>MCU used in example	</th>
  </tr>
  <tr>
    <td>[Button Matrix](F103C8T6_DemoProject_ButtonMatrix)</td>
    <td>Setup and read an interrupt based button matrix</td>
    <td>STM32F103C8T6 ("Blue-Pill" board)</td>
  </tr>
  <tr>
    <td>[E-Paper](F103C8T6_DemoProject_E-Paper_SSD1681)</td>
    <td>Setup and write / draw to an E-Paper Display</td>
    <td>STM32F103C8T6 ("Blue-Pill" board)</td>
  </tr>
  <tr>
    <td>[Motorized Fader](L053R8T6_DemoProject_MotorizedFader)</td>
    <td>Setup a and use a Motorized fader with: 
		<ul>
			<li>ADC using DMA</li>
			<li>TB6612FNG motor driver with interrupt based PWM</li>
			<li>PID with anti wind up and low pass for the D Term</li>
			<li>Touch Sensor using STM32 TSC Control (no STM middleware used here.)</li>
		</ul>
	</td>
    <td>STM32L053R8T6 (Nucleo Board)</td>
  </tr>
</table> 

| TABLE  | NUMBER | RELATIONSHIPS |
| ------ | ------ | ------------- |
| TABLEA | NUM001 | TABLE2        | 
|        |        | TABLE4        |
|        |        | TABLE7        |
| TABLEB | NUM002 | TABLEA        |