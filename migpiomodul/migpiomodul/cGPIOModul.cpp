#include "cGPIOModul.h"
#include <rapidjson/document.h>
#include <rapidjson/writer.h>
#include <rapidjson/stringbuffer.h>
#include <memory>
#include <string>
#include <stdexcept>
#include <sys/stat.h>

template<typename ... Args>
std::string string_format(const std::string& format, Args ... args)
{
	int size_s = std::snprintf(nullptr, 0, format.c_str(), args ...) + 1; // Extra space for '\0'
	if (size_s <= 0) { throw std::runtime_error("Error during formatting."); }
	auto size = static_cast<size_t>(size_s);
	std::unique_ptr<char[]> buf(new char[size]);
	std::snprintf(buf.get(), size, format.c_str(), args ...);
	return std::string(buf.get(), buf.get() + size - 1); // We don't want the '\0' inside
}

using namespace miModul;

Modulresult miModul::cGPIOModul::GpioEnable(int32_t pin, bool enable)
{
	std::fstream stream;
	std::string strPin = std::to_string(pin).c_str();
	stream.open(gpioPath + std::string("export"), std::fstream::in | std::fstream::out);
	if (stream.fail())
	{
		return Modulresult::ErrorInit;
	}
	stream.write(strPin.c_str(),strPin.size());
	if (stream.fail())
	{
		stream.close();
		return Modulresult::ErrorInit;
	}
	stream.close();
	return Modulresult::Ok;
}

Modulresult miModul::cGPIOModul::GpioSetDirection(int32_t pin, ModulIOMapDirections direction)
{
	std::fstream stream;
	std::string strCmd = string_format("%s/gpio%d/direction", gpioPath, pin);
	std::string strVal = "in";
	if (direction == ModulIOMapDirections::Out)
	{
		strVal = "out";
	}

	stream.open(strCmd.c_str(), std::fstream::in | std::fstream::out);
	if (stream.fail())
	{
		return Modulresult::ErrorInit;
	}
	stream.write(strVal.c_str(), strVal.size());
	if (stream.fail())
	{
		stream.close();
		return Modulresult::ErrorInit;
	}
	stream.close();
	return Modulresult::Ok;
}

bool miModul::cGPIOModul::GpioRead(int32_t pin, Modulresult* result)
{
	std::fstream stream;
	std::string strCmd = string_format("%s/gpio%d/value", gpioPath, pin);
	std::string strVal = "";
	int length = 0;
	struct stat statbuffer;

	stream.open(strCmd.c_str(), std::fstream::in | std::fstream::out);
	if (stream.fail())
	{
		if(result!= nullptr)
			*result = Modulresult::ErrorInit;
	}
	//std::filesystem::file_size(strCmd)
	::stat(strCmd.c_str(), &statbuffer);
	length = statbuffer.st_size;
	
	
	char* buffer = new char[length];
	stream.read(buffer, length);
	if (stream.fail())
	{
		stream.close();
		if (result != nullptr)
			*result = Modulresult::ErrorInit;
	}
	stream.close();
	if (buffer[0] == '1')
	{
		return true;
	}
	return false;
}

Modulresult miModul::cGPIOModul::GpioWrite(int32_t pin, bool value)
{
	std::fstream stream;
	std::string strCmd = string_format("%s/gpio%d/value", gpioPath, pin);
	std::string strVal = "0";
	if (value)
	{
		strVal = "1";
	}

	stream.open(strCmd.c_str(), std::fstream::in | std::fstream::out);
	if (stream.fail())
	{
		return Modulresult::ErrorInit;
	}
	stream.write(strVal.c_str(), strVal.size());
	if (stream.fail())
	{
		stream.close();
		return Modulresult::ErrorInit;
	}
	stream.close();
	return Modulresult::Ok;
}

miModul::cGPIOModul::cGPIOModul(ModulIOConfiguration configuration)
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
				entry.IoImageBitOffset = inputSize;
				inputSize += entry.BitSize;
				entry.Direction = ModulIOMapDirections::In;
			}
			else
			if (strcmp(dir, "out"))
			{
				entry.IoImageBitOffset = outputSize;
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
	_IOImageOutput = IOImage(outputSize, IOImageType::Output);

}

Modulresult miModul::cGPIOModul::Init()
{
	Modulresult result = Modulresult::Ok;
	for (const ModulIOMapEntry& map : _IOMap)
	{
		result = GpioEnable(map.SubAddress, true);
		if (result != Modulresult::Ok)
		{
			return result;
		}
		result = GpioSetDirection(map.SubAddress, map.Direction);
		if (result != Modulresult::Ok)
		{
			GpioEnable(map.SubAddress, false);
			return result;
		}
	}
	_State = result;
	return _State;
}

Modulresult miModul::cGPIOModul::ReadInputToIOImage()
{
	Modulresult result = Modulresult::Ok;
	bool value = false;
	for (const ModulIOMapEntry& map : _IOMap)
	{
		if (map.Direction != ModulIOMapDirections::In)
		{
			value = GpioRead(map.SubAddress, &result);
			if (result != Modulresult::Ok)
			{
				_IOImageInput.setValueError(IOImageResult::InvalidValue, map.IoImageBitOffset);
				result = Modulresult::ErrorRead;
				continue;
			}
			_IOImageInput.WriteBit(map.IoImageBitOffset, value);
		}
	}
	_State = result;
	return _State;
}

Modulresult miModul::cGPIOModul::WriteOutputFromImage()
{
	Modulresult result = Modulresult::Ok;
	bool value = false;
	for (const ModulIOMapEntry& map : _IOMap)
	{
		if (map.Direction != ModulIOMapDirections::Out)
		{
			value = _IOImageOutput.ReadBit(map.IoImageBitOffset);
			result = GpioWrite(map.SubAddress, value);
			if (result != Modulresult::Ok)
			{
				_IOImageInput.setValueError(IOImageResult::InvalidValue, map.IoImageBitOffset);
				result = Modulresult::ErrorRead;
				continue;
			}
			_IOImageInput.WriteBit(map.IoImageBitOffset, value);
		}
	}
	_State = result;
	return _State;
}

Modulresult miModul::cGPIOModul::Deinit()
{
	for (const ModulIOMapEntry& map : _IOMap)
	{
		GpioEnable(map.SubAddress, false);
	}
	return Modulresult::Ok;
}
