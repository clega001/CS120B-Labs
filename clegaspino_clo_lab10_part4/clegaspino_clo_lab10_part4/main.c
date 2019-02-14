/*
 * Christian Legaspino & clega001@ucr.edu
 * Carissa Lo & clo020@ucr.edu
 * Lab Section: 23
 * Assignment: Lab 10 Exercise 4
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
void set_PWM(double frequency) {
	static double current_frequency;
	if (frequency != current_frequency) {
		if (!frequency) { TCCR0B &= 0x08; }
		else { TCCR0B |= 0x03; }
		if (frequency < 0.954) { OCR0A = 0xFFFF; }
		else if (frequency > 31250) { OCR0A = 0x0000; }
		else { OCR0A = (short)(8000000 / (128 * frequency)) - 1; }

		TCNT0 = 0;
		current_frequency = frequency;
	}
}

void PWM_on() {
	TCCR0A = (1 << COM0A0) | (1 << WGM00);
	TCCR0B = (1 << WGM02) | (1 << CS01) | (1 << CS00);
	set_PWM(0);
}

void PWM_off() {
	TCCR0A = 0x00;
	TCCR0B = 0x00;
}

enum BL_States{BL_Start, BL_Blink} BL_state;
enum TL_States{TL_Start, TL_Blink, TL_Blink_Again} TL_state;
enum Butt_States{Butt_Start, Butt_Press} Butt_state;
enum Freq_States{Freq_Start, Up, Up_Press, Down, Down_Press} Freq_state;
enum Combine_States{Com_Start, Com_Action} Com_states;
	
unsigned char blinkingLED = 0x10;
unsigned char threeLED = 0x04;
unsigned char hold = 0x00;
unsigned char b = 0x00;
unsigned char i = 0x00;
unsigned char b_up = 0x00;
unsigned char b_down = 0x00;
double arr[8] = {261.63, 293.66, 329.63, 349.63, 392.00, 440.00, 493.88, 523.25};

void TickFct_BL(){
	
	/*blinkingLED = PORTB & 0x10;*/
	
	switch(BL_state){
		case BL_Start:
			BL_state = BL_Blink;
			break;
		case BL_Blink:
			BL_state = BL_Start;;
			break;
		default:
			BL_state = BL_Start;
			break;
	}
	switch(BL_state){
		case BL_Start:
			blinkingLED = 0x01;
			break;
		case BL_Blink:
			blinkingLED = 0x00;
		default:
			break;
	}
}
void TickFct_TL(){
	
	/*threeLED = PORTB & 0x07;*/
	
	switch(TL_state){
		case TL_Start:
			TL_state = TL_Blink;
			break;
		case TL_Blink:
			TL_state = TL_Blink_Again;
			break;
		case TL_Blink_Again:
			TL_state = TL_Start;
			break;
		default:
			TL_state = TL_Start;
			break;
	}
	switch(TL_state){
		case TL_Start:
			threeLED = 0x01;
			break;
		case TL_Blink:
			threeLED = 0x02;
			break;
		case TL_Blink_Again:
			threeLED = 0x04;
			break;
		default:
			break;
	}
}
void TickFct_Freq(){
	b_up = PINA & 0x01;
	b_down = PINA & 0x02;
	
	switch(Freq_state){
		case Freq_Start:
			if(!b_up){
				Freq_state = Up;
				break;
			}
			else if(!b_down){
				Freq_state = Down;
				break;
			}
			else{
				Freq_state = Freq_Start;
				break;
			}
		case Up:
			if(!b_up){
				Freq_state = Up;
				break;
			}
			else{
				Freq_state = Up_Press;
				break;
			}
		case Up_Press:
			Freq_state = Freq_Start;
			break;
		case Down:
			if(!b_down){
				Freq_state = Down;
				break;
			}
			else{
				Freq_state = Down_Press;
				break;
			}
		case Down_Press:
			Freq_state = Freq_Start;
			break;
		default:
			Freq_state = Freq_Start;
			break;
	}
	switch(Freq_state){
		case Freq_Start:
			break;
		case Up:
			break;
		case Up_Press:
			if(i < 7){
				i++;
				break;
			}
			else{
				break;
			}
		case Down:
			break;
		case Down_Press:
			if(i > 0){
				i--;
				break;
			}
			else{
				break;
			}
		default:
			break;
	}
}
void TickFct_Butt(){
	
	b = PINA & 0x04;
	
	switch(Butt_state){
		case Butt_Start:
			if(!b){
				Butt_state = Butt_Press;
				break;
			}
			else{
				Butt_state = Butt_Start;
				break;
			}
		case Butt_Press:
			if(!b){
				Butt_state = Butt_Press;
				break;
			}
			else{
				Butt_state = Butt_Start;
				break;
			}
		default:
			Butt_state = Butt_Start;
			break;
	}
	switch(Butt_state){
		case Butt_Start:
			set_PWM(0);
			break;
		case Butt_Press:
			set_PWM(arr[i]);
			break;
		default:
			set_PWM(0);
			break;
	}
}
void Combine_Fct(){
	b = PINA & 0x01;
	switch(Com_states){
		case Com_Start:
			Com_states = Com_Action;
		case Com_Action:
			Com_states = Com_Action;
		default:
			Com_states = Com_Start;
			break;
	}
	switch(Com_states){
		case Com_Start:
			break;
		case Com_Action:
			PORTB = (blinkingLED << 4) | threeLED;
			break;
		default:
			break;
	}
}


int main(void)
{
   DDRA = 0x00; PORTA = 0xFF;
   DDRB = 0xFF; PORTB = 0x00;
   
   unsigned long BL_elapsedTime = 0;
   unsigned long TL_elapsedTime = 0;
   unsigned long Butt_elapsedTime = 0;
   unsigned long Freq_elapsedTime = 0;
   const unsigned long timerPeriod = 100;
   
   TimerSet(timerPeriod);
   TimerOn();
   PWM_on();
   
   PORTB = blinkingLED | threeLED;
   
   BL_state = BL_Start;
   TL_state = TL_Start;
   Butt_state = Butt_Start;
   Freq_state = Freq_Start;
   Com_states = Com_Start;

   while(1){
	   if(BL_elapsedTime >= 1000){
		   TickFct_BL();
		   BL_elapsedTime = 0;
	   }
	   if(TL_elapsedTime >= 300){
		   TickFct_TL();
		   TL_elapsedTime = 0;
	   }
	   if(Freq_elapsedTime >= 2){
		   TickFct_Freq();
		   Freq_elapsedTime = 0;
	   }
 	   if(Butt_elapsedTime >= 2){
 		   TickFct_Butt();
 		   Butt_elapsedTime = 0;
 	   }
	   /*Combine_Fct();*/
	   PORTB = (blinkingLED << 4) | threeLED;
	   while(!TimerFlag){}
	   TimerFlag = 0;
	   BL_elapsedTime += timerPeriod;
	   TL_elapsedTime += timerPeriod;
	   Butt_elapsedTime += 1;
	   Freq_elapsedTime += 1;
   }
}

