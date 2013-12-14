/*
 * CandeflickerLED.c
 *
 * Created: 08.12.2013 22:51:08
 *
 * Emulates a Candleflicker-LED on an AVR microcontroller.
 * Please use AVR-GCC to compile and update the defines below according to your controller.
 *
 */ 

#define   F_CPU 16000000

#include <avr/io.h>
#include <util/delay.h>

/*
	32 Bit maximum length LFSR
	see http://www.ece.cmu.edu/~koopman/lfsr/index.html
	
	Using inverted values so the LFSR also works with zero initialisiation.
	
*/
uint8_t Rand(void) {
	static uint32_t Z;
	
	  if (Z & 1)  { Z = (Z >> 1); }
	  else        { Z = (Z >> 1) ^ 0x7FFFF159; }
	
	return (uint8_t)Z;
}

// Led connected to PB0
#define LEDPORT PORTB
#define LEDDDR  DDRB
#define LEDPIN  PB0

int main(void)
{
	CLKPR=_BV(CLKPCE);
	CLKPR=0;			// Set clk division factor to 1
		
	uint8_t PWM_CTR=0;		// 4 bit-Counter
	uint8_t FRAME_CTR=0;	// 5 bit-Counter
	
	uint8_t PWM_VAL=0;		// 4 bit-Register
	uint8_t NEXTBRIGHT=0;	// 4 bit-Register
	uint8_t RAND=0;			// 5 bit Signal
	uint8_t randflag=0;		// 1 bit Signal
	
	LEDDDR|=_BV(LEDPIN);			// LED is connected to PB0
	
    while(1)
    {				
		_delay_us(1e6/440/16);   // Main clock=440*16 Hz
		
		// PWM		
		PWM_CTR++;
		PWM_CTR&=0xf;		// only 4 bit
		
		if (PWM_CTR<=PWM_VAL) {LEDPORT|=_BV(LEDPIN);} else {LEDPORT&=~_BV(LEDPIN);}  

		// FRAME
		if (PWM_CTR==0) 
		{
			FRAME_CTR++;
			FRAME_CTR&=0x1f;
			
			if ((FRAME_CTR&0x07)==0)  // generate a new random number every 8 cycles. In reality this is most likely bit serial
			{
				RAND=Rand()&0x1f;  				
				if ((RAND&0x0c)!=0) randflag=1; // only update if valid 				
			}
			
			// NEW FRAME						
			if (FRAME_CTR==0)
			{
				PWM_VAL=NEXTBRIGHT; // reload PWM
				randflag=1;		    // force update at beginning of frame
			}					
			
			if (randflag)
			{
				NEXTBRIGHT=RAND>15?15:RAND;	 	
			}
		}
    }
}