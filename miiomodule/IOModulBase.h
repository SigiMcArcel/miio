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
	private:
		std::string _Name;
		IOModulResult _State;
		rapidjson::Document _ModuleDescription;
		
	public:
		IOModulBase()
			: _Name("Modul")
			,_State(IOModulResult::Ok)
		{
			
		};
		const IOModulResult State() const
		{
			return _State;
		}
		virtual ~IOModulBase() = 0;
		virtual IOModulResult Init() = 0;
		virtual IOModulResult Deinit() = 0;
	
	};
}

