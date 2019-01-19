/*
 * Christian Legaspino & clega001@ucr.edu
 * Carissa Lo & clo020@ucr.edu
 * Lab Section: 23
 * Assignment: Lab 3 Exercise 3
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

	unsigned char val = 0x00;
	unsigned char belt = 0x00;
	unsigned char tmp = 0x00;
	
	while(1){
		val = PINA & 0x0F;
		belt = PINA;
		tmp = 0x00;
		
		if(val == 0){
			tmp = 0x40;
		}
		else if(val <= 2){
			tmp = 0x01 << 5 | 0x40;
		}
		else if(val <= 4){
			tmp = 0x03 << 4 | 0x40;
		}
		else if(val <= 6){
			tmp = 0x07 << 3;
		}
		else if(val <= 9){
			tmp = 0x0F << 2;
		}
		else if(val <= 12){
			tmp = 0x1F << 1;
		}
		else if(val <= 15){
			tmp = 0x3F;
		}
		else{
			tmp = 0;
		}
		
		if(GetBit(belt,4) && GetBit(belt,5)){
			if(!GetBit(belt,7)){
				tmp = tmp | 0x80;
			}
		}
		
		PORTC = tmp;
		
	}
}
