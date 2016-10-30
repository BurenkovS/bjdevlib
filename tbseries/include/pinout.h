/*
 * BJ Devices Travel Box series midi controller library
 * @file	pinout.h
 * 
 * @brief	ATmega pins location and shift registers description.
 *			Supports all TB series midi controllers modification via global defines.
			User must define controller modification in any source file or in project properties.
			Following devices are supported
			TB_12_DEVICE  - TB-12 midi controller
			TB_5_DEVICE  - TB-5 midi controller
			TB_8_DEVICE  - TB-8 midi controller
			TB_11P_DEVICE  - TB-11P midi controller with expression pedal on board
			TB_6P_DEVICE  - TB-6P midi controller with expression pedal on board
			  
 *
 * Software is provided "as is" without express or implied warranty.
 * BJ Devices 2015
 */ 


#ifndef pinout_h_
#define pinout_h_

//Buttons

#ifdef TB_12_DEVICE
#	define FOOT_BUTTONS_NUM	12

#elif defined (TB_11P_DEVICE)
#	define FOOT_BUTTONS_NUM	11

#elif defined (TB_6P_DEVICE)
#	define FOOT_BUTTONS_NUM	6

#elif defined (TB_8_DEVICE)
#	define FOOT_BUTTONS_NUM	8

#elif defined (TB_5_DEVICE)
#	define FOOT_BUTTONS_NUM	5
#endif 

#define CONF_BUTTONS_NUM	6

#define KEY_1_PORT	PORTA
#define KEY_1_PIN	2

#define KEY_2_PORT	PORTA
#define KEY_2_PIN	1

#define KEY_3_PORT	PORTG
#define KEY_3_PIN	2

#define KEY_4_PORT	PORTA
#define KEY_4_PIN	7

#define KEY_5_PORT	PORTA
#define KEY_5_PIN	6

#define KEY_6_PORT	PORTA
#define KEY_6_PIN	5

#define KEY_7_PORT	PORTC
#define KEY_7_PIN	3

#define KEY_8_PORT	PORTG
#define KEY_8_PIN	1

#define KEY_9_PORT	PORTG
#define KEY_9_PIN	0

#define KEY_10_PORT	PORTD
#define KEY_10_PIN	7

#define KEY_11_PORT	PORTD
#define KEY_11_PIN	6

#define KEY_12_PORT	PORTD
#define KEY_12_PIN	5

#define KEY_UNDER_PEDAL_PORT	PORTB
#define KEY_UNDER_PEDAL_PIN		5


//Common for all models
#define KEY_INC_PORT	PORTG
#define KEY_INC_PIN		4

#define KEY_DEC_PORT	PORTG
#define KEY_DEC_PIN		3

#define KEY_LOAD_PORT	PORTD
#define KEY_LOAD_PIN	0

#define KEY_UP_PORT		PORTD
#define KEY_UP_PIN		1

#define KEY_SETUP_PORT	PORTD
#define KEY_SETUP_PIN	4

#define KEY_DOWN_PORT	PORTB
#define KEY_DOWN_PIN	7

//expression pedals pins
#define EXP_P1_PORT	PORTF
#define EXP_P1_PIN	3

#define EXP_P2_PORT	PORTF
#define EXP_P2_PIN	3

#define EXP_P_OB_PORT	PORTF
#define EXP_P_OB_PIN	6



//On board LEDs shift register pin numbers
#define LED1_R	6 
#define LED2_R	4
#define LED3_R	2
#define LED4_R	0
#define LED5_R	14
#define LED6_R	12
#define LED7_R	10
#define LED8_R	8
#define LED9_R	22
#define LED10_R	20
#define LED11_R  18
#define LED12_R  16

#define LED1_G   7
#define LED2_G   5
#define LED3_G   3
#define LED4_G   1
#define LED5_G   15
#define LED6_G   13
#define LED7_G   11
#define LED8_G   9
#define LED9_G   23
#define LED10_G  21
#define LED11_G  19
#define LED12_G  17

//shift registers and LEDS description
#define LEDS_NUM FOOT_BUTTONS_NUM
#define COLORS_PER_LED 2
#define TOTAL_LED_PINS (LEDS_NUM*COLORS_PER_LED)
#define REGS_NUM 3

#define LED_CLK_PORT	PORTA
#define LED_CLK_PIN		4

#define LED_DATA_PORT	PORTA
#define LED_DATA_PIN	3

#define LED_OE_PORT		PORTF
#define LED_OE_PIN		5

#endif /* pinout_h_ */