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


/*
* Modul description format (json)
* 
* mimoduldescription
* {
*		name:"gpiomodul"
*       modulconf:
*		{
*			configuration:
*			{
*				(*driver specific*)
*				gpiopins:
*				{
*					pin:
*					{
*						id:0,
*						direction:in,
*						number:24
*					{
*					pin:
*					{
*						id:1,
*						direction:out,
*						number:18
*					{
*				{
* 
*			}
*			imaps:
*			{
*				map:
*				{
*					name: "gpioin1",
*					id:0,
*					offset:0,
*					bitsize:1,
*					driverspecific:0,
*				}
*			}
*			omaps:
*			{
*				map:
*				{
*					name: "gpioout1",
* *					id:1,
*					offset:0,
*					bitsize:1,
*					driverspecific:1
*				}
*			}
*		}
* }
* 
* 
*/
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
		ErrorConfiguration
}IOManagerResult;

typedef std::shared_ptr<miModul::IOModulInterface> IOModulInterface_p;
typedef std::map<int32_t, miModul::IOModulIOMap> IOMap;

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

	IOModulInterface_p LoadModul(const std::string& libraryName,IOManagerResult& result);
	IOManagerResult UnLoadModul(const std::string& libraryName, const IOModulInterface_p& obj);
	IOManagerResult ReadConfig(const std::string& configuration);
	IOManagerResult ReadIOMaps(const rapidjson::Value& d, std::string key, IOMap& iomap);



public:
	IOManager(miIOImage::IOImageSize inputImageSize, miIOImage::IOImageSize outputImageSize);
		
	IOManagerResult AddIOModul(const std::string& configuration);
	IOManagerResult RemoveIOModul(const std::string& name);
	IOManagerResult StartIOCycle(int cycleTime);
	IOManagerResult StopIOCycle();
	IOManagerResult IOModulControl(const std::string& name,const std::string function,uint32_t parameter);
		

	// Geerbt über EventListener
	virtual void eventOccured(void* sender, const std::string& name) override;

};

}