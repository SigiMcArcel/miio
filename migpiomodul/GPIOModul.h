#pragma once
#include <rapidjson/document.h>
#include <map>
#include <mi/miio/IOModulBase.h>
#include <mi/midriver/GPIODriver.h>

namespace miModul
{
	class GPIOPinConfig
	{
	private:
		miIOImage::IOImageOffset _BitOffset;
		int32_t _GpioNumber;
	public:
		GPIOPinConfig() = default;
		GPIOPinConfig(miIOImage::IOImageOffset bitOffset,int32_t gpioNumber)
			: _BitOffset(bitOffset)
			, _GpioNumber(gpioNumber)
		{}

		
		const  miIOImage::IOImageOffset BitOffset() const { return _BitOffset; };
		const int32_t GpioNumber() const { return _GpioNumber; };
	};

	class GPIOModul : public IOModulBase
	{
	private:
		miDriver::GPIODriver _GPIODriver;
		std::map<int32_t,GPIOPinConfig> _PinOutConfiguration;
		std::map<int32_t, GPIOPinConfig> _PinInConfiguration;

		IOModulResult GetGpioConfig(const rapidjson::Value& item, GPIOPinConfig& config);

	public:
		GPIOModul();
		virtual ~GPIOModul();

		// Geerbt über IOModulBase
		virtual IOModulResult Init();
		virtual IOModulResult Deinit();
		virtual IOModulResult ReadDriverSpecificInputConfig(const rapidjson::Value& item);
		virtual IOModulResult ReadDriverSpecificOutputConfig(const rapidjson::Value& item);


		// Geerbt über IOModulInterface
		virtual IOModulResult Open(const std::string& configuration, const std::string& driverspecific) override;

		virtual IOModulResult Start() override;

		virtual IOModulResult Stop() override;

		virtual IOModulResult Close() override;

		virtual IOModulResult ReadInputs(const miIOImage::IOImage& image, const miIOImage::IOImageOffset bitOffset, const miIOImage::IOImageSize bitSize) override;

		virtual IOModulResult WriteOutputs(const miIOImage::IOImage& image, const miIOImage::IOImageOffset bitOffset, const miIOImage::IOImageSize bitSize) override;

		virtual IOModulResult Control(const std::string name, const std::string function, uint32_t parameter) override;

	};
}

