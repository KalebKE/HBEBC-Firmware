#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <avr/eeprom.h>
#include "timer.h"
#include "uart.h"
#include "serLCD.h"
#include "rprintf.h"
#include "a2d.h"
#include "boostControl.h"


extern volatile u32 systemCount;

//GLOBAL EEPROM STUFF//

extern float EEMEM userBoost;
extern float EEMEM userBoostKp;
extern float EEMEM userBoostKi;
extern float EEMEM userBoostKd;


void initBoost(boostCtrl *bstCtrl)
{
	float val;
	eeprom_read_block((void*)&val, (const void*)&userBoost, 4);
	bstCtrl->desiredBoost = val;						// set desired boost level;
	
	eeprom_read_block((void*)&val, (const void*)&userBoostKp, 4);	
	bstCtrl->Kp = val;									// set proportional multiplier

	eeprom_read_block((void*)&val, (const void*)&userBoostKi, 4);		
	bstCtrl->Ki = val;									// set integral multiplier
	bstCtrl->integral = 0;								// reset integral

	eeprom_read_block((void*)&val, (const void*)&userBoostKd, 4);	
	bstCtrl->Kd = val;									// set derivative multiplier
	readBoost(bstCtrl);									// read the boost sensor
}

void setBoost(boostCtrl *bstCtrl)
{
	
}

void readBoost(boostCtrl *bstCtrl)
{
	u16 temp;
	a2dSetChannel(BOOST_CH);							// set a2d to boost sensor 
	temp = a2dConvert10bit(BOOST_CH);					// read current boost
	
	// conversion for boost pressure
	bstCtrl->boost = (((float)temp)/1024*5*0.6479 - 0.8563)*14.22;


	// boost is too high
	if(((bstCtrl->boost - bstCtrl->desiredBoost) > 1.00) )
	{
		// turn on yellow 
		if(((bstCtrl->boost - bstCtrl->desiredBoost) < 2.00))
		{
			PORTB |= B_G + B_B;
			PORTB &= ~B_R;
		}

		// turn on red
		else
		{
			PORTB |= B_R;
			PORTB &= ~(B_G + B_B);		
		}
	}
	
	// boost is too low
	else if(((bstCtrl->boost - bstCtrl->desiredBoost) < -1.00) )
	{
		// turn on blue
		PORTB |= B_B;
		PORTB &= ~(B_R + B_G);
	}

	// boost at desired level
	else
	{
		// turn on green
		PORTB |= B_G;
		PORTB &= ~(B_R + B_B);
	}

}

void controlBoost(boostCtrl *bstCtrl)
{
	static u32 oldCount = 0;
	static float oldError = 0;	

	float error;
	float derivative;
	float deltaT;
	float control;
	u16 duty;

	// check if boosting
	if(bstCtrl->boost < 0.5)
	{
		bstCtrl->integral = 0.0;
		oldError = 0;
		oldCount = systemCount;
		bstCtrl->dutyCycle = 0.0;
		timer1PWMASet(0);	
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
	error = (bstCtrl->desiredBoost - bstCtrl->boost);

	// figure out integral
	bstCtrl->integral += error*deltaT;
	//bstCtrl->integral = 0;
	if(bstCtrl->Ki*bstCtrl->integral > 50.0)
	{
		bstCtrl->integral = 50.0/bstCtrl->Ki;
	}
	if(bstCtrl->Ki*bstCtrl->integral < -50.0)
	{
		bstCtrl->integral = -50.0/bstCtrl->Ki;
	}

	// figure out derivative
	derivative = (error-oldError)/deltaT;

	// figure out control signal
	control = bstCtrl->Kp*error + bstCtrl->Ki*bstCtrl->integral + bstCtrl->Kd*derivative;

	// add open loop offset
	control += 50;

	if(control < 0)
		control = 0;
	if(control > 100)
		control = 100;
	
	bstCtrl->dutyCycle = control;

	control = control/100*((float)PWM_SCALER);
	duty = (u16)control;
	
	timer1PWMASet(control);										// set duty cycle
	
	oldCount = systemCount;
	oldError = error;
}
