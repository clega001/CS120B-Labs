#include <avr/io.h>
#include <avr/interrupt.h>
#include <bit.h>
#include <timer.h>
#include <stdio.h>
#include <util/delay.h>
#include <stdlib.h>
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

//--------Shared Variables----------------------------------------------------

unsigned short x_axis = 0x0000;
unsigned short y_axis = 0x0000;

unsigned char direction = 0x00;
unsigned char moveRow = 0x00;
unsigned char moveColumn = 0x00;

unsigned short X = 0x00;
unsigned short Y = 0x00;

unsigned char arrX[] = {0x18, 0x7E, 0x3C, 0x18};
unsigned char arrY[] = {0xF0, 0xEF, 0xDF, 0xBF};

unsigned char arr2X[] = {0x18, 0x3C, 0x7E, 0x18};
unsigned char arr2Y[] = {0xFD, 0xFB, 0xF7, 0x0F};

unsigned char arr3X[] = {0x40, 0x20, 0x10, 0x0F};//{0x10, 0x30, 0x7F, 0x30, 0x10};
unsigned char arr3Y[] = {0xE7, 0xC3, 0x81, 0xE7};//{0xFD, 0xFB, 0xE7, 0xDF, 0xBF};

unsigned char arr4X[] = {0xF0, 0x08, 0x04, 0x02};
unsigned char arr4Y[] = {0xE7, 0x81, 0xC3, 0xE7};

unsigned char input = 0x00;
unsigned char level = 0x00;
unsigned char i = 0x00;
unsigned char release = 0x00;
unsigned char incorrect = 0x00;

bool up = false;
bool down = false;
bool right = false;
bool left = false;
bool isUP = false;

unsigned char arrows[] = {0, 1};
int m = 0;
bool chooseArrow = true;
int p = 0;
//--------End Shared Variables------------------------------------------------
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
/*void snakeDisplay(){
	for(int j = 0; j < sizeof(arrX)/sizeof(unsigned short); j++){
//		PORTC = ~arr2X[j]; PORTD = ~arr2Y[j]; //PORTB = 0x00;	
// 		if (input == 0x01)
// 		{
// 		PORTB = ~arr3X[j]; PORTD = ~arr3Y[j]; 	
// 		}
// 		
		if ((level == 0 || level == 3))
		{	
			if (input == 2)	{
				PORTC = ~arrX[j];
				PORTD = ~arrY[j];
// 				PORTC = ~arr2X[j];
// 				PORTD = ~arr2Y[j];
				level++;
			} 
			else if(input != 0){
				PORTB = ~arrX[j];
				PORTD = ~arrY[j];
			//	PORTC = 0xFF;
				level = 0;
			}
			else{
				PORTC = ~arrX[j];
				PORTD = ~arrY[j];
				PORTB = 0xFF;
			}
		}
		else if ((level == 1))
		{	
			if (input != 2){	
 			if (input == 1){
				PORTC = 0x00;
				PORTD = 0xFF;
// 				PORTC = ~arr2X[j];
// 				PORTD = ~arr2Y[j];
				//PORTB = 0xFF;
				level++;
			}
			else if(input != 0){
				PORTB = ~arr2X[j];
				PORTD = ~arr2Y[j];
				//PORTC = 0xFF;
				level = 0;
			}
			else{
 				PORTC = ~arr2X[j];
				PORTD = ~arr2Y[j];
				PORTB = 0xFF;
			}
			}	
		}
		else if (level == 2){
			if (input == 2){
				PORTC = ~arr3X[j];
				PORTD = ~arr3Y[j];
				//PORTB = 0xFF;
				level++;
			}
			else if(input != 0){
				PORTB = ~arr3X[j];
				PORTD = ~arr3Y[j];
				//PORTC = 0xFF;
				level = 0;
			}
			else{
				PORTC = ~arr3X[j];
				PORTD = ~arr3Y[j];
				PORTB = 0xFF;
			}
		}
		else if (level == 4){
			if (input == 2)
			{
				PORTC = 0x00;
				PORTD = 0xFF;
				//PORTB = 0xFF;
				level++;
			}
			else if(input != 0)
			{
				PORTB = ~arr4X[j];
				PORTD = ~arr4Y[j];
				//PORTC = 0xFF;
				level = 0;
			}
			else{
				PORTC = ~arr4X[j];
				PORTD = ~arr4Y[j];
				PORTB = 0xFF;
			}
		}
		else if (level == 5){
			if (release != 2)
			{
	//			PORTC = 0x00;
// 				PORTD = 0xFF;
				level = 0;
			}
		}
		_delay_ms(10);
	}
}*/


void isCorrect(int input){
	if(isUP && input == 1){
		up = false;
		chooseArrow = true;
	}
	
}

//--------User defined FSMs---------------------------------------------------
enum SM1_States{Wait1, Act1};
int SM1Tick(int state){
	x_axis = ADC_read(0);
	y_axis = ADC_read(1);
	
	switch(state){
		case Wait1:
			state = Act1;
			break;
		case Act1:
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
			if(x_axis >= 950 && isUP){
				PORTB = 0x02;
				input = 1;
				chooseArrow = true;
			}
			else if(x_axis <= 50){
					PORTB = 0x04;
					input = 2;
					chooseArrow = true;
			}
			else if(y_axis >= 950){
					PORTB = 0x01;
					input = 3;
					chooseArrow = true;
			}
			else if(y_axis <= 50){
					PORTB = 0x08;
					input = 4;
					chooseArrow = true;
			}
			else{
				input = 0;
				release = 0;
			}
			
			break;
				
		default:
			break;
	}
	return state;
}

enum SM3_States{START, LEVEL};
int SM3Tick(int state){
	switch(state){
		case START:
		state = LEVEL;
		break;
		case LEVEL:
		state = LEVEL;
		break;
		default:
		state = START;
		break;
	}
	switch(state){
		case START:
		break;
		case LEVEL:
			isUP = false;
		if(chooseArrow){
			m = arrows[p];
			switch(m){
				case 0:
				up = true;
				p +=1;
				break;
				case 1:
				down = true;
				p +=1;
				break;
				case 2:
				right = true;
				p +=1;
				break;
				case 3:
				left = true;
				p +=1;
				break;
			}
		}
		
	}
	return state;
}

//up arrow
enum SM2_States{Wait2, Display};
int SM2Tick(int state){
	switch(state){
		case Wait2:
			if(up){
				state = Display;
			}
			else{
				state = Wait2;
			}
			break;
		case Display:
			if(up){
				state = Display;
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
			break;
		case Display:
			chooseArrow = false;
			isUP = true;
// 			for(int j = 0; j < sizeof(arr2X)/sizeof(unsigned char); j++){
// 				isCorrect(input);
// 				PORTC = ~arr2X[j];
// 				PORTD = ~arr2Y[j];
// 				PORTB = ~0x00;
// 				_delay_ms(10);
//			}
			PORTC = ~0x18;
			PORTD = ~0xE7;
 			PORTB = ~0x00;
			break;
		default:
			break;
	}
	return state;
}

//down arrow
enum SM4_States{Wait4, Display4};
int SM4Tick(int state){
	switch(state){
		case Wait4:
		if(down){
			state = Display4;
		}
		else{
			state = Wait4;
		}
		break;
		case Display:
		if(down){
			state = Display4;
		}
		else{
			state = Wait4;
		}
		break;
		default:
		state = Wait4;
		break;
	}
	switch(state){
		case Wait4:
		break;
		case Display4:
		for(int j = 0; j < sizeof(arrX)/sizeof(unsigned char); j++){
			PORTC = ~arrX[j];
			PORTD = ~arrY[j];
			PORTB = ~0x00;
			_delay_ms(10);
		}
		break;
		default:
		break;
	}
	return state;
}
						
// --------END User defined FSMs-----------------------------------------------

// Implement scheduler code from PES.
int main()
{
DDRA = 0x00; PORTA = 0x00;
DDRB = 0xFF; PORTB = 0x00;
DDRC = 0xFF; PORTC = 0x00;
DDRD = 0xFF; PORTD = 0x00;


// Period for the tasks
unsigned long int SMTick1_calc = 1;
unsigned long int SMTick2_calc = 1;
unsigned long int SMTick3_calc = 1;

//Calculating GCD
unsigned long int tmpGCD = 1;
tmpGCD = findGCD(SMTick1_calc, SMTick2_calc);
tmpGCD = findGCD(tmpGCD, SMTick3_calc);

//Greatest common divisor for all tasks or smallest time unit for tasks.
unsigned long int GCD = tmpGCD;

//Recalculate GCD periods for scheduler
unsigned long int SMTick1_period = SMTick1_calc/GCD;
unsigned long int SMTIck2_period = SMTick2_calc/GCD;
unsigned long int SMTIck3_period = SMTick3_calc/GCD;

//Declare an array of tasks 
static task task1, task2, task3;
task *tasks[] = {&task1, &task2, &task3};
const unsigned short numTasks = sizeof(tasks)/sizeof(task*);

// Task 1
task1.state = -1;//Task initial state.
task1.period = SMTick1_period;//Task Period.
task1.elapsedTime = SMTick1_period;//Task current elapsed time.
task1.TickFct = &SM1Tick;//Function pointer for the tick.

// Task 2
// task2.state = -1;
// task2.period = SMTIck2_period;
// task2.elapsedTime = SMTIck2_period;
// task2.TickFct = &SM2Tick;
// 
// //Task3
// task3.state = -1;
// task3.period = SMTIck3_period;
// task3.elapsedTime = SMTIck3_period;
// task3.TickFct = &SM3Tick;


// Set the timer and turn it on
TimerSet(1);
TimerOn();
ADC_init();
//generateFood();
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

