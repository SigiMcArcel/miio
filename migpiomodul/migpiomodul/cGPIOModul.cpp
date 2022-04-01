#include "cGPIOModul.h"
#include <rapidjson/document.h>
#include <rapidjson/writer.h>
#include <rapidjson/stringbuffer.h>

using namespace miModul;

miModul::cGPIOModul::cGPIOModul(ModulIOConfiguration configuration)
	:cModulBase(configuration)
	
{
	IOImageSize_t inputSize = 0;
	IOImageSize_t outputSize = 0;
	rapidjson::Document d;
	d.Parse(_Configuration.c_str());

	if (!d.HasMember("gpiomodul"))
	{
		_State = Modulresult::ErrorConf;
	}
	const rapidjson::Value& gpiomodul = d["gpiomodul"];

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

		if (!iomap.HasMember("bitsize"))
		{
			_State = Modulresult::ErrorConf;
			return;
		}
		entry.BitSize = iomap["bitsize"].GetUint();
		if (entry.BitSize != 1)
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
				inputSize += entry.BitSize;
				entry.Direction = ModulIOMapDirections::In;
			}
			else
			if (strcmp(dir, "out"))
			{
				outputSize += entry.BitSize;
				entry.Direction = ModulIOMapDirections::Out;
			}
		}
		
		if (!iomap.HasMember("gpionumber"))
		{
			_State = Modulresult::ErrorConf;
			return;
		}
		entry.SubAddress = iomap["gpionumber"].GetUint();
		_IOMap.push_back(entry);
	}

	//Create inputMaping
	_IOImageInput = IOImage(inputSize, IOImageType::Input);
	_IOImageInput = IOImage(outputSize, IOImageType::Output);

}

Modulresult miModul::cGPIOModul::Init()
{
	return Modulresult();
}

Modulresult miModul::cGPIOModul::Read()
{
	return Modulresult();
}

Modulresult miModul::cGPIOModul::Write()
{
	return Modulresult();
}

Modulresult miModul::cGPIOModul::Deinit()
{
	return Modulresult();
}
