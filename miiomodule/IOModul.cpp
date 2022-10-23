#include <mi/miio/IOModul.h>
#include <dlfcn.h>
#include <fstream>
#include <iostream>
#include <sstream> //std::stringstream

miModul::IOManagerModulLoadResult miModul::IOModul::Load()
{
    miModul::IOModulInterface* (*create) ();
    _Valid = false;
    std::string libfileName = std::string("lib") +_Name + std::string(".so");

    void* dynamicHandle = dlopen(libfileName.c_str(), RTLD_LAZY);

    if (nullptr == dynamicHandle)
    {
        std::cerr << __PRETTY_FUNCTION__ << " :Could not load iomodul"<<  libfileName.c_str();
        return IOManagerModulLoadResult::ErrorLoadDriver;
    }
    void* interface = dlsym(dynamicHandle, _IOModulCreationFunctionName.c_str());
    if (interface == nullptr)
    {
        std::cerr << __PRETTY_FUNCTION__ << " :Could not load iomodul entry " << libfileName << std::endl;
        return IOManagerModulLoadResult::ErrorLoadDriver;
    }
    create = reinterpret_cast<miModul::IOModulInterface * (*) ()>(interface);
    miModul::IOModulInterface* tp = create();
    if (tp == nullptr)
    {
        std::cerr << __PRETTY_FUNCTION__ << " :Could not load iomodul interface" << libfileName << std::endl;
        return IOManagerModulLoadResult::ErrorLoadDriver;
    }
   
    if (miModul::_IOMODULVERSION_ != tp->Version())
    {
        std::cerr << __PRETTY_FUNCTION__ << " :Interface Version not match " << miModul::_IOMODULVERSION_ << tp->Version() << _Name << std::endl;
        return IOManagerModulLoadResult::ErrorLoadDriver;
    }
    _Modul = IOModulInterface_p(tp);
    _Valid = true;
    return IOManagerModulLoadResult::Ok;
}
miModul::IOManagerModulLoadResult miModul::IOModul::Unload()
{
    void (*destroy) (miModul::IOModulInterface*);

    void* dynamicHandle = dlopen(_Name.c_str(), RTLD_NOW);

    if (0 == dynamicHandle)
    {
        return IOManagerModulLoadResult::ErrorLoadDriver;
    }
    void* interface = dlsym(dynamicHandle, _IOModulDestroyFunctionName.c_str());
    if (interface == nullptr)
    {
        return IOManagerModulLoadResult::ErrorLoadDriver;
    }
    destroy = reinterpret_cast<void (*) (miModul::IOModulInterface*)>(interface);
    destroy(_Modul.get());
    return IOManagerModulLoadResult::Ok;
}

miModul::IOModul::IOModul(std::string name)
	:_Name(name)
    ,_Modul()
    ,_Valid(false)
    , _State(IOManagerModulLoadResult::Ok)
{
    _State =  Load();
}
miModul::IOModul::IOModul()
{

}
miModul::IOModul::IOModul(const IOModul& other)
    : _Name(other._Name)
    , _Modul(other._Modul)
    ,_Valid(other._Valid)
    , _State(other._State)
{
   
}
miModul::IOModul::~IOModul()
{
    _State = Unload();
}

miModul::IOModulResult miModul::IOModul::Open(const std::string& descriptionFile, const std::string& driverspecific) const
{
    if (!_Valid)
    {
        return miModul::ErrorInit;
    }
    return _Modul.get()->Open(descriptionFile,driverspecific);
}

miModul::IOModulResult miModul::IOModul::Start() const
{
    if (!_Valid)
    {
        return miModul::ErrorInit;
    }
    return _Modul.get()->Start();
}

miModul::IOModulResult miModul::IOModul::Stop() const
{
    if (!_Valid)
    {
        return miModul::ErrorInit;
    }
    return _Modul.get()->Stop();
}

miModul::IOModulResult miModul::IOModul::Close() const
{
    if (!_Valid)
    {
        return miModul::ErrorInit;
    }
    return _Modul.get()->Close();
}

miModul::IOModulResult miModul::IOModul::ReadInputs(const miIOImage::IOImage& image, const miIOImage::IOImageOffset bitOffset, const miIOImage::IOImageSize bitSize) const
{
    if (!_Valid)
    {
        return miModul::ErrorInit;
    }
    return _Modul.get()->ReadInputs(image, bitOffset, bitSize);
}

miModul::IOModulResult miModul::IOModul::WriteOutputs(const miIOImage::IOImage& image, const miIOImage::IOImageOffset bitOffset, const miIOImage::IOImageSize bitSize) const
{
    if (!_Valid)
    {
        return miModul::ErrorInit;
    }
    return _Modul.get()->WriteOutputs(image, bitOffset, bitSize);
}

miModul::IOModulResult miModul::IOModul::Control(const std::string name, const std::string function, uint32_t parameter) const
{
    if (!_Valid)
    {
        return miModul::ErrorInit;
    }
    return _Modul.get()->Control(name, function, parameter);
}