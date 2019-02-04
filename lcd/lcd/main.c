/*
 * lcd.c
 *
 * Created: 1/31/2019 2:50:24 PM
 * Author : Christian Legaspino
 */ 

#include <avr/io.h>
#include "io.c"

int main(void)
{
	DDRC = 0xFF; PORTC = 0x00; // LCD data lines
	DDRD = 0xFF; PORTD = 0x00; // LCD control lines
	
	// Initializes the LCD display
	LCD_init();
	
	// Starting at position 1 on the LCD screen, writes Hello World
	const unsigned char* s = (const unsigned char*) "Hello World";
	LCD_DisplayString(1, s);
	
	while(1) {continue;}
}


