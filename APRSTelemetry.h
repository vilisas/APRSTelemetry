/*
 * APRSTelemetry.h
 *
 *  Created on: 2017-11-28
 *      Author: Vilius, LY3FF
 */

#ifndef APRSTELEMETRY_H_
#define APRSTELEMETRY_H_

#ifndef TELEMETRY_PACKET_INTERVAL
# define	TELEMETRY_PACKET_INTERVAL 60
#endif

#include "APRSBase91.h"

typedef enum {CH1 = 0, CH2, CH3, CH4, CH5} TChannel;


class APRSTelemetry{
    /*
     * Analog channel 1 - Battery Voltage
     * Analog channel 2 - Inside temperature
     * Analog channel 3 - Outside temperature
     * Analog channel 4 - Altitude
     * Analog channel 5 - Speed
     *
     * 8 digital channels
     *
     * every digital channel is set as literal 1 or 0 directly in bits[8] char array.
     * Later we just add this array to the telemetry packet end
     *
     * Note: Again, the field widths are not all the same, and the byte counts include the comma separators
     * where shown. The list can terminate after any field.
    */

    private:
	void (*send_packet__)() = nullptr;
	APRS_Base91 base91;

    unsigned int _sequence_number = 0;
    unsigned char _temperature[2] = { 0, 0 };	// generated temperature bytes for telemetry out.
    unsigned char _t_voltage = 0;				// generated byte for telemetry output

    double _battery_voltage=0;			// actual battery voltage
    unsigned char _packet_to_send   = 0;	// which packet will be sent next (units, params, eqns)
    unsigned int _ts_packet_to_send = 0;
    unsigned int _ts_read_sensors = 0; //
    unsigned int _ts_telemetry_packet = 0;

    public:
    char _packet_buffer[80];
    int _packet_length = 0;
    char _bits[9] = { '0', '0', '0', '0', '0', '0', '0', '0', 0 };
    // (sequence + 5 analog channels + 1 digital ch) * 2 + 1 null = (1 + 5 + 1) * 2 + 1 = 15
    char _telemetry_comment[15];

    // const ?
    const char *_tpn;		// TELEMETRY_PARAM_NAMES
    const char *_tpu;		// TELEMETRY_PARAM_UNITS
    const char *_tpe;		// TELEMETRY_PARAM_EQUATIONS;

    void setup();
    void loop();
    void setBit(unsigned char bit, bool state);
    bool getBit(unsigned char bit);
    void setBatteryVoltage(double voltage)	{ this->_battery_voltage = voltage; updateTelemetryChannel(CH1, _battery_voltage * 100); }
    void updateBatteryVoltage();
    void updateTemperatures(int temp1, int temp2);
    void updateTelemetrySequence();
    void generateParametersPacket();
    void generateUnitsPacket();
    void generateEQNSPacket();
    void sendTelemetryPacket();
    void updateTelemetryChannel(const TChannel channel, unsigned int value);

    void setSendPacketCallback(void (* send_packet_callback)()){ this->send_packet__ = send_packet_callback;}

};




#endif
