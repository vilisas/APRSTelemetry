/*
 * APRSTelemetry.h
 *
 *  Created on: 2017-11-28
 *      Author: Vilius, LY3FF
 */

#include "APRSTelemetry.h"
#include <Arduino.h>

    /**
     * formats telemetry comment part
     */
    void APRSTelemetry::updateTelemetryPacket(){

	// max seq number is 8191
	(++this->_sequence_number) &= 0x1FFF;

	updateTelemetryChannel(SEQ, _sequence_number);
	updateTelemetryChannel(CH1, _battery_voltage * 100);
	updateTelemetryChannel(CH2, _temperature[0]+80);
	updateTelemetryChannel(CH3, _temperature[1]+80);
	updateTelemetryChannel(CH4, _pressure);
	updateTelemetryChannel(CH5, _altitude);

	updateTelemetryChannel(CH_BITS, _bits);

    }

//  this sends already generated packet
    void APRSTelemetry::sendTelemetryPacket(){

	Serial.print(F("packet_buffer='"));
	Serial.print(packet_buffer);
	Serial.println(F("'"));
	Serial.print(F("packet_length="));
	Serial.println(this->packet_length);

	//	call actual method (if set) to send packet
	//	this class doesn't know how to push packet in the air, but konws what to call to do it
		if (this->send_packet__ != nullptr) {
			this->send_packet__();
		}

	memset(packet_buffer,0,sizeof(packet_buffer));
	packet_length=0;
    }

    // sets bit on or off
    // using APRS defined bit numbering 1-8 (least significant bit is first on the left)
    // 12345678
    void APRSTelemetry::setBit(uint8_t bit, bool state) {
	if (bit - 1 < 8) {
		if (state == true){
			_bits |= 1UL << (8 - bit);
		} else{
			_bits &= ~(1UL << (8 - bit));
		}
	}
    }

    bool APRSTelemetry::getBit(uint8_t bit){
	if (bit - 1 < 8) {
		return (_bits >> (8-bit)) & 1U;
	}
	return false;
    }

void APRSTelemetry::updateTelemetryChannel(const TChannel channel, unsigned int value){
	// seq, ch1 .. ch5.. digital

	if (value >8280 ){
		value = 0;
	}
	this->base91.encode(&_telemetry_comment[channel * 2], value);
}

void APRSTelemetry::setup(){
	memset(_telemetry_comment, '!', sizeof(_telemetry_comment));
	_telemetry_comment[sizeof(_telemetry_comment)-1] = 0;
}

void APRSTelemetry::loop(){
    unsigned int seconds = (millis() / 1000);

    if (seconds - this->_ts_packet_to_send > TELEMETRY_PACKET_INTERVAL) {
	switch (_packet_to_send) {
	case 0:
//	    this->generateUnitsPacket();
		packet_length = strlen_P(_tpu);
		strncpy_P(packet_buffer, _tpu, packet_length);

	    break;
	case 1:
//	    this->generateParametersPacket();
		packet_length = strlen_P(_tpn);
		strncpy_P(packet_buffer, _tpn, packet_length);

	    break;
	case 2:
//	    this->generateEQNSPacket();
		packet_length = strlen_P(_tpe);
		strncpy_P(packet_buffer, _tpe, packet_length);

	    break;
	case 3:
//	    this->generateBITSPacket();
		packet_length = strlen_P(_tpb);
		strncpy_P(packet_buffer, _tpb, packet_length);

	    break;
	}
	++this->_packet_to_send %= 4;
	this->sendTelemetryPacket();
	this->_ts_packet_to_send = seconds;
    }
}

