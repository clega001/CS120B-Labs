/*
 * Christian Legaspino & clega001@ucr.edu
 * Carissa Lo & clo020@ucr.edu
 * Lab Section: 23
 * Assignment: Lab 9 Exercise 3
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


enum States{START, WAIT, PLAY, INTERVAL, PRE_WAIT} state;
double notes[11] = {392.00, 261.63, 329.63, 261.63, 440.00, 293.66, 349.23, 293.66, 392.00, 329.63, 261.63};
double time[11]= {50, 50, 50, 50, 50, 50, 50, 50, 70, 70, 70};
double interval[11] = {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10};
unsigned char b0 = 0x00;
unsigned char cnt = 0x00;
unsigned char iter = 0x00;
unsigned char stop = 0x00;


void Tick(){
	
	b0 = PINA & 0x01;
	
	switch(state){
		case START:
			state = WAIT;
			break;
		case WAIT:
			if(!b0){
				state = PLAY;
			}
			else{
				state = WAIT;
			}
			break;
		case PLAY:
			if(cnt == 11){
				state = PRE_WAIT;
				break;
			}
			else if(iter <= time[cnt]){
				state = PLAY;
				break;
			}
			else{
				stop = 0;
				state = INTERVAL;
				break;
			}
		case INTERVAL:
			if(stop <= interval[cnt]){
				state = INTERVAL;
				break;
			}
			else{
				iter = 0;
				cnt++;
				state = PLAY;
				break;
			}
		case PRE_WAIT:
			if(!b0){
				state = PRE_WAIT;
				break;
			}
			else{
				state = WAIT;
				break;
			}
		default:
			break;
	}
	switch(state){
		case START:
			break;
		case WAIT:
			cnt = 0;
			iter = 0;
			stop = 0;
			set_PWM(0);
			break;
		case PLAY:
			set_PWM(notes[cnt]);
			iter++;
			//PORTB = 0x00;
			break;
		case INTERVAL:
			set_PWM(0);
			stop++;
			//PORTB = 0x01;
		case PRE_WAIT:
			set_PWM(0);
			break;
		default:
			break;
	}
}

int main()
{
	DDRA = 0x00; PORTA = 0xFF;
	DDRB = 0xFF; PORTB = 0x00;
	
	TimerSet(20);
	TimerOn();
	PWM_on();
	
	state = START;
	while(1) {
		Tick();
		while (!TimerFlag){};
		TimerFlag = 0;
	}
}

