#pragma once
#include "ModulBase.h"
#include "GPIODriver.h"

namespace miModul
{
	class GPIOModul : public ModulBase
	{
	private:
		miDriver::GPIODriver _GPIODriver;
	public:
		GPIOModul(ModulIOConfiguration configuration);

		// Geerbt über cModulBase
		virtual Modulresult Init() override;

		virtual Modulresult ReadInputToIOImage() override;

		virtual Modulresult WriteOutputFromIOImage() override;

		virtual Modulresult Deinit() override;

	};
}

