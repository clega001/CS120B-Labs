/*
 * Christian Legaspino & clega001@ucr.edu
 * Carissa Lo & clo020@ucr.edu
 * Lab Section: 23
 * Assignment: Lab 4 Exercise 1
 * 
 * I acknowledge all content contained herein, excluding template or example
 * code, is my own original work.
 */ 

#include <avr/io.h>
//#include "RIMS.h"

enum States{START, Wait, Do} state;
	
void Ticks(){
	switch(state){
		case START:
			state = Wait;
			break;
		case Wait: 
			state = (PINA == 0x01 ? Do : Wait);
			break;
		case Do:
			state = (PINA == 0x01 ? Wait : Do);
			break;
		default:
			state = SM1;
			break;
	}
	switch(state){
		case START:
			PORTB = 0x01;
			break;
		case Wait:
			PORTB = 0x01;
			break;
		case Do:
			PORTB = 0x02;
			break;
		default:
			PORTB = 0x01;
			break;
	}
}

int main(void)
{
	DDRA = 0x00; PORTA = 0xFF;
	DDRB = 0xFF; PORTB = 0x00;
	PORTB = 0x00;
	state = START;
    while (1) {Ticks();}
}

