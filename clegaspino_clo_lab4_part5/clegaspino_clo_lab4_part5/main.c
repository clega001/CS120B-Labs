/*
 * Christian Legaspino & clega001@ucr.edu
 * Carissa Lo & clo020@ucr.edu
 * Lab Section: 23
 * Assignment: Lab 4 Exercise 5
 *
 * I acknowledge all content contained herein, excluding template or example
 * code is my own original work.
 */ 
 

#include <avr/io.h>

enum States {Start, Init, Lock, Press, Unlock, Unlock_code} state;

char arr[4] = {0, 0, 0, 0};
char arr2[4] = {0, 0, 0, 0};
	
unsigned char GetBit(unsigned char x, unsigned char k) {
	return ((x & (0x01 << k)) != 0);
}
void Tick() {
	switch(state) {
		case Start:
			state = Init;
			break;
		case Init:
			state = Lock;
			break;
		case Lock:
			if (PINA == 0x04) {
				arr[0] = 4;
				state = Press;
			}
			else {
				state = Lock;
			}
			break;
		case Press:
			if (PINA == 0x04 && arr[0] == 0x04) {
				state = Press;
			}
			else if((PINA == 0x01) && (arr[0] == 0x04) && (arr[1] == 0) && arr[2] == 0 && arr[3] == 0) {
				arr[1] = 0x01;
				state = Press;
			}
			else if((PINA == 0x02) && (arr[0] == 0x04) && (arr[1] == 0x01) && arr[2] == 0 && arr[3] == 0) {
				arr[2] = 0x02;
				state = Press;
			}
			else if((PINA == 0x01) && (arr[0] == 0x04) && (arr[1] == 0x01) && (arr[2] == 0x02) && arr[3] == 0) {
				arr[3] = 0x01;
				state = Unlock;
			}
			else {
				arr[0] = 0;
				arr[1] = 0;
				arr[2] = 0;
				arr[3] = 0;
				state = Lock;
			}
			break;
		case Unlock:
			if(PINA == 0x80) {
				state = Lock;
			}	
			else if (PINA == 0x04) {
				arr2[0] = 4;
				state = Unlock_code;
			}		
			else {
				state = Unlock;
			}
			break;
		case Unlock_code:
			if (PINA == 0x04 && arr2[0] == 0x04) {
				state = Unlock_code;
			}
			else if((PINA == 0x01) && (arr2[0] == 0x04) && (arr2[1] == 0) && arr2[2] == 0 && arr2[3] == 0) {
				arr2[1] = 0x01;
				state = Unlock_code;
			}
			else if((PINA == 0x02) && (arr2[0] == 0x04) && (arr2[1] == 0x01) && arr2[2] == 0 && arr2[3] == 0) {
				arr2[2] = 0x02;
				state = Unlock_code;
			}
			else if((PINA == 0x01) && (arr2[0] == 0x04) && (arr2[1] == 0x01) && (arr2[2] == 0x02) && arr2[3] == 0) {
				arr2[3] = 0x01;
				state = Lock;
			}
			else {
				arr2[0] = 0;
				arr2[1] = 0;
				arr2[2] = 0;
				arr2[3] = 0;
				state = Unlock;
			}
			break;
		default:
			state = Start;
			break;
	}
	switch(state) {
		case Start:
			PORTC = 1;
			break;
		case Init:
			PORTC = 2;
			break;
		case Lock:
			PORTB = 0;
			PORTC = 3;
			break;
		case Press:
			PORTC = 4;
			break;
		case Unlock:
			PORTC = 5;
			PORTB = 1;
			break;
		case Unlock_code:
			PORTC = 6;
			break;
		default:
			PORTC = 1;
			break;
	}
}

int main(void) {
	DDRA = 0x00; PINA = 0xFF;
	DDRB = 0xFF; PORTB = 0x00;
	DDRC = 0xFF; PORTC = 0x00;
	state = Start;
    while (1) 
    {
	Tick();	
    }
}


