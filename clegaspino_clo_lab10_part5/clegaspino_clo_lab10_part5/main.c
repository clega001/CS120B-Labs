/*
 * Christian Legaspino & clega001@ucr.edu
 * Carissa Lo & clo020@ucr.edu
 * Lab Section: 23
 * Assignment: Lab 10 Exercise 5
 *
 * I acknowledge all content contained herein, excluding template or example
 * code, is my own original work.
 */


#include <avr/io.h>
#include <avr/interrupt.h>

#define button_A0 (~PINA & 0x01)
#define button_A1 (~PINA & 0x02)


volatile unsigned char TimerFlag = 0;
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
enum C_States { C_SMStart, C_Wait, C_Inc, C_Dec, C_Reset} C_State;
enum SB_States { SB_SMStart, SB_Set} SB_State;
unsigned char tempB = 0x00;
unsigned long current = 0x00;
unsigned long set_period = 0x00;


void Held_Button()
{
	if(set_period == 1000 && current >= 3000)
	set_period = 400;
	else if(set_period == 1 && current >= 1000)
	set_period = 1000;
	if (set_period == 400 || set_period == 1000)
	{
		if(C_State == C_Inc && tempB < 9)
		++tempB;
		else if(C_State == C_Dec && tempB > 0)
		--tempB;
	}
}
void TickFct_Counter()
{
	switch(C_State)
	{
		case C_SMStart:
		C_State = C_Wait;
		break;
		case C_Wait:
		if(button_A0 && !button_A1)
		C_State = C_Inc;
		else if(!button_A0 && button_A1)
		C_State = C_Dec;
		else if(button_A0 && button_A1)
		C_State = C_Reset;
		else
		C_State = C_Wait;
		break;
		case C_Inc:
		if(button_A0 && !button_A1)
		{
			C_State = C_Inc;
			Held_Button();
		}
		else if(button_A0 && button_A1)
		C_State = C_Reset;
		else
		{
			C_State = C_Wait;
			if(tempB < 9)
			++tempB;
		}
		break;
		case C_Dec:
		if(!button_A0 && button_A1)
		{
			C_State = C_Dec;
			Held_Button();
		}
		else if(button_A0 && button_A1)
		C_State = C_Reset;
		else
		{
			C_State = C_Wait;
			if(tempB > 0)
			--tempB;
		}
		break;
		case C_Reset:
		if(!button_A0 && !button_A1)
		C_State = C_Wait;
		else
		C_State = C_Reset;
		break;
		default:
		break;
	}
	switch(C_State)
	{
		case C_SMStart:
		break;
		case C_Wait:
		current = 0;
		set_period = 1;
		break;
		case C_Inc:
		break;
		case C_Dec:
		break;
		case C_Reset:
		tempB = 0;
		break;
		default:
		break;
	}
}
void TickFct_SetB() {
	switch(SB_State){
		case SB_SMStart:
		SB_State = SB_Set;
		break;
		case SB_Set:
		SB_State = SB_Set;
		break;
		default:
		break;
	}
	switch(SB_State){
		case SB_SMStart:
		PORTB = 0x00;
		break;
		case SB_Set:
		PORTB = tempB;
		break;
		default:
		break;
	}
}
int main(void){
	DDRA = 0x00; PORTA = 0xFF; // Configure port A's 8 pins as inputs, initialize to 0s
	DDRB = 0xFF; PORTB = 0x00; // Configure port B's 8 pins as outputs, initialize to 0s
	unsigned long C_elapsedTime = 1;

	const unsigned long timerPeriod = 1;
	TimerSet(timerPeriod);
	TimerOn();
	C_State = C_SMStart;
	SB_State = SB_SMStart;
	set_period = timerPeriod;
	while (1) {
		if (C_elapsedTime >= set_period) {
			TickFct_Counter();
			C_elapsedTime = 0;
		}
		TickFct_SetB();
		while (!TimerFlag){}   // Wait for timer period
		TimerFlag = 0;         // Lower flag raised by timer
		C_elapsedTime += timerPeriod;
		current += timerPeriod;
	}
}


