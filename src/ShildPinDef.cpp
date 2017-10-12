/*
 * ShildPinDef.cpp
 *
 *  Created on: Feb 22, 2016
 *      Author: ionel.g.popa
 */
/***********************************************************
 *                  SMS Cotrol module
 This Software uses Arduino project to conect GSM modul SIM300L
 and it sends and recive a SMS to comand the I/O ports.

 Copyright (C) 2012  Popa Ionel  -- popai307@gmail.com --


 ***********************************************************/

#include "ShildPinDef.h"

#include <avr/io.h>
#include "Arduino.h"

void pinSetUp()
{
	//output pins
	pinMode(outD1, OUTPUT);
	digitalWrite(outD1, HIGH);

	pinMode(outD2, OUTPUT);
	digitalWrite(outD2, HIGH);

	pinMode(outD3, OUTPUT);
	digitalWrite(outD3, HIGH);

	pinMode(outD4, OUTPUT);
	digitalWrite(outD4, HIGH);

	pinMode(outD5, OUTPUT);
	digitalWrite(outD5, HIGH);

	pinMode(outD6, OUTPUT);
	digitalWrite(outD6, HIGH);

	pinMode(errLED, OUTPUT);
	pinMode(useLED, OUTPUT);

	//input pin
	pinMode(inD1, INPUT);
	pinMode(inD2, INPUT);
	pinMode(inD3, INPUT);

	pinMode(JPCFG, INPUT);

	//SoftTX, SoftRX and GSM_ON are defined in MyGSM library.
}

