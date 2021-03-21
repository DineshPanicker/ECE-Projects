#include <reg51.h>

#define	ACCM_DATA			 	P2
#define SERV_PORT				P1

#define BYTE	unsigned char
	
#define TILT_RIGHT_X		3   // D3 = Acce TiltRight 1 if  (X ad value < 120) 
#define TILT_LEFT_X			2   // D2 = Acc Tilt Left  1 if   (X ad value > 135)
#define TILT_BACK_Y			1		// D1 = Acc Tilt Back  1 if (Y ad value <120)
#define TILT_FORWD_Y		0		// D0 = Acc Tilt Forward if (Y value >135)


#define MOT_MIN_90_DUTYCYCLE_VAL	2.7						//servo at +90
#define MOT_MAX_90_DUTYCYCLE_VAL	12						//servo at +90 
#define MOT_0_DUTYCYCLE_VAL				8.057					//servo at 0 deg

//sbit M0=P1^0;
//sbit M1=P1^1;


/* Define value to be loaded in timer for PWM period of 20 milli second */
#define PWM_PERIOD 0xB7FE

#define SERVO_M0	0
#define SERVO_M1	1

typedef enum _ServoStates {_IN_MIN_90_POS, _IN_0_POS, _IN_MAX_90_POS} SERVO_STATE;



struct _SerM{
	SERVO_STATE  	Serv_State;
	unsigned int 	ON_Period;
	unsigned int	OFF_Period;// Control patameters for PWM

}
Servo_Motor [2];



sbit Servo_Motor_Pin_M0	= P1^0;
sbit Servo_Motor_Pin_M1 = P1^1;



void ms_delay( unsigned int time)
{
	unsigned int i,j;

	for(i=0;i<time;i++)
		for(j=0;j<113;j++); 
}
//-------------------------------
//-------------------

/* Function to setup ISR for PWM */
void Timer_init()
{
	
	EA  = 1;		/* Enable global interrupt */
  ET0 = 1;         	/* Enable timer0 interrupt */
	//TMOD = 0x01;		/* Timer0 mode1 */
	TH0 = (PWM_PERIOD  >> 8);/* 20ms timer value */
	TL0 = PWM_PERIOD ;
	TR0 = 1;				/* Start timer0 */
	
  ET1 = 1;         	/* Enable timer1 interrupt */
	TMOD = 0x11;		/* Timer1 and To,er 0 mode1 and Tmer0 */
	TH1 = (PWM_PERIOD  >> 8);/* 20ms timer value */
	TL1 = PWM_PERIOD ;
	TR1 = 1;				/* Start timer1 */	
	
}

/* Timer0 interrupt service routine (ISR) */
void Timer0_ISR() interrupt 1	
{
  Servo_Motor_Pin_M0 = !Servo_Motor_Pin_M0;
	if(Servo_Motor_Pin_M0)
		{
		  TH0 = (Servo_Motor[SERVO_M0].ON_Period>> 8);
		  TL0 = Servo_Motor[SERVO_M0].ON_Period;
	  }	
	else
	 {
		TH0 = (Servo_Motor[SERVO_M0].OFF_Period >> 8);
		TL0 = Servo_Motor[SERVO_M0].OFF_Period;
	 }

}

/* Timer1 interrupt service routine (ISR) */
void Timer1_ISR() interrupt 3	
{
 
  Servo_Motor_Pin_M1 = !Servo_Motor_Pin_M1;
	if(Servo_Motor_Pin_M1)
		{
		  TH1 = (Servo_Motor[SERVO_M1].ON_Period >> 8);
		  TL1 = Servo_Motor[SERVO_M1].ON_Period;
	  }	
	else
	 {
		TH1 = (Servo_Motor[SERVO_M1].OFF_Period >> 8);
		TL1 = Servo_Motor[SERVO_M1].OFF_Period;
	 }

}

/* Calculate ON & OFF period from duty cycle */
void Set_DutyCycle_To(float duty_cycle, int ServMotIndex)
{
	float period = 65535 - PWM_PERIOD;
	Servo_Motor[ServMotIndex].ON_Period = ((period/100.0) * duty_cycle);
	Servo_Motor[ServMotIndex].OFF_Period = (period - Servo_Motor[ServMotIndex].ON_Period);	
	Servo_Motor[ServMotIndex].ON_Period = 65535 - Servo_Motor[ServMotIndex].ON_Period;	
	Servo_Motor[ServMotIndex].OFF_Period = 65535 - Servo_Motor[ServMotIndex].OFF_Period;
	
	if (duty_cycle ==  MOT_MIN_90_DUTYCYCLE_VAL) 
		Servo_Motor[ServMotIndex].Serv_State = _IN_MIN_90_POS; 
	else if 	(duty_cycle ==  MOT_MAX_90_DUTYCYCLE_VAL) 
		 Servo_Motor[ServMotIndex].Serv_State = _IN_MAX_90_POS; 
	else   
			 Servo_Motor[ServMotIndex].Serv_State = _IN_0_POS; 

	ms_delay(100); // wait for the motor to position
}

//-----------------------------------------

//////////////////////////////////

// Function to asseble 4 bits to transmit
//
//
//
//---------------------------------
void	DecodeAndInitaiteServo(BYTE AccXm_Info)
{
	
	if (AccXm_Info & (1<< TILT_RIGHT_X))
	{
		// do action to rotate servo for TILT Right (X axis)
	//	Set_DutyCycle_To(2.7);/* 0.54ms(2.7%) of 20ms(100%) period */
		Set_DutyCycle_To(MOT_MAX_90_DUTYCYCLE_VAL, SERVO_M0);	
		
	}
	else if (AccXm_Info & (1<< TILT_LEFT_X))
	{
		// do action to rotate servo for TILT Left (X axis)
		Set_DutyCycle_To(MOT_MIN_90_DUTYCYCLE_VAL, SERVO_M0);	
	
	//	Set_DutyCycle_To(7);/* 1.4ms(7%) of 20ms(100%) period */
	}
	else if (!(AccXm_Info & 0xC0))
	{
	//   Set_DutyCycle_To(12);/* 2.4ms(12%) of 20ms(100%) period */
		Set_DutyCycle_To(MOT_0_DUTYCYCLE_VAL, SERVO_M0);	
	}
	
	if (AccXm_Info & (1<< TILT_BACK_Y))
	{
	    Set_DutyCycle_To(MOT_MAX_90_DUTYCYCLE_VAL, SERVO_M1);	
	}
  else if (AccXm_Info & (1<< TILT_FORWD_Y))
	{
		// do action to rotate servo for TILT Forward (Y axis)
		Set_DutyCycle_To(MOT_MIN_90_DUTYCYCLE_VAL, SERVO_M1);
	
	}
	else if (!(AccXm_Info & 0x03))
	{
		Set_DutyCycle_To(MOT_0_DUTYCYCLE_VAL, SERVO_M1);	
	}


}
/////////////////////////////////


void main()
	{
		
		BYTE	AccXm_Info;
		
	
		//P0=0x00;
		ACCM_DATA = 0xFF;   //Port 2 as input port
		SERV_PORT = 0x00;   // Port 1 as output
		
	
   Timer_init();
		
//ADC_DATA=ADC_TEST	;
	//	Set_DutyCycle_To(2.7);/* 0.54ms(2.7%) of 20ms(100%) period */
		//Set_DutyCycle_To(7);
	Set_DutyCycle_To(MOT_0_DUTYCYCLE_VAL, SERVO_M0);	
	Set_DutyCycle_To(MOT_0_DUTYCYCLE_VAL, SERVO_M1);
	 
		
	while(1)
		{
			AccXm_Info =ACCM_DATA; // read the Accelerometer x values
			AccXm_Info=  AccXm_Info>>4; // simce MSB of port 2 is connected, move 4 bits to get bits in the 4 bit LSB 
			DecodeAndInitaiteServo(AccXm_Info);
			
			
		}	
	}