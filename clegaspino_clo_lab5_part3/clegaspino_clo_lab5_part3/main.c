/*
 * Christian Legaspino & clega001@ucr.edu
 * Carissa Lo & clo020@ucr.edu
 * Lab Section: 23
 * Assignment: Lab 5 Exercise 3
 * 
 * I acknowledge all content contained herein, excluding template or example
 * code, is my own original work.
 */ 

#include <avr/io.h>

enum States{START, WAIT, B_PRESS, B_RELEASE} state;
	
unsigned char b0 = 0x00;

void Ticks(){
	
	b0 = PINA & 0x01;
	
	switch(state){
		case START:
			state = WAIT;
			break;
		case WAIT:
			if(!b0){
				state = B_PRESS;
			}
			else{
				state = WAIT;
			}
			break;
		case B_PRESS:
			if(!b0){
				state = B_PRESS;
			}
			else{
				state = B_RELEASE;
			}
			break;
		case B_RELEASE:
			state = WAIT;
			break;
		default:
			state = START;
			break;
	}
	switch(state){
		case START:
			PORTB = 0;
			break;
		case WAIT:
			break;
		case B_PRESS:
			break;
		case B_RELEASE:
			PORTB = PORTB + 1;
			break;
		default:
			PORTB = 0;
			break;
	}
}

int main(void)
{
	DDRA = 0x00; PORTA = 0xFF;
	DDRB = 0xFF; PORTB = 0x00;
	
	state = START;
	while (1) {Ticks();}
}