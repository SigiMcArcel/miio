#pragma once
#include <stdint.h>
#include <string>
#include <list>
#include <map>
/// <summary>
/// Interface for machina imaginis control
/// abstracts several IO's or IO Interfaces like gpio.SPI,i2c etc
/// Used in CIModul
/// </summary>
/// 

namespace miIOImage
{
	typedef uint32_t IOImageOffset_t;
	typedef uint32_t IOImageSize_t;
	typedef uint8_t* IOImagePointer_t;

	typedef enum IOImageResult_e
	{
		Ok,
		ErrorRangeExceeded,
		InvalidMemory,
		InvalidParameter
	}IOImageResult;

	typedef enum IOImageType_e
	{
		None,
		Input,
		Output,
		InOut
	}IOImageType;

	class IOImageMemory
	{
	private:
		IOImagePointer_t _IOImageMem;
		IOImageSize_t _IOImageByteSize;
		bool _valid;


	};
	class IOImage
	{
	private:
		bool _valid;
		IOImagePointer_t _IOImageMem;
		IOImageSize_t _IOImageByteSize;
		IOImageType _IOImageType;


		IOImageResult Write(IOImageSize_t byteOffset, void* value, IOImageSize_t size) const;
		IOImageResult Read(IOImageSize_t byteOffset, void* value, IOImageSize_t size) const;
		

	public:
		IOImage();
		IOImage(IOImageSize_t size, IOImageType type);
		~IOImage();
		

		//get set
		const IOImageSize_t IOImageByteSize() const;
		bool Valid() const;
		const IOImagePointer_t Memory() const;
		

		//functions
		IOImageResult CopyTo(IOImageSize_t byteOffset, const IOImage& destination, IOImageSize_t destinationByteOffset, IOImageSize_t size) const;
		IOImageResult CopyFrom(IOImageSize_t byteOffset, const IOImage& source, IOImageSize_t destinationByteOffset, IOImageSize_t size) const;

		IOImageResult WriteBit(IOImageSize_t byteOffset, bool value) const;
		IOImageResult WriteUint8(IOImageSize_t byteOffset, uint8_t value) const;
		IOImageResult WriteUint16(IOImageSize_t byteOffset, uint16_t value) const;
		IOImageResult WriteUint32(IOImageSize_t byteOffset, uint32_t value) const;
		bool ReadBit(IOImageSize_t bitOffset, IOImageResult* result = nullptr) const;
		uint8_t ReadUint8(IOImageSize_t bitOffset, IOImageResult* result = nullptr) const;
		uint16_t ReadUint16(IOImageSize_t bitOffset, IOImageResult* result = nullptr) const;
		uint32_t ReadUint32(IOImageSize_t bitOffset, IOImageResult* result = nullptr) const;
		

		
	};
}
