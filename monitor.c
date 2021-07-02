//Expt.No. 8: 8.	Interface analog voltage 0-5V to internal ADC and display value on LCD
/*The analog voltage is applied to analog pin AN1(PORTA - RA1)
The resultant digital data in BCD is put on LCD */
			
//Includes
#include <p18f4550.h>		//Include controller specific .h file
#include "vector_relocate.h" //Header file for vector relocation - USB HID Bootloader

//Declarations for LCD Connection
#define LCD_DATA    PORTD               //LCD data port
#define en          PORTEbits.RE2      // enable signal
#define rw          PORTEbits.RE1      // read/write signal
#define rs          PORTEbits.RE0     // register select signal
#define VREF		5000.0
#define temp_ref	0.488281
#define green		PORTBbits.RB1
#define yellow		PORTBbits.RB2
#define red			PORTBbits.RB3
#define RELAY		PORTCbits.RC7
#define scale		185
#define dt			4.2
#define cap			3.6

//Function Prototypes
void ADC_Init(void);					//Function to initialize the ADC
void ADC_Init1(void);	
void ADC_Init2(void);		
void monitor(float v);		
unsigned int Get_ADC_Result(void);		//Function to Get ADC result after conversion
void Start_Conversion(void);			//Function to Start of Conversion
void msdelay (unsigned int time);		//Function to generate delay
void init_LCD(void);					//Function to initialise the LCD		
void LCD_command(unsigned char cmd);	//Function to pass command to the LCD
void LCD_data(unsigned char data);		//Function to write character to the LCD
void LCD_write_string(static char *str);//Function to write string to the LCD
void SOC();
//Start of main program
float vtg,temp,vout,cur,dod,disc;
char msg10[]="SOC: ";
char msg11[]="%";
	float soc;
		unsigned char j,Hundreds,Tens,Ones;
void main()
{ 
 char msg1[] = "Battery Monitor";
 char msg2[] = "Voltage: ";
 char msg3[] = ".";
 char msg4[] = "V";
 char msg5[] = "Temp:";
 char msg6[] = "        ";
 char msg7[] = "C";
 char msg8[] = "Current: ";
 char msg9[] = " mA";
 unsigned char i, Thousands,Hundreds,Tens,Ones;
 unsigned int adc_val,adc_val2,k=0,adc_val3;


 ADCON1 = 0x0F;        //Configuring the PORT pins as digital I/O 
 TRISD = 0x00;         //Configuring PORTD as output
 TRISE = 0x00;         //Configuring PORTE as output
 TRISC = 0x00;
TRISB=0x00;
//TRISA=0x00;
 RELAY = 0;
 red=0;
 yellow=0;
 green=0;
 ADC_Init();				// Init ADC peripheral
 init_LCD();				// Init LCD Module

 while(1)
 {	
	 
		
	 if(k == 0)
	 {
		LCD_command(0x01);
		 ADC_Init();				// Init ADC peripheral
  	Start_Conversion();			//Trigger conversion
	  LCD_write_string(msg1);	// Display Welcome Message
 LCD_command(0xC0);			// Goto second line, 0th place of LCD
 LCD_write_string(msg2);	// Display Message "ADC O/P"
  	adc_val= Get_ADC_Result();//Get the ADC output by polling GO bit
	LCD_command (0xC8);			//Goto 9th place on second line of LCD
	vtg= (VREF *adc_val/1023.0);
	
	i = vtg/1000 ;		//Get the thousands place 
  	Thousands = i + 0x30;		// Convert it to ASCII
  	LCD_data (Thousands);	// Display thousands place
    i = (unsigned int) vtg;
	i = (i %1000)/100;	//Get the Hundreds place
	LCD_write_string(msg3);
  	Hundreds = i + 0x30;		// Convert it to ASCII
  	LCD_data (Hundreds);		//Display Hundreds place
    i = (unsigned int) vtg;
	i = ((i%1000)%100)/10; //Get the Tens place
  	Tens = i + 0x30;			// Convert it to ASCII
  	LCD_data (Tens);			//Display Tens place
    i = (unsigned int) vtg;
	i = i%10 ;			//Get the Ones place
  	Ones = i + 30; 			// Convert it to ASCII
  	LCD_data (i + 0x30);		//Display Ones place
 	LCD_write_string(msg4);
	LCD_command(0xC8);
	LCD_write_string(msg6);
	monitor(vtg);
	
	k=1;
	msdelay(300);		//Delay between conversions. It is a library function,refer delays.h file in MCC18 installation directory
	}
	
	else if(k == 1)
	{	
		LCD_command(0x01);
		ADC_Init1();					// Init ADC peripheral
		Start_Conversion();				//Trigger conversion
		adc_val2= Get_ADC_Result();		//Get the ADC output by polling GO bit
		LCD_command(0x80);				
		LCD_write_string(msg5);
		adc_val2=Get_ADC_Result();
		temp=(adc_val2)*temp_ref;
		
		 i = temp/100 ;				//Get the Hundreds place
  	     Hundreds = i + 0x30;		// Convert it to ASCII
  	     LCD_data (Hundreds);		//Display Hundreds place
    	i = (unsigned int) temp;
		i = ((i%1000)%100)/10; //Get the Tens place
  		Tens = i + 0x30;			// Convert it to ASCII
  		LCD_data (Tens);			//Display Tens place
    	i = (unsigned int) temp;
		i = i%10 ;			//Get the Ones place
  		Ones = i + 30; 			// Convert it to ASCII
  		LCD_data (i + 0x30);		//Display Ones place
		LCD_write_string(msg7);
		if(temp >=60.0 || temp <= 10.0)
		{
			RELAY = 1;
	
		}
		else
		{
			RELAY = 0;
		}
		k=2;
		msdelay(300);
	}
	else if(k == 2)
	{
		ADC_Init2();
		Start_Conversion();
		adc_val3=Get_ADC_Result();
		LCD_command(0xC0);
		LCD_write_string(msg8);
		
		vout=(adc_val3*VREF)/1023;
		cur=(vout-2500)/scale;
		i = cur/1000 ;		//Get the thousands place 
  	Thousands = i + 0x30;		// Convert it to ASCII
  	LCD_data (Thousands);	// Display thousands place
    i = (unsigned int) cur;
	i = (i %1000)/100;	//Get the Hundreds place
  	Hundreds = i + 0x30;		// Convert it to ASCII
  	LCD_data (Hundreds);		//Display Hundreds place
    i = (unsigned int) cur;
	i = ((i%1000)%100)/10; //Get the Tens place
  	Tens = i + 0x30;			// Convert it to ASCII
  	LCD_data (Tens);			//Display Tens place
    i = (unsigned int) cur;
	i = i%10 ;			//Get the Ones place
  	Ones = i + 30; 			// Convert it to ASCII
  	LCD_data (i + 0x30);		//Display Ones place
 	LCD_write_string(msg9);
	SOC();
	
	 
	 k=0;
	msdelay(300);
	
	}
		
}
}
//Function Definitions
	
void SOC()
{
	LCD_command(0x01);
	LCD_command(0x80);

	LCD_write_string(msg10);
	LCD_command(0x85);

	disc = cap/dt;
	dod = (dt*disc)/cap;
	soc = 100.0 * (1-(dod/cap)); 

	j = soc/100;		//Get the thousands place 
  	Hundreds = j + 0x30;		// Convert it to ASCII
  	LCD_data (Hundreds);		//Display Hundreds place
    j = (unsigned int) soc;
	j = ((j%1000)%100)/10; //Get the Tens place
  	Tens = j + 0x30;			// Convert it to ASCII
  	LCD_data (Tens);			//Display Tens place
    j = (unsigned int) soc;
	j = j%10 ;			//Get the Ones place
  	Ones = j + 30; 			// Convert it to ASCII
  	LCD_data (j + 0x30);		//Display Ones place
	LCD_command(0x88);
 	LCD_write_string(msg11);
}
void ADC_Init2()
{
	ADCON0=0b0001000;	//A/D Module is OFF and Channel 0 is selected
 ADCON1=0b00001101;	// Reference as VDD & VSS, AN1 set as analog pins
 ADCON2=0b10001110; // Result is right Justified
					//Acquisition Time 2TAD
					//ADC Clk FOSC/64
 ADCON0bits.ADON=1; //Turn ON ADC module
}
void ADC_Init1()
{
 ADCON0=0b0000000;	//A/D Module is OFF and Channel 0 is selected
 ADCON1=0b00001101;	// Reference as VDD & VSS, AN1 set as analog pins
 ADCON2=0b10001110; // Result is right Justified
					//Acquisition Time 2TAD
					//ADC Clk FOSC/64
 ADCON0bits.ADON=1; //Turn ON ADC module
}
void ADC_Init()
{
 ADCON0=0b00000100;	//A/D Module is OFF and Channel 1 is selected
 ADCON1=0b00001101;	// Reference as VDD & VSS, AN1 set as analog pins
 ADCON2=0b10001110; // Result is right Justified
					//Acquisition Time 2TAD
					//ADC Clk FOSC/64
 ADCON0bits.ADON=1; //Turn ON ADC module
}

void Start_Conversion()
{
 ADCON0bits.GO=1;
}
void monitor(float v)
{
	if(v <1500)
	{
		red=1;
		green=0;
		yellow=0;
	}
	else if(v > 1500 && v < 2600)
	{
		yellow=1;
		red=0;
		green=0;
	}
	else if(v > 2600)
	{
		green=1;
		red=0;
		yellow=0;
	}
}
//If you do not wish to use adc conversion interrupt you can use this
//to do conversion manually. It assumes conversion format is right adjusted
unsigned int Get_ADC_Result()
{
 unsigned int ADC_Result=0;
 while(ADCON0bits.GO);
 ADC_Result=ADRESL;
 ADC_Result|=((unsigned int)ADRESH) << 8;
 return ADC_Result;
} 

void msdelay (unsigned int time) //Function to generate delay
{
unsigned int i, j;
  for (i = 0; i < time; i++)
	for (j = 0; j < 710; j++);//Calibrated for a 1 ms delay in MPLAB
}


void init_LCD(void)		// Function to initialise the LCD
{
    LCD_command(0x38);      // initialization of 16X2 LCD in 8bit mode
    msdelay(15);
    LCD_command(0x01);      // clear LCD
    msdelay(15);
    LCD_command(0x0C);      // cursor off
    msdelay(15);
    LCD_command(0x80);      // go to first line and 0th position
    msdelay(15);
}

void LCD_command(unsigned char cmd) //Function to pass command to the LCD
{
    LCD_DATA = cmd;		//Send data on LCD data bus
    rs = 0;				//RS = 0 since command to LCD
    rw = 0;				//RW = 0 since writing to LCD
    en = 1;				//Generate High to low pulse on EN
    msdelay(10);
    en = 0;
}

void LCD_data(unsigned char data)//Function to write data to the LCD
{
    LCD_DATA = data;	//Send data on LCD data bus
    rs = 1;				//RS = 1 since data to LCD
    rw = 0;				//RW = 0 since writing to LCD
    en = 1;				//Generate High to low pulse on EN
  	msdelay(10);
    en = 0;
}
//Function to write string to LCD
void LCD_write_string(static char *str)   
{
    int i = 0;
    while (str[i] != 0)
    {
        LCD_data(str[i]);      // sending data on LCD byte by byte
        msdelay(10);
        i++;
    }
}
