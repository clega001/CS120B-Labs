/*
 * Christian Legaspino & clega001@ucr.edu
 * Carissa Lo & clo020@ucr.edu
 * Lab Section: 23
 * Assignment: Lab 5 Exercise 2
 * 
 * I acknowledge all content contained herein, excluding template or example
 * code, is my own original work.
 */ 

#include <avr/io.h>
#include <avr/delay.h>
//#include "RIMS.h"


//_delay_ms();
enum States{START, WAIT, ADD_PRESS, ADD_RELEASE, SUB_PRESS, SUB_RELEASE, RESET_PRESS, RESET_RELEASE} state;

unsigned char b0 = 0x00;
unsigned char b1 = 0x00;

void Ticks(){
	
	b0 = PINA & 0x01;
	b1 = PINA & 0x02;
	
	switch(state){
		case START:
			state = WAIT;
			break;
		case WAIT:
			if(!b0 && b1){
				state = ADD_PRESS;
				break;
			}
			else if(b0 && !b1){
				state = SUB_PRESS;
				break;
			}
			else if(!b0 && !b1){
				state = RESET_PRESS;
				break;
			}
			else{
				state = WAIT;
				break;
			}
		case ADD_PRESS:
			if(!b0 && !b1){
				state = RESET_PRESS;
			}
			else if(!b0){
				state = ADD_PRESS;
			}
			else{
				state = ADD_RELEASE;
			}
			break;
		case ADD_RELEASE:
			state = WAIT;
			break;
		case SUB_PRESS:
			if(!b0 && !b1){
				state = RESET_PRESS;
			}
			if(!b1){
				state = SUB_PRESS;
			}
			else{
				state = SUB_RELEASE;
			}
			break;
		case SUB_RELEASE:
			state = WAIT;
			break;
		case RESET_PRESS:
			if(!b1 && !b0){
				state = RESET_PRESS;
				break;
			}
			else{
				state = RESET_RELEASE;
				break;
			}
			
		case RESET_RELEASE: 
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
		case ADD_PRESS:
			break;
		case ADD_RELEASE:
			if(PORTB < 9){
				PORTB = PORTB + 1;
			}
			else{
				break;
			}
		case SUB_PRESS:
			break;
		case SUB_RELEASE:
			if(PORTB > 0){
				PORTB = PORTB - 1;
			}
			else{
				break;
			}
		case RESET_PRESS:
			break;
		case RESET_RELEASE:
			PORTB = 0;
			break;
		default:
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
