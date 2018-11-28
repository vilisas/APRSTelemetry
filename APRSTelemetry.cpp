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

    void APRSTelemetry::readTemperatureSensors(){

//	  Serial.print(F("Requesting temperatures..."));
//	_sensors->requestTemperatures();
//	  Serial.println(F("DONE"));
//		  Serial.print(F("Temperature for the device 1 (index 0) is: "));
//		  Serial.println(sensors.getTempCByIndex(0)); // onboard
//		  Serial.println(sensors.getTempCByIndex(1)); // ant laido


    }
    void APRSTelemetry::setBatteryVoltage(double voltage){
	// read ADC channel, calculate value and store it to class attribute
	// Jei reference voltage: 5.20V  daliklis: + 3k --- 750R --- (-)
	// max adc reiksme = 1024 prie 26V

//  uint16_t adcValue = analogRead(VOLTAGE_ADC_PIN);
//	Serial.print(F("adcValue="));
//	Serial.println(adcValue);
//	this->_battery_voltage = ((double) (adcValue * _mbMaxVoltage) / 1024);
    	this->_battery_voltage = voltage;
	/*Parodo baterijos itampa serial porte, BET, itampa skaiciuojama analog reference atzvilgiu, kas pas mus yra maitinimo itampa.
	 * Kai modulis maitinasi is baterijos per itampos stabilizatoriu, tai analog reference = 3.2V, bet jei prisijungiam per USB
	 * tai itampa gaunasi 5V. Rezultate _battery_voltage reiksme bus skirtinga. Darbiniam rezime telemetrijos pakete viskas turetu eiti.
	 * Norint serial porte matyti teisinga reiksme, reikia pakeisti MB_ADC_VOLTAGE is 3.2 i 5.0
	 */
	Serial.println(F("Battery Voltage:"));
	Serial.println(this->_battery_voltage);
    }

void APRSTelemetry::setup(){
//    Serial.println(F("APRSTelemetry::setup()\nInitializing temp. sensors"));

//    this->_oneWire = new OneWire(ONE_WIRE_BUS);
//    this->_sensors = new DallasTemperature(this->_oneWire);

//    _sensors->begin();
//    _sensors->setResolution(9); // zemesne rezoliucija - greiciau nuskaito (9 bitai - puses laipsnio tikslumu)


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

    if (seconds - this->_ts_packet_to_send > 45) {
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

