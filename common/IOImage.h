#pragma once
#include <stdint.h>
#include <string>
#include <vector>
#include <map>
#include <array>
#include <type_traits>
/// <summary>
/// Interface for machina imaginis control
/// abstracts several IO's or IO Interfaces like gpio.SPI,i2c etc
/// Used in CIModul
/// </summary>
/// 

namespace miIOImage
{
	class IOImage;
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

	
	class IOImageValue
	{
	private:
		std::string _Name;
		std::string _AliasName;
		IOImageSize _Bitsize;
		IOValueType _Type;
		const void* _Value;
		IOImageResult _State;
		
	public:
		IOImageValue()
			:_Name("")
			, _AliasName("")
			, _Bitsize(0)
			, _Type(IOValueType::Void)
			, _Value()
			, _State(IOImageResult::Ok)
		{

		}

		IOImageValue(const std::string& name, IOImageSize bitSize, const void* value)
			:_Name(name)
			, _AliasName("")
			, _Bitsize(bitSize)
			, _Type(IOValueType::Void)
			, _Value(value)
			, _State(IOImageResult::Ok)
		{

		}

		const std::string& Name() const { return _Name; };
		const std::string& AliasName() const { return _AliasName; };
		const IOImageSize& Bitsize() const { return _Bitsize; };
		const IOValueType Type() const { return _Type; };
		const IOImageResult& State() const { return _State; };

		void Name(const std::string& name) { _Name = name; };
		void AliasName(const std::string& aliasName) { _AliasName = aliasName; };
		void Bitsize(const IOImageSize& bitSize) { _Bitsize = bitSize; };
		void Type(const IOValueType& type) { _Type = type; };
		void State(const IOImageResult& state) { _State = state; };


		const bool GetBit(const std::string& name, IOImageResult& result) const
		{
			return false;
		}

		const uint8_t GetUint8(const std::string& name, IOImageResult& result) const
		{
			return 0;
		}

		const uint16_t GetUint16(const std::string& name, IOImageResult& result) const
		{
			return 0;
		}

		const uint32_t GetInt32(const std::string& name, IOImageResult& result) const
		{
			return 0;
		}

		const int8_t Getint8(const std::string& name, IOImageResult& result) const
		{
			return 0;
		}

		const int16_t GetInt16(const std::string& name, IOImageResult& result) const
		{
			return 0;
		}

		const int32_t GetUint32(const std::string& name, IOImageResult& result) const
		{
			return 0;
		}

		const std::string GetString(const std::string& name, IOImageResult& result) const
		{
			std::string str;
			return str;
		}

		const Uint8Array getUInt8Array(const std::string& name, IOImageResult& result) const
		{
			Uint8Array arr;
			return arr;
		}

		

	};

	class IOImageRange
	{
	private:
		std::string _Name;
		IOImagePointer _Memory;
		IOImageSize _Size;
		IOImageOffset _Offset;

		const IOImagePointer getMemory() const
		{
			return _Memory;
		}
		void SetMemory(const IOImagePointer& memory)
		{
			_Memory = memory;
		}
		

	public:
		IOImageRange()
			:_Name("")
			, _Memory(nullptr)
			, _Size(0)
			, _Offset(0)
		{

		}

		IOImageRange(std::string name, IOImageSize size, IOImageOffset offset)
			:_Name("")
			, _Memory(nullptr)
			, _Size(size)
			, _Offset(offset)
		{

		}

		


		friend class IOImage;
	};


	typedef std::map<std::string, IOImage> IoImageMap;
	typedef std::vector<std::string> IOImagePath;


	class IOImage
	{
	private:
		bool _valid;
		IOImagePointer _IOImageMem;
		IOImageSize _IOImageByteSize;
		IOImageType _IOImageType;
		IOImageSize _InvalidValueBitOffset;
		IOImageResult _State;



		IOImageResult Write(IOImageSize byteOffset, void* value, IOImageSize size) const;
		IOImageResult Read(IOImageSize byteOffset, void* value, IOImageSize size) const;

		IOImagePath ResolveIoImagePath(std::string path);
		

	public:
		IOImage();
		IOImage(IOImageSize size, IOImageType type);
		~IOImage();
		

		//get set
		const IOImageSize IOImageByteSize() const;
		bool Valid() const;
		const IOImagePointer Memory() const;
		void setValueError(IOImageResult error, IOImageSize bitOffset);
		const IOImageSize InvalidValueBitOffset() const;
		const IOImageResult State() const;

		//functions
		IOImageResult AddRange(IOImageSize size, IOImageSize byteOffset);
		
		IOImageResult Remove(const IOImage& image);
		IOImageResult Alloc();



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

		IOImageResult WriteBit(const std::string& name, bool value) const;
		IOImageResult WriteUint8(const std::string& name, uint8_t value) const;
		IOImageResult WriteUint16(const std::string& name, uint16_t value) const;
		IOImageResult WriteUint32(const std::string& name, uint32_t value) const;
		bool ReadBit(const std::string& name, IOImageResult* result = nullptr) const;
		uint8_t ReadUint8(const std::string& name , IOImageResult* result = nullptr) const;
		uint16_t ReadUint16(const std::string& name , IOImageResult* result = nullptr) const;
		uint32_t ReadUint32(const std::string& name , IOImageResult* result = nullptr) const;	
	};
}
