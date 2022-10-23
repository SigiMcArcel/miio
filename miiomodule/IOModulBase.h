#pragma once
#include <stdint.h>
#include <string>
#include <rapidjson/document.h>
#include <rapidjson/writer.h>
#include <rapidjson/stringbuffer.h>
#include <map>
#include <mi/miio/IOImage.h>
#include <mi/miio/IOModulInterface.h>


/// <summary>
/// Interface for machina imaginis control
/// abstracts several IO's or IO Interfaces like gpio.SPI,i2c etc
/// Used in CIModul
/// </summary>
/// 

using namespace miIOImage;

namespace miModul
{
	
	typedef std::string ModulIOConfiguration;
	
	

	class IOModulBase : public IOModulInterface
	{
	
		
	protected:
		std::string _Name;
		std::string _IOModuleDescriptionPath;
		
		IOModulResult _State;
		IOModulResult ReadModulConfiguration(const std::string& descriptionFile);
		
	public:
		IOModulBase()
			:_Name("Modul")
			, _IOModuleDescriptionPath("/etc/mi/iomodules")
			,_State(IOModulResult::Ok)
		{
			
		};
		~IOModulBase() = default;
		const IOModulResult State() const
		{
			return _State;
		}
		virtual IOModulResult Init() = 0;
		virtual IOModulResult Deinit() = 0;
		virtual IOModulResult ReadDriverSpecificInputConfig(const rapidjson::Value& item) = 0;
		virtual IOModulResult ReadDriverSpecificOutputConfig(const rapidjson::Value& item) = 0;
	
	};
}

