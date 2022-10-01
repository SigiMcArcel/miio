#pragma once
#include <map>
#include <mi/miio/IOModulBase.h>
#include "GPIODriver.h"

namespace miModul
{
	typedef enum GPIOPinDirection_e
	{
		In,
		Out
	}GPIOPinDirection;
	class GPIOPinConfig
	{
	private:
		GPIOPinDirection _Direction;
		int32_t _Id;
		int32_t _PinNumber;
	public:
		GPIOPinConfig() = default;
		GPIOPinConfig(GPIOPinDirection direction,int32_t id,int32_t pinNumber)
			:_Direction(direction)
			,_Id(id)
			,_PinNumber(pinNumber)
		{}

		const GPIOPinDirection& Direction() const { return _Direction; };
		const int32_t& Id() const { return _Id; };
		const int32_t& PinNumber() const { return _PinNumber; };
	};

	class GPIOModul : public IOModulBase
	{
	private:
		miDriver::GPIODriver _GPIODriver;
		std::map<int32_t,GPIOPinConfig> _PinConfiguration;

	public:
		GPIOModul();

		// Geerbt über IOModulBase
		virtual IOModulResult Init();
		virtual IOModulResult Deinit();


		// Geerbt über IOModulInterface
		virtual IOModulResult open(const std::string& configuration) override;

		virtual IOModulResult start() override;

		virtual IOModulResult stop() override;

		virtual IOModulResult close() override;

		virtual IOModulResult readInputs(const miIOImage::IOImage& image,  const IOModulIOMap& map) override;

		virtual IOModulResult writeOutputs(const miIOImage::IOImage& image, const IOModulIOMap& map) override;

		virtual IOModulResult control(const std::string name, const std::string function, uint32_t parameter) override;

	};
}

