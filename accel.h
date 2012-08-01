#define AX_CH		0x02
#define AY_CH		0x03
#define	AZ_CH		0x04


typedef struct
{
	float x;
	float y;
	float z;
}TriAx;


void readAccel(TriAx *accel);
