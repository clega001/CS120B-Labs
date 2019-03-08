/*
 * CustomLab_Complexity2.c
 *
 * Created: 2/28/2019 4:18:48 PM
 * Author : Christian Legaspino
 */ 

#include <avr/io.h>


#include <avr/io.h>
#include <avr/interrupt.h>
#include <bit.h>
#include <timer.h>
#include <stdio.h>

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
unsigned char pass = 0x00;
unsigned char b1 = 0x00;
unsigned char b2 = 0x00;
//--------End Shared Variables------------------------------------------------

//--------User defined FSMs---------------------------------------------------
enum SM1_States{Wait, Act} state1;

int SM1Tick1(int state1){
b1 = PINA & 0x01;
b2 = PINA & 0x02;
	switch(state1){
		case Wait:
			state1 = Act;
			break;
		case Act:
			state1 = Act;
			break;
		default:
			state1 = Wait;
			break;
	}
	switch(state1){
		case Wait:
			PORTC = 0x00;
			PORTB = 0xFF;
			break;
		case Act:
			PORTC = 0x02;
			PORTB = 0x10;
			break;
		default:
			break;
	}
	return state1;
}
// --------END User defined FSMs-----------------------------------------------

// Implement scheduler code from PES.
int main()
{
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
task1.TickFct = &SM1Tick1;//Function pointer for the tick.

// Set the timer and turn it on
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
	while(!TimerFlag);
	TimerFlag = 0;
}

// Error: Program should not exit!
return 0;
}

