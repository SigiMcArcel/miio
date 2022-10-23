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
#include <mi/miio/IOModul.h>

namespace miIOManager
{

	typedef enum class IOManagerResult_e
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

typedef std::map<std::string, IOModulValue> IOModulValues;
typedef int16_t IOModulAddress;
typedef int16_t IOModulSubAddress;



class IOModulDescription
{
private:
	std::string _Library;
	std::string _DescriptionName;
	std::string _InstanceName;
	std::string _DriverSpecific;
	miModul::IOModul _IOModul;
	IOModulValues _IOModulInputValues;
	IOModulValues _IOModulOutputValues;
	miIOImage::IOImageSize _IOModulInputBitSize;
	miIOImage::IOImageSize _IOModulOutputBitSize;

public:
	IOModulDescription() = default;
	IOModulDescription(
		const std::string& library,
		const std::string& descriptionName,
		const std::string& instanceName,
		const std::string driverSpecific,
		const miModul::IOModul& ioModul,
		const IOModulValues& iValues,
		const IOModulValues& oValues,
		miIOImage::IOImageSize ioModulInputBitSize,
		miIOImage::IOImageSize ioModulOutputBitSize
	)
		:_Library(library)
		, _DescriptionName(descriptionName)
		, _InstanceName(instanceName)
		, _DriverSpecific(driverSpecific)
		, _IOModul(ioModul)
		, _IOModulInputValues(iValues)
		, _IOModulOutputValues(oValues)
		,_IOModulInputBitSize(ioModulInputBitSize)
		,_IOModulOutputBitSize(ioModulOutputBitSize)
	{

	}


	const std::string& Library() const { return _Library; };
	const std::string& DescriptionName() const { return _DescriptionName; };
	const std::string& InstanceName() const { return _InstanceName; };
	const std::string& DriverSpecific() const { return _DriverSpecific; };
	const miModul::IOModul& IOModul() const { return _IOModul; };
	const IOModulValues& IOModulInputValues() const { return _IOModulInputValues; };
	const IOModulValues& IOModulOutputValues() const { return _IOModulOutputValues; };
	const miIOImage::IOImageSize IOModulInputBitSize() const { return _IOModulInputBitSize; };
	const miIOImage::IOImageSize IOModulOutputBitSize() const { return _IOModulOutputBitSize; };
	
};

class IOManager : public miutils::EventListener
{
private:

	const std::string  _IOModulDescriptionPath = "/etc/mi/iomodules";
	miIOImage::IOImage _InputImage;
	miIOImage::IOImage _OutputImage;
	std::map<std::string,IOModulDescription> _ModulList;
	miutils::Timer _Timer;
	miIOImage::IOImageOffset _ActInputOffset;
	miIOImage::IOImageOffset _ActOutputOffset;
	std::string _IOModulPath;
	IOManagerResult _State;

	
	IOManagerResult ReadConfig(
		const std::string& descriptionName
		,miIOImage::IOImageOffset startInputOffset
		,miIOImage::IOImageOffset startOutputOffset
		,const std::string& driverspecific
		,IOModulDescription& desc);
	IOManagerResult ReadIOModuleValueConfig(const rapidjson::Value& d,IOModulValue& moduleValue);

public:
	IOManager(miIOImage::IOImageSize inputImageSize, miIOImage::IOImageSize outputImageSize);
		
	IOManagerResult AddIOModul(
		const std::string& instanceName
		,const std::string& descriptionName
		,const std::string& driverspecific);
	IOManagerResult AddIOModul(
		const std::string& instanceName
		,const std::string& descriptionName
		,const std::string& driverspecific
		,const miIOImage::IOImageOffset inputOffset
		,const miIOImage::IOImageOffset outputOffset);
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