#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <avr/eeprom.h>
#include "uart.h"
#include "serLCD.h"
#include "rprintf.h"
#include "a2d.h"
#include "timer.h"
#include "boostControl.h"
#include "waterControl.h"
#include "accel.h"

#define S_G				0x40
#define S_R 			0x80

typedef struct
{
volatile	u08 mainMode;
volatile	u08 displayMode;
volatile	u08 numDisplayModes;
volatile	u08 menuMode;		
volatile	u08 numMenuModes;
volatile	u08 units;
volatile	u08 editVal;
volatile	float val;
}mainCtrl;


//GLOBAL EEPROM STUFF//

u32   EEMEM initialBoot;
float EEMEM userBoost;
float EEMEM userBoostKp;
float EEMEM userBoostKi;
float EEMEM userBoostKd;
float EEMEM userPressure;
float EEMEM userOnBoost;
float EEMEM userWMKp;
float EEMEM userWMKi;
float EEMEM userWMKd;




void initProcessor(mainCtrl *master);

void initEEPROM();

void checkButtons(mainCtrl *master, boostCtrl *bstCtrl, waterMethCtrl *wmCtrl);

void displayData(mainCtrl *master, boostCtrl *bstCtrl, waterMethCtrl *wmCtrl, TriAx *accel);

void editParameter(mainCtrl *master, boostCtrl *bstCtrl, waterMethCtrl *wmCtrl);

void systemCounter();
volatile u32 systemCount;					//Global system count

float abs(float x);


typedef struct 
{
	u08 id;
}button;


button scrollUpButton;
button scrollDownButton;
button selectButton;
button returnButton;


void initButtons(void)
{
	scrollUpButton.id = 0x01;
	scrollDownButton.id = 0x02;
	selectButton.id = 0x80;
	returnButton.id = 0x40;
}

u08 getButtonState(button *sw)
{
	u08 state1, state2;
	state1 = (PINC & sw->id);
	if(state1 == 0)
		return 255;
	else
		return 0;	
}


ISR(BADISR_vect)
{
 asm("nop");   // user code here
}

int main(void)
{
	u08 mode;
	boostCtrl bstCtrl;
	waterMethCtrl wmCtrl;
	mainCtrl master;
	TriAx accel;

	initProcessor(&master);									// initialize the processor
	// initialize sensors
	initBoost(&bstCtrl);
	initWM(&wmCtrl);	
	lcdWelcomeScreen();
	
	mode = 255;

	while(1)
	{
		// check button state
		checkButtons(&master, &bstCtrl, &wmCtrl);
		
		if(master.editVal)
		{
			lcdEditValue(master.menuMode, master.val);
			mode = 255;
		}

		// update display
		if(!master.mainMode)
		{
			
			// update sensors
			readAccel(&accel);
			readBoost(&bstCtrl);
			readWM(&wmCtrl);
			wmCtrl.boost = bstCtrl.boost;

			// update controllers
			controlBoost(&bstCtrl);
			controlWM(&wmCtrl);

			// display data
			displayData(&master, &bstCtrl, &wmCtrl, &accel);	
		}


		else
		{
			if(((mode != master.menuMode) && (!master.editVal)))
			{
				lcdMenuOptions(master.menuMode);
				mode = master.menuMode;
			}
		}

	}

	
	return 0;
}


void initProcessor(mainCtrl *master)
{
	u32 val;

	uartInit();												// initialize UART
	a2dInit();												// initialize A2D
	timerInit();											// initialize timer
	sbi(DDRD, PD5);											// set PD5 to PWM function
	sbi(DDRD, PD4);											// set PD4 to PWM function
	timer1PWMInitICR(PWM_SCALER);							// set PWM period
	rprintfInit(uartSendByte);								// initialize float printing
	initButtons();											// initialize buttons
	timer1PWMAOn();											// turn on PWMA
	timer1PWMASet(0);										// set PWMA to 0% duty cycle
	timer1PWMBOn();											// turn on PWMB
	timer1PWMBSet(0);										// set PWMB to 0% duty cycle

	timerAttach(0, systemCounter);

	DDRC &= 0x00;											// set PC0-PC7 as button inputs 
	DDRB |= 0xFF;	 										// set PB-PB7 as outputs
	asm ("nop");
	PORTB = 0x00;											// turn off all lights
	PORTB |= S_R + S_G;										// turn on status lights	

	master->mainMode = 0;									// reset main mode
	master->displayMode = 0;								// reset dislplay mode
	master->numDisplayModes = 7;							// set number of display modes
	master->menuMode = 0;									// reset menu mode
	master->numMenuModes = 4;
	master->units = 0;
	master->editVal = 0;

	eeprom_read_block((void*)&val, (const void*)&initialBoot, 4);

	if(val != 0x0F2010F0)
	{
		initEEPROM();
	}
}

void initEEPROM()
{
	u08 val08;
	u16 val16;
	u32 val32;
	float valf;

	val32 = 0x0F2010F0;
	eeprom_write_block((const void*)&val32, (void*)&initialBoot, 4);

	valf = 10.0;
	eeprom_write_block((const void*)&valf, (void*)&userBoost, 4);

	valf = 2.5;
	eeprom_write_block((const void*)&valf, (void*)&userBoostKp, 4);

	valf = 0.001;
	eeprom_write_block((const void*)&valf, (void*)&userBoostKi, 4);

	valf = 0.0001;
	eeprom_write_block((const void*)&valf, (void*)&userBoostKd, 4);

	valf = 100.0;
	eeprom_write_block((const void*)&valf, (void*)&userPressure, 4);

	valf = 1.0;
	eeprom_write_block((const void*)&valf, (void*)&userOnBoost, 4);
	
	valf = 0.1;
	eeprom_write_block((const void*)&valf, (void*)&userWMKp, 4);

	valf = 0.001;
	eeprom_write_block((const void*)&valf, (void*)&userWMKi, 4);

	valf = 0.0001;
	eeprom_write_block((const void*)&valf, (void*)&userWMKd, 4);

}


void checkButtons(mainCtrl *master, boostCtrl *bstCtrl, waterMethCtrl *wmCtrl)
{
	static u08 recentPress = 0;
	static u32 pressCount = 0; 
	static u08 hold = 0;	

	if(recentPress)
	{
		pressCount = systemCount;
		recentPress = 0;
		hold = 1;
	}

	if((((systemCount - pressCount) < 2000) && hold))
	{
		return;
	}
	else
	{
		hold = 0;
	}

	// switch from display to menu mode
	if(getButtonState(&selectButton))
	{
		recentPress = 1;
		master->mainMode = 1;		

		master->displayMode = 0;
		master->menuMode = 0;
		return;		
	}

	// scroll through menu and display modes
	if(getButtonState(&scrollUpButton))
	{
		recentPress = 1;

		if(master->editVal)
		{	
			if(master->menuMode == 2)
			{
				master->val += 2;
			}
			else if(master->menuMode == 1)
			{
				master->val *= 10;
			}

			else
			{
				master->val += 0.1;
			}
			return;
		}

		// in edit parameter mode 
		if(master->mainMode)
		{
			if(master->menuMode < (master->numMenuModes - 1))
			{
				master->menuMode++;
			}
		}

		else
		{
			if(master->displayMode < (master->numDisplayModes - 1))
			{
				master->displayMode++;
			}
			
		}
		return;
	}

	if(getButtonState(&scrollDownButton))
	{
		recentPress = 1;

		if(master->editVal)
		{	
			if(master->menuMode == 2)
			{
				master->val -= 2;
			}
			else if(master->menuMode == 1)
			{
				master->val /= 10;
			}
			else
			{
				master->val -= 0.1;
			}
			return;
		}

		// in edit parameter mode 
		if(master->mainMode)
		{
			if(master->menuMode > 0)
			{
				master->menuMode--;
			}
		}

		else
		{
			if(master->displayMode > 0)
			{
				master->displayMode--;
			}
			
		}
		return;
	}

	if(getButtonState(&returnButton))
	{
		recentPress = 1;
		if(master->mainMode)
		{
			if(master->editVal)
			{
				master->editVal = 0;

				switch(master->menuMode)
				{
					case 0:
						eeprom_write_block((void*)&master->val, (const void*)&userBoost, 4);
						break;
	
					case 1:
						eeprom_write_block((void*)&master->val, (const void*)&userBoostKi, 4);
						break;
	
					case 2:
						eeprom_write_block((void*)&master->val, (const void*)&userPressure, 4);
						break;
		
					case 3:
						eeprom_write_block((void*)&master->val, (const void*)&userOnBoost, 4);
						break;

					case 4:
						break;
					
					default:
						break;
				}
				initProcessor(master);
				initBoost(bstCtrl);
				initWM(wmCtrl);
				return;
			}

			switch(master->menuMode)
			{
				case 0:
					eeprom_read_block((void*)&master->val, (const void*)&userBoost, 4);
					master->editVal = 1;
					break;

				case 1:
					eeprom_read_block((void*)&master->val, (const void*)&userBoostKi, 4);
					master->editVal = 1;					
					break;

				case 2:
					eeprom_read_block((void*)&master->val, (const void*)&userPressure, 4);
					master->editVal = 1;
					break;

				case 3:
					eeprom_read_block((void*)&master->val, (const void*)&userOnBoost, 4);
					master->editVal = 1;
					break;
				
				case 4:
					break;									
				default:
					break;
			}
		}
	}

}

void displayData(mainCtrl *master, boostCtrl *bstCtrl, waterMethCtrl *wmCtrl, TriAx *accel)
{
	static float first = 1;
	static float oldVal;
	static float oldMode = 0;

	if(first)
	{
		lcdClearDisplay();		
		(master->units > 0) ? lcdPrintMetricBoostPressure(bstCtrl->boost/14.7) : 
							  lcdPrintStandBoostPressure(bstCtrl->boost);
		oldVal = bstCtrl->boost;
		first = 0;		
	}

	switch(master->displayMode)
	{
		case 0:
			if((abs(oldVal - bstCtrl->boost) > 0.5) || (oldMode != master->displayMode))
			{
				lcdClearDisplay();		
				(master->units > 0) ? lcdPrintMetricBoostPressure(bstCtrl->boost/14.7) : 
									  lcdPrintStandBoostPressure(bstCtrl->boost);
				oldVal = bstCtrl->boost;
				oldMode = master->displayMode;
			}
			break;
		case 1: 
			if((abs(oldVal - wmCtrl->pressure) > 5.0) || (oldMode != master->displayMode))
			{
				lcdClearDisplay();
				(master->units > 0) ? lcdPrintMetricWaterPressure(wmCtrl->pressure/14.7) : 
									  lcdPrintStandWaterPressure(wmCtrl->pressure);
				oldVal = wmCtrl->pressure;
				oldMode = master->displayMode;
			}
			break;
		case 2:
			if((abs(oldVal - accel->x) > 0.1) || (oldMode != master->displayMode))
			{
				lcdClearDisplay();
				lcdPrintAccelAxis(0, accel->x);
				oldVal = accel->x;
				oldMode = master->displayMode;
			}
			break;
        case 3:
			if((abs(oldVal - accel->y) > 0.1) || (oldMode != master->displayMode))
			{
				lcdClearDisplay();
				lcdPrintAccelAxis(1, accel->y);
				oldVal = accel->y;
				oldMode = master->displayMode;
			}
			break;
		case 4:
			if((abs(oldVal - accel->z) > 0.1) || (oldMode != master->displayMode))
			{	
				lcdClearDisplay();
				lcdPrintAccelAxis(2, accel->z);
				oldVal = accel->z;
				oldMode = master->displayMode;
			}
			break;
		case 5:
			if((abs(oldVal - bstCtrl->dutyCycle) > 0.1) || (oldMode != master->displayMode))
			{	
				lcdClearDisplay();
				lcdSendString(" Boost Duty:");
				lcdNewLine();
				lcdPrintFloat(bstCtrl->dutyCycle);
				oldVal = bstCtrl->dutyCycle;
				oldMode = master->displayMode;
			}
			break;
	
		case 6:
			if((abs(oldVal - bstCtrl->dutyCycle) > 0.1) || (oldMode != master->displayMode))
			{	
				lcdClearDisplay();
				lcdSendString(" Boost Integral:");
				lcdNewLine();
				lcdPrintFloat(bstCtrl->integral);
				oldVal = bstCtrl->integral;
				oldMode = master->displayMode;
			}
			break;

		default:
			break;
	}
}

void editParameter(mainCtrl *master, boostCtrl *bstCtrl, waterMethCtrl *wmCtrl)
{
	static float oldVal;
	static float oldMode = 0;
	
}


void systemCounter()
{
	systemCount++;
}

float abs(float x)
{
	((x > 0) ? (x *= 1) : (x *= -1));
	return x;
}		
