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
	typedef uint32_t IOImageSize;
	typedef uint8_t* IOImagePointer_t;

	typedef enum IOImageResult_e
	{
		Ok,
		ErrorRangeExceeded,
		InvalidMemory,
		InvalidParameter,
		InvalidValue
	}IOImageResult;

	typedef enum IOImageType_e
	{
		None,
		Input,
		Output,
		InOut
	}IOImageType;

	class IOImage
	{
	private:
		bool _valid;
		IOImagePointer_t _IOImageMem;
		IOImageSize _IOImageByteSize;
		IOImageType _IOImageType;
		IOImageSize _InvalidValueBitOffset;
		IOImageResult _State;


		IOImageResult Write(IOImageSize byteOffset, void* value, IOImageSize size) const;
		IOImageResult Read(IOImageSize byteOffset, void* value, IOImageSize size) const;
		

	public:
		IOImage();
		IOImage(IOImageSize size, IOImageType type);
		~IOImage();
		

		//get set
		const IOImageSize IOImageByteSize() const;
		bool Valid() const;
		const IOImagePointer_t Memory() const;
		void setValueError(IOImageResult error, IOImageSize bitOffset);
		const IOImageSize InvalidValueBitOffset() const;
		const IOImageResult State() const;

		//functions
		IOImageResult CopyTo(IOImageSize byteOffset, const IOImage& destination, IOImageSize destinationByteOffset, IOImageSize size) const;
		IOImageResult CopyFrom(IOImageSize byteOffset, const IOImage& source, IOImageSize destinationByteOffset, IOImageSize size) const;

		IOImageResult WriteBit(IOImageSize bitOffset, bool value) const;
		IOImageResult WriteUint8(IOImageSize byteOffset, uint8_t value) const;
		IOImageResult WriteUint16(IOImageSize byteOffset, uint16_t value) const;
		IOImageResult WriteUint32(IOImageSize byteOffset, uint32_t value) const;
		bool ReadBit(IOImageSize bitOffset, IOImageResult* result = nullptr) const;
		uint8_t ReadUint8(IOImageSize byteOffset, IOImageResult* result = nullptr) const;
		uint16_t ReadUint16(IOImageSize byteOffset, IOImageResult* result = nullptr) const;
		uint32_t ReadUint32(IOImageSize byteOffset, IOImageResult* result = nullptr) const;
		

		
	};
}
