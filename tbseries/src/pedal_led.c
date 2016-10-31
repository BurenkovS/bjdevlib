/*
 * BJ Devices Travel Box series midi controller library
 * @file	pedal_led.c
 * 
 * @brief	Build-in expression pedal LEDs control functions
 *
 * Software is provided "as is" without express or implied warranty.
 * BJ Devices 2016
 */
 
 #include "pedal_led.h"
 #include "lldled.h"

 #include <avr/pgmspace.h>

 static const uint8_t ledsNumTable[PEDAL_LEDS_NUM] PROGMEM =
		{    45,46,47, 42,43,44, 39,40,41, 36,37,38, 33,34,35, 30,31,32, 27,28,29, 24,25,26
			,0,1,2,    3,4,5,    6,7,8,    9,10,11,  12,13,14, 15,16,17, 18,19,20, 21,22,23};

/*
 * prepare led interface
 */
static uint8_t ledBuffer[PEDAL_REGS_NUM];

static const ioPort clkPort = {&PEDAL_LED_CLK_PORT, PEDAL_LED_CLK_PIN};
static const ioPort dataPort = {&PEDAL_LED_DATA_PORT, PEDAL_LED_DATA_PIN};
static const ioPort oePort = {&PEDAL_LED_OE_PORT, PEDAL_LED_OE_PIN};

//TODO place it to PROGMEM
static RegsChainDescriptor chainDescriptors;

void initPedalLed()
{
	initOutput(&clkPort, 0);
	initOutput(&dataPort, 0);
	initOutput(&oePort, 0);
	
	chainDescriptors.ledsNum_ = PEDAL_LEDS_NUM;
	chainDescriptors.regsNum_ = PEDAL_REGS_NUM;
	chainDescriptors.clk_ = &clkPort;
	chainDescriptors.data_ = &dataPort;
	chainDescriptors.oe_ = &oePort;
	chainDescriptors.buffer_ = ledBuffer;
	chainDescriptors.ledNumTable_ = (uint8_t*)&ledsNumTable;//data in PROGMEM!!
}

void ledPedalSend()
{
	lldLedSend(&chainDescriptors);
}

void ledSetPedalColor(uint8_t ledNum, PedalLedColor color, bool send)
{
	uint8_t phyLedNum = ledNum*3;
	switch(color)
	{
		case PEDAL_COLOR_NO :
			lldLedSetVal(&chainDescriptors, phyLedNum, LED_OFF);
			lldLedSetVal(&chainDescriptors, phyLedNum + 1, LED_OFF);
			lldLedSetVal(&chainDescriptors, phyLedNum + 2, LED_OFF);
			break;

        case PEDAL_COLOR_R :
			lldLedSetVal(&chainDescriptors, phyLedNum, LED_ON);
			lldLedSetVal(&chainDescriptors, phyLedNum + 1, LED_OFF);
			lldLedSetVal(&chainDescriptors, phyLedNum + 2, LED_OFF);
			break;
        
        case PEDAL_COLOR_G :
			lldLedSetVal(&chainDescriptors, phyLedNum, LED_OFF);
			lldLedSetVal(&chainDescriptors, phyLedNum + 1, LED_ON);
			lldLedSetVal(&chainDescriptors, phyLedNum + 2, LED_OFF);
			break;
        
        case PEDAL_COLOR_B :
			lldLedSetVal(&chainDescriptors, phyLedNum, LED_OFF);
			lldLedSetVal(&chainDescriptors, phyLedNum + 1, LED_OFF);
			lldLedSetVal(&chainDescriptors, phyLedNum + 2, LED_ON);
			break;
        
        case PEDAL_COLOR_RG :
			lldLedSetVal(&chainDescriptors, phyLedNum, LED_ON);
			lldLedSetVal(&chainDescriptors, phyLedNum + 1, LED_ON);
			lldLedSetVal(&chainDescriptors, phyLedNum + 2, LED_OFF);
			break;
        
        case PEDAL_COLOR_RB :
			lldLedSetVal(&chainDescriptors, phyLedNum, LED_ON);
			lldLedSetVal(&chainDescriptors, phyLedNum + 1, LED_OFF);
			lldLedSetVal(&chainDescriptors, phyLedNum + 2, LED_ON);
			break;

        case PEDAL_COLOR_GB :
			lldLedSetVal(&chainDescriptors, phyLedNum, LED_OFF);
			lldLedSetVal(&chainDescriptors, phyLedNum + 1, LED_ON);
			lldLedSetVal(&chainDescriptors, phyLedNum + 2, LED_ON);
			break;

        case PEDAL_COLOR_RGB :
			lldLedSetVal(&chainDescriptors, phyLedNum, LED_ON);
			lldLedSetVal(&chainDescriptors, phyLedNum + 1, LED_ON);
			lldLedSetVal(&chainDescriptors, phyLedNum + 2, LED_ON);
			break;
				
		
		default : break;
	}
	if(send)
	ledPedalSend();
}

void ledSetPedalColorAll(PedalLedColor color, bool send)
{
    unsigned char i;
    for (i = 0; i < PEDAL_TRICOLOR_VIRTUAL_LEDS_NUM; ++i)
		ledSetPedalColor(i, color, false);
    
    if(send)
		ledPedalSend();
}



 
  
