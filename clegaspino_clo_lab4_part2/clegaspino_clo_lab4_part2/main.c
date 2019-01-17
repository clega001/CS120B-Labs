/*
 * Christian Legaspino & clega001@ucr.edu
 * Carissa Lo & clo020@ucr.edu
 * Lab Section: 23
 * Assignment: Lab 4 Exercise 2
 * 
 * I acknowledge all content contained herein, excluding template or example
 * code, is my own original work.
 */ 

#include <avr/io.h>
//#include "RIMS.h"

enum States{START, INIT, WAIT, ADD, SUB, RESET} state;
	
void Ticks(){
	switch(state){
		case START:
			state = INIT;
			break;
		case INIT:
			state = WAIT;
			break;
		case WAIT:
			if((PINA == 0x01) && (PORTC < 9))
			{
				state = ADD;
				//break;
			}
			else if ((PINA == 0x02) && (PORTC > 0))
			{
				state = SUB;
				//break;
			}
			else if (PINA == 0x03)
			{
				state = RESET;
				//break;
			}
			else
			{
				state = WAIT;
				//break;
			}
			break;
		case ADD:
			state = WAIT;
			break;
		case SUB:
			state = WAIT;
			break;
		case RESET:
			state = WAIT;
			break;
		default:
			state = START;
			break;
	}
	switch(state){
		case START:
			break;
		case INIT:
			PORTC = 7;
			break;
		case WAIT:
			break;
		case ADD:
			PORTC = PORTC + 1;
			break;
		case SUB:
			PORTC = PORTC - 1;
			break;
		case RESET:
			PORTC = 0;
			break;
		default:
			break;	
	}
}

int main(void)
{
	DDRA = 0x00; PORTA = 0xFF;
	DDRC = 0xFF; PORTC = 0x00;
	state = START;
    while (1) {Ticks();}
}