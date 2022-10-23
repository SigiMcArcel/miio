#pragma once
#include <string>
#include "mi/miio/IOModulInterface.h"

namespace miModul
{
	typedef enum class IOManagerModulLoadResult_e
	{
		Ok,
		ErrorParameter,
		ErrorLoadDriver,
		ErrorImageSizeExceeded,
		ErrorStart,
		ErrorModulNotFound,
		ErrorModulCtrl,
		ErrorModulWrite,
		ErrorModulRead,
		ErrorConfiguration,
		ErrorModulStart,
		ErrorModulFileNotFound
	}IOManagerModulLoadResult;

	typedef std::shared_ptr<miModul::IOModulInterface> IOModulInterface_p;

	const std::string _IOModulCreationFunctionName = "CreateIOModulInterface";
	const std::string _IOModulDestroyFunctionName = "DestroyIOModulInterface";

	class IOModul
	{
	private:
		std::string _Name;
		IOModulInterface_p _Modul;
		bool _Valid;
		IOManagerModulLoadResult _State;

		IOManagerModulLoadResult Load();
		IOManagerModulLoadResult Unload();

	public:
		IOModul();
		IOModul(const IOModul& other);
		IOModul(std::string name);
		~IOModul();

		const IOManagerModulLoadResult State() const{ return _State; }
		const bool Valid() const{ return _Valid; }

		// Geerbt über IOModulInterface
		miModul::IOModulResult Open(const std::string& descriptionFile, const std::string& driverspecific) const;

		miModul::IOModulResult Start() const;

		miModul::IOModulResult Stop() const;

		miModul::IOModulResult Close() const;

		miModul::IOModulResult ReadInputs(const miIOImage::IOImage& image, const miIOImage::IOImageOffset bitOffset, const miIOImage::IOImageSize bitSize) const;

		miModul::IOModulResult WriteOutputs(const miIOImage::IOImage& image, const miIOImage::IOImageOffset bitOffset, const miIOImage::IOImageSize bitSize) const;

		miModul::IOModulResult Control(const std::string name, const std::string function, uint32_t parameter) const;

	};
}