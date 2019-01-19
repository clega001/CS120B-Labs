/*
 * Christian Legaspino & clega001@ucr.edu
 * Carissa Lo & clo020@ucr.edu
 * Lab Section: 23
 * Assignment: Lab 3 Exercise 5
 * 
 * I acknowledge all content contained herein, excluding template or 
 * example code, is my own original work.
 */ 

#include <avr/io.h>


// Bit-access function
unsigned char SetBit(unsigned char x, unsigned char k, unsigned char b) {
	return (b ? x | (0x01 << k) : x & ~(0x01 << k));
}
unsigned char GetBit(unsigned char x, unsigned char k) {
	return ((x & (0x01 << k)) != 0);
}

int main(void)
{

	DDRB = 0xFE; PORTB = 0x01;
	DDRD = 0x00; PORTD = 0xFF;
	
	unsigned char weight = 0x00;
	
	while(1){
		weight = PIND + PINB;
		
		if(weight >= 5 && weight < 70){
			PORTB = SetBit(PORTB,2,1);
			PORTB = SetBit(PORTB,1,0);
		}
		else if(weight >= 70){
			PORTB = SetBit(PORTB,1,1);
			PORTB = SetBit(PORTB,2,0);
		}
		else{
			PORTB = SetBit(PORTB,2,0);
			PORTB = SetBit(PORTB,1,0);
		}
	}
}


