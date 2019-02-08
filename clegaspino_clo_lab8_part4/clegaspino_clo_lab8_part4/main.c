/*
 * Christian Legaspino & clega001@ucr.edu
 * Carissa Lo & clo020@ucr.edu
 * Lab Section: 23
 * Assignment: Lab 8 Exercise 4
 *
 * I acknowledge all content contained herein, excluding template or example
 * code, is my own original work.
 */

#include <avr/io.h>

void ADC_init() {
	ADCSRA |= (1 << ADEN) | (1 << ADSC) | (1 << ADATE);
}

unsigned short small_adc = 0x000;
unsigned short MAX = 0x00C7;
unsigned short split = 0x000;

int main(void)
{
	DDRA = 0x00; PORTA = 0xFF;
	DDRB = 0xFF; PORTB = 0x00;
	DDRD = 0xFF; PORTD = 0x00;
	split = MAX / 8;
	
	
	ADC_init();
	
	while (1) 
    {
		small_adc = ADC;
		
		if(small_adc == 0){PORTB = 0x00;}
		else if(small_adc <= split){PORTB = 0x01;}
		else if(small_adc <= (2 * split)){PORTB = 0x03;}
		else if(small_adc <= (3 * split)){PORTB = 0x07;}
		else if(small_adc <= (4 * split)){PORTB = 0x0F;}
		else if(small_adc <= (5 * split)){PORTB = 0x1F;}
		else if(small_adc <= (6 * split)){PORTB = 0x3F;}
		else if(small_adc <= (7 * split)){PORTB = 0x7F;}
		else{PORTB = 0xFF;}
		
    }
}
