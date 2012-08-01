#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <avr/eeprom.h>
#include "timer.h"
#include "uart.h"
#include "serLCD.h"
#include "rprintf.h"
#include "a2d.h"
#include "waterControl.h"
#include "boostControl.h"

extern volatile u32 systemCount;

//GLOBAL EEPROM STUFF//

extern float EEMEM userPressure;
extern float EEMEM userOnBoost;
extern float EEMEM userWMKp;
extern float EEMEM userWMKi;
extern float EEMEM userWMKd;


void initWM(waterMethCtrl *wmCtrl)
{
	float val;

	eeprom_read_block((void*)&val, (const void*)&userOnBoost, 4);
	wmCtrl->onBoost = val;								// set desired turn-on point
	
	eeprom_read_block((void*)&val, (const void*)&userPressure, 4);	
	wmCtrl->desiredPressure = val;						// set desired wm pressure level;

	eeprom_read_block((void*)&val, (const void*)&userWMKp, 4);
	wmCtrl->Kp = val;									// set proportional multiplier

	eeprom_read_block((void*)&val, (const void*)&userWMKi, 4);
	wmCtrl->Ki = val;									// set integral multiplier
	wmCtrl->integral = 0;								// reset integral

	eeprom_read_block((void*)&val, (const void*)&userWMKd, 4);
	wmCtrl->Kd = val;									// set derivative multiplier
	readWM(wmCtrl);										// read the boost sensor
}

void setWM(waterMethCtrl *wmCtrl)
{
	
}

void readWM(waterMethCtrl *wmCtrl)
{
	u16 temp;
	a2dSetChannel(WATER_CH);							// set a2d to WM sensor 
	temp = a2dConvert10bit(WATER_CH);					// read current pressure
	
	// conversion for water pressure
	wmCtrl->pressure = (-800*(((float)temp)/1024*5-0.125)/(((float)temp)/1024*5-5));
	//wmCtrl->boost = bstCtrl->boost;
	
	// pressure is too high
	if(((wmCtrl->pressure - wmCtrl->desiredPressure) > 10.00) )
	{
		// turn on yellow 
		if(((wmCtrl->pressure - wmCtrl->desiredPressure) < 20.00))
		{
			PORTB |= W_G + W_B;
			PORTB &= ~W_R;
		}

		// turn on red
		else
		{
			PORTB |= W_R;
			PORTB &= ~(W_G + W_B);		
		}
	}
	
	// pressure is too low
	else if(((wmCtrl->pressure - wmCtrl->desiredPressure) < -20.00) )
	{
		// turn on blue
		PORTB |= W_B;
		PORTB &= ~(W_R + W_G);
	}

	// pressure at desired level
	else
	{
		// turn on green
		PORTB |= W_G;
		PORTB &= ~(W_R + W_B);
	}

}

void controlWM(waterMethCtrl *wmCtrl)
{
	static u32 oldCount = 0;
	static float oldError = 0;	

	float error;
	float derivative;
	float deltaT;
	float control;
	u16 duty;

	// check if boosting
	if(wmCtrl->boost < wmCtrl->onBoost)
	{
		wmCtrl->integral = 0.0;
		oldError = 0;
		oldCount = systemCount;
		wmCtrl->dutyCycle = 0.0;
		timer1PWMBSet(0);	
		return;	
	}
	
	if(systemCount == oldCount)
	{
		return;
	}

	// figure out elpased time
	if(oldCount > systemCount)
	{
		oldCount = systemCount;
	}

	deltaT = ((float)(systemCount - oldCount));
	
	// figure out error
	error = (wmCtrl->desiredPressure - wmCtrl->pressure);

	// figure out integral
	wmCtrl->integral += error*deltaT;
	//wmCtrl->integral = 0;
	if(wmCtrl->Ki*wmCtrl->integral > 50.0)
	{
		wmCtrl->integral = 50.0/wmCtrl->Ki;
	}
	if(wmCtrl->Ki*wmCtrl->integral < -50.0)
	{
		wmCtrl->integral = -50.0/wmCtrl->Ki;
	}

	// figure out derivative
	derivative = (error-oldError)/deltaT;

	// figure out control signal
	control = wmCtrl->Kp*error + wmCtrl->Ki*wmCtrl->integral + wmCtrl->Kd*derivative;

	// add open loop offset
	control += 50;

	if(control < 0)
		control = 0;
	if(control > 100)
		control = 100;
	
	wmCtrl->dutyCycle = control;

	control = control/100*((float)PWM_SCALER);
	duty = (u16)control;
	
	timer1PWMBSet(control);										// set duty cycle
	
	oldCount = systemCount;
	oldError = error;
}
