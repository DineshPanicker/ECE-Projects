#include <reg51.h>



#define	ADC_DATA			 	P2		//ADC output
#define	ENCODER_OUT		 	P1

#define	ACCM_X_CHANEL	 6
#define	ACCM_Y_CHANEL	 7

#define BYTE	unsigned char				//DB Define Byte
	
#define	ACC_BIT_HIGH_ADCLIMIT_1	120	// 3 and 1
#define	ACC_BIT_HIGH_ADCLIMIT_0	135 // 2 and 0				

	
#define TILT_RIGHT_X		3   // D3 = Acce TiltRight 1 if  (X ad value < 120 
#define TILT_LEFT_X			2   // D2 = Acc Tilt Left  1 if   (X ad value > 135
#define TILT_BACK_Y			1		// D1 = Acc Tilt Back  1 if (Y ad value <120
#define TILT_FORWD_Y		0		// D0 = Acc Tilt Forward if (Y value >135


	


sbit	ACCM_X_AD_SELECT = P3^0;
sbit	ACCM_Y_AD_SELECT  = P3^1;
//sbit ADDR_C = P3^2;
sbit	ALE = P3^3;
sbit 	START = P3^4;
sbit 	OE = P3^5;
sbit 	EOC = P3^6;

sbit 	CLK = P3^7;

// Generate square wave of 100 KHz
void timer0() interrupt 1
{
	CLK = ~CLK;
	TF0 = 0;
}
void ms_delay( unsigned int time)
{
	unsigned int i,j;

	for(i=0;i<time;i++)
		for(j=0;j<113;j++); 
}
//-------------------------------
BYTE read_adc(BYTE Chanel)				//reading your 8-bit ADC values
{
	BYTE adValue;

  ACCM_X_AD_SELECT = 0 ;
  ACCM_Y_AD_SELECT  = 0;
	// select chanel to read Accelerometer X or Y inputs
	if (Chanel == ACCM_X_CHANEL)
		ACCM_X_AD_SELECT = 0;//starts reading ADC values
	else
		ACCM_X_AD_SELECT = 1;//finishes reading the ADC values
	//	ACCM_Y_AD_SELECT = 1;
	
   ALE=1;   // Start ADC
   START=1;
   ms_delay(1);
   ALE = 0;
   START = 0;
   while(EOC==1);
	 //ms_delay(1);
   while(EOC==0);
   OE = 1;
   adValue = ADC_DATA;
  // ms_delay(1);
   OE=0;

	 return adValue;
}

//////////////////////////////////

// Function to asseble 4 bits to transmit
//
//
//
//---------------------------------
BYTE	ConvertToEncodedValue(BYTE xAd, BYTE yAd)				//8-bit ADC values to 4-bit ADC values
{
	
	BYTE  xEncoded ;
	
	// logic to combine 4 bits for Accelerator Pos as per the logic below
	//  TILT_RIGHT_X		3   // D3 = Acce TiltRight 1 if  (X ad value < 120 
	//  TILT_LEFT_X			2   // D2 = Acc Tilt Left  1 if   (X ad value > 135
  // TILT_BACK_Y2		1		// D1 = Acc Tilt Back  1 if (Y ad value <120
  // TILT_FORWD_Y3		0		// D0 = Acc Tilt Forward if (Y value >135

	xEncoded =  0; // default 0 for all pos
	// Construct D3, D2 for X Position
	if (xAd < ACC_BIT_HIGH_ADCLIMIT_1)
	{
		xEncoded |=  (1<< TILT_RIGHT_X); 
	}
	else if (xAd > ACC_BIT_HIGH_ADCLIMIT_0)
	{
		xEncoded |=  (1<< TILT_LEFT_X); 
	}

	// Construct D1, D0 for Y Position
	if (yAd < ACC_BIT_HIGH_ADCLIMIT_1)
	{
		xEncoded |=  (1<< TILT_BACK_Y); 
	}
	else if (yAd > ACC_BIT_HIGH_ADCLIMIT_0)
	{
		xEncoded |=  (1<< TILT_FORWD_Y); 
	}		
	



	return xEncoded ;
}
/////////////////////////////////


void main()
	{
		
		BYTE	xAD, yAD;
		BYTE	xEncoded;
		
		TMOD = 0x02; //timer 0 auto reload mode 8 bit timer
		TH0 = 0xFA;  //auto reload value
		IE = 0x82;   // enable timer0 overflow interrupt
		TR0 = 1;     // start timer 0
		//P0=0x00;
		P2 = 0xFF;   //Port 2 as input port
		P1 = 0x00;   // Port 1 as output
		P3 = 0x40	;  // set all pins of port 3 as output except P3.6
		ENCODER_OUT =0;
		
//ADC_DATA=ADC_TEST	;
	while(1)
		{
			xAD = read_adc(ACCM_X_CHANEL); // read the Accelerometer x values
			yAD = read_adc(ACCM_Y_CHANEL); // read the Accelerometer y values
						
			//ENCODER_OUT = xAD;
 			//P0 = ~xAD;
			xEncoded =  ConvertToEncodedValue(xAD, yAD);
			ENCODER_OUT = xEncoded;			
		}	
	}