/*
 * Christian Legaspino & clega001@ucr.edu
 * Carissa Lo & clo020@ucr.edu
 * Lab Section: 23
 * Assignment: Lab 7 Exercise 2
 *
 * I acknowledge all content contained herein, excluding template or example
 * code, is my own original work.
 */ 


#include <avr/io.h>
#include <avr/interrupt.h>
#include "io.c"


volatile unsigned char TimerFlag = 0; 
unsigned char tmp = 0x00;
unsigned char holder = 0x00;
unsigned char b0 = 0x00;
unsigned char val = 0x00;
unsigned char cnt = 0x00;

enum States{INIT, S1, PRESS, PRESS_RELEASE} state;

unsigned long _avr_timer_M = 1; 
unsigned long _avr_timer_cntcurr = 0; 

void TimerOn() {
	TCCR1B = 0x0B;
	OCR1A = 125;
	TIMSK1 = 0x02;
	TCNT1=0;
	_avr_timer_cntcurr = _avr_timer_M;
	SREG |= 0x80;
}

void TimerOff() {
	TCCR1B = 0x00; 
}

void TimerISR() {
	TimerFlag = 1;
}

ISR(TIMER1_COMPA_vect) {
	_avr_timer_cntcurr--;
	if (_avr_timer_cntcurr == 0) { 
		TimerISR(); 
		_avr_timer_cntcurr = _avr_timer_M;
	}
}
void TimerSet(unsigned long M) {
	_avr_timer_M = M;
	_avr_timer_cntcurr = _avr_timer_M;
}
void Tick(){
	
	b0 = PINA & 0x01;
	
	switch(state){
		case INIT:
		state = S1;
		break;
		case S1:
		if(b0 == 0){
			state = PRESS;
		}
		else{
			state = S1;
		}
		break;
		case PRESS:
		if(b0 == 0){
			state = PRESS;
		}
		else{
			state = PRESS_RELEASE;
		}
		break;
		case PRESS_RELEASE:
		if(b0 == 0){
			state = INIT;
		}
		else{
			state = PRESS_RELEASE;
		}
		break;
		default:
		state = INIT;
		break;
	}
	switch(state){
		case INIT:
			PORTB = 0x01;
			val = 5;
			LCD_Cursor(1);
			LCD_WriteData(val + '0');
			break;
		case S1:
			if(PORTB >= 7){
				PORTB = 0x00;
			}
			else if(PORTB == 0x00){
				PORTB = PORTB + 1;
			}
			else if(PORTB == 0x04){
				PORTB = 0x01;
			}
			else {
				tmp = PORTB << 1;
				PORTB = tmp;
			}
			holder = PORTB;
			LCD_DisplayString(1, "Winner!");
			break;
		case PRESS:
			break;
		case PRESS_RELEASE:
			PORTB = holder;
			cnt++;
			break;
		default:
			PORTB = 0x01;
			break;
	}
}

int main()
{
	DDRA = 0x00; PORTA = 0xFF;
	DDRB = 0xFF; PORTB = 0x00;
	DDRC = 0xFF; PORTC = 0x00;
	
	LCD_init();
	TimerSet(100);
	TimerOn();
	
	
	state = INIT;
	while(1) {
		
		Tick();
		while (!TimerFlag){};
		TimerFlag = 0;
	}
}

