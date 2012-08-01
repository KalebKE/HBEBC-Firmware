
#define B_R			0x01
#define B_G			0x02
#define B_B			0x04
#define BOOST_CH	0x00
#define PWM_SCALER	6250*2.2
//#define PWM_SCALER 	0xFFFF

typedef struct
{
	float boost;				// reading from boost sensor
	float desiredBoost;			// desired boost for control
	float dutyCycle;
	float Kp;
	float Ki;
	float integral;
	float Kd;
}boostCtrl;


void initBoost(boostCtrl *bstCtrl);
void setBoost(boostCtrl *bstCtrl);
void readBoost(boostCtrl *bstCtrl);
void controlBoost(boostCtrl *bstCtrl);

