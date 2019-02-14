/*
 * Christian Legaspino & clega001@ucr.edu
 * Carissa Lo & clo020@ucr.edu
 * Lab Section: 23
 * Assignment: Lab 10 Exercise 2
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

enum BL_States{BL_Start, BL_Blink} BL_state;
enum TL_States{TL_Start, TL_Blink} TL_state;
	
unsigned char blinkingLED = 0x08;
unsigned char threeLED = 0x04;
unsigned char hold = 0x00;

void TickFct_BL(){
	switch(BL_state){
		case BL_Start:
			BL_state = BL_Blink;
			break;
		case BL_Blink:
			BL_state = BL_Blink;
			break;
		default:
			BL_state = BL_Start;
			break;
	}
	switch(BL_state){
		case BL_Start:
			blinkingLED = 0x08;
			break;
		case BL_Blink:
			if(blinkingLED == 0x08){
				blinkingLED = 0x00;
				break;
			}
			else if(blinkingLED == 0x00){
				blinkingLED = 0x08;
				break;
			}
			else{
				break;
			}
		default:
			break;
	}
}
void TickFct_TL(){
	switch(TL_state){
		case TL_Start:
			TL_state = TL_Blink;
			break;
		case TL_Blink:
			TL_state = TL_Blink;
			break;
		default:
			TL_state = TL_Start;
			break;
	}
	switch(TL_state){
		case TL_Start:
			threeLED = 0x04;
			break;
		case TL_Blink:
			if(threeLED == 0x04){
				threeLED = 0x01;
				break;
			}
			else if(threeLED == 0x01){
				threeLED = 0x02;
				break;
			}
			else if(threeLED == 0x02){
				threeLED = 0x04;
				break;
			}
			else{
				break;
			}
		default:
			break;
	}
}


int main(void)
{
   DDRB = 0xFF; PORTB = 0x00;
   
   unsigned long BL_elapsedTime = 0;
   unsigned long TL_elapsedTime = 0;
   const unsigned long timerPeriod = 100;
   
   PORTB = 0x00;
   TimerSet(timerPeriod);
   TimerOn();
   
   BL_state = BL_Start;
   TL_state = TL_Start;
   PORTB = blinkingLED & threeLED;
   
   while(1){
	   if(BL_elapsedTime >= 1000){
		   TickFct_BL();
		   BL_elapsedTime = 0;
	   }
	   if(TL_elapsedTime >= 300){
		   TickFct_TL();
		   TL_elapsedTime = 0;
	   }
	   PORTB = blinkingLED | threeLED;
	   while(!TimerFlag){}
	   TimerFlag = 0;
	   BL_elapsedTime += timerPeriod;
	   TL_elapsedTime += timerPeriod;
   }
}

