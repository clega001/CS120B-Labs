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
unsigned long _avr_timer_M = 1;
unsigned long _avr_timer_cntcurr = 0;

void TimerSet(unsigned long M) {
	_avr_timer_M = M;
	_avr_timer_cntcurr = _avr_timer_M;
}
void TimerOn() {
	TCCR1B = 0x0B;
	OCR1A = 125;
	TIMSK1 = 0x02;
	TCNT1=0;
	_avr_timer_cntcurr = _avr_timer_M;
	SREG |= 0x80;
}
void TimerOff() {TCCR1B = 0x00;}
void TimerISR() {TimerFlag = 1;}
ISR(TIMER1_COMPA_vect) {
	_avr_timer_cntcurr--; 
	if (_avr_timer_cntcurr == 0) {
		TimerISR();
		_avr_timer_cntcurr = _avr_timer_M;
	}
}

enum States{Start, Init, Press, Press_Release}state;
unsigned char b0 = 0x00;
unsigned char val = 0x00;
unsigned char temp = 0x00;
void Tick(){
	b0 = PINA & 0x01;
	switch(state){
		case Start:
			state = Init;
			break;
		case Init:
			if(~b0){
				state = Press;
			}
			else{
				state = Init;
			}
			break;
		case Press:
			if(~b0){
				state = Press;
			}
			else{
				state = Press_Release;
			}
			break;
		case Press_Release:
			state = Init;
			break;
		default:
			state = Start;
			break;
		switch(state){
			case Start:
				PORTB = 0x01;
				val = 5;
				LCD_Cursor(1);
				LCD_WriteData(val + '0');
				break;
			case Init:
				if(PORTB == 0x04){
					PORTB = 0x01;
				}
				else{
					temp = PORTB << 1;
					PORTB = temp;
				}
				break;
			case Press:
				break;
			case Press_Release:
				break;
			default:
				PORTB = 0x01;
				val = 5;
				LCD_Cursor(1);
				LCD_WriteData(val + '0');
				break;			
		}
	}
}

int main(void)
{
	DDRC = 0xFF; PORTC = 0x00;
	DDRD = 0xFF; PORTD = 0x00;
	DDRA = 0x00; PORTA = 0xFF;

	LCD_init();
	TimerSet(100);
	TimerOn();
	
	state = Start;

	while (1)
	{
		Tick();
		while(!TimerFlag){}
		TimerFlag = 0;
		continue;
	}
}

