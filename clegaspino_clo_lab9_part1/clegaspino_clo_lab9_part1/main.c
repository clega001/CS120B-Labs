/*
 * Christian Legaspino & clega001@ucr.edu
 * Carissa Lo & clo020@ucr.edu
 * Lab Section: 23
 * Assignment: Lab 9 Exercise 1
 *
 * I acknowledge all content contained herein, excluding template or example
 * code, is my own original work.
 */

#include <avr/io.h>

// 0.954 hz is lowest frequency possible with this function,
// based on settings in PWM_on()
// Passing in 0 as the frequency will stop the speaker from generating sound
void set_PWM(double frequency) {
	static double current_frequency; // Keeps track of the currently set frequency
	// Will only update the registers when the frequency changes, otherwise allows
	// music to play uninterrupted.
	if (frequency != current_frequency) {
		if (!frequency) { TCCR0B &= 0x08; } //stops timer/counter
		else { TCCR0B |= 0x03; } // resumes/continues timer/counter
		
		// prevents OCR3A from overflowing, using prescaler 64
		// 0.954 is smallest frequency that will not result in overflow
		if (frequency < 0.954) { OCR0A = 0xFFFF; }
		
		// prevents OCR0A from underflowing, using prescaler 64                    // 31250 is largest frequency that will not result in underflow
		else if (frequency > 31250) { OCR0A = 0x0000; }
		
		// set OCR3A based on desired frequency
		else { OCR0A = (short)(8000000 / (128 * frequency)) - 1; }

		TCNT0 = 0; // resets counter
		current_frequency = frequency; // Updates the current frequency
	}
}

void PWM_on() {
	TCCR0A = (1 << COM0A0) | (1 << WGM00);
	// COM3A0: Toggle PB3 on compare match between counter and OCR0A
	TCCR0B = (1 << WGM02) | (1 << CS01) | (1 << CS00);
	// WGM02: When counter (TCNT0) matches OCR0A, reset counter
	// CS01 & CS30: Set a prescaler of 64
	set_PWM(0);
}

void PWM_off() {
	TCCR0A = 0x00;
	TCCR0B = 0x00;
}

enum States{START, C_PRESS, C_RELEASE, D_PRESS, D_RELEASE, E_PRESS, E_RELEASE, RESET_PRESS, RESET_RELEASE} state;
unsigned char b0 = 0x00;
unsigned char b1 = 0x00;
unsigned char b2 = 0x00;


void Tick(){
	
	b0 = PINA & 0x01;
	b1 = PINA & 0x02;
	b2 = PINA & 0x04;
	
	switch(state){
		case START:
			if(!b0 && b1 && b2){
				state = C_PRESS;
				break;
			}
			else if(b0 && !b1 && b2){
				state = D_PRESS;
				break;
			}
			else if(b0 && b1 && !b2){
				state = E_PRESS;
				break;
			}
			else{
				state = START;
				break;
			}
		case C_PRESS:
			if(!b0 && b1 && b2){
				state = C_PRESS;
			}
			else if(!b0 && (!b1||!b2)){
				state = RESET_PRESS;
			}
			else{
				state = C_RELEASE;
			}
			break;
		case C_RELEASE:
			state = START;
			break;
		case D_PRESS:
			if(b0 && !b1 && b2){
				state = D_PRESS;
			}
			else if(!b1 && (!b0||!b2)){
				state = RESET_PRESS;
			}
			else{
				state = D_RELEASE;
			}
			break;
		case D_RELEASE:
			state = START;
			break;
		case E_PRESS:
			if(b0 && b1 && !b2){
				state = E_PRESS;
			}
			else if(!b2 && (!b0||!b1)){
				state = RESET_PRESS;
			}
			else{
				state = E_RELEASE;
			}
			break;
		case E_RELEASE:
			state = START;
			break;
		case RESET_PRESS:
			if(!b2 || !b0 || !b1){
				state = RESET_PRESS;
			}
			else{
				state = RESET_RELEASE;
			}
			break;
		case RESET_RELEASE:
			state = START;
			break;
		default:
			state = START;
			break;
	}
	switch(state){
		case START:
			set_PWM(0);
			break;
		case C_PRESS:
			set_PWM(261.63);
			break;
		case C_RELEASE:
			set_PWM(0);
			break;
		case D_PRESS:
			set_PWM(293.66);
			break;
		case D_RELEASE:
			set_PWM(0);
			break;
		case E_PRESS:
			set_PWM(329.63);
			break;
		case E_RELEASE:
			set_PWM(0);
			break;
		case RESET_PRESS:
			set_PWM(0);
			break;
		case RESET_RELEASE:
			break;
		default:
			set_PWM(0);
			break;
	}
}

int main(void)
{
   DDRA = 0x00; PORTA = 0xFF;
   DDRB = 0xFF; PORTB = 0x00;
   
   PWM_on();
   state = START;
    while (1) 
    {
		Tick();
    }
}

