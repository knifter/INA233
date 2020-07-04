#include "Arduino.h"
#include <TwoWireDevice.h>

#include "INA233.h"
#include "INA233_regs.h"

bool INA233::begin()
{
	if(!TwoWireDevice::begin())
		return false;

	// Get DeviceID/Revision
	uint8_t id[8];
	readreg(REG_MFR_MODEL, id, 7);
	id[7]=0x00;
	if(id[0] != 0x06)
		return false;
	if(strncmp((char*)(id+1), "INA233", 6))
		return false;

	// set device and reg cache to defaults
	setDefaults();

	return _wire.lastError();
};

void INA233::clearFaults()
{
	writereg(REG_CLEAR_FAULTS);
	return;
}

void INA233::setDefaults()
{
	writereg(REG_RESTORE_DEFAULT_ALL);
	_device_config = MFR_DEVICE_CONFIG_DFLT;
	_adc_config = MFR_ADC_CONFIG_DFLT;
}

uint16_t INA233::getVBUS()
{
	return readreg16_LM(REG_READ_VIN);
}

float INA233::getBusVoltage()
{
	return readreg16_LM(REG_READ_VIN) * 0.00125;
}

int16_t INA233::getShunt()
{
	return (int16_t) readreg16_LM(REG_MFR_READ_SHUNT);
};

float INA233::getShuntVoltage()
{
	return (float)(int16_t)readreg16_LM(REG_MFR_READ_SHUNT) * 0.0000025;
};

float INA233::getCurrent()
{
	return readreg16_LM(REG_READ_IIN) / 100;
};

float INA233::getCurrentSWmA()
{
	return (float)(int16_t)readreg16_LM(REG_MFR_READ_SHUNT) * _Rinv;
};

void INA233::shutdown()
{
	_adc_config &= ~ADC_CONFIG_MODE_MASK;
	_adc_config |= ADC_CONFIG_MODE_POWERDOWN;

	writereg16_LM(REG_MFR_ADC_CONFIG, _adc_config);
}

void INA233::setAverage(Average_t avg)
{
	_adc_config &= ~ADC_CONFIG_AVG_MASK;
	_adc_config |= (avg << ADC_CONFIG_AVG_SHIFT) & ADC_CONFIG_AVG_MASK;

	writereg16_LM(REG_MFR_ADC_CONFIG, _adc_config);
};

void INA233::setShuntIntegrate(IntegrateTime_t i)
{
	_adc_config &= ~ADC_CONFIG_SHNT_MASK;
	_adc_config |= (i << ADC_CONFIG_SHNT_SHIFT) & ADC_CONFIG_SHNT_MASK;
}

void INA233::setVBUSIntegrate(IntegrateTime_t i)
{
	_adc_config &= ~ADC_CONFIG_VBUS_MASK;
	_adc_config |= (i << ADC_CONFIG_VBUS_SHIFT) & ADC_CONFIG_VBUS_MASK;
}

void INA233::setOperatingMode(OperatingMode_t mode)
{
	_adc_config &= ~ADC_CONFIG_MODE_MASK;
	_adc_config |= mode & ADC_CONFIG_MODE_MASK;

	writereg16_LM(REG_MFR_ADC_CONFIG, _adc_config);
};

void INA233::setCalibration(uint16_t cal)
{
	writereg16_LM(REG_MFR_CALIBRATION, cal);
}

int16_t INA233::getCalibration()
{
	return readreg16_LM(REG_MFR_CALIBRATION);
}

void INA233::setR(float R, Precision_t p)
{
	switch(p)
	{
		case PREC_A:  _Rinv = 0.000025 / R; return;
		case PREC_MA: _Rinv = 0.0025 / R; return;
		case PREC_UA: _Rinv = 2.5 / R; return;
	};
};
