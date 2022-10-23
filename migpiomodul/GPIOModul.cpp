#include "GPIOModul.h"
#include <rapidjson/writer.h>
#include <rapidjson/stringbuffer.h>
#include <string>
#include <iostream>
#include <fstream>

using namespace miModul;

miModul::GPIOModul::GPIOModul()
	:IOModulBase()
{
	_Name = std::string("migpiomodul");
}

miModul::GPIOModul::~GPIOModul()
{
}

IOModulResult miModul::GPIOModul::Init()
{
	miDriver::DriverResults result = miDriver::DriverResults::Ok;
	_State = IOModulResult::Ok;
	for (const auto& n : _PinInConfiguration)
	{
		result = _GPIODriver.GpioEnable(n.second.GpioNumber(), true);
		if (result != miDriver::DriverResults::Ok)
		{
			_State = IOModulResult::ErrorInit;
		}
		result = _GPIODriver.GpioSetDirection(n.second.GpioNumber(), 1);
		if (result != miDriver::DriverResults::Ok)
		{
			_GPIODriver.GpioEnable(n.second.GpioNumber(), false);
			_State = IOModulResult::ErrorInit;
		}
	}
	for (const auto& n : _PinOutConfiguration)
	{
		result = _GPIODriver.GpioEnable(n.second.GpioNumber(), true);
		if (result != miDriver::DriverResults::Ok)
		{
			_State = IOModulResult::ErrorInit;
		}
		result = _GPIODriver.GpioSetDirection(n.second.GpioNumber(), 0);
		if (result != miDriver::DriverResults::Ok)
		{
			_GPIODriver.GpioEnable(n.second.GpioNumber(), false);
			_State = IOModulResult::ErrorInit;
		}
	}
	return _State;
}

IOModulResult miModul::GPIOModul::Deinit()
{
	for (const auto& n : _PinInConfiguration)
	{
		_GPIODriver.GpioEnable(n.second.GpioNumber(), false);
	}
	for (const auto& n : _PinOutConfiguration)
	{
		_GPIODriver.GpioEnable(n.second.GpioNumber(), false);
	}
	return IOModulResult::Ok;
}

IOModulResult miModul::GPIOModul::ReadDriverSpecificInputConfig(const rapidjson::Value& item)
{
	GPIOPinConfig config;
	IOModulResult result = GetGpioConfig(item, config);
	if (result != IOModulResult::Ok)
	{
		_State = result;
		return _State;
	}
	_PinInConfiguration[config.BitOffset()] = config;
	_State = IOModulResult::Ok;
	return _State;
}

IOModulResult miModul::GPIOModul::ReadDriverSpecificOutputConfig(const rapidjson::Value& item)
{
	GPIOPinConfig config;
	IOModulResult result = GetGpioConfig(item, config);
	if (result != IOModulResult::Ok)
	{
		_State = result;
		return _State;
	}
	_PinOutConfiguration[config.BitOffset()] = config;
	_State = IOModulResult::Ok;
	return _State;
}

IOModulResult GPIOModul::Open(const std::string& descriptionFile,const std::string& driverspecific)
{
	_State = ReadModulConfiguration(descriptionFile);
	if (_State != IOModulResult::Ok)
	{
		std::cerr << __PRETTY_FUNCTION__ << " :Read description file failed. Errornumber " << _State << std::endl;
		return _State;
	}
	_State = Init();
	return _State;
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

IOModulResult miModul::GPIOModul::ReadInputs(const miIOImage::IOImage& image, const miIOImage::IOImageOffset bitOffset, const miIOImage::IOImageSize bitSize)
{
	bool val = false;
	miDriver::DriverResults result = miDriver::DriverResults::Ok;
	
	val = _GPIODriver.GpioRead(_PinInConfiguration[bitOffset].GpioNumber(), &result);
	if (result != miDriver::DriverResults::Ok)
	{
		_State = IOModulResult::ErrorRead;
		return _State;
	}
	miIOImage::IOImageResult imageResult = image.WriteBit(bitOffset, val);
	if (imageResult != miIOImage::IOImageResult::Ok)
	{
		_State = IOModulResult::ErrorRead;
		return IOModulResult::ErrorRead;
	}
	return IOModulResult::Ok;
}

IOModulResult miModul::GPIOModul::WriteOutputs(const miIOImage::IOImage& image, const miIOImage::IOImageOffset bitOffset, const miIOImage::IOImageSize bitSize)
{
	bool val = false;
	miDriver::DriverResults result = miDriver::DriverResults::Ok;
	miIOImage::IOImageResult imageResult = miIOImage::IOImageResult::Ok;

	
	val = image.ReadBit(bitOffset, imageResult);
	if (imageResult != miIOImage::IOImageResult::Ok)
	{
		_State = IOModulResult::ErrorWrite;
		return _State;
	}

	result = _GPIODriver.GpioWrite(_PinOutConfiguration[bitOffset].GpioNumber(),val);
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

IOModulResult miModul::GPIOModul::GetGpioConfig(const rapidjson::Value& item, GPIOPinConfig& config)
{

		int32_t gpioNumber = 0;
		miIOImage::IOImageOffset bitOffset = 0;
		
		if (!item.HasMember("bitoffset"))
		{
			return IOModulResult::ErrorConf;
		}
		bitOffset = item["bitoffset"].GetInt();

		if (!item.HasMember("driverspecific"))
		{
			return IOModulResult::ErrorConf;
		}
		const rapidjson::Value& driverspecific = item["driverspecific"];

		if (!driverspecific.HasMember("gpionumber"))
		{
			return IOModulResult::ErrorConf;
		}
		gpioNumber = driverspecific["gpionumber"].GetInt();

		config = GPIOPinConfig(bitOffset,gpioNumber);
		return IOModulResult();
}

extern "C"
{
	IOModulInterface* CreateIOModulInterface()
	{
		return new GPIOModul();
	}

	void DestroyIOModulInterface(IOModulInterface* obj)
	{
		GPIOModul* delObj = reinterpret_cast<GPIOModul*>(obj);
		delete delObj;
		delObj = nullptr;
	}
}