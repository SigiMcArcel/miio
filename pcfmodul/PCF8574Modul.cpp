#include <mi/miio/PCF8574Modul.h>
#include <rapidjson/document.h>
#include <rapidjson/writer.h>
#include <rapidjson/stringbuffer.h>
#include <string>
#include <iostream>
#include <sstream>


using namespace miModul;

miModul::PCF8574Modul::PCF8574Modul()
	:IOModulBase()
{
	_Name = std::string("mipcf8574modul");
}

miModul::PCF8574Modul::~PCF8574Modul()
{
}

IOModulResult miModul::PCF8574Modul::Init()
{
	//miDriver::DriverResults result = miDriver::DriverResults::Ok;
	//int32_t iDir = 0;
	_State = IOModulResult::Ok;
	return _State;
}

IOModulResult miModul::PCF8574Modul::Deinit()
{
	return IOModulResult::Ok;
}

IOModulResult PCF8574Modul::ReadDriverSpecificInputConfig(const rapidjson::Value& item)
{
	return IOModulResult::Ok;
}
IOModulResult PCF8574Modul::ReadDriverSpecificOutputConfig(const rapidjson::Value& item)
{
	return IOModulResult::Ok;
}

IOModulResult PCF8574Modul::Open(const std::string& configuration, const std::string& driverspecific)
{
	_State = ReadModulConfiguration(configuration);
	if (_State != IOModulResult::Ok)
	{
		return _State;
	}
	_State = ResolveDriverSpecific(driverspecific);
	if (_State != IOModulResult::Ok)
	{
		return _State;
	}
	_State = Init();
	return _State;
}

IOModulResult miModul::PCF8574Modul::Start()
{
	return IOModulResult::ErrorNotImplemented;
}

IOModulResult miModul::PCF8574Modul::Stop()
{
	return IOModulResult::ErrorNotImplemented;
}

IOModulResult miModul::PCF8574Modul::Close()
{
	return Deinit();
}

IOModulResult miModul::PCF8574Modul::ReadInputs(const miIOImage::IOImage& image, const miIOImage::IOImageOffset bitOffset, const miIOImage::IOImageSize bitSize)
{
	uint8_t val = false;
	miDriver::DriverResults result = miDriver::DriverResults::Ok;
	result = _I2CDriver.I2CRead(_Address, 1, & val);
	if (result != miDriver::DriverResults::Ok)
	{
		_State = IOModulResult::ErrorRead;
		return _State;
	}
	miIOImage::IOImageResult imageResult = image.WriteUint8(bitOffset/8, val);
	if (imageResult != miIOImage::IOImageResult::Ok)
	{
		_State = IOModulResult::ErrorRead;
		return _State;
	}
	return IOModulResult::Ok;
}

IOModulResult miModul::PCF8574Modul::WriteOutputs(const miIOImage::IOImage& image, const miIOImage::IOImageOffset bitOffset, const miIOImage::IOImageSize bitSize)
{
	uint8_t val = false;
	miDriver::DriverResults result = miDriver::DriverResults::Ok;
	miIOImage::IOImageResult imageResult = miIOImage::IOImageResult::Ok;
	val = image.ReadUint8(bitOffset/8, imageResult);
	if (imageResult != miIOImage::IOImageResult::Ok)
	{
		_State = IOModulResult::ErrorWrite;
		return _State;
	}

	result = _I2CDriver.I2CWrite(_Address, 1, &val);
	if (result != miDriver::DriverResults::Ok)
	{
		_State = IOModulResult::ErrorWrite;
		return _State;
	}

	return IOModulResult::Ok;
}

IOModulResult miModul::PCF8574Modul::Control(const std::string name, const std::string function, uint32_t parameter)
{
	_State = IOModulResult::ErrorNotImplemented;
	return _State;
}

IOModulResult miModul::PCF8574Modul::ResolveDriverSpecific(const std::string& driverspecific)
{
	std::vector <std::string> params;
	std::vector <std::string> values;
	std::stringstream param(driverspecific);
	std::string intermediate;
	if (driverspecific == "")
	{
		return IOModulResult::ErrorConf;
	}
	while (std::getline(param, intermediate, ','))
	{
		params.push_back(intermediate);
	}
	if (params.size() > 1 || params.empty())
	{
		return IOModulResult::ErrorConf;
	}
	
	for (std::string& par : params)
	{
		std::stringstream value(par);
		while (std::getline(value, intermediate, '='))
		{
			values.push_back(intermediate);
		}
		if (values.size() != 3)
		{
			return IOModulResult::ErrorConf;
		}
		if (values[0] != std::string("address"))
		{
			return IOModulResult::ErrorConf;
		}

		_Address = static_cast<uint8_t>(std::stoi(values[1]));
	}
	return IOModulResult::Ok;;
}

extern "C"
{
	IOModulInterface* create()
	{
		return new PCF8574Modul();
	}
	void destroy(IOModulInterface* obj)
	{
		PCF8574Modul* delObj = reinterpret_cast<PCF8574Modul*>(obj);
		delete delObj;
		delObj = nullptr;
	}
}