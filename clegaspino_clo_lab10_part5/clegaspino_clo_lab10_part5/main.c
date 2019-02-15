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
enum Cnt_States {Start, Wait, Inc, Dec, Reset} Cnt_state;
enum SB_States { SB_SMStart, SB_Set} SB_State;
unsigned char val = 0x00;
unsigned long cnt_timer = 0x00;
unsigned long change_period = 0x00;
unsigned char b0 = 0x00;
unsigned char b1 = 0x00;

void Hold3Sec()
{
	if(change_period == 1000 && cnt_timer >= 3000)
	change_period = 400;
	else if(change_period == 1 && cnt_timer >= 1000)
	change_period = 1000;
}
void TickFct_Counter(){
	
	b0 = PINA & 0x01;
	b1 = PINA & 0x02;
	switch(Cnt_state){
		case Start:
			Cnt_state = Wait;
			break;
		case Wait:
			if(!b0){
				Cnt_state = Inc;
			}
			else if(!b1){
				Cnt_state = Dec;
			}
			else if(!b0 && !b1){
				Cnt_state = Reset;
			}
			else{
				Cnt_state = Wait;
			}
			break;
		case Inc:
			if(!b0){
				Cnt_state = Inc;
				Hold3Sec();
			}
			else if(!b1){
				Cnt_state = Reset;
			}
			else{
				Cnt_state = Wait;
				if(val < 9){
					val++;
				}
			}
			break;
		case Dec:
			if(!b1){
				Cnt_state = Dec;
				Hold3Sec();
				break;
			}
			else if(!b0 && !b1){
				Cnt_state = Reset;
				break;
			}
			else{
				Cnt_state = Wait;
				if(val > 0){
					val--;
				}
				break;
			}
		case Reset:
			if(b0 || b1){
				Cnt_state = Wait;
			}
			else{
				Cnt_state = Reset;
			}
			break;
		default:
			Cnt_state = Wait;
			break;
	}
	switch(Cnt_state){
		case Start:
			break;
		case Wait:
			cnt_timer = 0;
			change_period = 1;
			break;
		case Inc:
			val++;
			break;
		case Dec:
			val--;
			break;
		case Reset:
			val = 0;
			break;
		default:
			break;
	}
}

int main(void){
	DDRA = 0x00; PORTA = 0xFF; // Configure port A's 8 pins as inputs, initialize to 0s
	DDRB = 0xFF; PORTB = 0x00; // Configure port B's 8 pins as outputs, initialize to 0s
	unsigned long Cnt_elapsedTime = 1;

	const unsigned long timerPeriod = 1;
	TimerSet(timerPeriod);
	TimerOn();
	Cnt_state = Start;
	
	change_period = timerPeriod;
	
	
	while (1) {
		if (Cnt_elapsedTime >= change_period) {
			TickFct_Counter();
			Cnt_elapsedTime = 0;
		}
		PORTB = val;
		while (!TimerFlag){} 
			TimerFlag = 0;
			Cnt_elapsedTime += timerPeriod;
			cnt_timer += timerPeriod;
	}
}


