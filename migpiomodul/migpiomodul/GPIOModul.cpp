#include "GPIOModul.h"
#include <rapidjson/document.h>
#include <rapidjson/writer.h>
#include <rapidjson/stringbuffer.h>
#include <string>
#include "GPIODriver.h"



using namespace miModul;



miModul::GPIOModul::GPIOModul(ModulIOConfiguration configuration)
	:cModulBase(configuration)
	
{
	IOImageSize inputSize = 0;
	IOImageSize outputSize = 0;
	rapidjson::Document d;
	d.Parse(_Configuration.c_str());

	if (!d.HasMember("gpiomodul"))
	{
		_State = Modulresult::ErrorConf;
	}
	const rapidjson::Value& gpiomodul = d["gpiomodul"];

	if (!d.HasMember("name"))
	{
		_State = Modulresult::ErrorConf;
		return;
	}
	_Name = gpiomodul["name"].GetString();

	if (!d.HasMember("iomaps"))
	{
		_State = Modulresult::ErrorConf;
		return;
	}
	const rapidjson::Value& iomaps = gpiomodul["iomaps"];

	if (iomaps.Size() == 0)
	{
		_State = Modulresult::ErrorConf;
		return;
	}

	for (rapidjson::SizeType i = 0; i < iomaps.Size(); i++)
	{
		ModulIOMapEntry entry;
		const char* dir = nullptr;
		const rapidjson::Value& iomap = iomaps[i];

		if (!iomap.HasMember("name"))
		{
			_State = Modulresult::ErrorConf;
			return;
		}
		entry.Name(iomap["direction"].GetString());

		if (!iomap.HasMember("bitsize"))
		{
			_State = Modulresult::ErrorConf;
			return;
		}
		entry.BitSize(iomap["bitsize"].GetUint());
		if (entry.BitSize() != 1)
		{
			_State = Modulresult::ErrorConf;
			return;
		}

		if (!iomap.HasMember("direction"))
		{
			_State = Modulresult::ErrorConf;
			return;
		}
		dir = iomap["direction"].GetString();
		if (dir != nullptr)
		{
			if (strcmp(dir, "in"))
			{
				entry.Direction(ModulIOMapDirections::In);
			}
			else
			if (strcmp(dir, "out"))
			{
				entry.Direction(ModulIOMapDirections::Out);
			}
		}
		
		if (!iomap.HasMember("gpionumber"))
		{
			_State = Modulresult::ErrorConf;
			return;
		}
		entry.SubAddress(iomap["gpionumber"].GetUint());
		//_IOMap.insert(entry.Name(),entry);
	}

	//Create inputMaping
	_IOImageInput = IOImage(inputSize, IOImageType::Input);
	_IOImageOutput = IOImage(outputSize, IOImageType::Output);

}

Modulresult miModul::GPIOModul::Init()
{
	miDriver::DriverResults result = miDriver::DriverResults::Ok;
	int32_t iDir = 0;
	for (const auto& n : _IOMap)
	{
		result = _GPIODriver.GpioEnable(n.second.SubAddress(), true);
		if (result != miDriver::DriverResults::Ok)
		{
			_State = Modulresult::ErrorInit;
		}
		if (n.second.Direction() == ModulIOMapDirections::In)
		{
			iDir = 1;
		}
		result = _GPIODriver.GpioSetDirection(n.second.SubAddress(), iDir);
		if (result != miDriver::DriverResults::Ok)
		{
			_GPIODriver.GpioEnable(n.second.SubAddress(), false);
			_State = Modulresult::ErrorInit;
		}
	}
	return _State;
}

Modulresult miModul::GPIOModul::ReadInputToIOImage()
{
	miDriver::DriverResults result = miDriver::DriverResults::Ok;
	bool value = false;
	for (const auto& n : _IOMap)
	{
		if (n.second.Direction() == ModulIOMapDirections::In)
		{
			value = _GPIODriver.GpioRead(n.second.SubAddress(), &result);
			if (result != miDriver::DriverResults::Ok)
			{
				//_IOImageInput.setValueError(IOImageResult::InvalidValue, map.IoImageBitOffset);
				_State = Modulresult::ErrorRead;
				continue;
			}
			//_IOImageInput.WriteBit(map.IoImageBitOffset, value);
		}
	}

	return _State;
}

Modulresult miModul::GPIOModul::WriteOutputFromIOImage()
{
	miDriver::DriverResults result = miDriver::DriverResults::Ok;
	bool value = false;
	
	for (const auto& n : _IOMap)
	{
		//value = _IOImageOutput.ReadBit(n.second.IoImageBitOffset);
		result = _GPIODriver.GpioWrite(n.second.SubAddress(), value);
		if (result != miDriver::DriverResults::Ok)
		{
			//_IOImageInput.setValueError(IOImageResult::InvalidValue, n.second.IoImageBitOffset);
			_State = Modulresult::ErrorRead;
			continue;
		}
		//_IOImageInput.WriteBit(map.IoImageBitOffset, value);
	}
	
	return _State;
}

Modulresult miModul::GPIOModul::Deinit()
{
	for (const auto& n : _IOMap)
	{
		_GPIODriver.GpioEnable(n.second.SubAddress(), false);
	}
	return Modulresult::Ok;
}
