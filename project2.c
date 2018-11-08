// Jonic Mecija, Project 2, 014467048
// Steven Phan , Project 2, 014358810

// contains the memory locations of the registers 
#include <reg51.h>

#define ldata P2
#define COMMAND 0
#define LCD_DATA 1

// LCD variable declaration
sbit rs = P3^6;//P1^0;  
sbit rw = P3^5;//P1^1; 
sbit en = P3^7;//P1^2; brown

// Port 1 is for the LEDS
sbit green   = P1^5;
sbit yellow  = P1^6;
sbit red     = P1^7;

sbit laser = P1^2;
sbit alarm = P1^0;
sbit a1  =  P0^1;
sbit a2  =  P0^2;

// connected to DB7 on the LCD
sbit busy = P2^7;

// mode control
sbit mode = P0^0;

unsigned int j;

// encoder pins for rotation direction
sbit pinA = P3^2;
sbit pinB = P1^3;

// variables for counter
unsigned char lsb = 0x30; //lsb
unsigned char msb = 0x30; //msb

// dummy variables for the arm count
unsigned char msb2 = 0x30;
unsigned char lsb2 = 0x30;

// FUNCTION DECLARATIONS

// mode functions
void displayTimer();
void startTimer();
void countDown(unsigned char msb, unsigned char lsb);
void led_status(unsigned char msb, unsigned char lsb);
void intruder();
void laserOn(unsigned char msb, unsigned char lsb);

// LCD functions
void init_lcdhold();
void write_to_lcd(unsigned char value, bit mode);
void lcdready(void);

// timing functions
void _25msDelay();
void second();
void half_();


// trying things out here
void armed ();
void intruding();
void arm_Count();
void intrude( unsigned char msb, unsigned char lsb );

unsigned int flag = 0;
unsigned int someone = 0;

/******************************************************
*
*  MAIN FUNCTION		MAIN FUNCTION		MAIN FUNCTION
*
*******************************************************/
void main()
{
	lsb = 0x37;  // set lsb to display 7
	P0 = 0xFF;   // declaring P0 as input port
	TMOD = 0x01; // timer 0, mode 1
	IE = 0x85;   // IE register EX0 and EX1
	IT1 = 1;     // enable external EX1 triggered by falling edge signal
	IT0 = 1;     // enable external EX0 triggered by falling edge signal
	
	alarm =0;
	laser = 0;
	a1 = 0;
	a2 = 0;
	
	red = 0;
	yellow  = 0;
	green = 0;
	
	while(1)
		{
			
			// disamred mode display timer
			if ( mode == 0 )
				{
					// display initial timer in disarmed state
					displayTimer();
				}
			// arm and count down
			else if ( mode == 1 )
				{
					// start counting down in armed state
					startTimer();
				}
			
		} 
		
} // end main
/******************************************************
*
*  INTERRUPTS		INTERRUPTS		INTERRUPTS		INTERRUPTS
*
*******************************************************/

// interrupts current mode and sets mode according to pins

void changeTimer() interrupt 0 //P3.2
{	
 // change the mode so that it goes to DISARMED state
 // if encoder is turned clock wise
		if ( ( pinA == 0 )  && ( pinB == 1 ) ) 
		{
			// if the LSB is 9 increment MSB
			if ( ( lsb == 0x39 ) && ( msb == 0x39 ) )
			{
				write_to_lcd(0x87, COMMAND);
				write_to_lcd(lsb,   LCD_DATA);
				
				write_to_lcd(0x86, COMMAND);
				write_to_lcd(msb,   LCD_DATA);
			}
			else
			{
				if ( lsb == 0x39 )		
					{
						lsb = 0x30;
						msb += 0x01;
					}
				// else increment the LSB
				else 
					{
						lsb += 0x01;
					}
				
					// update the MSB and LSB on the LCD 
					write_to_lcd(0x87, COMMAND);
					write_to_lcd(lsb,   LCD_DATA);
					
					write_to_lcd(0x86, COMMAND);
					write_to_lcd(msb,   LCD_DATA);
			}
		}
		
 // if encoder is turned counter clock wise
		if ( (pinA == 0)  && (pinB == 0) )
		{
			// if the display is "07"
			if ( ( lsb == 0x37 ) && ( msb == 0x30 ) )
			{
				write_to_lcd(0x87, COMMAND);
				write_to_lcd(lsb,   LCD_DATA);
				
				write_to_lcd(0x86, COMMAND);
				write_to_lcd(msb,   LCD_DATA);
			}
			else
			{
			// if the LSB is 0 decrement the MSB
			if ( lsb == 0x30 )		
				{
					lsb = 0x39;
					msb -= 0x01;
				}
			// else decrement the LSB
			else 
				{
					lsb -= 0x01;
				}
			
				// update the MSB and LSB on the LCD
				write_to_lcd(0x87, COMMAND);
				write_to_lcd(lsb,   LCD_DATA);
				
				write_to_lcd(0x86, COMMAND);
				write_to_lcd(msb,   LCD_DATA);
			}
		}
}


//INTERRPUT FOR THE INTRUDER GOES HERE:
void stranger() interrupt 2 //P3.3
{
	if (flag == 1){
		arm_Count();
	}
}

void intrude( unsigned char msb, unsigned char lsb )
{
	
if ( ( msb == 0x30 ) && ( lsb == 0x30 ) && (mode =1))
	{
	unsigned char i = 0;
	unsigned char code intrude[]= "INTRUDER!!!";	
	
	alarm = 1;
		write_to_lcd(0xC0, COMMAND);
	
			while (intrude[i]!='\0') 
				write_to_lcd(intrude[i++],LCD_DATA);
			
			while(1);
		}
	else if ( ( msb == 0x30 ) && ( lsb == 0x30 ) && (mode =1))
	{
		main();
	}

	
}


/******************************************************
*
* 			MODES			MODES			MODES				MODES
*
*******************************************************/

// displays the timer prompt, calls countdown()
void startTimer()
{
	while(1)
	{
		unsigned char code msg[]="Timer:   seconds";	
		unsigned char i = 0;
		
		unsigned char code msg1[]= "Armed";	
		unsigned char k = 0;
		
		init_lcdhold(); 

		
		// writes " Timer: xx seconds " quickly
		while (msg[i]!='\0') 
		 write_to_lcd(msg[i++],LCD_DATA);
		
		// move cursor to second row
		write_to_lcd(0xC0, COMMAND);
		
		// writes " Armed "
		while (msg1[k]!='\0') 
		 write_to_lcd(msg1[k++],LCD_DATA);
		
		// call the counter function
		countDown(msb, lsb);
		
		// holds display
		while((1) && (mode == 1));
	}
}

// only displays the timer prompt
void displayTimer()
{
		unsigned char code msg[]= "Timer:07 seconds";	
		unsigned char i = 0;
	
		unsigned char code msg0[]= "Disarmed";	
		unsigned char x = 0;
		
		init_lcdhold();          
		msb = 0x30;
		lsb = 0x37;
		// writes " Timer: xx seconds " 
		while (msg[i]!='\0') 
		 write_to_lcd(msg[i++],LCD_DATA);
	
		write_to_lcd(0xC0, COMMAND);

		while (msg0[x]!='\0') 
		 write_to_lcd(msg0[x++],LCD_DATA);
		
		while((1) && (mode == 0));
}

// count down from the inputted value 
void countDown(unsigned char msb, unsigned char lsb)
{
	while (mode == 1){
	// create variables to assign to parameters
	unsigned char M = msb;
	unsigned char L = lsb;

	// display the initial value paramaters
	write_to_lcd(0x87, COMMAND);
	write_to_lcd(L,   LCD_DATA);
				
	write_to_lcd(0x86, COMMAND);
	write_to_lcd(M,   LCD_DATA);
	
	// while display is " 00 "
	while( ( M >= 0x30 ) && ( L >= 0x30 ) && (mode == 1) )
		{
				// if display is " x0 "
				if ( ( L == 0x30 ) && ( M != 0x30 ) && (mode == 1) )		
					{
						// reset LSB to 9 AND decrement MSB
						L = 0x39;
						M -= 0x01;
						
						// update the MSB and LSB on the LCD
						second();
						write_to_lcd(0x87, COMMAND);
						write_to_lcd(L,   LCD_DATA);
			
						write_to_lcd(0x86, COMMAND);
						write_to_lcd(M,   LCD_DATA);
					}
					
				// if display is not " x0 "
				else if ( L != 0x30 && (mode == 1) )
					{
						// decrement LSB only
						L -= 0x01;
						
						// update the MSB and LSB on the LCD
						second();
						write_to_lcd(0x87, COMMAND);
						write_to_lcd(L,   LCD_DATA);
										
						write_to_lcd(0x86, COMMAND);
						write_to_lcd(M,   LCD_DATA);
					}
					
				// if display is " 00 " then keep display at " 00 "
				else if ( ( M == 0 ) && ( L == 0 ) && (mode == 1) )
				{
						write_to_lcd(0x87, COMMAND);
						write_to_lcd(0x30,   LCD_DATA);
										
						write_to_lcd(0x86, COMMAND);
						write_to_lcd(0x30,   LCD_DATA);
				}	
				else if (mode ==0)
				{
					//main();
					displayTimer();
				}
				
				
				led_status(M, L);
				laserOn(M, L);
		}
	} 
		main();
}

void laserOn( unsigned char msb, unsigned char lsb )
{
	if ( ( msb == 0x30 ) && ( lsb == 0x30 ) )
	{
		laser = 1;
		second();
		flag = 1;
		
			a1 = 1;
			a2 = 0;
			half_();
			a1 = 0;
			a2 = 1;
	}
}

void led_status(unsigned char msb, unsigned char lsb)
{
				if ((msb > 0x30) && (lsb >=7))
				{
					green = 1;
					yellow = 0;
					red = 0;
				}
				if ( ( msb == 0x30) && (lsb < 0x37) && (lsb > 0x33) )
				{
					green = 0; 
					yellow = 1;
					red = 0;
				}
			  if (( msb == 0x30) && (lsb <= 0x33) && (lsb != 0x37))
				{
					green = 0;
					yellow = 0;
					red  = 1;
				}
}

// countdown after its tripped
void arm_Count()
{
	while (mode == 1){
	// create variables to assign to parameters
	unsigned char M = msb;
	unsigned char L = lsb;

	// display the initial value paramaters
	write_to_lcd(0x87, COMMAND);
	write_to_lcd(L,   LCD_DATA);
				
	write_to_lcd(0x86, COMMAND);
	write_to_lcd(M,   LCD_DATA);
	
	// while display is " 00 "
	while( ( M >= 0x30 ) && ( L >= 0x30 ) && (mode == 1) )
		{
				// if display is " x0 "
				if ( ( L == 0x30 ) && ( M != 0x30 ) && (mode == 1) )		
					{
						// reset LSB to 9 AND decrement MSB
						L = 0x39;
						M -= 0x01;
						
						// update the MSB and LSB on the LCD
						second();
						write_to_lcd(0x87, COMMAND);
						write_to_lcd(L,   LCD_DATA);
			
						write_to_lcd(0x86, COMMAND);
						write_to_lcd(M,   LCD_DATA);
					}
					
				// if display is not " x0 "
				else if ( L != 0x30 && (mode == 1) )
					{
						// decrement LSB only
						L -= 0x01;
						
						// update the MSB and LSB on the LCD
						second();
						write_to_lcd(0x87, COMMAND);
						write_to_lcd(L,   LCD_DATA);
										
						write_to_lcd(0x86, COMMAND);
						write_to_lcd(M,   LCD_DATA);
					}
					
				// if display is " 00 " then keep display at " 00 "
				else if ( ( M == 0 ) && ( L == 0 ) && (mode == 1) )
				{
						write_to_lcd(0x87, COMMAND);
						write_to_lcd(0x30,   LCD_DATA);
										
						write_to_lcd(0x86, COMMAND);
						write_to_lcd(0x30,   LCD_DATA);
				}	
				else if (mode ==0)
				{
					//main();
					displayTimer();
				}
				
				
				led_status(M, L);
				intrude(M, L);
		}
	} 
		main();
}
/********************************************************
*
* 	LCD FUNCTIONS			LCD FUNCTIONS			LCD FUNCTIONS
*
*********************************************************/
void write_to_lcd(unsigned char value, bit mode)
{
	lcdready();
  ldata = value; 
  rs = mode; // set for data, reset for command
  rw = 0;
  en = 1;
  en = 0;
}

void lcdready(void)
{
  busy = 1;
  en = 1;
  rs = 0; // It's a command
  rw = 1; // It's a read command
	
  while (busy) 
		{
			en = 0;
			_25msDelay();
			en = 1;
		}
  en=0;
  rw=0;
}

// setup LCD for the required display 
void init_lcdhold()
{
  // Function set format: 001 DL N F  * *
  // Function set value: 00111000
  // DL=1, use 8-bit data bus; N=1,1/16 duty(2 lines),
  // F=0, 5x7 dot character font 
  write_to_lcd(0x38,COMMAND); 
                              
  // Display On/Off Control format: 00001 D C B
  // Display On/Off Control value: 00001110
  // D=1, display on; C=1, cursor on; B=0, cursor blink off
  write_to_lcd(0x0E,COMMAND);

  // Entry mode set format: 000001 I/D S
  // Entry mode set value: 00000110
  // I/D=1, Increment cursor position; S=0, no display shift
  write_to_lcd(0x06,COMMAND);

  // Clear display and returns cursor to the home position(address 0) 
  write_to_lcd(0x01,COMMAND);
}


/********************************************************
*
* TIMING DELAYS     TIMING DELAYS     TIMING DELAYS
*
*********************************************************/


// timing delay for half a second
void second()
{
		for ( j = 0; j < 40; j++)
		{
			_25msDelay();
		}
}
void half_()
{
	for ( j = 0; j < 20; j++)
		{
			_25msDelay();
		}
}

// timing delay for 
void _25msDelay()
{
		TL0 = 0xFF;
		TH0 = 0xA5;
		TR0 = 1;
		while ( TF0 == 0 );
		TR0 = 0;
		TF0 = 0;
}