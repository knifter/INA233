#ifndef __INA233_H
#define __INA233_H

#include <Arduino.h>
#include <TwoWireDevice.h>

#define INA233_ADDRESS_DEFAULT              (0x40)

class INA233: public TwoWireDevice {
    public:    
		typedef enum
		{
			AVG1,							// (POR)
			AVG4,
			AVG16,
			AVG64,
			AVG128,
			AVG256,
			AVG1024
		} Average_t;

		typedef enum
		{
			INT_140US = 0b000,
			INT_204US = 0b001,
			INT_332US = 0b010,
			INT_558US = 0b011,
			INT_1100US = 0b100,
			INT_2116US = 0b101,
			INT_4156US = 0b110,
			INT_8244US = 0b111,
		} IntegrateTime_t;

		typedef enum
		{
			MODE_POWERDOWN 		= 0b0000,	// Power Down
			MODE_SHUNT_TRIG 	= 0b0001,	// Shunt voltage, triggered
			MODE_BUS_TRIG		= 0b0010,	// Bus voltage, triggered
			MODE_SHUNTBUS_TRIG	= 0b0011, 	// Shun and bus, triggered
			MODE_ADCOFF			= 0b0100,	// ADC Off
			MODE_SHUNT			= 0b0101,	// Shunt voltage, continues
			MODE_BUS			= 0b0110,	// Bus voltage, continues
			MODE_SHUNTBUS		= 0b0111,	// Shunt and bus, continues (POR default)
		} OperatingMode_t;

		typedef enum
		{
			PREC_A,
			PREC_MA,
			PREC_UA
		} Precision_t;

        INA233(TwoWire& wire, const uint8_t addr = INA233_ADDRESS_DEFAULT) : TwoWireDevice(wire, addr) {};
        INA233(const uint8_t addr = INA233_ADDRESS_DEFAULT) : TwoWireDevice(addr) {};

        bool begin();
		void clearFaults();
		void setDefaults();
		void shutdown();
		void setAverage(Average_t);
		void setShuntIntegrate(IntegrateTime_t i);
		void setVBUSIntegrate(IntegrateTime_t i);
		void setOperatingMode(OperatingMode_t);

		// VBUS reading
		uint16_t getVBUS();
		float getBusVoltage();

		// Current reading: raw shunt
		int16_t getShunt();					// RAW REG_MFR_READ_SHUNT
		float getShuntVoltage();			// Shunt voltage

		// Current reading: internal conversion
		void setCalibration(uint16_t);		// Set CAL Register
		int16_t getCalibration();			// Get CAL Register
		float getCurrent();					// Get Current from device (uses CAL register)

		// Current reading: external (library) conversion
		void setR(float, Precision_t p = PREC_MA);		// Set Shunt Resistor value for getCurrentSWmA()
		float getCurrentSWmA();				// Get Current (calc. from setR & READ_SHUNT)

		// int16_t getPower();

    protected:

    private:
        INA233(const INA233&);
        INA233& operator=(const INA233&);

		uint8_t _device_config;
		uint16_t _adc_config;
		float _Rinv = 1 * 0.0025;
};

#endif // __INA233_H
