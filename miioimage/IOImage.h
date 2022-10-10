#pragma once
#include <stdint.h>
#include <string>
#include <vector>
#include <map>
#include <array>
#include <type_traits>
#include <memory>

/// <summary>
/// Interface for machina imaginis control
/// abstracts several IO's or IO Interfaces like gpio.SPI,i2c etc
/// Used in CIModul
/// </summary>
/// 

namespace miIOImage
{
	static int32_t _InstanceCounter;
	typedef struct Bits_t
	{
		bool b0 : 1;
		bool b1 : 1;
		bool b2 : 1;
		bool b3 : 1;
		bool b4 : 1;
		bool b5 : 1;
		bool b6 : 1;
		bool b7 : 1;
	}Bits;
	
	typedef uint32_t IOImageOffset;
	typedef uint32_t IOImageSize;
	typedef uint8_t* IOImagePointer;

	typedef std::vector<uint8_t> Uint8Array;
	typedef enum IOImageResult_e
	{
		Ok,
		ErrorRangeExceeded,
		InvalidMemory,
		InvalidParameter,
		InvalidValue,
		InvalidType
	}IOImageResult;

	typedef enum IOImageType_e
	{
		None,
		Input,
		Output,
		InOut
	}IOImageType;


	typedef enum IOValueType_e
	{
		Void,
		Bool,
		Uint8,
		Uint16,
		Uint32,
		Int8,
		Int16,
		Int32,
		String,
		ArrayOfUint8
	}IOValueType;

	class IOImageValue;

	

	class IOImage
	{
	private:
		bool _Valid;
		IOImagePointer _IOImageMem;
		IOImageSize _IOImageByteSize;
		IOImageType _IOImageType;
		IOImageResult _State;
		std::string _Name;

		IOImageResult Alloc();
		
	public:
		IOImage();
		IOImage(IOImageSize size, IOImageType type, std::string name);
		IOImage(const IOImage& other);
		~IOImage();
		const IOImage& operator=(const IOImage& other);
		
		//get set
		const IOImageSize IOImageByteSize() const;
		bool Valid() const;
		const IOImagePointer Memory() const;
		const IOImageSize InvalidValueBitOffset() const;
		const IOImageResult State() const;

		//functions
		IOImageResult CopyTo(IOImageSize byteOffset, const IOImage& destination, IOImageSize destinationByteOffset, IOImageSize size) const;
		IOImageResult CopyFrom(IOImageSize byteOffset, const IOImage& source, IOImageSize destinationByteOffset, IOImageSize size) const;

		IOImageResult Write(IOImageSize byteOffset, void* value, IOImageSize size) const;
		IOImageResult WriteBit(IOImageSize bitOffset, bool value) const;
		IOImageResult WriteUint8(IOImageSize byteOffset, uint8_t value) const;
		IOImageResult WriteUint16(IOImageSize byteOffset, uint16_t value) const;
		IOImageResult WriteUint32(IOImageSize byteOffset, uint32_t value) const;

		IOImageResult Read(IOImageSize byteOffset, void* value, IOImageSize size) const;
		bool ReadBit(IOImageSize bitOffset, IOImageResult& result) const;
		uint8_t ReadUint8(IOImageSize byteOffset, IOImageResult& result) const;
		uint16_t ReadUint16(IOImageSize byteOffset, IOImageResult& result) const;
		uint32_t ReadUint32(IOImageSize byteOffset, IOImageResult& result) const;

		
	};

}
