#include "GPIOModul.h"
#include <rapidjson/document.h>
#include <rapidjson/writer.h>
#include <rapidjson/stringbuffer.h>
#include <string>
#include "GPIODriver.h"


using namespace miModul;

miModul::GPIOModul::GPIOModul()
	:IOModulBase()
{
	_Name = std::string("GPIOModul");
}

miModul::GPIOModul::~GPIOModul()
{
}

IOModulResult miModul::GPIOModul::Init()
{
	miDriver::DriverResults result = miDriver::DriverResults::Ok;
	int32_t iDir = 0;
	_State = IOModulResult::Ok;
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


IOModulResult GPIOModul::Open(const std::string& configuration)
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
	
	if (!modulconf.HasMember("gpiopins"))
	{
		_State = IOModulResult::ErrorConf;
		return _State;
	}
	const rapidjson::Value& gpiopins = modulconf["gpiopins"];
	if (gpiopins.Size() == 0)
	{
		_State = IOModulResult::ErrorConf;
		return _State;
	}

	for (rapidjson::SizeType i = 0; i < gpiopins.Size(); i++)
	{
		GPIOPinDirection dir;
		int32_t id = 0;
		int32_t pinNumber = 0;
		const rapidjson::Value& pin = gpiopins[i];

		if (!pin.HasMember("id"))
		{
			_State = IOModulResult::ErrorConf;
			return _State;
		}
		id = pin["id"].GetInt();

		if (!pin.HasMember("number"))
		{
			_State = IOModulResult::ErrorConf;
			return _State;
		}
		pinNumber = pin["number"].GetInt();

		if (!pin.HasMember("direction"))
		{
			_State = IOModulResult::ErrorConf;
			return _State;
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
			_State = IOModulResult::ErrorConf;
			return _State;
		}
		GPIOPinConfig conf = GPIOPinConfig(dir, id, pinNumber);
		_PinConfiguration[id] = conf;
	}
	return IOModulResult::Ok;
}

IOModulResult miModul::GPIOModul::Start()
{
	return IOModulResult::ErrorNotImplemented;
}

IOModulResult miModul::GPIOModul::Stop()
{
	return IOModulResult::ErrorNotImplemented;
}

IOModulResult miModul::GPIOModul::Close()
{
	return Deinit();
}

IOModulResult miModul::GPIOModul::ReadInputs(const miIOImage::IOImage& image, const IOModulIOMap& map)
{
	bool val = false;
	miDriver::DriverResults result = miDriver::DriverResults::Ok;
	if (_PinConfiguration.count(map.Id()) == 0)
	{
		_State = IOModulResult::ErrorNotConfigured;
		return _State;
	}
	val = _GPIODriver.GpioRead(_PinConfiguration[map.Id()].PinNumber(), &result);
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

IOModulResult miModul::GPIOModul::WriteOutputs(const miIOImage::IOImage& image, const IOModulIOMap& map)
{
	bool val = false;
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

	result = _GPIODriver.GpioWrite(_PinConfiguration[map.Id()].PinNumber(),val);
	if (result != miDriver::DriverResults::Ok)
	{
		_State = IOModulResult::ErrorWrite;
		return _State;
	}
	
	return IOModulResult::Ok;
}

IOModulResult miModul::GPIOModul::Control(const std::string name, const std::string function, uint32_t parameter)
{
	_State = IOModulResult::ErrorNotImplemented;
	return _State;
}


extern "C"
{
	IOModulInterface* create()
	{
		return new GPIOModul();
	}
	void destroy(IOModulInterface* obj)
	{
		GPIOModul* delObj = reinterpret_cast<GPIOModul*>(obj);
		delete delObj;
		delObj = nullptr;
	}
}