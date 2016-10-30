/*
 * BJ Devices Travel Box series midi controller library
 * @file	led.c
 * 
 * @brief	Control bi-colored LEDs near the foot buttons
 *
 * Software is provided "as is" without express or implied warranty.
 * BJ Devices 2016
 */ 

#include "led.h"
#include "lldled.h"

#include <avr/pgmspace.h>

#ifdef TB_12_DEVICE
static const uint8_t ledsNumTable[TOTAL_LED_PINS] PROGMEM =  
		{LED1_G, LED1_R, LED2_G, LED2_R, LED3_G, LED3_R, LED4_G, LED4_R, LED5_G, LED5_R, LED6_G, LED6_R,
		LED7_G, LED7_R, LED8_G, LED8_R, LED9_G, LED9_R, LED10_G, LED10_R, LED11_G, LED11_R, LED12_G, LED12_R};
		
#elif defined (TB_11P_DEVICE)
static const uint8_t ledsNumTable[TOTAL_LED_PINS] PROGMEM = 
		{LED1_G, LED1_R, LED2_G, LED2_R, LED3_G, LED3_R, LED5_G, LED5_R, LED6_G, LED6_R , LED7_G, LED7_R
		,LED8_G, LED8_R, LED9_G, LED9_R, LED10_G, LED10_R, LED11_G, LED11_R
		,LED4_G, LED4_R};//Dummy leds

#elif defined (TB_5_DEVICE)
static const uint8_t ledsNumTable[TOTAL_LED_PINS] PROGMEM = 
		{LED1_G, LED1_R, LED2_G, LED2_R, LED3_G, LED3_R, LED6_G, LED6_R, LED4_G, LED4_R};

#elif defined (TB_6P_DEVICE)
		static const uint8_t ledsNumTable[TOTAL_LED_PINS] PROGMEM =
		{LED1_G, LED1_R, LED2_G, LED2_R, LED3_G, LED3_R, LED6_G, LED6_R, LED4_G, LED4_R
		,LED8_G, LED8_R};//Dummy leds

#elif defined (TB_8_DEVICE)
		static const uint8_t ledsNumTable[TOTAL_LED_PINS] PROGMEM =
		{LED2_G, LED2_R, LED3_G, LED3_R, LED5_G, LED5_R, LED6_G, LED6_R, LED7_G, LED7_R
		,LED8_G, LED8_R, LED10_G, LED10_R, LED11_G, LED11_R};

#endif

/*
 * prepare led interface
 */
static uint8_t ledBuffer[REGS_NUM];

static const ioPort clkPort = {&LED_CLK_PORT, LED_CLK_PIN};
static const ioPort dataPort = {&LED_DATA_PORT, LED_DATA_PIN};
static const ioPort oePort = {&LED_OE_PORT, LED_OE_PIN};

//TODO place it to PROGMEM
static RegsChainDescriptor chainDescriptors;

void initLed()
{
	initOutput(&clkPort, 0);
	initOutput(&dataPort, 0);
	initOutput(&oePort, 0);
	
	chainDescriptors.ledsNum_ = TOTAL_LED_PINS;
	chainDescriptors.regsNum_ = REGS_NUM;
	chainDescriptors.clk_ = &clkPort;
	chainDescriptors.data_ = &dataPort;
	chainDescriptors.oe_ = &oePort;
	chainDescriptors.buffer_ = ledBuffer;
	chainDescriptors.ledNumTable_ = (uint8_t*)&ledsNumTable;//data in PROGMEM!!
}

void ledSetColor(uint8_t ledNum, LedColor color, bool send)
{
    uint8_t phyLedNum = ledNum*2;//Number of GREEN physical led. Corresponding RED led will have phyLedNum+1
    switch(color)
    {
	    case COLOR_BLACK :
			lldLedSetVal(&chainDescriptors, phyLedNum, LED_OFF);
			lldLedSetVal(&chainDescriptors, phyLedNum + 1, LED_OFF);
			break;
	    
	    case COLOR_GREEN :
			lldLedSetVal(&chainDescriptors, phyLedNum, LED_ON);
			lldLedSetVal(&chainDescriptors, phyLedNum + 1, LED_OFF);
			break;
	    
	    case COLOR_RED :
			lldLedSetVal(&chainDescriptors, phyLedNum, LED_OFF);
			lldLedSetVal(&chainDescriptors, phyLedNum + 1, LED_ON);
			break;
	    
	    case COLOR_YELLOW :
			lldLedSetVal(&chainDescriptors, phyLedNum, LED_ON);
			lldLedSetVal(&chainDescriptors, phyLedNum + 1, LED_ON);
			break;
	    
	    default : break;
	}
	if(send)
		ledSend();	
}

void ledSetColorAll(LedColor color, bool send)
{
	uint8_t i;
	for (i = 0; i < LEDS_NUM; ++i)
		ledSetColor(i, color, false);

	if(send)
		ledSend();	
}

void ledSend()
{
	lldLedSend(&chainDescriptors);
}
