#pragma once
#include <stdint.h>
#include <string>
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

	typedef enum ModulIOMapDirections_e
	{
		None,
		In,
		Out
	}ModulIOMapDirections;

	class ModulIOMapEntry
	{
	private:
		std::string _Name;
		uint32_t _SubAddress;
		IOImageSize _BitSize;
		ModulIOMapDirections _Direction;
	public:
		const std::string  Name() const { return _Name; };
		const uint32_t  SubAddress() const { return _SubAddress; };
		const IOImageSize  BitSize() const { return _BitSize; };
		const ModulIOMapDirections  Direction() const { return _Direction; };
		

		void Name(const std::string& name) { _Name = name; };
		void SubAddress(const uint32_t& subAddress) { _SubAddress = subAddress; };
		void BitSize(const IOImageSize& bitSize) { _BitSize = bitSize; };
		void Direction(const ModulIOMapDirections direction) { _Direction = direction; };
	};
	
	typedef std::string ModulIOConfiguration;
	typedef std::map<std::string,ModulIOMapEntry> ModulIOMaps;

	class ModulBase
	{
	protected:
		std::string _Name;
		ModulIOMaps _IOMap;
		ModulIOConfiguration _Configuration;
		IOImage _IOImageInput;
		IOImage _IOImageOutput;
		IOImageOffset _IOImageInputOffset;
		IOImageOffset _IOImageOutputOffset;
		Modulresult _State;
		



	public:
		ModulBase(ModulIOConfiguration configuration)
			:_Configuration(configuration)
			, _IOImageInput()
			, _IOImageOutput()
			, _State(Modulresult::Ok)
		{
			
		};

		virtual Modulresult Init() = 0;
		virtual Modulresult ReadInputToIOImage() = 0;
		virtual Modulresult WriteOutputFromIOImage() = 0;
		virtual Modulresult Deinit() = 0;
		virtual Modulresult AddToInputIOImage(const IOImage& image, miIOImage::IOImageOffset offset);
		virtual Modulresult AddToOutputIOImage(const IOImage& image, miIOImage::IOImageOffset offset);

	};
}

