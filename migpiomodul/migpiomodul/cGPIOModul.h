#pragma once
#include "cModulBase.h"
namespace miModul
{
	class cGPIOModul : public cModulBase
	{
	private:
		const std::string gpioPath = "/sys/class/gpio";
	public:
		cGPIOModul(ModulIOConfiguration configuration);
			
	

		// Geerbt über cModulBase
		virtual Modulresult Init() override;

		virtual Modulresult Read() override;

		virtual Modulresult Write() override;

		virtual Modulresult Deinit() override;

	};
}

