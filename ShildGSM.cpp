/*
 * ShildGSM.cpp
 *
 *  Created on: Feb 14, 2016
 *      Author: popai
 */

#include "ShildGSM.h"

#include "lib/myGSM/MyGSM.h"
#include <avr/eeprom.h>
#include <avr/wdt.h>

#include "ShildCmd.h"
#include "ShildPinDef.h"

extern GSM gsm;		//gsm handler class define in cmd.cpp

char sms_rx[122];   //Received text SMS
char number[20];	//sender phone number
//uint8_t nr_pfonnr = 0;	//hold number of phone number on sim

bool config = false; //, delEEPROM = false;	//define state of controller

int Check_SMS();  //Check if there is SMS

/**
 * @brief : The setup function is called once at startup of the sketch
 *
 * @param : no parameters
 * @return: no returns
 */
void setup()
{
// Add your initialization code here
	wdt_disable();
	pinSetUp();			//set pins

	Serial.begin(115200);	//start hardwre serial
	delay(100);
	Serial.println(F("system startup"));
	//startup gsm module
	gsm.TurnOn(9600);       //module power on
	//SetPort();
	eeprom_read_block(sms_rx, (int*) 486, 24);
	if (strlen(sms_rx) == 0)
	{
		strcpy_P(sms_rx, PSTR("Comanda ne scrisa"));
		eeprom_write_block(sms_rx, (int*) 486, 24);
		//eeprom_update_block(sms_rx, (int*) 486, 24);
		strcpy_P(sms_rx, 0x00);
	}

	//Check status
	//int error = 0;			//error from function
	//error = gsm.SendATCmdWaitResp("AT", 500, 100, "OK", 5);
	if (gsm.SendATCmdWaitResp("AT", 500, 100, "OK", 5) == AT_RESP_OK)
	{
		gsm.InitParam(PARAM_SET_1);		//configure the module
		gsm.Echo(0);		//enable/disable AT echo
		Serial.println("GSM OK");
		//error=gsm.SendSMS("+40745183841","Modul ON");
	}
	else
	{
		Serial.println(F("GSM init error"));
		digitalWrite(errLED, HIGH);			//ERROR LED on
	}

	uint8_t nr_pfonnr = 0;	//hold number of phone number on sim
	for (byte i = 1; i < 7; i++)
		if (gsm.GetPhoneNumber(i, number) == 1)  //Find number in specified position
			++nr_pfonnr;

	Serial.println(nr_pfonnr);

	//if (nr_pfonnr == 0)
	//MIXP |= (1 << errLED);
	//pinMode(A5, INPUT);
	if (digitalRead(A5) == LOW)
	//if ((INPP & (1 << JPCFG)) == 0)
		config = true;

	wdt_enable(WDTO_8S);
}

void loop()
{
// The loop function is called in an endless loop
	int id = 0;
	//int error = 0;			//error from function
	byte i = 0;

	if (config)
	{
		if (Serial.available() > 0)
		{
			while (Serial.available() > 0)
			{
				sms_rx[i] = (Serial.read()); //read data
				i++;
			}
			sms_rx[i - 1] = 0;

			delay(5);
			Serial.println(sms_rx);
			if (strstr_P(sms_rx, PSTR("citeste")) != 0)
			{
				for (int i = 0; i <= 512; i += 18)
					ReadEprom(sms_rx, i);
			}
			else if (strlen(sms_rx) != 0)
				if (!CfgCmd(sms_rx))
					Serial.println("ERROR");
			*sms_rx = 0x00;
		}

		else
		{
			id = Check_SMS();
			if (id == GETSMS_AUTH_SMS || id == GETSMS_NOT_AUTH_SMS)
				Config(number, sms_rx);
			*sms_rx = 0x00;
			id = 0;
		}
		//test SIM900 module up
		if (AT_RESP_OK == gsm.SendATCmdWaitResp("AT", 500, 100, "OK", 5))
			digitalWrite(errLED, LOW);			//ERROR LED off
		else digitalWrite(errLED, HIGH);		//ERROR LED on

		//XXX feeding the dog
		wdt_reset();
	}
	else
	{
		VerificIN();
		id = Check_SMS();
		//Serial.println(id);
		if (id == GETSMS_AUTH_SMS)
			Comand(number, sms_rx);
		else if (id == GETSMS_NOT_AUTH_SMS)
		{
			//Check if receive a pas
			char buffer[64];
			ReadEprom(buffer, 18 * 24);
			if (strcmp(buffer, sms_rx) == 0)
			{
				//write number on sim
				uint8_t nr_pfonnr = 1;	//hold number of phone number on sim
				char tmpnr[20];
				for (byte i = 1; i < 7; i++)
				{
					if (1 == gsm.GetPhoneNumber(i, tmpnr))
						++nr_pfonnr;	//Find number in specified position
					else
						break;
				}

				if (nr_pfonnr < 7) //max 6 number
				{
					if (gsm.WritePhoneNumber(nr_pfonnr, number) != 0)
					{
#ifndef DEBUG_GSMRX
						sprintf_P(buffer, PSTR("%s writed at position %d"),
								number, nr_pfonnr);
						Serial.println(buffer);
#endif
						//++nr_pfonnr;
						strcpy_P(buffer, PSTR("Acceptat"));
						gsm.SendSMS(number, buffer);

					}
					else
					{
						strcpy_P(buffer, PSTR("ERROR"));
						Serial.println(buffer);
						gsm.SendSMS(number, buffer);
					}
				}
				else
				{
					strcpy_P(buffer, PSTR("Nu slot"));
					gsm.SendSMS(number, buffer);
				}
			}
		}
		*number = 0x00;
		*sms_rx = 0x00;
		id = 0;

	}
	//XXX feeding the dog
	wdt_reset();

	//test SIM900 module up
	if (AT_RESP_OK == gsm.SendATCmdWaitResp("AT", 500, 100, "OK", 5))
		digitalWrite(errLED, LOW);			//ERROR LED off
	else digitalWrite(errLED, HIGH);		//ERROR LED on

	delay(50);
}

/**
 * @brief :Check if there is an sms
 *
 * @param : no parameters
 * @return: -1 no sms received
 * 			GETSMS_AUTH_SMS		received authorized sms
 * 			GETSMS_NOT_AUTH_SMS received not authorized sms
 */
int Check_SMS()
{
	int error = 0;			//error from function
	char str[200];
	int pos_sms_rx = -1;  //Received SMS position
	pos_sms_rx = gsm.IsSMSPresent(SMS_ALL);
	//Serial.println(pos_sms_rx);
	if (pos_sms_rx > 0)
	{
		//Read text/number/position of sms
		//gsm.GetSMS(pos_sms_rx, number, sms_rx, 120);
		digitalWrite(useLED, HIGH);
		error = gsm.GetAuthorizedSMS(pos_sms_rx, number, sms_rx, 122, 1, 6);
		if (error == GETSMS_AUTH_SMS || error == GETSMS_NOT_AUTH_SMS)
		//if(error > 0)
		{
#ifndef DEBUG_GSMRX
			sprintf_P(str, PSTR("SMS from %s: %s"), number, sms_rx);
			Serial.println(str);
			//Serial.println(sms_rx);
#endif
			digitalWrite(useLED, LOW);
			if (1 == gsm.DeleteSMS(pos_sms_rx))
				Serial.println(F("Sters"));
			else
				Serial.println(F("EROOR"));
			return error;
		}

	}
	return pos_sms_rx;
}
