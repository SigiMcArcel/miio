#include "PCF8574Modul.h"
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

IOModulResult PCF8574Modul::Open(const std::string& configuration, const std::string& driverspecific)
{
	rapidjson::Document d;
	d.Parse(configuration.c_str());
	if (!d.HasMember("mimoduldescription"))
	{
		_State = IOModulResult::ErrorConf;
		return _State;
	}
	const rapidjson::Value& mimoduldescription = d["mimoduldescription"];

	if (!mimoduldescription.HasMember("name"))
	{
		_State = IOModulResult::ErrorConf;
		return _State;
	}
	const rapidjson::Value& name = mimoduldescription["name"];
	if (std::string(name.GetString()) != _Name)
	{
		_State = IOModulResult::ErrorDescriptionNotMatch;
		return _State;
	}

	if (!mimoduldescription.HasMember("modulconf"))
	{
		return IOModulResult::ErrorConf;
	}
	const rapidjson::Value& modulconf = mimoduldescription["modulconf"];

	if (!modulconf.HasMember("ios"))
	{
		_State = IOModulResult::ErrorConf;
		return _State;
	}
	const rapidjson::Value& ios = modulconf["ios"];
	if (ios.Size() == 0)
	{
		_State = IOModulResult::ErrorConf;
		return _State;
	}
	

	for (rapidjson::SizeType i = 0; i < ios.Size(); i++)
	{
		PCFDirection dir;
		int32_t id = 0;
		int32_t bit = 0;
		const rapidjson::Value& pin = ios[i];

		if (!pin.HasMember("id"))
		{
			_State = IOModulResult::ErrorConf;
			return _State;
		}
		id = pin["id"].GetInt();

		if (!pin.HasMember("direction"))
		{
			_State = IOModulResult::ErrorConf;
			return _State;
		}
		if (std::string(pin["direction"].GetString()) == std::string("in"))
		{
			dir = PCFDirection::In;
		}
		else if (std::string(pin["direction"].GetString()) == std::string("out"))
		{
			dir = PCFDirection::Out;
		}
		else
		{
			_State = IOModulResult::ErrorConf;
			return _State;
		}

		if (!pin.HasMember("bit"))
		{
			_State = IOModulResult::ErrorConf;
			return _State;
		}
		bit = pin["bit"].GetInt();
		PCFPinConfig conf = PCFPinConfig(dir, id, bit);
		_PinConfiguration[id] = conf;
	}
	ResolveDriverSpecific(driverspecific);
	Init();
	return IOModulResult::Ok;
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

IOModulResult miModul::PCF8574Modul::ReadInputs(const miIOImage::IOImage& image, const IOModulIOMap& map)
{
	uint8_t val = false;
	miDriver::DriverResults result = miDriver::DriverResults::Ok;
	if (_PinConfiguration.count(map.Id()) == 0)
	{
		_State = IOModulResult::ErrorNotConfigured;
		return _State;
	}
	result = _I2CDriver.I2CRead(0, 1, &val);
	if (result != miDriver::DriverResults::Ok)
	{
		_State = IOModulResult::ErrorRead;
		return _State;
	}
	miIOImage::IOImageResult imageResult = image.WriteBit(map.Offset(), val);
	if (imageResult != miIOImage::IOImageResult::Ok)
	{
		_State = IOModulResult::ErrorRead;
		return IOModulResult::ErrorRead;
	}
	return IOModulResult::Ok;
}

IOModulResult miModul::PCF8574Modul::WriteOutputs(const miIOImage::IOImage& image, const IOModulIOMap& map)
{
	uint8_t val = false;
	miDriver::DriverResults result = miDriver::DriverResults::Ok;
	miIOImage::IOImageResult imageResult = miIOImage::IOImageResult::Ok;

	if (_PinConfiguration.count(map.Id()) == 0)
	{
		_State = IOModulResult::ErrorNotConfigured;
		return _State;
	}
	val = image.ReadBit(map.Offset(), imageResult);
	if (imageResult != miIOImage::IOImageResult::Ok)
	{
		_State = IOModulResult::ErrorWrite;
		return _State;
	}

	result = _I2CDriver.I2CWrite(0, 1, &val);
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