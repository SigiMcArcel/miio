#pragma once
#include <stdint.h>
#include <string>
#include <list>
#include <map>
#include "IOImage.h"


/// <summary>
/// Interface for machina imaginis control
/// abstracts several IO's or IO Interfaces like gpio.SPI,i2c etc
/// Used in CIModul
/// </summary>
/// 

using namespace miIOImage;

namespace miModul
{
	
	typedef enum ModulResult_e
	{
		Ok,
		ErrorRead,
		ErrorWrite,
		ErrorInit,
		ErrorConf
	}Modulresult;

	enum ModulIOMapDirections
	{
		None,
		In,
		Out
	};

	typedef struct ModulIOMapEntry_t
	{
		uint32_t SubAddress;
		IOImageSize_t BitSize;
		IOImageOffset_t IoImageBitOffset;
		ModulIOMapDirections Direction;
	}ModulIOMapEntry;
	
	typedef std::string ModulIOConfiguration;
	typedef std::list<ModulIOMapEntry> ModulIOMaps;

	class cModulBase
	{
	protected:
		ModulIOMaps _IOMap;
		ModulIOConfiguration _Configuration;
		IOImage _IOImageInput;
		IOImage _IOImageOutput;
		Modulresult _State;
		



	public:
		cModulBase(ModulIOConfiguration configuration)
			:_Configuration(configuration)
			, _IOImageInput()
			, _IOImageOutput()
			, _State(Modulresult::Ok)
		{
			
		};

		virtual Modulresult Init() = 0;
		virtual Modulresult Read() = 0;
		virtual Modulresult Write() = 0;
		virtual Modulresult Deinit() = 0;

	};
}

