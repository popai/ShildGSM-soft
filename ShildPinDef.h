/*
 * ShildPinDef.h
 *
 *  Created on: Feb 14, 2016
 *      Author: popai
 */
/***********************************************************
 *      GSM shield for Arduino board
 This Software uses Arduino project to connect GSM SIM800L
 and it sends and receive a SMS to command the I/O ports.

  ***********************************************************/


#ifndef SHILDPINDEF_H_
#define SHILDPINDEF_H_
/*
#if defined(__AVR_ATmega168__) || defined(__AVR_ATmega328P__)
#define errLED		PINB5		//pin LED to signal an error
#define useLED		PINB4		//pin LED to signal board activity
#define JPCFG		PINC5		//pin jumper for configuration the board

#define OUTP		PORTD	//output port
#define OUTDP		DDRD
#define OUTPP		PIND
#define INP			PORTC
#define INDP		DDRC
#define INPP		PINC
#define MIXP		PORTB
#define MIXDP		DDRB
#define MIXPP		PINB

#define outD1		PIND2
#define outD2		PIND3
#define outD3		PIND4
#define outD4		PIND5
#define outD5		PIND6
#define outD6		PIND7

#define softTX		PINC0
#define softRX		PINC1
#define senzorT1	PINC2
#define senzorT2	PINC3
#define senzorV		PINC4

#endif

#if defined(__AVR_ATmega32U4__)

#define errLED		13			//pin LED to signal an error
#define useLED		12			//pin LED to signal an error
#define JPCFG		PIND0		//pin jumper for configuration the board


#define outD1		PIND1
//#define outD2		PIND0
#define outD3		PIND5
#define outD4		PINC6
#define outD5		PIND7
#define outD6		PINE6

#define inD1		PINB1
#define inD2		PINB2
#define inD3		PINB3
//#define inD4
#define senzorT1	PINF4
#define senzorT2	PINF5
#define softRX		PINF6
#define softTX		PINF7
#define senzorV		PINF1

#endif
*/

#if defined(__AVR_ATmega328P__)
#define errLED		13		//pin LED to signal an error
#define useLED		12		//pin LED to signal an gsm activitis
#define JPCFG		A5		//pin jumper for configuration the board

#define outD1		2
#define outD2		3
#define outD3		4
#define outD4		5
#define outD5		6
#define outD6		7

#define inD1		9
#define inD2		10
#define inD3		11
//#define inD4
//#define softTX		A0	defined in MyGSM library
//#define softRX		A1	defined in MyGSM library
#define senzorT1	A2
#define senzorT2	A3
#define senzorV		A4
#endif

#if defined(__AVR_ATmega32U4__)
#define errLED		13		//pin LED to signal an error
#define useLED		12		//pin LED to signal an error
#define JPCFG		3		//pin jumper for configuration the board


#define outD1		2
#define outD2		4
#define outD3		5
#define outD4		6
#define outD5		7

#define inD1		9
#define inD2		10
#define inD3		11
//#define inD4
#define softTX		A0
#define softRX		A1
#define senzorT1	A2
#define senzorT2	A3
#define senzorV		A4
#endif

void pinSetUp();

#endif /* SHILDPINDEF_H_ */
