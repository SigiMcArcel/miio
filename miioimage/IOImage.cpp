#include "IOImage.h"
#include "cstring"


using namespace miIOImage;

miIOImage::IOImage::IOImage()
	:_Valid(false)
	,_IOImageMem(nullptr)
	, _IOImageByteSize(0)
	, _IOImageType(IOImageType::None)
{
	_InstanceCounter = 0;
}

IOImage::IOImage(IOImageSize size,IOImageType type, std::string name)
	:_Valid(false)
	, _IOImageMem(nullptr)
	, _IOImageByteSize(size)
	, _IOImageType(type)
	,_Name(name)
	, _Criticalsection(new std::mutex)
{
	
	if (Alloc() == IOImageResult::Ok)
	{
		_InstanceCounter++;
	}
	printf("init constructor instance = %d\n", _InstanceCounter);
}

IOImage::IOImage(const IOImage& other)
	:_Valid(other._Valid)
	, _IOImageMem(other._IOImageMem)
	, _IOImageByteSize(other._IOImageByteSize)
	, _IOImageType(other._IOImageType)
	, _Criticalsection(other._Criticalsection)
{
	_InstanceCounter++;
	printf("copy constructor instance = %d  from %s\n", _InstanceCounter, other._Name.c_str());
}
IOImage::~IOImage()
{
	printf("deconstructor instance = %d\n", _InstanceCounter);
	_InstanceCounter--;
	if (_InstanceCounter == 0)
	{
		printf("deconstructor deleted\n");
		_IOImageByteSize = 0;
		_Valid = false;
		delete _IOImageMem;
		_IOImageMem = nullptr;
		_InstanceCounter = 0;
		delete _Criticalsection;
	}
	if (_InstanceCounter < 0)
	{
		_InstanceCounter = 0;
	}
	

}

const IOImage& miIOImage::IOImage::operator=(const IOImage& other)
{
	_InstanceCounter++;
	_Valid = other._Valid;
	_IOImageMem = other._IOImageMem;
	_IOImageByteSize = other._IOImageByteSize;
	_IOImageType = other._IOImageType;
	_Criticalsection = other._Criticalsection;
	printf("assigne instance = %d from %s\n", _InstanceCounter,other._Name.c_str());
	return *this;
}

const IOImageSize IOImage::IOImageByteSize() const
{
	return _IOImageByteSize;
}

bool IOImage::Valid() const 
{
	return false;
}

const IOImagePointer miIOImage::IOImage::Memory() const
{
	return _IOImageMem;
}

const IOImageResult miIOImage::IOImage::State() const
{
	return _State;
}

IOImageResult miIOImage::IOImage::Alloc()
{
	if (_IOImageByteSize == 0)
	{
		return IOImageResult::InvalidParameter;
	}
	_IOImageMem = new uint8_t[_IOImageByteSize];
	if (_IOImageMem == nullptr)
	{
		_Valid = false;
		return IOImageResult::InvalidMemory;
	}
	_Valid = true;
	return IOImageResult::Ok;
}

IOImageResult IOImage::CopyTo(IOImageSize byteOffset, const IOImage& destination, const IOImageSize destinationByteOffset, IOImageSize size) const
{
	
	if (!_Valid)
	{
		return IOImageResult::InvalidMemory;
	}
	if (!destination.Valid())
	{
		return IOImageResult::InvalidMemory;
	}
	if (byteOffset > _IOImageByteSize)
	{
		return IOImageResult::ErrorRangeExceeded;
	}
	if (destinationByteOffset > destination.IOImageByteSize())
	{
		return IOImageResult::ErrorRangeExceeded;
	}
	if((size + destinationByteOffset) > destination.IOImageByteSize())
	{
		return IOImageResult::ErrorRangeExceeded;
	}
	if (size + byteOffset > _IOImageByteSize)
	{
		return IOImageResult::ErrorRangeExceeded;
	}
	if (destination.Memory() == nullptr)
	{
		return IOImageResult::InvalidMemory;
	}
	_Criticalsection->lock();
	std::memcpy(destination.Memory() + destinationByteOffset, _IOImageMem + byteOffset, size);
	_Criticalsection->unlock();
	return IOImageResult::Ok;
}

IOImageResult miIOImage::IOImage::CopyFrom(IOImageSize byteOffset, const IOImage& source, const IOImageSize sourceByteOffset, IOImageSize size) const
{
	if (!_Valid)
	{
		return IOImageResult::InvalidMemory;
	}
	if (!source.Valid())
	{
		return IOImageResult::InvalidMemory;
	}
	if (byteOffset > _IOImageByteSize)
	{
		return IOImageResult::ErrorRangeExceeded;
	}
	if (sourceByteOffset > source.IOImageByteSize())
	{
		return IOImageResult::ErrorRangeExceeded;
	}
	if ((size + sourceByteOffset) > source.IOImageByteSize())
	{
		return IOImageResult::ErrorRangeExceeded;
	}
	if (size + byteOffset > _IOImageByteSize)
	{
		return IOImageResult::ErrorRangeExceeded;
	}
	_Criticalsection->lock();
	std::memcpy(_IOImageMem + byteOffset,source.Memory() + sourceByteOffset,size);
	_Criticalsection->unlock();
	return IOImageResult::Ok;

}

IOImageResult miIOImage::IOImage::Write(IOImageSize byteOffset, void* data, IOImageSize size) const
{
	if (data == nullptr)
	{
		return IOImageResult::InvalidParameter;
	}
	if (!_Valid)
	{
		return IOImageResult::InvalidMemory;
	}
	if (byteOffset > _IOImageByteSize)
	{
		return IOImageResult::ErrorRangeExceeded;
	}
	if (size + byteOffset > _IOImageByteSize)
	{
		return IOImageResult::ErrorRangeExceeded;
	}
	_Criticalsection->lock();
	std::memcpy(_IOImageMem + byteOffset, data , size);
	_Criticalsection->unlock();
	return IOImageResult::Ok;
}

IOImageResult miIOImage::IOImage::Read(IOImageSize byteOffset, void* data, IOImageSize size) const
{
	if (data == nullptr)
	{
		return IOImageResult::InvalidParameter;
	}
	if (!_Valid)
	{
		return IOImageResult::InvalidMemory;
	}
	if (byteOffset > _IOImageByteSize)
	{
		return IOImageResult::ErrorRangeExceeded;
	}
	if (size + byteOffset > _IOImageByteSize)
	{
		return IOImageResult::ErrorRangeExceeded;
	}
	_Criticalsection->lock();
	std::memcpy(data, _IOImageMem + byteOffset, size);
	_Criticalsection->unlock();
	return IOImageResult::Ok;
}

IOImageResult miIOImage::IOImage::WriteBit(IOImageSize bitOffset, bool value) const
{
	IOImageResult result = IOImageResult::Ok;
	IOImageSize byteOffset = bitOffset / 8;
	int bitNumber = static_cast<int>(bitOffset % 8);
	int bitmask = bitNumber << 1;
	uint8_t byteVal = 0;
	int val = 0;
	result = Read(byteOffset, &byteVal, 1);
	if (result != IOImageResult::Ok)
	{
		return result;
	}
	val = static_cast<int>(byteVal);
	if (value)
	{
		val = val | bitmask;
	}
	else
	{
		val = val & ~bitmask;
	}
	byteVal = static_cast<uint8_t>(val);
	return Write(byteOffset, &byteVal, 1);
}

IOImageResult miIOImage::IOImage::WriteUint8(IOImageSize byteOffset, uint8_t value) const
{
	
	return Write(byteOffset, &value, sizeof(uint8_t));
}

IOImageResult miIOImage::IOImage::WriteUint16(IOImageSize byteOffset, uint16_t value) const
{
	return Write(byteOffset, &value, sizeof(uint16_t));
}

IOImageResult miIOImage::IOImage::WriteUint32(IOImageSize byteOffset, uint32_t value) const
{
	return Write(byteOffset, &value, sizeof(uint32_t));
}

bool miIOImage::IOImage::ReadBit(IOImageSize bitOffset, IOImageResult& result) const
{
	IOImageSize byteOffset = bitOffset / 8;
	uint8_t bitNumber = static_cast<uint8_t>(bitOffset % 8);
	uint8_t bitmask = bitNumber << 1;
	uint8_t byteVal = 0;


	result = Read(byteOffset, &byteVal, 1);
	if (result != IOImageResult::Ok)
	{
		return false;
	}
	if ((byteVal & bitmask) > 0)
	{
		return true;
	}
	return false;
}

uint8_t miIOImage::IOImage::ReadUint8(IOImageSize byteOffset, IOImageResult& result) const
{
	uint8_t val = 0;
	result = Read(byteOffset, &val, sizeof(uint8_t));
	if (result != IOImageResult::Ok)
	{
		return 0;
	}
	return val;
}

uint16_t miIOImage::IOImage::ReadUint16(IOImageSize byteOffset, IOImageResult& result) const
{
	uint16_t val = 0;
	result = Read(byteOffset, &val, sizeof(uint16_t));
	if (result != IOImageResult::Ok)
	{
		return 0;
	}
	return val;
}

uint32_t miIOImage::IOImage::ReadUint32(IOImageSize byteOffset, IOImageResult& result) const
{
	uint32_t val = 0;
	result = Read(byteOffset, &val, sizeof(uint32_t));
	if (result != IOImageResult::Ok)
	{
		return 0;
	}
	return val;
}


