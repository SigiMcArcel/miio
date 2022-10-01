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

IOImage::IOImage(IOImageSize size,IOImageType type)
	:_valid(false)
	, _IOImageMem(nullptr)
	, _IOImageByteSize(0)
	, _IOImageType(type)
{
	
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

void miIOImage::IOImage::setValueError(IOImageResult error, IOImageSize bitOffset)
{
	_InvalidValueBitOffset = bitOffset;
	_State = error;
}

const IOImageSize miIOImage::IOImage::InvalidValueBitOffset() const
{
	return _InvalidValueBitOffset;
}

const IOImageResult miIOImage::IOImage::State() const
{
	return _State;
}



IOImageResult miIOImage::IOImage::AddRange(IOImageSize size, IOImageSize byteOffset)
{
	return IOImageResult();
}

IOImageResult miIOImage::IOImage::Remove(const IOImage& image)
{
	return IOImageResult();
}

IOImageResult miIOImage::IOImage::Alloc()
{
	if (_IOImageByteSize == 0)
	{
		return IOImageResult::InvalidParameter;
	}
	_IOImageMem = new IOImageByte[_IOImageByteSize]();
	if (_IOImageMem == nullptr)
	{
		_valid = false;
		return IOImageResult::InvalidMemory;
	}
	_valid = true;
	return IOImageResult::Ok;
}

IOImageResult IOImage::CopyTo(IOImageSize byteOffset, const IOImage& destination, const IOImageSize destinationByteOffset, IOImageSize size) const
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

IOImageResult miIOImage::IOImage::CopyFrom(IOImageSize byteOffset, const IOImage& source, const IOImageSize sourceByteOffset, IOImageSize size) const
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

IOImageResult miIOImage::IOImage::Write(IOImageSize byteOffset, void* data, IOImageSize size) const
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

IOImageResult miIOImage::IOImage::Read(IOImageSize byteOffset, void* data, IOImageSize size) const
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

IOImagePath miIOImage::IOImage::ResolveIoImagePath(std::string path)
{
	return IOImagePath();
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

bool miIOImage::IOImage::ReadBit(IOImageSize bitOffset, IOImageResult* result) const
{
	IOImageResult resultIntern = IOImageResult::Ok;
	IOImageSize byteOffset = bitOffset / 8;
	uint8_t bitNumber = static_cast<uint8_t>(bitOffset % 8);
	uint8_t bitmask = bitNumber << 1;
	uint8_t byteVal = 0;

	if (result != nullptr) { result = &resultIntern; }

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

IOImageResult miIOImage::IOImage::WriteUint32(const std::string& name, uint32_t value) const
{
	return IOImageResult();
}

bool miIOImage::IOImage::ReadBit(const std::string& name, IOImageResult* result) const
{
	return false;
}

uint8_t miIOImage::IOImage::ReadUint8(const std::string& name, IOImageResult* result) const
{
	return uint8_t();
}

uint16_t miIOImage::IOImage::ReadUint16(const std::string& name, IOImageResult* result) const
{
	return uint16_t();
}

uint32_t miIOImage::IOImage::ReadUint32(const std::string& name, IOImageResult* result) const
{
	return uint32_t();
}

uint8_t miIOImage::IOImage::ReadUint8(IOImageSize byteOffset, IOImageResult* result) const
{
	IOImageResult resultIntern = IOImageResult::Ok;
	uint8_t val = 0;
	if (result != nullptr) { result = &resultIntern; }
	*result = Read(byteOffset, &val, sizeof(uint8_t));
	if (*result != IOImageResult::Ok)
	{
		return 0;
	}
	return val;
}

uint16_t miIOImage::IOImage::ReadUint16(IOImageSize byteOffset, IOImageResult* result) const
{
	IOImageResult resultIntern = IOImageResult::Ok;
	uint16_t val = 0;
	if (result != nullptr) { result = &resultIntern; }
	*result = Read(byteOffset, &val, sizeof(uint16_t));
	if (*result != IOImageResult::Ok)
	{
		return 0;
	}
	return val;
}

uint32_t miIOImage::IOImage::ReadUint32(IOImageSize byteOffset, IOImageResult* result) const
{
	IOImageResult resultIntern = IOImageResult::Ok;
	uint32_t val = 0;
	if (result != nullptr) { result = &resultIntern; }
	*result = Read(byteOffset, &val, sizeof(uint32_t));
	if (*result != IOImageResult::Ok)
	{
		return 0;
	}
	return val;
}

IOImageResult miIOImage::IOImage::WriteBit(const std::string& name, bool value) const
{
	return IOImageResult();
}

IOImageResult miIOImage::IOImage::WriteUint8(const std::string& name, uint8_t value) const
{
	return IOImageResult();
}

IOImageResult miIOImage::IOImage::WriteUint16(const std::string& name, uint16_t value) const
{
	return IOImageResult();
}

