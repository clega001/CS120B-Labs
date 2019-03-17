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
#include <alloca.h>
#include <stdlib.h>

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
//--------Shared/Global Variables----------------------------------------------------
//Button
unsigned char b = 0x00;

//Joystick
unsigned short x_axis = 0x0000;
unsigned short y_axis = 0x0000;
unsigned char direction = 0x00;
//Seven Segment LED
unsigned char led7Seg[] = {0x88, 0xF9, 0x4C, 0x68, 0x39, 0x2A, 0x0A, 0xF8, 0x08, 0x38};
unsigned char test = 0;

//Notes fall column wise
unsigned char arrX[] = {0xC0, 0x0C, 0x30, 0x03, 0x30, 0x03, 0x0C, 0x03, 0xC0, 0xC0, 0x0C, 0x30};
unsigned char arrY[] = {0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE};

//Display when Game Won
unsigned char finMessageX[] = {0x24, 0x42, 0x3C};
unsigned char finMessageY[] = {0xFC, 0xF7, 0xEF};
	
//Display when Game Lost
unsigned char loseMessageX[] = {0x24, 0x42, 0x3C};
unsigned char loseMessageY[] = {0xFC, 0xDF, 0xEF};
	
//Randomize Column States
bool randNum = true;
	
//Boolean values, each will be used to determine different speeds
bool south1 = false; bool south2 = false;
bool north1 = false; bool north2 = false;
bool west1 = false; bool west2 = false;
bool east1 = false; bool east2 = false;
bool target = false;
bool startGame = false;

//Win?
bool win = false;
//--------End Shared/Global Variables------------------------------------------------

void match(int direction){
	if(startGame && !win){
		if(direction == 3 && PORTD == 0xBF && PORTC == 0xC0){
			if(test != 9){test += 1; target = true;}
				if(test == 9){win = true;}
		}
		else if(direction == 2 && PORTD == 0xBF && PORTC == 0x03){
			if(test != 9){test += 1; target = true;}
				if(test == 9){win = true;}
		}
		else if(direction == 1 && PORTD == 0xBF && PORTC == 0x0C){
			if(test != 9){test += 1; target = true;}
				if(test == 9){win = true;}
		}
		else if(direction == 4 && PORTD == 0xBF && PORTC == 0x30){
			if(test != 9){test += 1; target = true;}
				if(test == 9){win = true;}
		}
		else{
			if(test == 9){win = true;}
			else{
				if(test != 0){test -= 1;}
				set_PWM(261.63);
				_delay_ms(500);
			}
		}
	}
}
void missedTarget(){
	if(~target && PORTD == 0x7F && PORTC == 0xC0){
		test -= 1;
		set_PWM(261.63);
		_delay_ms(500);
	}
	else if(~target && PORTD == 0x7F && PORTC == 0x03){
		test -= 1;
		set_PWM(261.63);
		_delay_ms(500);
	}
	else if(~target && PORTD == 0x7F && PORTC == 0x0C){
		test -= 1;
		set_PWM(261.63);
		_delay_ms(500);
	}
	else if(~target && PORTD == 0x7F && PORTC == 0x30){
		test -= 1;
		set_PWM(261.63);
		_delay_ms(500);
	}
	else{
		target = false;
	}
}
//--------User defined FSMs---------------------------------------------------
//SM1 Joystick
enum SM1_States{Wait1, Act1, Up1, UpPress1, Down1, DownPress1, Right1, RightPress1, Left1, LeftPress1};
int SM1Tick(int state){
	x_axis = ADC_read(0);
	y_axis = ADC_read(1);
	
	switch(state){
		case Wait1:
			state = Act1;
			break;
		case Act1:
			if(startGame && !win){
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
			}
			else if(startGame && win){
				set_PWM(0);
				for(int m = 0; m < sizeof(finMessageX)/sizeof(unsigned char); m++){
					PORTC = finMessageX[m]; PORTD = finMessageY[m];
					_delay_ms(10);
				}
			}
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
			break;
		case UpPress1:
			match(direction);
			break;
		case Down1:
			break;
		case DownPress1:
			match(direction);
			break;
		case Right1:
			break;
		case RightPress1:
			match(direction);
			break;
		case Left1:
			break;
		case LeftPress1:
			match(direction);
			break;
		default:
			break;
	}
	return state;
}


//SM2 Displays Up Arrow Columns
enum SM2_States{Wait2, ColumnUp, ColumnUpShift};
unsigned char upTmp = 0;
int SM2Tick(int state){
	switch(state){
		case Wait2:
			if(north1 && !win){
				state = ColumnUp;
			}
			else{
				state = Wait2;
			}
			break;
		case ColumnUp:
			if(north1 && !win){
				state = ColumnUpShift;
			}
			else{
				state = Wait2;
			}
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
			else if(PORTD == 0x7F){
				north1 = false;
				randNum = true;
			}
			break;
		default:
			break;
	}
	return state;
}


//SM3 Displays Down Arrow Columns
enum SM3_States{Wait3, ColumnDown, ColumnDownShift};
unsigned char downTmp = 0;	
int SM3Tick(int state){
	switch(state){
		case Wait3:
			if(south1 && !win){
				state = ColumnDown;
			}
			else{
				state = Wait3;
			}
			break;
		case ColumnDown:
			if(south1 && !win){
				state = ColumnDownShift;
			}
			else{
				state = Wait3;
			}
			break;
		case ColumnDownShift:
			if(downTmp != 0xFE){
				state = ColumnDown;
			}
			else{
				state = Wait3;
			}
			break;
		default:
			state = Wait3;
			break;
	}
	switch(state){
		case Wait3:
			downTmp = 0xFE;
			break;
		case ColumnDown:
			PORTC = 0x30; PORTD = downTmp;
			break;
		case ColumnDownShift:
			if(PORTD != 0x7F){
				downTmp = downTmp << 1;
				downTmp = downTmp | 0x01;
			}
			else if(PORTD == 0x7F){
				south1 = false; 
				randNum = true;
			}
		default:
			break;
	}
	return state;
}


//SM4 Displays Left Arrow Columns
enum SM4_States{Wait4, ColumnLeft, ColumnLeftShift};
unsigned char leftTmp = 0;
int SM4Tick(int state){
	switch(state){
		case Wait4:
			if(west1 && !win){
				state = ColumnLeft;
			}
			else{
				state = Wait4;
			}
			break;
		case ColumnLeft:
			if(west1 && !win){
				state = ColumnLeftShift;
			}
			else{
				state = Wait4;
			}
			break;
		case ColumnLeftShift:
			if(leftTmp != 0xFE){
				state = ColumnLeft;
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
			leftTmp = 0xFE;
			break;
		case ColumnLeft:
			PORTC = 0xC0; PORTD = leftTmp;
			break;
		case ColumnLeftShift:
			if(PORTD != 0x7F){
				leftTmp = leftTmp << 1;
				leftTmp = leftTmp | 0x01;
			}
			else if(PORTD == 0x7F){
				west1 = false;
				randNum = true;
			}
		default:
			break;
	}
	return state;
}


//SM5 Displays Right Arrow Columns
enum SM5_States{Wait5, ColumnRight, ColumnRightShift};
unsigned char rightTmp = 0x00;
int SM5Tick(int state){
	switch(state){
		case Wait5:
			if(east1 && !win){
				state = ColumnRight;
			}
			else{
				state = Wait5;
			}
			break;
		case ColumnRight:
			if(east1 && !win){
				state = ColumnRightShift;
			}
			else{
				state = Wait5;
			}
			break;
		case ColumnRightShift:
			if(rightTmp != 0xFE){
				state = ColumnRight;
			}
			else{
				state = Wait5;
			}
			break;
		default:
			state = Wait5;
			break;
	}
	switch(state){
		case Wait5:
			rightTmp = 0xFE;
			break;
		case ColumnRight:
			PORTC = 0x03; PORTD = rightTmp;
			break;
		case ColumnRightShift:
			if(PORTD != 0x7F){
				rightTmp = rightTmp << 1;
				rightTmp = rightTmp | 0x01;
			}
			else if(PORTD == 0x7F){
				east1 = false;
				randNum = true;
			}
			break;
		default:
			break;
	}
	return state;
}


//SM7 Displays Up Arrow Columns - Speed 2
enum SM7_States{Wait7, ColumnUp7, ColumnUpShift7};
unsigned char upTmp2 = 0;
int SM7Tick(int state){
	switch(state){
		case Wait7:
			if(north2 && !win){
				state = ColumnUp7;
			}
			else{
				state = Wait7;
			}
			break;
		case ColumnUp:
			if(north2 && !win){
				state = ColumnUpShift7;
			}
			else{
				state = Wait7;
			}
			break;
		case ColumnUpShift7:
			if(upTmp2 != 0xFE){
				state = ColumnUp7;
			}
			else{
				state = Wait7;
			}
			break;
		default:
			state = Wait7;
			break;
	}
	switch(state){
		case Wait7:
			upTmp2 = 0xFE;
			break;
		case ColumnUp7:
			PORTC = 0x0C; PORTD = upTmp2;
			break;
		case ColumnUpShift7:
			if(PORTD != 0x7F){
				upTmp2 = upTmp2 << 1;
				upTmp2 = upTmp2 | 0x01;
			}
			else if(PORTD == 0x7F){
				north2 = false;
				randNum = true;
			}
			break;
		default:
			break;
	}
	return state;
}


//SM8 Displays Down Arrow Columns - Speed 2
enum SM8_States{Wait8, ColumnDown8, ColumnDownShift8};
unsigned char downTmp2 = 0;
int SM8Tick(int state){
	switch(state){
		case Wait8:
			if(south2 && !win){
				state = ColumnDown8;
			}
			else{
				state = Wait8;
			}
			break;
		case ColumnDown8:
			if(south2 && !win){
				state = ColumnDownShift8;
			}
			else{
				state = Wait8;
			}
			break;
		case ColumnDownShift8:
			if(downTmp2 != 0xFE){
				state = ColumnDown8;
			}
			else{
				state = Wait8;
			}
			break;
		default:
			state = Wait8;
			break;
	}
	switch(state){
		case Wait8:
			downTmp2 = 0xFE;
			break;
		case ColumnDown8:
			PORTC = 0x30; PORTD = downTmp2;
			break;
		case ColumnDownShift8:
			if(PORTD != 0x7F){
				downTmp2 = downTmp2 << 1;
				downTmp2 = downTmp2 | 0x01;
			}
			else if(PORTD == 0x7F){
				south2 = false;
				randNum = true;
			}
		default:
			break;
	}
	return state;
}


//SM9 Displays Left Arrow Columns - Speed 2
enum SM9_States{Wait9, ColumnLeft9, ColumnLeftShift9};
unsigned char leftTmp2 = 0;
int SM9Tick(int state){
	switch(state){
		case Wait9:
			if(west2 && !win){
				state = ColumnLeft9;
			}
			else{
				state = Wait9;
			}
			break;
		case ColumnLeft9:
			if(west2 && !win){
				state = ColumnLeftShift9;
			}
			else{
				state = Wait9;
			}
			break;
		case ColumnLeftShift9:
			if(leftTmp2 != 0xFE){
				state = ColumnLeft9;
			}
			else{
				state = Wait9;
			}
			break;
		default:
			state = Wait9;
			break;
	}
	switch(state){
		case Wait9:
			leftTmp2 = 0xFE;
			break;
		case ColumnLeft9:
			PORTC = 0xC0; PORTD = leftTmp2;
			break;
		case ColumnLeftShift9:
			if(PORTD != 0x7F){
				leftTmp2 = leftTmp2 << 1;
				leftTmp2 = leftTmp2 | 0x01;
			}
			else if(PORTD == 0x7F){
				west2 = false;
				randNum = true;
			}
		default:
			break;
	}
	return state;
}


//SM10 Displays Right Arrow Columns - Speed 2
enum SM10_States{Wait10, ColumnRight10, ColumnRightShift10};
unsigned char rightTmp2 = 0x00;
int SM10Tick(int state){
	switch(state){
		case Wait10:
			if(east2 && !win){
				state = ColumnRight10;
			}
			else{
				state = Wait10;
			}
			break;
		case ColumnRight10:
			if(east2 && !win){
				state = ColumnRightShift10;
			}
			else{
				state = Wait10;
			}
			break;
		case ColumnRightShift10:
			if(rightTmp2 != 0xFE){
				state = ColumnRight10;
			}
			else{
				state = Wait10;
			}
			break;
		default:
			state = Wait10;
			break;
	}
	switch(state){
		case Wait10:
			rightTmp2 = 0xFE;
			break;
		case ColumnRight10:
			PORTC = 0x03; PORTD = rightTmp2;
			break;
		case ColumnRightShift10:
			if(PORTD != 0x7F){
				rightTmp2 = rightTmp2 << 1;
				rightTmp2 = rightTmp2 | 0x01;
			}
			else if(PORTD == 0x7F){
				east2 = false;
				randNum = true;
			}
			break;
		default:
			break;
	}
	return state;
}


//Randomizes Columns
enum SM6_States{Wait6, Toggle};
unsigned char holder = 0x00;
void SM6Tick(int state){
	switch(state){
		case Wait6:
			if(startGame && !win){
				state = Toggle;
			}
			else{
				state = Wait6;
			}
			break;
		case Toggle:
			if(startGame && !win){
				state = Toggle;
			}
			else{
				state = Wait6;
			}
			break;
		default:
			break;
	}
	switch(state){
		case Wait6:
			break;
		case Toggle:
			if(randNum){
				holder = rand() % 4;
				switch(holder){
					case 0:
						if(test >= 5){north2 = true;}
							else{north1 = true;}
						randNum = false;
						break;
					case 1:
						if(test >= 5){south2 = true;}
							else{south1 = true;}
						randNum = false;
						break;
					case 2:
						if(test >= 5){east2 = true;}
							else{east1 = true;}
						randNum = false;
						break;
					case 3:
						if(test >= 5){west2 = true;}
							else{west1 = true;}
						randNum = false;
						break;
				}
			}
		default:
		break;
	}
}


//Game Start Button
enum SM11_States{Wait11, StartPress, StartRelease, InProgress, EndPress, EndRelease};
int SM11Tick(int state){
	b = PINA & 0x08;
	
	switch(state){
		case Wait11:
			if(!b){
				state = StartPress;
			}
			else{
				state = Wait11;
			}
			break;
		case StartPress:
			if(!b){
				state = StartPress;
			}
			else{
				state = StartRelease;
			}
			break;
		case StartRelease:
			state = InProgress;
			break;
		case InProgress:
			if(!b){
				state = EndPress;
			}
			else{
				state = InProgress;
			}
			break;
		case EndPress:
			if(!b){
				state = EndPress;
			}
			else{
				state = EndRelease;
			}
			break;
		case EndRelease:
			state = Wait11;
			break;
		default:
			state = Wait11;
			break;
	}
	switch(state){
		case Wait11:
			break;
		case StartPress:
			break;
		case StartRelease:
			startGame = true;
			win = false;
			break;
		case InProgress:
			break;
		case EndPress:
			break;
		case EndRelease:
			startGame = false; randNum = true; win = false;
			north1 = false; north2 = false; south1 = false; south2 = false;
			west1 = false; west2 = false; east1 = false; east2 = false;
			test = 0;
		default:
			break;
	}
	return state;
}


//Lose Message
enum SM12_States{Wait12, Display12};	
int SM12Tick(int state){
	switch(state){
		case Wait12:
			if(!startGame && !win){
				state = Display12;
			}
			else{
				state = Wait12;
			}
			break;
		case Display12:
			if(!startGame){
				state = Display12;
			}
			else{
				state = Wait12;
			}
			break;
		default:
			state = Wait12;
			break;
	}
	switch(state){
		case Wait12:
			break;
		case Display12:
			for(int n = 0; n < sizeof(loseMessageX)/sizeof(unsigned char); n++){
				PORTC = loseMessageX[n]; PORTD = loseMessageY[n];
				_delay_ms(10);
			}
			break;
		default:
			break;
	}
	return state;
}

//Win??
enum SM13_States{Wait13, Win13};
int SM13Tick(int state){
	switch(state){
		case Wait13:
			if(win){
				state = Win13;
			}
			else{
				state = Wait13;
			}
			break;
		case Win13:
			if(win){
				state = Win13;
			}
			else{
				state = Wait13;
			}
			break;
		default:
			state = Wait13;
			break;
	}
	switch(state){
		case Wait13:
			break;
		case Win13:
			set_PWM(0);
			for(int m = 0; m < sizeof(finMessageX)/sizeof(unsigned char); m++){
				PORTC = finMessageX[m]; PORTD = finMessageY[m];
				_delay_ms(10);
			}
			break;
		default:
			break;
	}
}
// --------END User defined FSMs-----------------------------------------------

// Implement scheduler code from PES.
int main()
{
DDRA = 0x00; PORTA = 0xFF;
DDRB = 0xFF; PORTB = 0x00;
DDRC = 0xFF; PORTC = 0x00;
DDRD = 0xFF; PORTD = 0x00;


// Period for the tasks
unsigned long int SMTick1_calc = 1;
unsigned long int SMTick2_calc = 100;
unsigned long int SMTIck3_calc = 100;
unsigned long int SMTIck4_calc = 100;
unsigned long int SMTick5_calc = 100;
unsigned long int SMTick6_calc = 1;
unsigned long int SMTick7_calc = 50;
unsigned long int SMTick8_calc = 50;
unsigned long int SMTick9_calc = 50;
unsigned long int SMTick10_calc = 50;
unsigned long int SMTick11_calc = 1;
unsigned long int SMTick12_calc = 1;
unsigned long int SMTick13_calc = 1;

//Calculating GCD
unsigned long int tmpGCD = 1;
tmpGCD = findGCD(SMTick1_calc, SMTick2_calc);
tmpGCD = findGCD(tmpGCD, SMTIck3_calc);
tmpGCD = findGCD(tmpGCD, SMTIck4_calc);
tmpGCD = findGCD(tmpGCD, SMTick5_calc);
tmpGCD = findGCD(tmpGCD, SMTick6_calc);
tmpGCD = findGCD(tmpGCD, SMTick7_calc);
tmpGCD = findGCD(tmpGCD, SMTick8_calc);
tmpGCD = findGCD(tmpGCD, SMTick9_calc);
tmpGCD = findGCD(tmpGCD, SMTick10_calc);
tmpGCD = findGCD(tmpGCD, SMTick11_calc);
tmpGCD = findGCD(tmpGCD, SMTick12_calc);
tmpGCD = findGCD(tmpGCD, SMTick13_calc);

//Greatest common divisor for all tasks or smallest time unit for tasks.
unsigned long int GCD = tmpGCD;

//Recalculate GCD periods for scheduler
unsigned long int SMTick1_period = SMTick1_calc/GCD;
unsigned long int SMTIck2_period = SMTick2_calc/GCD;
unsigned long int SMTick3_period = SMTIck3_calc/GCD;
unsigned long int SMTick4_period = SMTIck4_calc/GCD;
unsigned long int SMTick5_period = SMTick5_calc/GCD;
unsigned long int SMTick6_period = SMTick6_calc/GCD;
unsigned long int SMTick7_period = SMTick7_calc/GCD;
unsigned long int SMTick8_period = SMTick8_calc/GCD;
unsigned long int SMTick9_period = SMTick9_calc/GCD;
unsigned long int SMTick10_period = SMTick10_calc/GCD;
unsigned long int SMTick11_period = SMTick11_calc/GCD;
unsigned long int SMTick12_period = SMTick12_calc/GCD;
unsigned long int SMTick13_period = SMTick13_calc/GCD;


//Declare an array of tasks 
static task task1, task2, task3, task4, task5, task6, task7, task8, task9, task10, task11, task12, task13;
task *tasks[] = {&task1, &task2, &task3, &task4, &task5, &task6, &task7, &task8, &task9, &task10, &task11, &task12, &task13};
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
task3.state = -1;
task3.period = SMTick3_period;
task3.elapsedTime = SMTick3_period;
task3.TickFct = &SM3Tick;

//Task 4
task4.state = -1;
task4.period = SMTick4_period;
task4.elapsedTime = SMTick4_period;
task4.TickFct = &SM4Tick;

//Task 5
task5.state = -1;
task5.period = SMTick5_period;
task5.elapsedTime = SMTick5_period;
task5.TickFct = &SM5Tick;

//Task 6
task6.state = -1;
task6.period = SMTick6_period;
task6.elapsedTime = SMTick6_period;
task6.TickFct = &SM6Tick;

// Task 7
task7.state = -1;
task7.period = SMTick7_period;
task7.elapsedTime = SMTick7_period;
task7.TickFct = &SM7Tick;

// Task 8
task8.state = -1;
task8.period = SMTick8_period;
task8.elapsedTime = SMTick8_period;
task8.TickFct = &SM8Tick;

// Task 9
task9.state = -1;
task9.period = SMTick9_period;
task9.elapsedTime = SMTick9_period;
task9.TickFct = &SM9Tick;

// Task 10
task10.state = -1;
task10.period = SMTick10_period;
task10.elapsedTime = SMTick10_period;
task10.TickFct = &SM10Tick;

// Task 11
task11.state = -1;
task11.period = SMTick11_period;
task11.elapsedTime = SMTick11_period;
task11.TickFct = &SM11Tick;

// Task 12
task12.state = -1;
task12.period = SMTick12_period;
task12.elapsedTime = SMTick12_period;
task12.TickFct = &SM12Tick;

// Task 13
task13.state = -1;
task13.period = SMTick13_period;
task13.elapsedTime = SMTick13_period;
task13.TickFct = &SM13Tick;

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