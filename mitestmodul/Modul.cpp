#include "Modul.h"
#include <rapidjson/document.h>
#include <rapidjson/writer.h>
#include <rapidjson/stringbuffer.h>
#include <string>



using namespace miModul;



Modul::Modul()
	:IOModulBase()
{
	
}

miModul::Modul::~Modul()
{
}

IOModulResult miModul::Modul::Init()
{
	
	return IOModulResult::Ok;
}



IOModulResult miModul::Modul::Deinit()
{
	
	return IOModulResult::Ok;
}


IOModulResult Modul::Open(const std::string& descriptionFile, const std::string& configuration)
{
	rapidjson::Document d;
	d.Parse(configuration.c_str());
	if (!d.HasMember("mimoduldescription"))
	{
		return IOModulResult::ErrorConf;
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
	
	if (!modulconf.HasMember("items"))
	{
		return IOModulResult::ErrorConf;
	}
	const rapidjson::Value& items = modulconf["items"];
	if (items.Size() == 0)
	{
		return IOModulResult::ErrorConf;
	}

	for (rapidjson::SizeType i = 0; i < items.Size(); i++)
	{
		ModulDirection dir;
		int32_t id = 0;
		int32_t bitOffset = 0;
		int32_t bitSize = 0;
		int32_t pinNumber = 0;
		const rapidjson::Value& pin = items[i];

		if (!pin.HasMember("id"))
		{
			return IOModulResult::ErrorConf;
		}
		id = pin["id"].GetInt();

		if (!pin.HasMember("bitoffset"))
		{
			return IOModulResult::ErrorConf;
		}
		bitOffset = pin["bitoffset"].GetInt();

		if (!pin.HasMember("bitsize"))
		{
			return IOModulResult::ErrorConf;
		}
		bitSize = pin["bitsize"].GetInt();

		if (!pin.HasMember("direction"))
		{
			return IOModulResult::ErrorConf;
		}
		if (std::string(pin["direction"].GetString()) == std::string("in"))
		{
			dir = ModulDirection::In;
		}
		else
		if (std::string(pin["direction"].GetString()) == std::string("out"))
		{
			dir = ModulDirection::Out;
		}
		else
		{
			return IOModulResult::ErrorConf;
		}
		ModulConfig conf = ModulConfig(dir, id, pinNumber, bitOffset, bitSize);
		_ModulConfig[id] = conf;
		
	}
	return Init();
}

IOModulResult miModul::Modul::Start()
{
	return IOModulResult::ErrorNotImplemented;
}

IOModulResult miModul::Modul::Stop()
{
	return IOModulResult::ErrorNotImplemented;
}

IOModulResult miModul::Modul::Close()
{
	return Deinit();
}

IOModulResult miModul::Modul::ReadInputs(const miIOImage::IOImage& image, const IOModulValue& map)
{
	bool val = false;
	miIOImage::IOImageResult imageResult  = miIOImage::IOImageResult::Ok;
	
	if (map.Size() == 1)
	{
		int byteOffset = map.Offset() / 8;
		int bitOfset = map.Offset() % 8;
		val = false;
		if ((data[byteOffset] & (1 << bitOfset)) > 0)
		{
			val = true;
		}
		
		imageResult = image.WriteBit(map.Offset(), val);
	}
	else
	{
		imageResult = image.Write(map.Offset() / 8, &data[map.Offset() / 8], map.Size());
	}

	if (imageResult != miIOImage::IOImageResult::Ok)
	{
		return IOModulResult::ErrorRead;
	}
	return IOModulResult::Ok;
}

IOModulResult miModul::Modul::WriteOutputs(const miIOImage::IOImage& image, const IOModulValue& map)
{
	bool val = false;
	int byteOffset = map.Offset() / 8;
	int bitOffset = map.Offset() % 8;
	miIOImage::IOImageResult imageResult = miIOImage::IOImageResult::Ok;

	if (map.Size() == 1)
	{
		val = image.ReadBit(map.Offset(), imageResult);
		if (val)
		{
			data[byteOffset] |= static_cast<uint8_t>(1 << bitOffset);
		}
		else
		{
			data[byteOffset] &= static_cast<uint8_t>(~(1 << bitOffset));
		}
	}
	else
	{
		image.Read(byteOffset, &data[byteOffset], map.Size());
	}
	if (imageResult != miIOImage::IOImageResult::Ok)
	{
		return IOModulResult::ErrorRead;
	}
	return IOModulResult::Ok;
}

IOModulResult miModul::Modul::Control(const std::string name, const std::string function, uint32_t parameter)
{
	return IOModulResult::ErrorNotImplemented;
}


extern "C"
{
	IOModulInterface* create()
	{
		return new Modul();
	}
	void destroy(IOModulInterface* obj)
	{
		Modul* delObj = reinterpret_cast<Modul*>(obj);
		delete delObj;
		delObj = nullptr;
	}
}