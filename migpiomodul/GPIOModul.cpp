#include "GPIOModul.h"
#include <rapidjson/document.h>
#include <rapidjson/writer.h>
#include <rapidjson/stringbuffer.h>
#include <string>
#include "GPIODriver.h"
/*
* Modul description format (json)
*
* mimoduldescription
* {
*		name:"gpiomodul"
*       modulconf:
*		{
*			configuration:
*			{
*				(*driver specific*)
*				gpiopins:
*				{
*					pin:
*					{
*						id:0,
*						direction:in,
*						number:24
*					{
*					pin:
*					{
*						id:1,
*						direction:out,
*						number:18
*					{
*				{
*
*			}
*			iomaps:
*			{
*				map:
*				{
*					name: "gpioin1",
*					offset:0,
*					bitsize:1,
*					direction:"in",
*					driverspecific:0,
*				}
*				map:
*				{
*					name: "gpioout1",
*					offset:0,
*					bitsize:1,
*					direction:"out",
*					driverspecific:1
*				}
*			}
*		}
* }
*
*
*/

using namespace miModul;



GPIOModul::GPIOModul()
	:IOModulBase()
{
	
}

IOModulResult miModul::GPIOModul::Init()
{
	miDriver::DriverResults result = miDriver::DriverResults::Ok;
	int32_t iDir = 0;
	for (const auto& n : _PinConfiguration)
	{
		result = _GPIODriver.GpioEnable(n.second.PinNumber(), true);
		if (result != miDriver::DriverResults::Ok)
		{
			_State = IOModulResult::ErrorInit;
		}
		if (n.second.Direction() == GPIOPinDirection::In)
		{
			iDir = 1;
		}
		result = _GPIODriver.GpioSetDirection(n.second.PinNumber(), iDir);
		if (result != miDriver::DriverResults::Ok)
		{
			_GPIODriver.GpioEnable(n.second.PinNumber(), false);
			_State = IOModulResult::ErrorInit;
		}
	}
	return _State;
}



IOModulResult miModul::GPIOModul::Deinit()
{
	for (const auto& n : _PinConfiguration)
	{
		_GPIODriver.GpioEnable(n.second.PinNumber(), false);
	}
	return IOModulResult::Ok;
}


IOModulResult GPIOModul::open(const std::string& configuration)
{
	rapidjson::Document d;
	d.Parse(configuration.c_str());
	if (!d.HasMember("mimoduldescription"))
	{
		return IOModulResult::ErrorConf;
	}
	const rapidjson::Value& mimoduldescription = d["mimoduldescription"];
	if (!mimoduldescription.HasMember("modulconf"))
	{
		return IOModulResult::ErrorConf;
	}
	const rapidjson::Value& modulconf = mimoduldescription["modulconf"];
	if (!modulconf.HasMember("gpiopins"))
	{
		return IOModulResult::ErrorConf;
	}
	const rapidjson::Value& gpiopins = modulconf["gpiopins"];
	if (gpiopins.Size() == 0)
	{
		return IOModulResult::ErrorConf;
	}

	for (rapidjson::SizeType i = 0; i < gpiopins.Size(); i++)
	{
		GPIOPinDirection dir;
		int32_t id = 0;
		int32_t pinNumber = 0;
		const rapidjson::Value& pin = gpiopins[i];

		if (!pin.HasMember("id"))
		{
			return IOModulResult::ErrorConf;
		}
		id = pin["id"].GetInt();

		if (!pin.HasMember("number"))
		{
			return IOModulResult::ErrorConf;
		}
		pinNumber = pin["number"].GetInt();

		if (!pin.HasMember("direction"))
		{
			return IOModulResult::ErrorConf;
		}
		if (std::string(pin["direction"].GetString()) == std::string("in"))
		{
			dir = GPIOPinDirection::In;
		}
		else
		if (std::string(pin["direction"].GetString()) == std::string("out"))
		{
			dir = GPIOPinDirection::In;
		}
		else
		{
			return IOModulResult::ErrorConf;
		}
		GPIOPinConfig conf = GPIOPinConfig(dir, id, pinNumber);
		_PinConfiguration[id] = conf;
	}
	return Init();
}

IOModulResult miModul::GPIOModul::start()
{
	return IOModulResult::ErrorNotImplemented;
}

IOModulResult miModul::GPIOModul::stop()
{
	return IOModulResult::ErrorNotImplemented;
}

IOModulResult miModul::GPIOModul::close()
{
	return Deinit();
}

IOModulResult miModul::GPIOModul::readInputs(const miIOImage::IOImage& image, const IOModulIOMap& map)
{
	bool val = false;
	miDriver::DriverResults result = miDriver::DriverResults::Ok;
	if (_PinConfiguration.count(map.Id()) == 0)
	{
		return IOModulResult::ErrorNotConfigured;
	}
	val = _GPIODriver.GpioRead(_PinConfiguration[map.Id()].PinNumber(), &result);
	if (result != miDriver::DriverResults::Ok)
	{
		return IOModulResult::ErrorRead;
	}
	miIOImage::IOImageResult imageResult = image.WriteBit(map.Offset(), val);
	if (imageResult != miIOImage::IOImageResult::Ok)
	{
		return IOModulResult::ErrorRead;
	}
	return IOModulResult::Ok;
}

IOModulResult miModul::GPIOModul::writeOutputs(const miIOImage::IOImage& image, const IOModulIOMap& map)
{
	bool val = false;
	miDriver::DriverResults result = miDriver::DriverResults::Ok;
	miIOImage::IOImageResult imageResult = miIOImage::IOImageResult::Ok;

	if (_PinConfiguration.count(map.Id()) == 0)
	{
		return IOModulResult::ErrorNotConfigured;
	}
	val = image.ReadBit(map.Offset(), &imageResult);
	if (imageResult != miIOImage::IOImageResult::Ok)
	{
		return IOModulResult::ErrorRead;
	}

	result = _GPIODriver.GpioWrite(_PinConfiguration[map.Id()].PinNumber(),val);
	if (result != miDriver::DriverResults::Ok)
	{
		return IOModulResult::ErrorRead;
	}
	
	return IOModulResult::Ok;
}

IOModulResult miModul::GPIOModul::control(const std::string name, const std::string function, uint32_t parameter)
{
	return IOModulResult::ErrorNotImplemented;
}


extern "C"
{
	IOModulInterface* create()
	{
		return new GPIOModul();
	}
	void destroy(IOModulInterface* obj)
	{
		delete obj;
	}
}