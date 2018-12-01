/*
 * APRSBase91.cpp
 *
 *  Created on: 2018-10-21
 *      Author: vilisas
 */

#include "APRSBase91.h"

/**
 * takes unsigned integer
 * returns byte count, puts base91 encoded bytes in buffer string.
 * no null termination
 */
unsigned int APRS_Base91::encode(char* buffer, unsigned int skaicius) {

	//static char buffer[8];
	unsigned int i = 0;
	unsigned int powered = 1;
	while (skaicius >= powered) {
		powered *= 91;
		i++;
	}
	char byte = 0;
	for (unsigned int j = 0; j < i; j++) {
		powered /= 91;
		byte = (skaicius / powered) % this->_base;
		byte += this->_shift;
		buffer[j] = byte;
	}
	// ! padding... TODO: sugalvoti ka nors geresnio
	if (i == 1){
		buffer[0] = '!';
		buffer[1] = byte;
	}
//	buffer[i] = 0;
	return i;
}

/**
 * takes null terminated base91 string
 * returns decoded integer
 */
unsigned int APRS_Base91::decode(char * buffer){

	int i = 0;
	unsigned int output = 0;
	// randam ilgi, null terminated arba paieskos riba
	while (buffer[i] != 0 && i < this->_limit) {
		i++;
	}
	if (i == this->_limit) return 0; 	// = kazkas nepavyko

	unsigned int daugiklis = 1;
	for (int j = i-1; j>= 0; j--){
		output += (buffer[j]-this->_shift) * daugiklis;
		daugiklis *= this->_base;
	}
	return output;

}

