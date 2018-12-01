/*
 * APRSTelemetry.h
 *
 *  Created on: 2017-11-28
 *      Author: Vilius, LY3FF
 */

#include "APRSTelemetry.h"
#include <Arduino.h>

    void APRSTelemetry::updateBatteryVoltage(){
    	updateTelemetryChannel(CH1, _battery_voltage * 100);
    }

    void APRSTelemetry::updateTemperatures(int temp1, int temp2){
//	 reported temp = temp + 55, receiving end should use equations packet to decode.
//	this->_temperature[0] = temp1+55;
//	this->_temperature[1] = temp2+55;
    	updateTelemetryChannel(CH1, temp1+80);
    	updateTelemetryChannel(CH2, temp2+80);

    }

    void APRSTelemetry::updateTelemetrySequence(){
	/*
	 * suraso telemetrijos paketa i packet_buffer. nurodo paketo ilgi i packet_length
	 */
	// T#MIC199,000,255,073,123,01101001   -- 33 bytes
//		packet_length = sprintf_P(packet_buffer, PSTR("aaa"),0);
//	_packet_length = sprintf_P(_packet_buffer, PSTR("T#%03u,%03u,%03u,%03u,%03u,%03u,"),
//		this->_sequence_number, this->_t_voltage, this->_temperature[0], this->_temperature[1], 0, 0);
//	strncat(&_packet_buffer[_packet_length],_bits, 8);
//	_packet_length +=8;

	// max seq number is 8191
	(++this->_sequence_number) &= 0x1FFF;

//	char encoded[3];
//	char encoded[2];

	this->base91.encode(_telemetry_comment, _sequence_number);
//	memcpy(&_telemetry_comment, &encoded, 2);


    }

    // generate parameters packet
    void APRSTelemetry::generateParametersPacket(){
	Serial.println(F("Generating parameters packet"));
	_packet_length = strlen_P(_tpn);
	strncpy_P(_packet_buffer, _tpn, _packet_length);
    }

	//generate unit description packet
    void APRSTelemetry::generateUnitsPacket(){
	Serial.println(F("Generating units packet"));
	_packet_length = strlen_P(_tpu);
	strncpy_P(_packet_buffer, _tpu, _packet_length);
    }

//  generate equations packet
    void APRSTelemetry::generateEQNSPacket(){
	Serial.println(F("Generating equations packet"));
	_packet_length = strlen_P(_tpe);
	strncpy_P(_packet_buffer, _tpe, _packet_length);
    }

//  this sends already generated packet
    void APRSTelemetry::sendTelemetryPacket(){

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

void APRSTelemetry::updateTelemetryChannel(const TChannel channel, unsigned int value){
	// seq, ch1 .. ch5.. digital
	char encoded[3];
//	char encoded[2];

	if (value >8280 ){
		value = 0;
	}
	this->base91.encode(&_telemetry_comment[2 + channel * 2], value);
//	this->base91.encode(encoded, value);
//	memcpy(&_telemetry_comment[2 + channel * 2],&encoded, 2);

}

void APRSTelemetry::setup(){
	memset(_telemetry_comment, '!', sizeof(_telemetry_comment));
	_telemetry_comment[sizeof(_telemetry_comment)-1] = 0;



}

void APRSTelemetry::loop(){
    unsigned int seconds = (millis() / 1000);

    if (seconds - this->_ts_read_sensors > 3){
	this->updateBatteryVoltage();
//			telemetrija.updateTemperatures(sensors.getTempCByIndex(0), sensors.getTempCByIndex(1));
	this->_ts_read_sensors = seconds;

    }

//    if (seconds - this->_ts_telemetry_packet > TELEMETRY_PACKET_INTERVAL){
//	this->generateTelemetryPacket();
//	this->sendTelemetryPacket();
//
//	this->_ts_telemetry_packet = seconds;
//    }


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

