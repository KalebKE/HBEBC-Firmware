


#ifndef SERLCD_H
#define SERLCD_H

#define LCD_COMMAND_INIT			(0xFE)
#define LCD_CLEAR_DISPLAY			(0x01)
#define LCD_MOVE_CURSOR_RIGHT_ONE   (0x14)
#define LCD_MOVE_CURSOR_LEFT_ONE	(0x10)
#define LCD_SCROLL_RIGHT			(0x1C)
#define LCD_SCROLL_LEFT				(0x18)
#define LCD_TURN_DISPLAY_ON 		(0x0C)
#define LCD_TURN_DISPLAY_OFF 		(0x08)
#define LCD_UNDERLINE_CURSOR_ON		(0x0E)
#define LCD_UNDERLINE_CURSOR_OFF    (0x0C)
#define LCD_BOX_CURSOR_ON 		(0x0D)
#define LCD_BOX_CURSOR_OFF   (0x0C)
#define BASE_CURSOR_POSITION		(0x80) 

void lcdClearDisplay(void);
void lcdMoveRightOne(void);
void lcdMoveLeftOne(void);
void lcdScrollRight(void);
void lcdScrollLeft(void);
void lcdTurnOn(void);
void lcdTurnOff(void);
void lcdUnderlineCursorOn(void);
void lcdUnderlineCursorOff(void);
void lcdBlinkingCursorOn(void);
void lcdBlinkingCursorOff(void);
void lcdSetCursorPosition(u08 position);
void lcdSendString(const u08 *string);
void lcdPrintStandBoostPressure(float standBoostPressure);
void lcdPrintStandWaterPressure(float standWaterPressure);
void lcdPrintStandWaterVolume(float standVolume);
void lcdPrintMetricBoostPressure(float metricBoostPressure);
void lcdPrintMetricWaterPressure(float metricWaterPressure);
void lcdPrintMetricWaterVolume(float metricVolume);
void lcdPrintAccelAxis(u08 axis, float gravities);
void lcdPrintFloat(float x);
void lcdNewLine(void);
void lcdWelcomeScreen(void);
void lcdMenuOptions(u08 mode);
void lcdEditValue(u08 mode, float val);


#endif /* SERLCD_H */
