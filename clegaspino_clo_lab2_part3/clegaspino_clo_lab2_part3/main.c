/*
 * Christian Legaspino & clega001@ucr.edu
 * Carissa Lo & clo020@ucr.edu
 * Lab Section: 23
 * Assignment: Lab 2 Exercise 3
 *
 *Exercise Description: Same idea as exercise 2, however, PC7 is set to 1 if all
 *parking spots are taken.
 *
 * I acknowledge all content contained herein, excluding template or example
 * code is my own original work.
 */ 

#include <avr/io.h>


int main(void)
{
    /* Replace with your application code */
    DDRA = 0x00; PORTA = 0xFF;
	DDRB = 0xFF; PORTB = 0x00;
	//unsigned char p3 = 0x00;
	//unsigned char p2 = 0x00;
	//unsigned char p1 = 0x00;
	//unsigned char p0 = 0x00;
	
	unsigned char cntavail;
	
	while(1){
		//p3 = PINA & 0x08;
		//p2 = PINA & 0x04;
		//p1 = PINA & 0x02;
		//p0 = PINA & 0x01;
		if(PINA == 0x0F) {
			cntavail = 0x80;
		}
		else if(PINA == 0x0E || PINA == 0x0D || PINA == 0x0B
				|| PINA == 0x07){
			cntavail = 0x01;
		}
		else if(PINA == 0x0C || PINA == 0x0A || PINA == 0x09 
				|| PINA == 0x06 || PINA == 0x05 || PINA == 0x03) {
			cntavail = 0x02;
		}
		else if(PINA == 0x01 || PINA == 0x02 || PINA == 0x04 || PINA == 0x08){
			cntavail = 0x03;
		}
		else {cntavail = 0x04;}
			
		PORTC = cntavail;

	}
}

