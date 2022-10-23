#pragma once
#include <mi/miio/IOImage.h>
#include <string>


namespace miModul
{
	const std::string _IOMODULVERSION_ = "0.0.0.2";

	typedef enum IOModulResult_e
	{
		Ok,
		ErrorRead,
		ErrorWrite,
		ErrorInit,
		ErrorConf,
		ErrorControl,
		ErrorNotConfigured,
		ErrorNotImplemented,
		ErrorDescriptionNotMatch,
		ErrorConfigurationFileNotFound
	}IOModulResult;

	class IOModulValue
	{
		int32_t _Id;
		int32_t _Driverspecific;
		miIOImage::IOImageOffset _Offset;
		miIOImage::IOImageSize _Size;

	public:
		IOModulValue() = default;
		IOModulValue(int32_t id, int32_t driverspecific, miIOImage::IOImageOffset offset, miIOImage::IOImageSize size)
		:_Id(id)
		,_Driverspecific(driverspecific)
		,_Offset(offset)
		,_Size(size)
		{

		}
		const int32_t& Id() const { return _Id; };
		const int32_t& Driverspecific() const { return _Driverspecific; };
		const miIOImage::IOImageOffset& Offset() const { return _Offset; };
		const miIOImage::IOImageSize& Size() const { return _Size; };
	};

	
	class IOModulInterface
	{ 
	public:
		const std::string Version()
		{
			return _IOMODULVERSION_;
		}
		virtual IOModulResult Open(const std::string& descriptionFile,const std::string& driverspecific) = 0;
		virtual IOModulResult Start() = 0;
		virtual IOModulResult Stop() = 0;
		virtual IOModulResult Close() = 0;
		virtual IOModulResult ReadInputs(const miIOImage::IOImage& image,const miIOImage::IOImageOffset bitOffset,const miIOImage::IOImageSize bitSize) = 0;
		virtual IOModulResult WriteOutputs(const miIOImage::IOImage& image, const miIOImage::IOImageOffset bitOffset, const miIOImage::IOImageSize bitSize) = 0;
		virtual IOModulResult Control(const std::string name, const std::string function, uint32_t parameter) = 0;
	};

}


