/*
 * cmd.h
 *
 *  Created on: Apr 15, 2013
 *      Author: popai
 *  Modify on Aug 26, 2015
 */

#ifndef SHILDCMD_H_
#define SHILDCMD_H_


bool CfgCmd(char *buffer);
void ReadEprom(char* str_citit, int const adresa);
void DellEprom();
void Config(char *nrtel, char *inmsg);
void Comand(char *nrtel, char *inmsg);
float Thermistor(int RawADC);
void VerificIN();
void VerificOUT();
bool ToffSet();
//void SetPort();

#endif /* SHILDCMD_H_ */
