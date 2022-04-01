#include "IOImage.h"
#include "cstring"
using namespace miIOImage;



miIOImage::IOImage::IOImage()
	:_valid(false)
	,_IOImageMem(nullptr)
	, _IOImageByteSize(0)
	, _IOImageType(IOImageType::None)
{
}

IOImage::IOImage(IOImageSize_t size,IOImageType type)
	:_valid(false)
	, _IOImageMem(nullptr)
	, _IOImageByteSize(0)
	, _IOImageType(IOImageType::None)
{
	_IOImageMem = new uint8_t[size]();
	if (_IOImageMem == nullptr)
	{
		return;
	}
	_IOImageByteSize = size;
	_valid = true;
}

IOImage::~IOImage()
{
	if (_IOImageMem != nullptr)
	{
		delete _IOImageMem;
		_IOImageByteSize = 0;
		_valid = false;
	}

}

const IOImageSize_t IOImage::IOImageByteSize() const
{
	return _IOImageByteSize;
}

bool IOImage::Valid() const 
{
	return false;
}

const IOImagePointer_t miIOImage::IOImage::Memory() const
{
	return _IOImageMem;
}

IOImageResult IOImage::CopyTo(IOImageSize_t byteOffset, const IOImage& destination, const IOImageSize_t destinationByteOffset, IOImageSize_t size) const
{
	if (!_valid)
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
	std::memcpy(destination.Memory() + destinationByteOffset, _IOImageMem + byteOffset, size);
	return IOImageResult::Ok;
}

IOImageResult miIOImage::IOImage::CopyFrom(IOImageSize_t byteOffset, const IOImage& source, const IOImageSize_t sourceByteOffset, IOImageSize_t size) const
{
	if (!_valid)
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
	std::memcpy(_IOImageMem + byteOffset,source.Memory() + sourceByteOffset,size);
	return IOImageResult::Ok;

}

IOImageResult miIOImage::IOImage::Write(IOImageSize_t byteOffset, void* data, IOImageSize_t size) const
{
	if (data == nullptr)
	{
		return IOImageResult::InvalidParameter;
	}
	if (!_valid)
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
	return IOImageResult();
}

IOImageResult miIOImage::IOImage::Read(IOImageSize_t byteOffset, void* data, IOImageSize_t size) const
{
	if (data == nullptr)
	{
		return IOImageResult::InvalidParameter;
	}
	if (!_valid)
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
	return IOImageResult();
}

IOImageResult miIOImage::IOImage::WriteBit(IOImageSize_t bitOffset, bool value) const
{
	IOImageResult result = IOImageResult::Ok;
	IOImageSize_t byteOffset = bitOffset / 8;
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

IOImageResult miIOImage::IOImage::WriteUint8(IOImageSize_t byteOffset, uint8_t value) const
{
	
	return Write(byteOffset, &value, sizeof(uint8_t));
}

IOImageResult miIOImage::IOImage::WriteUint16(IOImageSize_t byteOffset, uint16_t value) const
{
	return Write(byteOffset, &value, sizeof(uint16_t));
}

IOImageResult miIOImage::IOImage::WriteUint32(IOImageSize_t byteOffset, uint32_t value) const
{
	return Write(byteOffset, &value, sizeof(uint32_t));
}

bool miIOImage::IOImage::ReadBit(IOImageSize_t bitOffset, IOImageResult* result) const
{
	IOImageSize_t byteOffset = bitOffset / 8;
	uint8_t bitNumber = static_cast<uint8_t>(bitOffset % 8);
	uint8_t bitmask = bitNumber << 1;
	uint8_t byteVal = 0;
	*result = Read(byteOffset, &byteVal, 1);
	if (*result != IOImageResult::Ok)
	{
		return false;
	}
	if ((byteVal & bitmask) > 0)
	{
		return true;
	}
	return false;
}

uint8_t miIOImage::IOImage::ReadUint8(IOImageSize_t byteOffset, IOImageResult* result) const
{
	uint8_t val = 0;
	*result = Read(byteOffset, &val, sizeof(uint8_t));
	if (*result != IOImageResult::Ok)
	{
		return 0;
	}
	return val;
}

uint16_t miIOImage::IOImage::ReadUint16(IOImageSize_t byteOffset, IOImageResult* result) const
{
	uint16_t val = 0;
	*result = Read(byteOffset, &val, sizeof(uint16_t));
	if (*result != IOImageResult::Ok)
	{
		return 0;
	}
	return val;
}

uint32_t miIOImage::IOImage::ReadUint32(IOImageSize_t byteOffset, IOImageResult* result) const
{
	uint32_t val = 0;
	*result = Read(byteOffset, &val, sizeof(uint32_t));
	if (*result != IOImageResult::Ok)
	{
		return 0;
	}
	return val;
}

