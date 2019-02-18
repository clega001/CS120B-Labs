/*
 * Christian Legaspino & clega001@ucr.edu
 * Carissa Lo & clo020@ucr.edu
 * Lab Section: 23
 * Assignment: Lab 11 Exercise 2
 *
 * I acknowledge all content contained herein, excluding template or example
 * code, is my own original work.
 */

#include <avr/io.h>
#include <avr/interrupt.h>
#include <bit.h>
#include <timer.h>
#include <stdio.h>
#include "io.c"

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
unsigned char b = 0x00;
const unsigned char* s = (const	unsigned char*)"Legend...";

//--------End Shared Variables------------------------------------------------
void Display(){
	LCD_DisplayString(1, s);
}
//--------User defined FSMs---------------------------------------------------
enum States{Wait, Press, Release} state;
	
int TickFct_LCD(int state){
	
	b = PINA & 0x01;
	
	switch(state){
		case Wait:
			if(!b){
				state = Press;
				break;
			}
			else{
				state = Wait;
				break;
			}
		case Press:
			if(!b){
				state = Press;
				break;
			}
			else{
				state = Release;
				break;
			}
		case Release:
			state = Wait;
			break;
		default:
			state = Wait;
			break;
	}
	switch(state){
		case Wait:
			PORTB = 0x00;
			break;
		case Press:
			PORTB = 0x01;
			break;
		case Release:
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
// Set Data Direction Registers
// Buttons PORTA[0-7], set AVR PORTA to pull down logic
DDRA = 0x00; PORTA = 0xFF;
DDRB = 0xFF; PORTB = 0x00;
DDRC = 0xFF; PORTC = 0x00;

// Period for the tasks
unsigned long int SMTick1_calc = 50;


//Calculating GCD
unsigned long int tmpGCD = 1;
tmpGCD = findGCD(tmpGCD, SMTick1_calc);

//Greatest common divisor for all tasks or smallest time unit for tasks.
unsigned long int GCD = tmpGCD;

//Recalculate GCD periods for scheduler
unsigned long int SMTick1_period = SMTick1_calc/GCD;

//Declare an array of tasks 
static task task1;
task *tasks[] = {&task1};
const unsigned short numTasks = sizeof(tasks)/sizeof(task*);

// Task 1
task1.state = -1;//Task initial state.
task1.period = SMTick1_period;//Task Period.
task1.elapsedTime = SMTick1_period;//Task current elapsed time.
task1.TickFct = &TickFct_LCD;//Function pointer for the tick.


// Set the timer and turn it on
LCD_init();
TimerSet(GCD);
TimerOn();

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
	LCD_ClearScreen();
    while(!TimerFlag);
    TimerFlag = 0;
	continue;
}

// Error: Program should not exit!
return 0;
}


