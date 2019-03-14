/*
 * CustomLab_Complexity2.c
 *
 * Created: 2/28/2019 4:18:48 PM
 * Author : Christian Legaspino
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>
#include <bit.h>
#include <timer.h>
#include <stdio.h>
#include <util/delay.h>
#include <stdbool.h>

//--------Find GCD function --------------------------------------------------
unsigned long int findGCD(unsigned long int a, unsigned long int b)
{
    unsigned long int c;
    while(1){
        c = a%b;
        if(c==0){return b;}
        a = b;
b = c;
    }
    return 0;
}
//--------End find GCD function ----------------------------------------------

//--------Task scheduler data structure---------------------------------------
// Struct for Tasks represent a running process in our simple real-time operating system.
typedef struct _task {
    /*Tasks should have members that include: state, period,
        a measurement of elapsed time, and a function pointer.*/
    signed char state; //Task's current state
    unsigned long int period; //Task period
    unsigned long int elapsedTime; //Time elapsed since last task tick
    int (*TickFct)(int); //Task tick function
} task;

//--------End Task scheduler data structure-----------------------------------
/////////////////////////////////////////////////////////////////////////////
void ADC_init() {
	//ADCSRA |= (1 << ADEN) | (1 << ADSC) | (1 << ADATE);
	// AREF = AVcc
	ADMUX = (1<<REFS0);
	
	// ADC Enable and prescaler of 128
	// 16000000/128 = 125000
	ADCSRA = (1<<ADEN)|(1<<ADPS2)|(1<<ADPS1)|(1<<ADPS0);
}
uint16_t ADC_read(uint8_t ch)
{
	// select the corresponding channel 0~7
	// ANDing with ’7? will always keep the value
	// of ‘ch’ between 0 and 7
	ch &= 0b00000111;  // AND operation with 7
	ADMUX = (ADMUX & 0xF8)|ch; // clears the bottom 3 bits before ORing
	
	// start single conversion
	// write ’1? to ADSC
	ADCSRA |= (1<<ADSC);
	
	// wait for conversion to complete
	// ADSC becomes ’0? again
	// till then, run loop continuously
	while(ADCSRA & (1<<ADSC));
	
	return (ADC);
}
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
//////////////////////////////////////////////////////////////////////////////////
//--------Shared Variables----------------------------------------------------
unsigned char b = 0x00;

unsigned short x_axis = 0x0000;
unsigned short y_axis = 0x0000;

unsigned char direction = 0x00;
unsigned char tmp = 0x00;

//w/o buzzer
//unsigned short led7Seg[] = {0xC0, 0xF9, 0xA4, 0xB0, 0x99, 0x92, 0x82, 0xF8, 0x80, 0x98};
//w/ buzzer
unsigned char led7Seg[] = {0x88, 0xF9, 0x4C, 0x68, 0x39, 0x2A, 0x0A, 0xF8, 0x08, 0x38};
unsigned char test = 5;

//Notes fall column wise
unsigned char arrX[] = {0xC0, 0x0C, 0x30, 0x03, 0x30, 0x03, 0x0C, 0x03, 0xC0, 0xC0, 0x0C, 0x30};
unsigned char arrY[] = {0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE};
	
unsigned char finMessageX[] = {0xD0, 0x80, 0xD7, 0x95};
unsigned char finMessageY[] = {0xFD, 0xFB, 0xF7, 0xEF};
	
unsigned char loseMessageX[] = {0x24, 0x18, 0x24};
unsigned char loseMessageY[] = {0xFE, 0xFC, 0xF7};


//--------End Shared Variables------------------------------------------------

// void noteDisplay(){
// 	for(int j = 0; j < sizeof(arrY)/sizeof(unsigned short); j++){
// 		PORTC = arrX[j]; PORTD = arrY[j];
// 		_delay_ms(1000);
// 		tmp = arrY[j];
// 		while(tmp != 0x7F){
// 			tmp = tmp << 1;
// 			tmp = tmp | 0x01;
// 			PORTD = tmp;
// 			_delay_ms(1000);
// 		}
// 	}
// }
void match(int direction){
	if(direction == 3 && PORTD == 0xBF && PORTC == 0xC0){
		if(test != 9){test += 1;}
	}
	else if(direction == 2 && PORTD == 0xBF && PORTC == 0x03){
		if(test != 9){test += 1;}
	}
	else if(direction == 1 && PORTD == 0xBF && PORTC == 0x0C){
		if(test != 9){test += 1;}
	}
	else if(direction == 4 && PORTD == 0xBF && PORTC == 0x30){
		if(test != 9){test += 1;}
	}
	else{
		if(test != 0){test -= 1;}
// 		set_PWM(261.63);
// 		_delay_ms(500);
	}
}

unsigned char temp = 0x00;
void upDisplay(){
	PORTC = 0x0C; PORTD = temp;
}
void upDisplayShift(){
	while(PORTD != 0x7F){
		tmp = tmp << 1;
		tmp = tmp | 0x01;
		PORTD = tmp;
	}
}

//--------User defined FSMs---------------------------------------------------
enum SM1_States{Wait1, Act1, Up1, UpPress1, Down1, DownPress1, Right1, RightPress1, Left1, LeftPress1};

int SM1Tick(int state){
	x_axis = ADC_read(0);
	y_axis = ADC_read(1);
	
	switch(state){
		case Wait1:
			state = Act1;
			break;
		case Act1:
			if(x_axis <= 50){
				state = Up1;
				direction = 1;
			}
			else if(x_axis >= 950){
				state = Down1;
				direction = 4;
			}
			else if(y_axis >= 950){
				state = Right1;
				direction = 2;
			}
			else if(y_axis <= 50){
				state = Left1;
				direction = 3;
			}
			else{
				state = Act1;
				direction = 0;
			}
			break;
		case Up1:
			if(x_axis <= 50){
				state = Up1;
			}
			else{
				state = UpPress1;
			}
			break;
		case UpPress1:
			state = Act1;
			break;
		case Down1:
			if(x_axis >= 950){
				state = Down1;
			}
			else{
				state = DownPress1;
			}
			break;
		case DownPress1:
			state = Act1;
			break;
		case Right1:
			if(y_axis >= 950){
				state = Right1;
			}
			else{
				state = RightPress1;
			}
			break;
		case RightPress1:
			state = Act1;
			break;
		case Left1:
			if(y_axis <= 50){
				state = Left1;
			}
			else{
				state = LeftPress1;
			}
			break;
		case LeftPress1:
			state = Act1;
			break;
		default:
			state = Wait1;
			break;
	}
	switch(state){
		case Wait1:
			break;
		case Act1:
			PORTB = led7Seg[test];
			set_PWM(0);
			break;
		case Up1:
			PORTB = 0x02;
			break;
		case UpPress1:
			match(direction);
			break;
		case Down1:
			PORTB = 0x04;
			break;
		case DownPress1:
			match(direction);
			break;
		case Right1:
			PORTB = 0x01;
			break;
		case RightPress1:
			match(direction);
			break;
		case Left1:
			PORTB = 0x10;
			break;
		case LeftPress1:
			match(direction);
			break;
		default:
			break;
	}
	return state;
}

enum SM2_States{Wait2, ColumnUp, ColumnUpShift};
int upTmp = 0;
int SM2Tick(int state){
	switch(state){
		case Wait2:
			state = ColumnUp;
			break;
		case ColumnUp:
			state = ColumnUpShift;
			break;
		case ColumnUpShift:
			if(upTmp != 0xFE){
				state = ColumnUp;
			}
			else{
				state = Wait2;
			}
			break;
		default:
			state = Wait2;
			break;
	}
	switch(state){
		case Wait2:
			upTmp = 0xFE;
			break;
		case ColumnUp:
			PORTC = 0x0C; PORTD = upTmp;
			break;
		case ColumnUpShift:
			if(PORTD != 0x7F){
				upTmp = upTmp << 1;
				upTmp = upTmp | 0x01;
			}
			break;
		default:
			break;
	}
	return state;
}
// enum SM3_States{Wait3, Display3};
// 	
// int SM3Tick(int state){
// 	switch(state){
// 		case Wait3:
// 			state = Display3;
// 			break;
// 		case Display3:
// 			state = Display3;
// 			break;
// 		default:
// 			state = Wait3;
// 			break;
// 	}
// 	switch(state){
// 		case Wait3:
// 			break;
// 		case Display3:
// 			break;
// 		default:
// 			break;
// 	}
// 	return state;
// }
// --------END User defined FSMs-----------------------------------------------

// Implement scheduler code from PES.
int main()
{
DDRA = 0x00; PORTA = 0xFF;
DDRB = 0xFF; PORTB = 0x00;
DDRC = 0xFF; PORTC = 0x00;
DDRD = 0xFF; PORTD = 0x00;


// Period for the tasks
unsigned long int SMTick1_calc = 100;
unsigned long int SMTick2_calc = 100;
//unsigned long int SMTIck3_calc = 1;

//Calculating GCD
unsigned long int tmpGCD = 1;
tmpGCD = findGCD(SMTick1_calc, SMTick2_calc);
//tmpGCD = findGCD(tmpGCD, SMTIck3_calc);

//Greatest common divisor for all tasks or smallest time unit for tasks.
unsigned long int GCD = tmpGCD;

//Recalculate GCD periods for scheduler
unsigned long int SMTick1_period = SMTick1_calc/GCD;
unsigned long int SMTIck2_period = SMTick2_calc/GCD;
//unsigned long int SMTick3_period = SMTIck3_calc/GCD;


//Declare an array of tasks 
static task task1, task2 /*,task3*/;
task *tasks[] = {&task1, &task2 /*&task3*/};
const unsigned short numTasks = sizeof(tasks)/sizeof(task*);

// Task 1
task1.state = -1;//Task initial state.
task1.period = SMTick1_period;//Task Period.
task1.elapsedTime = SMTick1_period;//Task current elapsed time.
task1.TickFct = &SM1Tick;//Function pointer for the tick.

// Task 2
task2.state = -1;
task2.period = SMTIck2_period;
task2.elapsedTime = SMTIck2_period;
task2.TickFct = &SM2Tick;

//Task 3
// task3.state = -1;
// task3.period = SMTick3_period;
// task3.elapsedTime = SMTick3_period;
// task3.TickFct = &SM3Tick;



// Set the timer and turn it on
TimerSet(GCD);
TimerOn();
ADC_init();
PWM_on();

unsigned short i; // Scheduler for-loop iterator
while(1) {
    // Scheduler code
    for ( i = 0; i < numTasks; i++ ) {
        // Task is ready to tick
        if ( tasks[i]->elapsedTime == tasks[i]->period ) {
            // Setting next state for task
            tasks[i]->state = tasks[i]->TickFct(tasks[i]->state);
            // Reset the elapsed time for next tick.
            tasks[i]->elapsedTime = 0;
        }
        tasks[i]->elapsedTime += 1;
    }
		while(!TimerFlag);
	TimerFlag = 0;
}

// Error: Program should not exit!
return 0;
}