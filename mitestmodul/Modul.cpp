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

IOModulResult miModul::Modul::Init()
{
	
	return IOModulResult::Ok;
}



IOModulResult miModul::Modul::Deinit()
{
	
	return IOModulResult::Ok;
}


IOModulResult Modul::open(const std::string& configuration)
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
		return  IOModulResult::ErrorConf;
	}
	const rapidjson::Value& name = mimoduldescription["name"];

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
		Direction dir;
		int32_t id = 0;
		int32_t offset = 0;
		int32_t bitsize = 0;
		const rapidjson::Value& pin = items[i];

		if (!pin.HasMember("id"))
		{
			return IOModulResult::ErrorConf;
		}
		id = pin["id"].GetInt();

		if (!pin.HasMember("offset"))
		{
			return IOModulResult::ErrorConf;
		}
		offset = pin["offset"].GetInt();

		if (!pin.HasMember("direction"))
		{
			return IOModulResult::ErrorConf;
		}
		if (std::string(pin["direction"].GetString()) == std::string("in"))
		{
			dir = Direction::In;
		}
		else
		if (std::string(pin["direction"].GetString()) == std::string("out"))
		{
			dir = Direction::In;
		}
		else
		{
			return IOModulResult::ErrorConf;
		}
		
	}
	return Init();
}

IOModulResult miModul::Modul::start()
{
	return IOModulResult::ErrorNotImplemented;
}

IOModulResult miModul::Modul::stop()
{
	return IOModulResult::ErrorNotImplemented;
}

IOModulResult miModul::Modul::close()
{
	return Deinit();
}

IOModulResult miModul::Modul::readInputs(const miIOImage::IOImage& image, const IOModulIOMap& map)
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

IOModulResult miModul::Modul::writeOutputs(const miIOImage::IOImage& image, const IOModulIOMap& map)
{
	bool val = false;
	int byteOffset = map.Offset() / 8;
	int bitOfset = map.Offset() % 8;
	miIOImage::IOImageResult imageResult = miIOImage::IOImageResult::Ok;

	if (map.Size() == 1)
	{
		val = image.ReadBit(map.Offset(), imageResult);
		if (val)
		{
			data[byteOffset] |= (1 << bitOfset);
		}
		else
		{
			data[byteOffset] &= ~(1 << bitOfset);
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

IOModulResult miModul::Modul::control(const std::string name, const std::string function, uint32_t parameter)
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
		delete obj;
	}
}