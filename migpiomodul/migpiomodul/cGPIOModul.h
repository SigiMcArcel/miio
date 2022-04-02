#pragma once
#include "cModulBase.h"
#include <iostream>                             // Standardstream-Funktionaliät einbinden
#include <fstream> 

namespace miModul
{
	class cGPIOModul : public cModulBase
	{
	private:
		const std::string gpioPath = "/sys/class/gpio";
		
		

		Modulresult GpioEnable(int32_t pin, bool enable);
		Modulresult GpioSetDirection(int32_t pin, ModulIOMapDirections direction);
		
		bool GpioRead(int32_t pin, Modulresult *result = nullptr);
		Modulresult GpioWrite(int32_t pin, bool value);

	public:
		cGPIOModul(ModulIOConfiguration configuration);

		

			
	

		// Geerbt über cModulBase
		virtual Modulresult Init() override;

		virtual Modulresult ReadInputToIOImage() override;

		virtual Modulresult WriteOutputFromImage() override;

		virtual Modulresult Deinit() override;

	};
}

