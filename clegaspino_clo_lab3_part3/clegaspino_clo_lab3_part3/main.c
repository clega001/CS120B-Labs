/*
 * Christian Legaspino & clega001@ucr.edu
 * Carissa Lo & clo020@ucr.edu
 * Lab Section: 23
 * Assignment: Lab 3 Exercise 2
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

	DDRA = 0x00; PORTA = 0xFF; 
	//DDRB = 0x00; PORTB = 0xFF; 
	DDRC = 0xFF; PORTC = 0x00;

	unsigned char val = 0x00; unsigned char val1 = 0x00;
	unsigned char tmp = 0x00;
	
	while(1){
		val = PINA;
		tmp = 0x00;
		
		val1 = val & 0x0F;
		
		if(val1 == 0x01 || val1 == 0x02){
			tmp = 0x01 << 5 | 0x40;
		}
		else if(val1 == 0x03 || val1 == 0x04){
			tmp = 0x03 << 4 | 0x40;
		}
		else if(val1 == 0x05 || val1 == 0x06){
			tmp = 0x06 << 3;
		}
		else if(val1 == 0x07 || val1 == 0x08 || val1 == 0x09){
			tmp = 0x0F << 2;
		}
		else if(val1 == 0x0A || val1 == 0x0B || val1 == 0x0C){
			tmp = 0x1F << 1;
		}
		else if(val1 == 0x0D || val1 == 0x0E || val1 == 0x0F){
			tmp = 0x2F;
		}
		PORTC = tmp;
	}
}
