/*
 * Christian Legaspino & clega001@ucr.edu
 * Carissa Lo & clo020@ucr.edu
 * Lab Section: 23
 * Assignment: Lab 3 Exercise 1
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
	DDRB = 0x00; PORTB = 0xFF; 
	DDRC = 0xFF; PORTC = 0x00;

	//unsigned char a = 0x00;	
	//unsigned char b = 0x00;
	unsigned char cnt = 0x00;
	unsigned char i = 0x00;
	
	//a = PINA;
	//b = PINB;
	
	while(1){
		cnt = 0;
		for(i = 0; i < 8; i++){
			if(GetBit(PINA,i)){
				cnt++;
			}
			if(GetBit(PINB,i)){
				cnt++;
			}
		}
		PORTC = cnt;
	}
}

