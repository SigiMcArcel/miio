#pragma once
#include <map>
#include <mi/miio/IOModulBase.h>
#include <mi/midriver/I2CDriver.h>

namespace miModul
{
	typedef enum PCFDirection_e
	{
		In,
		Out
	}PCFDirection;
	
	class PCFPinConfig
	{
	private:
		PCFDirection _Direction;
		int32_t _Id;
		int32_t _Bit;
	public:
		PCFPinConfig() = default;
		PCFPinConfig(PCFDirection direction, int32_t id, int32_t bit)
			:_Direction(direction)
			, _Id(id)
			, _Bit(bit)
		{}

		const PCFDirection& Direction() const { return _Direction; };
		const int32_t& Id() const { return _Id; };
		const int32_t& Bit() const { return _Bit; };
	};

	class PCF8574Modul : public IOModulBase
	{
	private:
		miDriver::I2CDriver _I2CDriver;
		std::map<int32_t, PCFPinConfig> _PinConfiguration;
		uint8_t _Address;

		IOModulResult ResolveDriverSpecific(const std::string& driverspecific);
	public:
		PCF8574Modul();
		virtual ~PCF8574Modul();

		// Geerbt über IOModulBase
		virtual IOModulResult Init();
		virtual IOModulResult Deinit();


		// Geerbt über IOModulInterface
		virtual IOModulResult Open(const std::string& configuration, const std::string& driverspecific) override;

		virtual IOModulResult Start() override;

		virtual IOModulResult Stop() override;

		virtual IOModulResult Close() override;

		virtual IOModulResult ReadInputs(const miIOImage::IOImage& image, const IOModulIOMap& map) override;

		virtual IOModulResult WriteOutputs(const miIOImage::IOImage& image, const IOModulIOMap& map) override;

		virtual IOModulResult Control(const std::string name, const std::string function, uint32_t parameter) override;

	};
}

