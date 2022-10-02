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
	}Direction;
	

	class Modul : public IOModulBase
	{
	private:
		
		uint8_t data[100];

	public:
		Modul();

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

