#pragma once
#include <stdint.h>
#include <string>
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
		IOModulResult _State;
		



	public:
		IOModulBase()
			:_State(IOModulResult::Ok)
		{
			
		};

		virtual IOModulResult Init() = 0;
		
		virtual IOModulResult Deinit() = 0;
		


	};
	
	IOModulInterface* create();
		
	
}

