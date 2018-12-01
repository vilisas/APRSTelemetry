/*
 * APRSTelemetry.h
 *
 *  Created on: 2017-11-28
 *      Author: Vilius, LY3FF
 */

#include "APRSTelemetry.h"
#include <Arduino.h>

    void APRSTelemetry::updateBatteryVoltage(){
	//TODO: nustatyti itampos kanalo baita pagal lygti
	// (int) (4.2 * 100) / 5,2 = 80
	this->_t_voltage =  (_battery_voltage * 100) / 5.2;

    }

    void APRSTelemetry::updateTemperatures(int temp1, int temp2){
	// reported temp = temp + 55, receiving end should use equations packet to decode.
	this->_temperature[0] = temp1+55;
	this->_temperature[1] = temp2+55;
    }

    void APRSTelemetry::generateTelemetryPacket(){
	/*
	 * suraso telemetrijos paketa i packet_buffer. nurodo paketo ilgi i packet_length
	 */
	// T#MIC199,000,255,073,123,01101001   -- 33 bytes
//		packet_length = sprintf_P(packet_buffer, PSTR("aaa"),0);
	_packet_length = sprintf_P(_packet_buffer, PSTR("T#%03u,%03u,%03u,%03u,%03u,%03u,"),
		this->_sequence_number, this->_t_voltage, this->_temperature[0], this->_temperature[1], 0, 0);
	strncat(&_packet_buffer[_packet_length],_bits, 8);
	_packet_length +=8;
	this->_sequence_number++;

    }

    void APRSTelemetry::generateParametersPacket(){
	Serial.println(F("Generating parameters packet"));
//		strcpy_P(packet_buffer, TELEMETRY_PARAM_NAMES)	}
//		packet_length = sizeof(TELEMETRY_PARAM_NAMES);
	_packet_length = strlen_P(_tpn);
	strncpy_P(_packet_buffer, _tpn, _packet_length);
//		_packet_buffer[_packet_length] = 0;	// null termination

    }

    void APRSTelemetry::generateUnitsPacket(){
	//generate unit description packet
	Serial.println(F("Generating units packet"));
//		packet_length = sizeof(TELEMETRY_PARAM_UNITS);
	_packet_length = strlen_P(_tpu);
	strncpy_P(_packet_buffer, _tpu, _packet_length);
//		_packet_buffer[_packet_length] = 0;	// null termination
    }

    void APRSTelemetry::generateEQNSPacket(){
	// generate equations packet
	Serial.println(F("Generating equations packet"));
//		packet_length = sizeof(TELEMETRY_PARAM_EQUATIONS);
	_packet_length = strlen_P(_tpe);
	strncpy_P(_packet_buffer, _tpe, _packet_length);
//		_packet_buffer[_packet_length] = 0;	// null termination
    }

    void APRSTelemetry::sendTelemetryPacket(){
	// this sends already generated packet
//	setPTT(ON);
//	APRS_sendPkt(_packet_buffer, _packet_length);
//	setPTT(OFF);

	Serial.print(F("packet_buffer='"));
	Serial.print(_packet_buffer);
	Serial.println(F("'"));
	Serial.print(F("packet_length="));
	Serial.println(this->_packet_length);

	//	call actual method (if set) to send packet
	//	this class doesn't know how to push packet in the air, but konws what to call to do it
		if (this->send_packet__ != nullptr) {
			this->send_packet__();
		}

	memset(_packet_buffer,0,sizeof(_packet_buffer));
	_packet_length=0;
    }


    void APRSTelemetry::setBit(uint8_t bit, bool state) {
	if (bit - 1 < 8) {
		_bits[bit] = (state ? '1' : '0');
	}
    }

    bool APRSTelemetry::getBit(uint8_t bit){
	if (bit - 1 < 8) {
	    return (_bits[bit] == '1');
	}
	return false;
    }

void APRSTelemetry::setup(){

}

void APRSTelemetry::loop(){
    unsigned int seconds = (millis() / 1000);

    if (seconds - this->_ts_read_sensors > 3){
//	 read temperature sensors
//	this->readTemperatureSensors();
//	this->readBatteryVoltage();

//	this->updateTemperatures(_sensors->getTempCByIndex(0), _sensors->getTempCByIndex(1));
//	TODO: temperatures should be updated outside this loop

	this->updateBatteryVoltage();
//			telemetrija.updateTemperatures(sensors.getTempCByIndex(0), sensors.getTempCByIndex(1));
	this->_ts_read_sensors = seconds;

    }

    if (seconds - this->_ts_telemetry_packet > TELEMETRY_PACKET_INTERVAL){
	this->generateTelemetryPacket();
	this->sendTelemetryPacket();

	this->_ts_telemetry_packet = seconds;
    }

    if (seconds - this->_ts_packet_to_send > TELEMETRY_PACKET_INTERVAL) {
	switch (_packet_to_send) {
	case 0:
	    this->generateUnitsPacket();
	    break;
	case 1:
	    this->generateParametersPacket();
	    break;
	case 2:
	    this->generateEQNSPacket();
	    break;

	}
	++this->_packet_to_send %= 3;
	this->sendTelemetryPacket();
	this->_ts_packet_to_send = seconds;
    }
}

