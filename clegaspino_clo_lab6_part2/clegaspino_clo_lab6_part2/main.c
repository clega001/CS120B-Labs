/*
 * Christian Legaspino & clega001@ucr.edu
 * Carissa Lo & clo020@ucr.edu
 * Lab Section: 23
 * Assignment: Lab 6 Exercise 2
 *
 * I acknowledge all content contained herein, excluding template or example
 * code, is my own original work.
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>

volatile unsigned char TimerFlag = 0; // TimerISR() sets this to 1. C programmer should clear to 0.
unsigned char tmp = 0x00;
unsigned char holder = 0x00;
unsigned char b0 = 0x00;
enum States{INIT, S1, PRESS, PRESS_RELEASE} state;

// Internal variables for mapping AVR's ISR to our cleaner TimerISR model.
unsigned long _avr_timer_M = 1; // Start count from here, down to 0. Default 1 ms.
unsigned long _avr_timer_cntcurr = 0; // Current internal count of 1ms ticks

void TimerOn() {
	TCCR1B = 0x0B;
	OCR1A = 125;
	TIMSK1 = 0x02;
	TCNT1=0;
	_avr_timer_cntcurr = _avr_timer_M;
	SREG |= 0x80; 
}

void TimerOff() {
	TCCR1B = 0x00; // bit3bit1bit0=000: timer off
}

void TimerISR() {
	TimerFlag = 1;
}

ISR(TIMER1_COMPA_vect) {
	_avr_timer_cntcurr--; 
	if (_avr_timer_cntcurr == 0) { // results in a more efficient compare
		TimerISR(); // Call the ISR that the user uses
		_avr_timer_cntcurr = _avr_timer_M;
	}
}

// Set TimerISR() to tick every M ms
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
			break;
		case PRESS:
			break;
		case PRESS_RELEASE:
			PORTB = holder;
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
	TimerSet(100);
	TimerOn();
	
	PORTB = 0x01;
	
	while(1) {
		// User code (i.e. synchSM calls)
		Tick();
		while (!TimerFlag){};    
		TimerFlag = 0;
	}
}

