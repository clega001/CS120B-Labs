/*
 * Christian Legaspino & clega001@ucr.edu
 * Carissa Lo & clo020@ucr.edu
 * Lab Section: 23
 * Assignment: Lab 4 Exercise 5
 * 
 * I acknowledge all content contained herein, excluding template or example
 * code, is my own original work.
 */ 

#include <avr/io.h>
//#include "RIMS.h"

enum States{START, INIT, LOCK, PRESS, UNLOCK, PRESS_UNLOCK} state;
unsigned char arr[4] = {0x04, 0x01, 0x02, 0x01};
unsigned char cnt = 0;
//unsigned char yes = 0;
	
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
			if(arr[cnt] == PINA){
				state = PRESS;
				cnt = cnt + 1;
				PORTC = 0x10;
			}
			else if(arr[cnt] == PINA){
				state = PRESS;
				cnt = cnt + 1;
				PORTC = 0x30;
			}
			else if(arr[cnt] == PINA){
				state = PRESS;
				cnt = cnt + 1;
				PORTC = 0x70;
			}
			else if(arr[cnt] == PINA){
				state = UNLOCK;
				cnt = 0;
				PORTC = 0xF0;
			}
			else{
				state = LOCK;
				cnt = 0;
				PORTC = 3;
			}
			PORTD = cnt;
			break;
		case PRESS:
			state = LOCK;
			break;
		case UNLOCK:
			if(PINA == 0x80){
				state = LOCK;
				cnt = 0;
				PORTC = 3;
				
			}
			else if(arr[cnt] == PINA){
				state = PRESS_UNLOCK;
				cnt = cnt + 1;
				PORTC = 5;
			}
			else if(arr[cnt] == PINA){
				state = PRESS_UNLOCK;
				cnt = cnt + 1;
				PORTC = 5;
			}
			else if(arr[cnt] == PINA){
				state = PRESS_UNLOCK;
				cnt = cnt + 1;
				PORTC = 5;
			}
			else if(arr[cnt] == PINA){
				state = PRESS_UNLOCK;
				cnt = 0;
				PORTC = 5;
			}
			else{
				state = UNLOCK;
				cnt = 0;
				PORTC = 5;
			}
			break;
			
		case PRESS_UNLOCK:
			state = UNLOCK;
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
	DDRD = 0xFF; PORTD = 0x00;
	
	state = START;
    while (1) {Ticks();}
}

