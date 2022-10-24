#pragma once
#include <map>
#include <rapidjson/document.h>
#include <mi/miio/IOModulBase.h>
#include <mi/midriver/I2CDriver.h>

namespace miModul
{
	class PCF8574Modul : public IOModulBase
	{
	private:
		miDriver::I2CDriver _I2CDriver;
		uint8_t _Address;

		IOModulResult ResolveDriverSpecific(const std::string& driverspecific);
	public:
		PCF8574Modul();
		virtual ~PCF8574Modul();

		// Geerbt über IOModulBase
		virtual IOModulResult Init();
		virtual IOModulResult Deinit();
		virtual IOModulResult ReadDriverSpecificInputConfig(const rapidjson::Value& item);
		virtual IOModulResult ReadDriverSpecificOutputConfig(const rapidjson::Value& item);


		// Geerbt über IOModulInterface
		virtual IOModulResult Open(const std::string& descriptionFile, const std::string& driverspecific) override;

		virtual IOModulResult Start() override;

		virtual IOModulResult Stop() override;

		virtual IOModulResult Close() override;

		virtual IOModulResult ReadInputs(const miIOImage::IOImage& image, const miIOImage::IOImageOffset bitOffset, const miIOImage::IOImageSize bitSize) override;

		virtual IOModulResult WriteOutputs(const miIOImage::IOImage& image, const miIOImage::IOImageOffset bitOffset, const miIOImage::IOImageSize bitSize) override;

		virtual IOModulResult Control(const std::string name, const std::string function, uint32_t parameter) override;

	};
}

