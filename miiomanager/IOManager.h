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
		ErrorModulStart
}IOManagerResult;

typedef std::shared_ptr<miModul::IOModulInterface> IOModulInterface_p;
typedef std::map<int32_t, miModul::IOModulIOMap> IOMapMap;

typedef struct IOMap_t
{
	IOMapMap iOMapMap;
	miIOImage::IOImageSize _BitSize;
}IOMap;

class IOModulDescription
{
private:
	std::string _Library;
	std::string _Name;
	std::string _Configuration;
	IOModulInterface_p _Interface;
	IOMap _IMap;
	IOMap _OMap;

public:
	IOModulDescription() = default;
	IOModulDescription(const std::string& library, 
		const std::string& name, 
		const std::string& configuration, 
		const IOModulInterface_p& modulInterface,
		IOMap iMap,
		IOMap oMap)
		:_Library(library)
		,_Name(name)
		,_Configuration(configuration)
		,_Interface(modulInterface)
		,_IMap(iMap)
		,_OMap(oMap)
	{

	}
	const std::string& Library() const { return _Library; };
	const std::string& Name() const { return _Name; };
	const std::string& Configuration() const { return _Configuration; };
	const IOModulInterface_p& Interface() const { return _Interface; };
	const IOMap& IMap() const { return _IMap; };
	const IOMap& OMap() const { return _OMap; };
};

class IOManager : public miutils::EventListener
{
private:
	miIOImage::IOImage _InputImage;
	miIOImage::IOImage _OutputImage;
	std::map<std::string,IOModulDescription> _ModulList;
	miutils::Timer _Timer;
	miIOImage::IOImageOffset _ActInputOffset;
	miIOImage::IOImageOffset _ActOutputOffset;
	IOManagerResult _State;

	IOModulInterface_p LoadModul(const std::string& libraryName,IOManagerResult& result);
	IOManagerResult UnLoadModul(const std::string& libraryName, const IOModulInterface_p& obj);
	IOManagerResult ReadConfig(const std::string& configuration, miIOImage::IOImageOffset startInputOffset, miIOImage::IOImageOffset startOutputOffset);
	IOManagerResult ReadIOMaps(const rapidjson::Value& d, std::string key, IOMap& iomap,const miIOImage::IOImageOffset startOffset);

public:
	IOManager(miIOImage::IOImageSize inputImageSize, miIOImage::IOImageSize outputImageSize);
		
	IOManagerResult AddIOModul(const std::string& configuration);
	IOManagerResult AddIOModul(const miIOImage::IOImageOffset inputOffset, const miIOImage::IOImageOffset outputOffset, const std::string& configuration);
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