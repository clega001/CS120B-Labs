/*
 * Christian Legaspino & clega001@ucr.edu
 * Carissa Lo & clo020@ucr.edu
 * Lab Section: 23
 * Assignment: Lab 4 Exercise 4
 * 
 * I acknowledge all content contained herein, excluding template or example
 * code, is my own original work.
 */ 

#include <avr/io.h>
//#include "RIMS.h"

enum States{START, INIT, LOCK, PRESS, UNLOCK, PRESS_UNLOCK} state;
	
void Ticks(){
	switch(state){
		case START: 
			state = INIT;
			PORTC = 1;
			break;
		case INIT:
			state = LOCK;
			PORTC = 2;
			break;
		case LOCK:
			if(PINA == 0x04){
				state = PRESS;
				PORTC = 4;
			}
			else{
				state = LOCK;
				PORTC = 3;
			}
			break;
		case PRESS:
			if(PINA == 0x02){
				state = UNLOCK;
				PORTC = 5;
			}
			else{
				state = LOCK;
				PORTC = 3;
			}
			break;
		case UNLOCK:
			if(PINA == 0x80){
				state = LOCK;
				PORTC = 3;
			}
			else if(PINA == 0x04){
				state = PRESS_UNLOCK;
				PORTC = 6;
			}
			else{
				state = UNLOCK;
				PORTC = 5;
			}
			break;
		case PRESS_UNLOCK:
			if(PINA == 0x02){
				state = LOCK;
				PORTC = 3;
			}
			else{
				state = UNLOCK;
				PORTC = 5;
			}
			break;
		default:
			state = LOCK;
			PORTC = 3;
			break;
	}
	switch(state){
		case START:
			break;
		case INIT:
			PORTB = 0x00;
			break;
		case LOCK: 
			PORTB = 0x00;
			break;
		case PRESS:
			break;
		case UNLOCK:
			PORTB = 0x01;
			break;
		case PRESS_UNLOCK:
			PORTB = 0x01;
			break;
		default:
			PORTB = 0x00;
			break;
	}
}

int main(void)
{
	DDRA = 0x00; PORTA = 0xFF;
	DDRB = 0xFF; PORTB = 0x00;
	DDRC = 0xFF; PORTC = 0x00;
	
	state = START;
    while (1) {Ticks();}
}

