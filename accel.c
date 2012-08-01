#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include "uart.h"
#include "serLCD.h"
#include "rprintf.h"
#include "a2d.h"
#include "accel.h"


void readAccel(TriAx *accel)
{
	u16 voltageAx, voltageAy, voltageAz;

	// read the accelerometer channels 
	a2dSetChannel(AX_CH);
	voltageAx = a2dConvert10bit(AX_CH);
	a2dSetChannel(AY_CH);
	voltageAy = a2dConvert10bit(AY_CH);
	a2dSetChannel(AZ_CH);
	voltageAz = a2dConvert10bit(AZ_CH);

	// update accelerometer struct
	accel->x = ((float)voltageAx)/1024*5/0.6 - 2.75;		
	accel->y = ((float)voltageAy)/1024*5/0.6 - 2.75;					
	accel->z = ((float)voltageAz)/1024*5/0.6 - 2.75;		
}
