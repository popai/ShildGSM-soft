/*
 * cmd.c
 *
 *  Created on: Apr 15, 2013
 *      Author: popai
 *  Modify on Aug 26, 2015
 */
#include "ShildCmd.h"

#include "Arduino.h"
#include "lib/myGSM/MyGSM.h"

//#include <avr/pgmspace.h>
#include <avr/eeprom.h>
#include <avr/io.h>
//#include <string.h>
#include <stdio.h>

#include "ShildPinDef.h"
//#include <math.h>

GSM gsm;			//gsm handler class

//Function prototypes
void static StareIN(char *nrtel);
void static StareOUT(char *nrtel);
void static StareTMP(char *nrtel);

//"Flash store commands etc are strings to store
const char IN1[] PROGMEM = "IN1";			//address 18*1
const char IN2[] PROGMEM = "IN2";			//address 18*2
const char IN3[] PROGMEM = "IN3";			//address 18*3
const char OUT1L[] PROGMEM = "OUT1L";		//address 18*4
const char OUT1H[] PROGMEM = "OUT1H";		//address 18*5
const char OUT1P[] PROGMEM = "OUT1P";		//address 18*6
const char OUT2L[] PROGMEM = "OUT2L";		//address 18*7
const char OUT2H[] PROGMEM = "OUT2H";		//address 18*8
const char OUT2P[] PROGMEM = "OUT2P";		//address 18*9
const char OUT3L[] PROGMEM = "OUT3L";		//address 18*10
const char OUT3H[] PROGMEM = "OUT3H";		//address 18*11
const char OUT3P[] PROGMEM = "OUT3P";		//address 18*12
const char OUT4L[] PROGMEM = "OUT4L";		//address 18*13
const char OUT4H[] PROGMEM = "OUT4H";		//address 18*14
const char OUT4P[] PROGMEM = "OUT4P";		//address 18*15
const char OUT5L[] PROGMEM = "OUT5L";		//address 18*16
const char OUT5H[] PROGMEM = "OUT5H";		//address 18*17
const char OUT5P[] PROGMEM = "OUT5P";		//address 18*18
const char OUT6L[] PROGMEM = "OUT6L";		//address 18*19
const char OUT6H[] PROGMEM = "OUT6H";		//address 18*20
const char OUT6P[] PROGMEM = "OUT6P";		//address 18*21
const char TMP1[] PROGMEM = "TMP1";		//address 18*22
const char TMP2[] PROGMEM = "TMP2";		//address 18*23
const char PASS[] PROGMEM = "PASS";		//address 18*24
const char LOGIN[] PROGMEM = "LOGIN";		//predefined command
const char DEL[] PROGMEM = "DEL";			//predefined command
const char DELEP[] PROGMEM = "DELEP";		//predefined command
const char STARE_IN[] PROGMEM = "STARE IN";	//predefined command
const char STARE_OUT[] PROGMEM = "STARE OUT";	//predefined command
const char STARE_TMP[] PROGMEM = "STARE TMP";	//predefined command
const char STARE_ALL[] PROGMEM = "STARE ALL";	//predefined command

// The table to refer to my strings.
const char *comenzi[] =
{ IN1, IN2, IN3, OUT1L, OUT1H, OUT1P, OUT2L, OUT2H, OUT2P, OUT3L, OUT3H, OUT3P,
		OUT4L, OUT4H, OUT4P, OUT5L, OUT5H, OUT5P, OUT6L, OUT6H, OUT6P, TMP1,
		TMP2, PASS };

//read data from eeprom on specified adres
void ReadEprom(char* str_citit, int const address)
{
	//char str_citit[18];
	eeprom_read_block(str_citit, (int*) address, 18);
//#if DEBUG
	//Serial.print("citesc la address: ");
	Serial.print(address);
	Serial.print(": ");
	Serial.println(str_citit);
//#endif
	//return str_citit;
}

void DellEprom()
{
	int i;
	for (i = 0; i < 512; i++)
		eeprom_write_byte((uint8_t*) i, 0);
}

/**
 * @brief : Write user defined commands on eeprom
 *
 * @param : char *inbuffer = command send by user
 * @return: 0 = write not succeed, 1 = succeed write
 */
bool CfgCmd(char *inbuffer)
{
	int address = 18;
	char comanda[7];
	int8_t i;

	for (i = 0; i < 24; ++i)
	{
		//strcpy_P(comanda, (char*) pgm_read_word(comenzi[i])); // Necessary casts and dereferencing, just copy.
		strcpy_P(comanda, comenzi[i]);
		if (strstr(inbuffer, comanda) != 0)
		{

			inbuffer += strlen(comanda) + 1;
			address = 18 * (1 + i);
//#if DEBUG
			//Serial.print("Scriu la address ");
			Serial.print(address);
			Serial.print(": ");
			Serial.println(inbuffer);
//#endif
			eeprom_write_block(inbuffer, (int*) address, 18);
			//EEPROM.updateBlock(inbuffer, address, 18);
			return 1;
		}
	}
	return 0;
}

/**
 * @brief : write the sms string for commands
 *
 * @param : char *nrtel -> sms sender phone number
 * 			char *inmsg -> sms message
 * @return: no return
 */
void Config(char *nrtel, char *inmsg)
{
	char buffer[64];
	if ((strlen(nrtel) != 0) && (strlen(inmsg) != 0))
	{
		//add number on authorized slot;
		if (strstr_P(inmsg, LOGIN) != 0)
		{
			uint8_t nr_pfonnr = 1;	  //hold numbers of the phone number on sim
			for (byte i = 1; i < 7; i++)
			{
				if (1 == gsm.GetPhoneNumber(i, buffer))
					++nr_pfonnr;	//Find number in specified position
				else
					break;
			}
			if (nr_pfonnr < 7)
			{
				if (0 != gsm.WritePhoneNumber(nr_pfonnr, nrtel))
				{
					strcpy_P(buffer, PSTR("Acceptat"));
					Serial.println(buffer);
					gsm.SendSMS(nrtel, buffer);

				}
				else
				{
					strcpy_P(buffer, PSTR("Eroare"));
					Serial.println(buffer);
					gsm.SendSMS(nrtel, buffer);
				}
			}
			else
			{
				strcpy_P(buffer, PSTR("Slot ocupat"));
				gsm.SendSMS(nrtel, buffer);
			}

			//CfgCmd(inmsg);

		}
		//delete phone number from sim
		else if (strstr_P(inmsg, DEL) != 0)
		{
			byte i = 1;

			for (i = 1; i < 7; i++)
				if (gsm.ComparePhoneNumber(i, nrtel) == 1)
					break;
			if (i < 7)
				if (0 != gsm.DelPhoneNumber(i))
				{
					strcpy_P(buffer, PSTR("Sters"));
					gsm.SendSMS(nrtel, buffer);
				}
				else
				{
					strcpy_P(buffer, PSTR("Ne Sters"));
					gsm.SendSMS(nrtel, buffer);
				}

		}
		//delete EEPROM of microcontroller
		else if (strstr_P(inmsg, DELEP) != 0)
			DellEprom();
		else
		{
			if (CfgCmd(inmsg))
				gsm.SendSMS(nrtel, "OK");
			else
				gsm.SendSMS(nrtel, "EROARE");
		}
	}
}

/**
 * @brief : Execute the commands received from SMS
 *
 * @param : nrtel - used to send confirmation
 * 			inmsg - command to be executed
 * @return: no return
 */
void Comand(char *nrtel, char *inmsg)
{
	char buffer[24];
	char OK[3] = "OK";

	ReadEprom(buffer, 18 * 4);
	if (strcasecmp(buffer, inmsg) == 0)
	{
		digitalWrite(outD1, LOW);
		gsm.SendSMS(nrtel, OK);
		return;
	}

	ReadEprom(buffer, 18 * 5);
	if (strcasecmp(buffer, inmsg) == 0)
	{
		digitalWrite(outD1, HIGH);
		gsm.SendSMS(nrtel, OK);
		return;
	}

	ReadEprom(buffer, 18 * 6);
	if (strcasecmp(buffer, inmsg) == 0)
	{
		digitalWrite(outD1, HIGH);
		delay(1000);
		digitalWrite(outD1, LOW);
		gsm.SendSMS(nrtel, OK);
		return;
	}

	ReadEprom(buffer, 18 * 7);
	if (strcasecmp(buffer, inmsg) == 0)
	{
		digitalWrite(outD2, LOW);
		gsm.SendSMS(nrtel, OK);
		return;
	}

	ReadEprom(buffer, 18 * 8);
	if (strcasecmp(buffer, inmsg) == 0)
	{
		digitalWrite(outD2, HIGH);
		gsm.SendSMS(nrtel, OK);
		return;
	}

	ReadEprom(buffer, 18 * 9);
	if (strcasecmp(buffer, inmsg) == 0)
	{
		digitalWrite(outD2, HIGH);
		delay(1000);
		digitalWrite(outD2, LOW);
		gsm.SendSMS(nrtel, OK);
		return;
	}

	ReadEprom(buffer, 18 * 10);
	if (strcasecmp(buffer, inmsg) == 0)
	{
		digitalWrite(outD3, LOW);
		gsm.SendSMS(nrtel, OK);
		return;
	}

	ReadEprom(buffer, 18 * 11);
	if (strcasecmp(buffer, inmsg) == 0)
	{
		digitalWrite(outD3, HIGH);
		gsm.SendSMS(nrtel, OK);
		return;
	}

	ReadEprom(buffer, 18 * 12);
	if (strcasecmp(buffer, inmsg) == 0)
	{
		digitalWrite(outD3, HIGH);
		delay(1000);
		digitalWrite(outD3, LOW);
		gsm.SendSMS(nrtel, OK);
		return;
	}

	ReadEprom(buffer, 18 * 13);
	if (strcasecmp(buffer, inmsg) == 0)
	{
		digitalWrite(outD4, LOW);
		gsm.SendSMS(nrtel, OK);
		return;
	}

	ReadEprom(buffer, 18 * 14);
	if (strcasecmp(buffer, inmsg) == 0)
	{
		digitalWrite(outD4, HIGH);
		gsm.SendSMS(nrtel, OK);
		return;
	}

	ReadEprom(buffer, 18 * 15);
	if (strcasecmp(buffer, inmsg) == 0)
	{
		digitalWrite(outD4, HIGH);
		delay(1000);
		digitalWrite(outD4, LOW);
		gsm.SendSMS(nrtel, OK);
		return;
	}

	ReadEprom(buffer, 18 * 16);
	if (strcasecmp(buffer, inmsg) == 0)
	{
		digitalWrite(outD5, LOW);
		gsm.SendSMS(nrtel, OK);
		return;
	}

	ReadEprom(buffer, 18 * 17);
	if (strcasecmp(buffer, inmsg) == 0)
	{
		digitalWrite(outD5, HIGH);
		gsm.SendSMS(nrtel, OK);
		return;
	}

	ReadEprom(buffer, 18 * 18);
	if (strcasecmp(buffer, inmsg) == 0)
	{
		digitalWrite(outD5, HIGH);
		delay(1000);
		digitalWrite(outD5, LOW);
		gsm.SendSMS(nrtel, OK);
		return;
	}
#if defined(__AVR_ATmega328P__)
	ReadEprom(buffer, 18 * 19);
	if (strcasecmp(buffer, inmsg) == 0)
	{
		//digitalWrite(outD5, LOW);
		PORTD &= ~(1 << PIND7);
		//pin_state |= (1 << PIND7);
		//eeprom_write_byte((uint8_t*) 396, pin_state);
		gsm.SendSMS(nrtel, OK);
		return;
	}

	ReadEprom(buffer, 18 * 20);
	if (strcasecmp(buffer, inmsg) == 0)
	{
		//digitalWrite(outD5, HIGH);
		PORTD |= (1 << PIND7);
		//pin_state |= (1 << PIND7);
		//eeprom_write_byte((uint8_t*) 396, pin_state);
		gsm.SendSMS(nrtel, OK);
		return;
	}

	ReadEprom(buffer, 18 * 21);
	if (strcasecmp(buffer, inmsg) == 0)
	{
		//digitalWrite(outD1, HIGH);
		PORTD |= (1 << PIND7);
		delay(1000);
		PORTD &= ~(1 << PIND7);
		////pin_state |= (1 << PIND2);
		////eeprom_write_byte((uint8_t*) 396, pin_state);
		gsm.SendSMS(nrtel, OK);
		return;
	}
#endif
	if (strcasecmp_P(inmsg, STARE_IN) == 0)
	{
		StareIN(nrtel);
		return;
	}

	//strcpy_P(buffer, (char*) pgm_read_word(&(comenzi[18])));
	//if (strcasecmp(buffer, inmsg) == 0)
	if (strcasecmp_P(inmsg, STARE_OUT) == 0)
	{
		StareOUT(nrtel);
		return;
	}
	//strcpy_P(buffer, (char*) pgm_read_word(&(comenzi[19])));
	//if (strcasecmp(buffer, inmsg) == 0)
	if (strcasecmp_P(inmsg, STARE_TMP) == 0)
	{
		StareTMP(nrtel);
		return;
	}

	//strcpy_P(buffer, (char*) pgm_read_word(&(comenzi[20])));
	//if (strcasecmp(buffer, inmsg) == 0)
	if (strcasecmp_P(inmsg, STARE_ALL) == 0)
	{
		StareIN(nrtel);
		StareOUT(nrtel);
		StareTMP(nrtel);
		return;
	}

	if (strcasecmp_P(inmsg, DEL) == 0)
	{
		byte i = 1;
		for (i = 1; i < 7; i++)
			if (gsm.ComparePhoneNumber(i, nrtel))
				break;
		if (i < 7 && 0 != gsm.DelPhoneNumber(i))
		{
			strcpy_P(buffer, PSTR("Sters"));
			Serial.print(buffer);
			gsm.SendSMS(nrtel, buffer);
		}
		else
		{
			strcpy_P(buffer, PSTR("Ne Sters"));
			Serial.print(buffer);
			gsm.SendSMS(nrtel, buffer);
		}
		return;
	}

	ReadEprom(buffer, 486);
	gsm.SendSMS(nrtel, buffer);
	Serial.println(buffer);
	return;

}

/**
 * @brief :	Send a sms with state of output pins
 *
 * @param : char *nrtel = phone number who interrogate the state
 * @return: no return
 */
void static StareOUT(char *nrtel)
{
	char mesage[256];
	char buffer[18];
	//int i = 108;
	*mesage = 0x00;
	if (digitalRead(outD1) == LOW)
	{
		ReadEprom(buffer, 18 * 4);
		//Serial.print(strlen(buffer));
		//Serial.println(buffer);

		if (strlen(buffer) != 0)
		{
			strcat(mesage, buffer);
			strcat_P(mesage, PSTR("\r\n"));
		}

	}
	else
	{
		ReadEprom(buffer, 18 * 5);
		if (strlen(buffer) != 0)
		{
			strcat(mesage, buffer);
			strcat_P(mesage, PSTR("\r\n"));
		}
	}

	if (digitalRead(outD2) == LOW)
	{
		ReadEprom(buffer, 18 * 7);
		if (strlen(buffer) != 0)
		{
			strcat(mesage, buffer);
			strcat_P(mesage, PSTR("\r\n"));
		}
	}
	else
	{
		ReadEprom(buffer, 18 * 8);
		if (strlen(buffer) != 0)
		{
			strcat(mesage, buffer);
			strcat_P(mesage, PSTR("\r\n"));
		}
	}

	if (digitalRead(outD3) == LOW)
	{
		ReadEprom(buffer, 18 * 10);
		if (strlen(buffer) != 0)
		{
			strcat(mesage, buffer);
			strcat_P(mesage, PSTR("\r\n"));
		}
	}
	else
	{
		ReadEprom(buffer, 18 * 11);
		if (strlen(buffer) != 0)
		{
			strcat(mesage, buffer);
			strcat_P(mesage, PSTR("\r\n"));
		}
	}

	if (digitalRead(outD4) == LOW)
	{
		ReadEprom(buffer, 18 * 13);
		if (strlen(buffer) != 0)
		{
			strcat(mesage, buffer);
			strcat_P(mesage, PSTR("\r\n"));
		}
	}
	else
	{
		ReadEprom(buffer, 18 * 14);
		if (strlen(buffer) != 0)
		{
			strcat(mesage, buffer);
			strcat_P(mesage, PSTR("\r\n"));
		}
	}

	if (digitalRead(outD5) == LOW)
	{
		ReadEprom(buffer, 18 * 16);
		if (strlen(buffer) != 0)
		{
			strcat(mesage, buffer);
			strcat_P(mesage, PSTR("\r\n"));
		}
	}
	else
	{
		ReadEprom(buffer, 18 * 17);
		if (strlen(buffer) != 0)
		{
			strcat(mesage, buffer);
			strcat_P(mesage, PSTR("\r\n"));
		}
	}
#if defined (__AVR_ATmega328P__)
	if ((PIND & (1 << PIND7)) == 0)
	{
		ReadEprom(buffer, 18 * 19);
		if (strlen(buffer) != 0)
		{
			strcat(mesage, buffer);
			strcat_P(mesage, PSTR("\r\n"));
		}
	}
	else
	{
		ReadEprom(buffer, 18 * 20);
		if (strlen(buffer) != 0)
		{
			strcat(mesage, buffer);
			//strcat(mesage, "\n");
		}
	}
#endif
	if (strlen(mesage) != 0)
		gsm.SendSMS(nrtel, mesage);
}

/**
 * @brief : Send a sms with state of input pins
 *
 * @param : nrtel -> telephone number to send sms
 * @return: no return
 */
void static StareIN(char *nrtel)
{

	char mesage[96];
	char buffer[18];
	*mesage = 0x00;
	int8_t Vo = 0;

	//Vo = 4 * analogRead(PINC4);
	Vo = (20 * analogRead(senzorV) / 1023);
	sprintf(mesage, " U = %d \n", Vo);
	ReadEprom(buffer, 18 * 1);
	if (strlen(buffer) != 0)
		if (digitalRead(inD1) == LOW)
		{
			{
				strcat(mesage, buffer);
				strcat_P(mesage, PSTR(" on"));
				strcat_P(mesage, PSTR("\r\n"));
			}

		}
		else
		{
			strcat(mesage, buffer);
			strcat_P(mesage, PSTR(" off"));
			strcat_P(mesage, PSTR("\r\n"));
		}

	ReadEprom(buffer, 18 * 2);
	if (strlen(buffer) != 0)
		if (digitalRead(inD2) == LOW)
		{
			{
				strcat(mesage, buffer);
				strcat_P(mesage, PSTR(" on"));
				strcat_P(mesage, PSTR("\r\n"));
			}

		}
		else
		{
			strcat(mesage, buffer);
			strcat_P(mesage, PSTR(" off"));
			strcat_P(mesage, PSTR("\r\n"));
		}

	ReadEprom(buffer, 18 * 3);
	if (strlen(buffer) != 0)
		if (digitalRead(inD3) == LOW)
		{
			{
				strcat(mesage, buffer);
				strcat_P(mesage, PSTR(" on"));
				strcat_P(mesage, PSTR("\r\n"));
			}

		}
		else
		{
			strcat(mesage, buffer);
			strcat_P(mesage, PSTR(" off"));
			//strcat_P(mesage, PSTR("\r\n"));
		}
	if (strlen(mesage) != 0)
		gsm.SendSMS(nrtel, mesage);

}
/**
 * @brief : Thermistor calculate the temperature in grades Celsius dead by a 10K thermistor
 *
 * @param : analog pin where connect the thermistor
 * @return: temperature in grade Celsius
 */
float Thermistor(int Tpin)
{
	float Vo;
	float logRt, Rt, T;
	float R = 9870; // fixed resistance, measured with multimeter
	// c1, c2, c3 are calibration coefficients for a particular thermistor
	float c1 = 1.009249522e-03, c2 = 2.378405444e-04, c3 = 2.019202697e-07;
	Vo = analogRead(Tpin);
	//Vo = ReadADC(Tpin);
	Rt = R * (1024.0 / Vo - 1.0);
	logRt = log(Rt);
	T = (1.0 / (c1 + c2 * logRt + c3 * logRt * logRt * logRt)) - 273.15;
	return T;
}

/**
 * @brief : Send a sms with temperature read from senzors
 *
 * @param : char *nrtel = phone number who interrogate the state
 * @return: no return
 */
static void StareTMP(char *nrtel)
{
	char mesage[96];
	char buffer[18];
	char tmpe[32];
	int tmp, tmp1, tmp2;
	*mesage = 0x00;

	tmp = Thermistor(senzorT1);
	delay(10);
	tmp1 = Thermistor(senzorT1);
	delay(10);
	tmp2 = Thermistor(senzorT1);
	tmp = (tmp + tmp1 + tmp2) / 3;

	ReadEprom(buffer, 18 * 22);
	if (strlen(buffer) != 0)
	{
		sprintf(tmpe, " %s: %d %s", buffer, tmp, "C\r\n");
		strcat(mesage, tmpe);
	}

	tmp = Thermistor(senzorT2);
	delay(10);
	tmp1 = Thermistor(senzorT2);
	delay(10);
	tmp2 = Thermistor(senzorT2);
	tmp = (tmp + tmp1 + tmp2) / 3;

	ReadEprom(buffer, 18 * 23);
	if (strlen(buffer) != 0)
	{
		sprintf(tmpe, " %s: %d %s", buffer, tmp, "C\r\n");
		strcat(mesage, tmpe);
	}

	if (strlen(mesage) != 0)
		gsm.SendSMS(nrtel, mesage);
}

//int8_t in1 = 1, in2 = 1, in3 = 1, in4 = 1;
bool static in1 = true;
bool static in2 = true;
bool static in3 = true;
bool static sms_sent = true;

/**
 * @brief : Verify the state of input pins
 *
 * @param : no parameters
 * @return: no return
 */
void VerificIN()
{
	//char mesage[80];
	char number[20];
	char buffer[32];

	if (digitalRead(inD1) == LOW)
	{
		if (in1)
		{
			in1 = false;
			ReadEprom(buffer, 18 * 1);
			if (strlen(buffer) != 0)
			{
				strcat_P(buffer, PSTR(" on"));
				for (byte i = 1; i < 7; i++)
					if (1 == gsm.GetPhoneNumber(i, number))
						gsm.SendSMS(number, buffer);
			}
		}
	}
	else
	{
		if (in1 == false)
		{
			ReadEprom(buffer, 18 * 1);
			if (strlen(buffer) != 0)
			{
				strcat_P(buffer, PSTR(" off"));
				for (byte i = 1; i < 7; i++)
					if (1 == gsm.GetPhoneNumber(i, number))
						gsm.SendSMS(number, buffer);
			}
		}
		in1 = true;
	}

	if (digitalRead(inD2) == LOW)
	{
		if (in2)
		{
			in2 = false;
			ReadEprom(buffer, 18 * 2);
			if (strlen(buffer) != 0)
			{
				strcat_P(buffer, PSTR(" on"));
				for (byte i = 1; i < 7; i++)
					if (1 == gsm.GetPhoneNumber(i, number))
						gsm.SendSMS(number, buffer);
			}
		}
	}
	else
	{
		if (in2 == false)
		{
			ReadEprom(buffer, 18 * 2);
			if (strlen(buffer) != 0)
			{
				strcat_P(buffer, PSTR(" off"));
				for (byte i = 1; i < 7; i++)
					if (1 == gsm.GetPhoneNumber(i, number))
						gsm.SendSMS(number, buffer);
			}
		}
		in2 = true;
	}

	if (digitalRead(inD3) == LOW)
	{
		if (in3)
		{
			in3 = false;
			ReadEprom(buffer, 18 * 3);
			if (strlen(buffer) != 0)
			{
				strcat_P(buffer, PSTR(" on"));
				for (byte i = 1; i < 7; i++)
					if (1 == gsm.GetPhoneNumber(i, number))
						gsm.SendSMS(number, buffer);
			}
		}
	}
	else
	{
		if (in3 == false)
		{
			ReadEprom(buffer, 18 * 3);
			if (strlen(buffer) != 0)
			{
				strcat_P(buffer, PSTR(" off"));
				for (byte i = 1; i < 7; i++)
					if (1 == gsm.GetPhoneNumber(i, number))
						gsm.SendSMS(number, buffer);
			}
		}
		in3 = true;
	}

	int Vo = 0;
	Vo = (20 * analogRead(senzorV) / 1023);
	sprintf(buffer, " Alerta: U = %d", Vo);
	if (Vo >= 15 || Vo <= 11)
	{
		if (sms_sent)
		{

			for (byte i = 1; i < 7; i++)
				if (gsm.GetPhoneNumber(i, number) == 1) //Find number in specified position
					gsm.SendSMS(number, buffer);
			sms_sent = false;
		}
	}
	else
		sms_sent = true;
}
