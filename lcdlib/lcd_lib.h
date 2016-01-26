//*****************************************************************************
#ifndef LCD_LIB
#define LCD_LIB

#include <inttypes.h>
//Uncomment this if LCD 4 bit interface is used
//******************************************
#define LCD_4bit
//***********************************************

#define LCD_RS	0 	//define MCU pin connected to LCD RS
#define LCD_RW	1 	//define MCU pin connected to LCD R/W
#define LCD_E	2	//define MCU pin connected to LCD E
#define LCD_D4	4	//define MCU pin connected to LCD D3
#define LCD_D5	5	//define MCU pin connected to LCD D4
#define LCD_D6	6	//define MCU pin connected to LCD D5
#define LCD_D7	7	//define MCU pin connected to LCD D6
#define LDP PORTC	//define MCU port connected to LCD data pins
#define LCP PORTC	//define MCU port connected to LCD control pins
#define LDDR DDRC	//define MCU direction register for port connected to LCD data pins
#define LCDR DDRC	//define MCU direction register for port connected to LCD control pins

#define LCD_CLR             0	//DB0: clear display
#define LCD_HOME            1	//DB1: return to home position
#define LCD_ENTRY_MODE      2	//DB2: set entry mode
#define LCD_ENTRY_INC       1	//DB1: increment
#define LCD_ENTRY_SHIFT     0	//DB2: shift
#define LCD_ON_CTRL         3	//DB3: turn lcd/cursor on
#define LCD_ON_DISPLAY      2	//DB2: turn display on
#define LCD_ON_CURSOR       1	//DB1: turn cursor on
#define LCD_ON_BLINK        0	//DB0: blinking cursor
#define LCD_MOVE            4	//DB4: move cursor/display
#define LCD_MOVE_DISP       3	//DB3: move display (0-> move cursor)
#define LCD_MOVE_RIGHT      2	//DB2: move right (0-> left)
#define LCD_FUNCTION        5	//DB5: function set
#define LCD_FUNCTION_8BIT   4	//DB4: set 8BIT mode (0->4BIT mode)
#define LCD_FUNCTION_2LINES 3	//DB3: two lines (0->one line)
#define LCD_FUNCTION_10DOTS 2	//DB2: 5x10 font (0->5x7 font)
#define LCD_CGRAM           6	//DB6: set CG RAM address
#define LCD_DDRAM           7	//DB7: set DD RAM address
// reading:
#define LCD_BUSY            7	//DB7: LCD is busy
#define LCD_LINES			2	//visible lines
#define LCD_LINE_LENGTH		16	//line length (in characters)
// cursor position to DDRAM mapping
#define LCD_LINE0_DDRAMADDR		0x00
#define LCD_LINE1_DDRAMADDR		0x40
#define LCD_LINE2_DDRAMADDR		0x14
#define LCD_LINE3_DDRAMADDR		0x54
// progress bar defines
#define PROGRESSPIXELS_PER_CHAR	6

#define OPERATION_DELAY_MS 2


void lcd_dat(uint8_t);		//forms data ready to send to 74HC164
void lcd_com(uint8_t);	//forms data ready to send to 74HC164
void lcd_init(void);			//Initializes LCD
void lcd_clr(void);				//Clears LCD
void lcd_home(void);			//LCD cursor home
void lcd_string(uint8_t*, uint8_t);	//Outputs string to LCD
void lcd_gotoxy(uint8_t, uint8_t);	//Cursor to X Y position
void copy_string_to_lcd(const uint8_t*, uint8_t, uint8_t);//copies flash string to LCD at x,y
void lcd_definechar(const uint8_t *,uint8_t);//write char to LCD CGRAM 
void lcd_shift_right(uint8_t);	//shift by n characters Right
void lcd_shift_left(uint8_t);	//shift by n characters Left
void lcd_cursor_on(void);		//Underline cursor ON
void lcd_cursor_blink(void);	//Underline blinking cursor ON
void lcd_cursor_off(void);		//Cursor OFF
void lcd_blank(void);			//LCD blank but not cleared
void lcd_visible(void);			//LCD visible
void lcd_cursor_left(uint8_t);	//Shift cursor left by n
void lcd_cursor_right(uint8_t);	//shif cursor right by n
void lcd_progress_bar(uint8_t progress, uint8_t maxprogress, uint8_t length);

#endif

