#define W_R			0x08
#define W_G			0x10
#define W_B			0x20
#define WATER_CH	0x01
typedef struct
{
	float pressure;				// reading from WM pressure sensor
	float boost;				// reading from boost sensor
	float onBoost;				// PSI at which to turn on WM
	float desiredPressure;		// the desired WM pressure
	float dutyCycle;
	float Kp;
	float Ki;
	float integral;
	float Kd;
}waterMethCtrl;


void initWM(waterMethCtrl *wmCtrl);
void setWM(waterMethCtrl *wmCtrl);
void readWM(waterMethCtrl *wmCtrl);
void controlWM(waterMethCtrl *wmCtrl);




