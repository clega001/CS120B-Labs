/*
 * Christian Legaspino & clega001@ucr.edu
 * Carissa Lo & clo020@ucr.edu
 * Lab Section: 23
 * Assignment: Lab 5 Exercise 3
 *
 * Lab challenge was to create a festive light sequence with only one button. 
 * 
 * I acknowledge all content contained herein, excluding template or example
 * code, is my own original work.
 */ 


#include <avr/io.h>
//#include <avr/delay.h>
//#include "RIMS.h"


//_delay_ms();
enum States{INIT, S1, S2, LIGHT_PRESS, LIGHT_RELEASE, LIGHT_2_PRESS, LIGHT_2_RELEASE} state;

unsigned char b0 = 0x00;
unsigned char tmp = 0x00;

void Ticks(){
	
	b0 = PINA & 0x01;
	
	switch(state){
		case INIT:
			state = S1;
			break;
			
		case S1:
			if(!b0 && PORTB != 0x3F){
				state = LIGHT_PRESS;
				break;
			}
			else if(!b0 && PORTB == 0x3F){
				state = LIGHT_2_PRESS;
				break;
			}
			else{
				state = S1;
				break;
			}
		case S2:
			if(!b0 && PORTB != 0x00){
				state = LIGHT_2_PRESS;
				break;
			}
			else if(!b0 && PORTB == 0x00){
				state = LIGHT_PRESS;
				break;
			}
			else{
				state = S2;
				break;
			}
		case LIGHT_PRESS:
			if(!b0){
				state = LIGHT_PRESS;
				break;
			}
			else{
				state = LIGHT_RELEASE;
				break;
			}
		case LIGHT_RELEASE:
			state = S1;
			break;
		case LIGHT_2_PRESS:
			if(!b0){
				state = LIGHT_2_PRESS;
				break;
			}
			else{
				state = LIGHT_2_RELEASE;
				break;
			}
		case LIGHT_2_RELEASE:
			state = S2;
			break;
		default:
			break;
	}
	switch(state){
		case INIT:
			PORTB = 0x00;
			break;
		case S1:
			break;
		case LIGHT_PRESS:
			break;
		case LIGHT_RELEASE:
			tmp = PORTB << 1;
			PORTB = tmp + 1;
			break;
		case LIGHT_2_PRESS:
			break;
		case LIGHT_2_RELEASE:
			tmp = PORTB >> 1;
			PORTB = tmp;
		default:
			break;
	}
}

int main(void)
{
	DDRA = 0x00; PORTA = 0xFF;
	DDRB = 0xFF; PORTB = 0x00;
	
	state = INIT;
	while (1) {
		Ticks();
	}
}