/*
 * Christian Legaspino & clega001@ucr.edu
 * Carissa Lo & clo020@ucr.edu
 * Lab Section: 23
 * Assignment: Lab 7 Exercise 1
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

enum States{Start, Init, Inc, Inc_Wait, Dec, Dec_Wait, Reset, Reset_Wait}state;
unsigned char b = 0x00;
unsigned char val = 0x00;
unsigned char cnt = 0x00;
void Tick(){
	b = (~PINA) & 0x03;
	switch(state){
		case Start:
		LCD_Cursor(1);
		LCD_WriteData( val + '0' );
		state = Init;
		break;
		case Init:
		if(b == 0x02){
			state = Dec;
		}
		else if(b == 0x01){
			state = Inc;
		}
		break;
		case Inc:
		if(val < 9){
			val++;
		}
		LCD_Cursor(1);
		LCD_WriteData(val + '0');
		state = Inc_Wait;
		break;
		case Inc_Wait:
		if(b == 0x03){
			cnt = 0;
			state = Reset;
		}
		else if(b == 0){
			cnt = 0;
			state = Init;
		}
		else if((b == 0x01) & (cnt == 10)){
			state = Inc;
			cnt = 0;
		}
		else{
			state = Inc_Wait;
			cnt++;
		}
		break;
		case Dec:
		if(val > 0){
			val--;
		}
		LCD_Cursor(1);
		LCD_WriteData(val + '0');
		state = Dec_Wait;
		break;
		case Dec_Wait:
		if(b == 0x03){
			cnt = 0;
			state = Reset;
		}
		else if(b == 0){
			cnt = 0;
			state = Init;
		}
		else if((b == 0x02) & (cnt == 10)){
			state = Dec;
			cnt = 0;
		}
		else{
			state = Dec_Wait;
			cnt++;
		}
		break;
		case Reset:
		val = 0;
		LCD_Cursor(1);
		LCD_WriteData(val + '0');
		state = Init;
		break;
		default:
		state = Start;
		break;
		
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
