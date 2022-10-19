#pragma once
#include <string>
#include <map>
#include <memory>
#include <rapidjson/document.h>
#include <rapidjson/writer.h>
#include <rapidjson/stringbuffer.h>

#include <mi/miutils/Event.h>
#include <mi/miutils/Timer.h>
#include <mi/miio/IOImage.h>
#include "mi/miio/IOModulInterface.h"

namespace miIOManager
{
	typedef enum IOManagerResult_e
	{
		Ok,
		ErrorParameter,
		ErrorLoadDriver,
		ErrorImageSizeExceeded,
		ErrorStart,
		ErrorModulNotFound,
		ErrorModulCtrl,
		ErrorModulWrite,
		ErrorModulRead,
		ErrorConfiguration,
		ErrorModulStart,
		ErrorModulFileNotFound
}IOManagerResult;

class IOModulValue
{
	std::string _Name;
	miIOImage::IOImageOffset _BitOffset;
	miIOImage::IOImageSize _BitSize;
	

public:
	IOModulValue() = default;
	IOModulValue(std::string name, miIOImage::IOImageOffset bitOffset, miIOImage::IOImageSize bitSize)
		:_Name(name)
		,_BitOffset(bitOffset)
		, _BitSize(bitSize)
	{

	}
	const miIOImage::IOImageOffset& Offset() const { return _BitOffset; };
	const miIOImage::IOImageSize& Size() const { return _BitSize; };
	const std::string Name() const { return _Name; };
};

typedef std::shared_ptr<miModul::IOModulInterface> IOModulInterface_p;
typedef std::map<std::string, IOModulValue> IOModulValues;
typedef int16_t IOModulAddress;
typedef int16_t IOModulSubAddress;



class IOModulDescription
{
private:
	std::string _Library;
	std::string _Name;
	std::string _Configuration;
	std::string _DriverSpecific;
	IOModulInterface_p _Interface;
	IOModulValues _IOModulInputValues;
	IOModulValues _IOModulOutputValues;
	miIOImage::IOImageSize _IOModulInputBitSize;
	miIOImage::IOImageSize _IOModulOutputBitSize;

public:
	IOModulDescription() = default;
	IOModulDescription(const std::string& library,
		const std::string& name,
		const std::string& configuration,
		const std::string driverSpecific,
		const IOModulInterface_p& modulInterface,
		const IOModulValues& iValues,
		const IOModulValues& oValues,
		miIOImage::IOImageSize ioModulInputBitSize,
		miIOImage::IOImageSize ioModulOutputBitSize
	)
		:_Library(library)
		, _Name(name)
		, _Configuration(configuration)
		, _DriverSpecific(driverSpecific)
		, _Interface(modulInterface)
		, _IOModulInputValues(iValues)
		, _IOModulOutputValues(oValues)
		,_IOModulInputBitSize(ioModulInputBitSize)
		,_IOModulOutputBitSize(ioModulOutputBitSize)
	{

	}


	const std::string& Library() const { return _Library; };
	const std::string& Name() const { return _Name; };
	const std::string& Configuration() const { return _Configuration; };
	const std::string& DriverSpecific() const { return _DriverSpecific; };
	const IOModulInterface_p& Interface() const { return _Interface; };
	const IOModulValues& IOModulInputValues() const { return _IOModulInputValues; };
	const IOModulValues& IOModulOutputValues() const { return _IOModulOutputValues; };
	const miIOImage::IOImageSize IOModulInputBitSize() const { return _IOModulInputBitSize; };
	const miIOImage::IOImageSize IOModulOutputBitSize() const { return _IOModulOutputBitSize; };
	
};

class IOManager : public miutils::EventListener
{
private:

	const std::string IOModulCreationFunctionName = "CreateIOModulInterface";
	const std::string IOModulDestroyFunctionName = "DestroyIOModulInterface";

	miIOImage::IOImage _InputImage;
	miIOImage::IOImage _OutputImage;
	std::map<std::string,IOModulDescription> _ModulList;
	miutils::Timer _Timer;
	miIOImage::IOImageOffset _ActInputOffset;
	miIOImage::IOImageOffset _ActOutputOffset;
	std::string _IOModulPath;
	IOManagerResult _State;

	IOModulInterface_p LoadModul(const std::string& libraryName,IOManagerResult& result);
	IOManagerResult UnLoadModul(const std::string& libraryName, const IOModulInterface_p& obj);
	IOManagerResult ReadConfig(const std::string& name, miIOImage::IOImageOffset startInputOffset, miIOImage::IOImageOffset startOutputOffset, const std::string& driverspecific);
	IOManagerResult ReadIOModuleValueConfig(const rapidjson::Value& d,IOModulValue& moduleValue);

public:
	IOManager(miIOImage::IOImageSize inputImageSize, miIOImage::IOImageSize outputImageSize, const std::string& ioModulPath);
		
	IOManagerResult AddIOModul(const std::string& name,const std::string& driverspecific);
	IOManagerResult AddIOModul(const miIOImage::IOImageOffset inputOffset, const miIOImage::IOImageOffset outputOffset, const std::string& name, const std::string& driverspecific);
	IOManagerResult RemoveIOModul(const std::string& name);
	IOManagerResult ReadInputs();
	IOManagerResult WriteOutputs();
	IOManagerResult StartIOCycle(int cycleTime);
	IOManagerResult StopIOCycle();
	IOManagerResult StartIOModulCycle();
	IOManagerResult StopIOModulCycle();
	IOManagerResult IOModulControl(const std::string& name,const std::string function,uint32_t parameter);

	const miIOImage::IOImage& OutputImage() const
	{
		return _OutputImage;
	}

	const miIOImage::IOImage& InputImage() const
	{
		return _InputImage;
	}

	const IOManagerResult State() const
	{
		return _State;
	}
	
		

	// Geerbt über EventListener
	virtual void eventOccured(void* sender, const std::string& name) override;

};

}