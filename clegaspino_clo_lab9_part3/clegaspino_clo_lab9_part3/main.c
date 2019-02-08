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
void PWM_on() {
	TCCR0A = (1 << COM0A0) | (1 << WGM00);
	TCCR0B = (1 << WGM02) | (1 << CS01) | (1 << CS00);
	set_PWM(0);
}

void PWM_off() {
	TCCR0A = 0x00;
	TCCR0B = 0x00;
}

enum States{OFF, OFF_PRESS, START, START_PRESS, UP_PRESS, UP_RELEASE, DOWN_PRESS, DOWN_RELEASE} state;
unsigned char b0 = 0x00;
unsigned char b1 = 0x00;
unsigned char b2 = 0x00;
double arr[8] = {261.63, 293.66, 329.63, 349.63, 392.00, 440.00, 493.88, 523.25};
unsigned char cnt = 0;


void Tick(){
	
	b0 = PINA & 0x01;
	b1 = PINA & 0x02;
	b2 = PINA & 0x04;
	
	switch(state){
		case OFF:
			if(!b2){
				state = START_PRESS;
				break;
			}
			else{
				state = OFF;
			}
		case OFF_PRESS:
			if(!b2){
				state = OFF_PRESS;
			}
			else{
				state = OFF;
			}
			break;
		case START_PRESS:
			if(!b2){
				state = START_PRESS;
				break;
			}
			else{
				state = START;
				break;
			}
		case START:
			if(!b2){
				state = OFF_PRESS;
				break;
			}
			else if(!b0 && b1){
				state = UP_PRESS;
				break;
			}
			else if(b0 && !b1){
				state = DOWN_PRESS;
				break;
			}
			else{
				state = START;
				break;
			}	
		case UP_PRESS:
			if(!b0){
				state = UP_PRESS;
			}
			else{
				state = UP_RELEASE;
			}
			break;
		case UP_RELEASE:
			state = START;
			break;
		case DOWN_PRESS:
			if(!b1){
				state = DOWN_PRESS;
			}
			else{
				state = DOWN_RELEASE;
			}
			break;
		case DOWN_RELEASE:
			state = START;
			break;
		default:
			state = START;
			break;
	}
	switch(state){
		case OFF:
			set_PWM(0);
			break;
		case OFF_PRESS:
			set_PWM(0);
			break;
		case START:
			set_PWM(arr[cnt]);
			break;
		case START_PRESS:
			break;
		case UP_PRESS:
			break;
		case UP_RELEASE:
			if(cnt == 7){
				break;
			}
			else{
				cnt++;
				break;
			}
		case DOWN_PRESS:
			break;
		case DOWN_RELEASE:
			if(cnt == 0){
				break;
			}
			else{
				cnt--;
				break;
			}
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
   state = OFF;
    while (1) 
    {
		Tick();
    }
}

