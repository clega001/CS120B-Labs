/*
 * Christian Legaspino & clega001@ucr.edu
 * Carissa Lo & clo020@ucr.edu
 * Lab Section: 23
 * Assignment: Lab 8 Exercise 3
 *
 * I acknowledge all content contained herein, excluding template or example
 * code, is my own original work.
 */

#include <avr/io.h>

void ADC_init() {
	ADCSRA |= (1 << ADEN) | (1 << ADSC) | (1 << ADATE);
}

int main(void)
{
	DDRA = 0x00; PORTA = 0xFF;
	DDRB = 0xFF; PORTB = 0x00;
	DDRD = 0xFF; PORTD = 0x00;
	
	unsigned short small_adc = 0x0000;
	unsigned short MAX = 0x00C7;
	
	ADC_init();
	
	while (1) 
    {
		small_adc = ADC;
		if(small_adc >= (MAX/2)){
			PORTB = 0x01;
		}
		else{PORTB = 0x00;}
    }
}

