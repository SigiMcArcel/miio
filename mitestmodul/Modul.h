#pragma once
#include <map>
#include <mi/miio/IOModulBase.h>
#include "GPIODriver.h"

namespace miModul
{
	typedef enum ModulDirection_e
	{
		In,
		Out
	}ModulDirection;
	struct ModulConfig
	{
		ModulDirection Direction;
		int32_t Id;
		int32_t PinNumber;
		int32_t BitOffset;
		int32_t Bitsize;
	public:
		ModulConfig() = default;
		ModulConfig(ModulDirection direction, int32_t id, int32_t pinNumber, int32_t bitOffset,int32_t bitSize)
			:Direction(direction)
			, Id(id)
			, PinNumber(pinNumber)
			, BitOffset(bitOffset)
			, Bitsize(bitSize)
		{}
	};
	

	class Modul : public IOModulBase
	{
	private:
		uint8_t data[100];
		std::map<int32_t, ModulConfig> _ModulConfig;
	public:
		Modul();
		virtual ~Modul();
		
		// Geerbt über IOModulBase
		virtual IOModulResult Init();
		virtual IOModulResult Deinit();


		// Geerbt über IOModulInterface
		virtual IOModulResult Open(const std::string& configuration) override;

		virtual IOModulResult Start() override;

		virtual IOModulResult Stop() override;

		virtual IOModulResult Close() override;

		virtual IOModulResult ReadInputs(const miIOImage::IOImage& image,  const IOModulIOMap& map) override;

		virtual IOModulResult WriteOutputs(const miIOImage::IOImage& image, const IOModulIOMap& map) override;

		virtual IOModulResult Control(const std::string name, const std::string function, uint32_t parameter) override;

	};
}

