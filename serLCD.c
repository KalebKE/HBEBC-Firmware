/******************************************************
 * Fucntions for contolling the LED display and strings
 * related to the water/methanol injection controler. 	
 *
 ******************************************************/
#include <avr/io.h>
#include <util/delay.h>
#include "uart.h"
#include "serLCD.h"
#include "rprintf.h"

// clears the lcd display of all characters
void lcdClearDisplay(void)            
{
	uartSendByte(LCD_COMMAND_INIT);
	uartSendByte(LCD_CLEAR_DISPLAY);
}

// moves the lcd cursor one space to the right
void lcdMoveRightOne(void)
{
	uartSendByte(LCD_COMMAND_INIT);
	uartSendByte(LCD_MOVE_CURSOR_RIGHT_ONE);
}

// moves the lcd cursor one space to the left
void lcdMoveLeftOne(void)
{
	uartSendByte(LCD_COMMAND_INIT);
	uartSendByte(LCD_MOVE_CURSOR_LEFT_ONE);
}

// scrolls entire message one space to the right
void lcdScrollRight(void)
{
	uartSendByte(LCD_COMMAND_INIT);
	uartSendByte(LCD_SCROLL_RIGHT);
}

// scrolls entire meassge one psace to the left
void lcdScrollLeft(void)
{
	uartSendByte(LCD_COMMAND_INIT);
	uartSendByte(LCD_SCROLL_LEFT);
}

// turns lcd display on
void lcdTurnOn(void)
{
	uartSendByte(LCD_COMMAND_INIT);
	uartSendByte(LCD_TURN_DISPLAY_ON);
}

// turns lcd display off
void lcdTurnOff(void)
{
	uartSendByte(LCD_COMMAND_INIT);
	uartSendByte(LCD_TURN_DISPLAY_OFF);
}

// turns on the underline cursor
void lcdUnderlineCursorOn(void)
{
	uartSendByte(LCD_COMMAND_INIT);
	uartSendByte(LCD_UNDERLINE_CURSOR_ON);
}

// turns off the underline cursor
void lcdUnderlineCursorOff(void)
{
	uartSendByte(LCD_COMMAND_INIT);
	uartSendByte(LCD_UNDERLINE_CURSOR_OFF);
}

// turns on the blinking cursor
void lcdBlinkingCursorOn(void)
{
	uartSendByte(LCD_COMMAND_INIT);
	uartSendByte(LCD_BOX_CURSOR_ON);
}

// turns off the bliking cursor
void lcdBlinkingCursorOff(void)
{
	uartSendByte(LCD_COMMAND_INIT);
	uartSendByte(LCD_BOX_CURSOR_OFF);
}

// sets the cursor position
void lcdSetCursorPosition(u08 position)
{
	uartSendByte(LCD_COMMAND_INIT);
	uartSendByte(BASE_CURSOR_POSITION+(position * 4));
}

// displays a string of characters
void lcdSendString(const u08 *string)
{
	u08 ch;
	
	while((ch = *string))
	{
		uartSendByte(ch);
		string++;
	}	
}

// displays boost pressure in standard units
void lcdPrintStandBoostPressure(float standBoostPressure)
{
	lcdClearDisplay();
	lcdSendString("Boost Pressure:");
	lcdNewLine();
	lcdSendString("   ");
	lcdPrintFloat(standBoostPressure);
	lcdSendString("  PSi"); 
}

// displays water pressure in standard units
void lcdPrintStandWaterPressure(float standWaterPressure)
{
	lcdClearDisplay();
	lcdSendString("Water Pressure:");
	lcdNewLine();
	lcdSendString("   ");
	lcdPrintFloat(standWaterPressure);
	lcdSendString("  PSi"); 
}

// displays water volume in standard units
void lcdPrintStandWaterVolume(float standVolume)
{
	lcdClearDisplay();
	lcdSendString(" Water Volume:");
	lcdNewLine();
	lcdSendString("   ");
	lcdPrintFloat(standVolume);
	lcdSendString("  Gph"); 
}


// displays boost pressure in metric units
void lcdPrintMetricBoostPressure(float metricBoostPressure)
{
	lcdClearDisplay();
	lcdSendString("Boost Pressure:");
	lcdNewLine();
	lcdSendString("   ");
	lcdPrintFloat(metricBoostPressure);
	lcdSendString(" BAR"); 
}

// displays boost pressure in metric units
void lcdPrintMetricWaterPressure(float metricWaterPressure)
{
	lcdClearDisplay();
	lcdSendString("Water Pressure:");
	lcdNewLine();
	lcdSendString("   ");
	lcdPrintFloat(metricWaterPressure);
	lcdSendString(" BAR"); 
}

// displays boost pressure in metric units
void lcdPrintMetricWaterVolume(float metricVolume)
{
	lcdClearDisplay();
	lcdSendString(" Water Volume:");
	lcdNewLine();
	lcdSendString("   ");
	lcdPrintFloat(metricVolume);
	lcdSendString(" ccPM"); 
}

// displays boost pressure in metric units
void lcdPrintAccelAxis(u08 axis, float gravities)
{

	lcdClearDisplay();
	switch(axis)
	{
		// x axis
		case 0:
			lcdSendString(" Forward Accel:");
			break;

		case 1: 
			lcdSendString(" Lateral Accel:");
			break;
		case 2: 
		lcdSendString("   Incline:");
		break;
	
			
	}
	lcdNewLine();
	lcdSendString("   ");
	lcdPrintFloat(gravities);
	lcdSendString(" G's");
 
}

// prints intro welcome screen
void lcdWelcomeScreen(void)
{
	lcdClearDisplay();
	lcdBlinkingCursorOn();
    _delay_ms(1000);
    lcdSendString(" ");
	_delay_ms(100);
	lcdSendString(" ");
	_delay_ms(100);
	lcdSendString(" ");
	_delay_ms(100);
	lcdSendString(" ");
	_delay_ms(100);
	lcdSendString("H");
	_delay_ms(100);
	lcdSendString("e");
	_delay_ms(100);
	lcdSendString("l");
	_delay_ms(100);
	lcdSendString("l");
	_delay_ms(100);
	lcdSendString("o");
	_delay_ms(100);
	lcdSendString("!");
	_delay_ms(1000);
	lcdNewLine();
	_delay_ms(1000);
	lcdSendString(" ");
	_delay_ms(100);
	lcdSendString(" ");
	_delay_ms(100);
	lcdSendString("D");
	_delay_ms(100);
	lcdSendString("B");
	_delay_ms(100);
	lcdSendString("W");
	_delay_ms(100);
	lcdSendString("M");
	_delay_ms(100);
	lcdSendString("C");
	_delay_ms(100);
	lcdSendString(" ");
	_delay_ms(100);
	lcdSendString("v");
	_delay_ms(100);
	lcdSendString("1");
	_delay_ms(100);
	lcdSendString(".");
	_delay_ms(100);
	lcdSendString("0");
	_delay_ms(2000);
	lcdBlinkingCursorOff(); 
	
	lcdClearDisplay();
	lcdSendString("  Boost Guage");
	_delay_ms(1500);
	lcdClearDisplay();
	lcdSendString(" W/M Controller");
	_delay_ms(1500);
	lcdClearDisplay();
	lcdSendString(" Accelerometer");
	_delay_ms(1500);
}

// displays floats
void lcdPrintFloat(float x)
{
	rprintfFloat(4, x);
}

void lcdNewLine(void)
{
	lcdSetCursorPosition(16);			/* The 16th character is the first character on the second row if the first character on row 1 is character 0 */
}

void lcdMenuOptions(u08 mode)
{
	lcdClearDisplay();
	switch(mode)
	{
		case 0:
			lcdSendString("  Set Desired");
			lcdNewLine();
			lcdSendString("     Boost");
			break;
		
		case 1:
			lcdSendString(" Set Boost Gains");
			break;

		case 2:
			lcdSendString("Set Desired W/M");
			lcdNewLine();
			lcdSendString("  Pressure");
			break;
		
		case 3:
			lcdSendString(" Set W/M on ");
			lcdNewLine();
			lcdSendString("    Boost");
			break;

		case 4:
			lcdSendString(" Set Boost Gains");
			break;
	}
}

void lcdEditValue(u08 mode, float val)
{
	static float oldVal;

	if(val == oldVal)
	{
		return;
	}

	lcdClearDisplay();
	switch(mode)
	{
		case 0:
			lcdSendString("Desired Boost:");
			lcdNewLine();
			lcdSendString("   ");
			lcdPrintFloat(val);
			lcdSendString("  PSi"); 
			break;
		
		case 1:
			lcdSendString("Desired Ki:");
			lcdNewLine();
			lcdSendString("   ");
			lcdPrintFloat(val);
			lcdSendString("  PSi"); 
			break;

		case 2:
			lcdSendString("Desired Pressure:");
			lcdNewLine();
			lcdSendString("   ");
			lcdPrintFloat(val);
			lcdSendString("  PSi"); 
			break;
		
		case 3:
			lcdSendString("Desired on Boost:");
			lcdNewLine();
			lcdSendString("   ");
			lcdPrintFloat(val);
			lcdSendString("  PSi"); 
			break;

		case 4:
			
			break;
	}
	
	oldVal = val;
}
