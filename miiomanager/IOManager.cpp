#include <dlfcn.h>
#include "IOManager.h"

using namespace miIOManager;

IOModulInterface_p IOManager::LoadModul(const std::string& libraryName, IOManagerResult& result)
{
    miModul::IOModulInterface* (*create) ();

    void* dynamicHandle = dlopen(libraryName.c_str(), RTLD_LAZY);

    if (nullptr == dynamicHandle)
    {
        result = IOManagerResult::ErrorLoadDriver;
        return IOModulInterface_p();
    }
    void* interface = dlsym(dynamicHandle, "create");
    if (interface == nullptr)
    {
        result = IOManagerResult::ErrorLoadDriver;
        return IOModulInterface_p();
    }
    create = reinterpret_cast<miModul::IOModulInterface* (*) ()>(interface);
    miModul::IOModulInterface* tp = create();
    if (tp == nullptr)
    {
        result = IOManagerResult::ErrorLoadDriver;
        return IOModulInterface_p();
    }
    return IOModulInterface_p(tp);
}

IOManagerResult IOManager::UnLoadModul(const std::string& libraryName, const IOModulInterface_p& obj)
{
    void (*destroy) (miModul::IOModulInterface*);

    void* dynamicHandle = dlopen(libraryName.c_str(), RTLD_NOW);

    if (0 == dynamicHandle)
    {
        return IOManagerResult::ErrorLoadDriver;
    }
    void* interface = dlsym(dynamicHandle, "destroy");
    if (interface == nullptr)
    {
        return IOManagerResult::ErrorLoadDriver;
    }
    destroy = reinterpret_cast<void (*) (miModul::IOModulInterface*)>(interface);
    destroy(obj.get());
    return IOManagerResult::Ok;
}

IOManagerResult miIOManager::IOManager::ReadConfig(const std::string& configuration, miIOImage::IOImageOffset startInputOffset, miIOImage::IOImageOffset startOutputOffset)
{
    rapidjson::Document d;
    IOManagerResult result = IOManagerResult::Ok;
    IOMap iMaps;
    IOMap oMaps;

    d.Parse(configuration.c_str());
    if (!d.HasMember("mimoduldescription"))
    {
        return IOManagerResult::ErrorLoadDriver;
    }
    const rapidjson::Value& mimoduldescription = d["mimoduldescription"];

   
    if (!mimoduldescription.HasMember("name"))
    {
        return IOManagerResult::ErrorConfiguration;
    }
    const rapidjson::Value& name = mimoduldescription["name"];

    result = ReadIOMaps(mimoduldescription, std::string("imaps"), iMaps, startInputOffset);
    if(result != IOManagerResult::Ok)
    {
        return result;
    }
    result = ReadIOMaps(mimoduldescription, std::string("omaps"), oMaps, startOutputOffset);
    if (result != IOManagerResult::Ok)
    {
        return result;
    }
    std::string libPath = std::string("lib") + std::string(name.GetString()) + std::string(".so");
    IOModulInterface_p tp = LoadModul(libPath, result);
    if (result != IOManagerResult::Ok)
    {
        return result;
    }

    IOModulDescription desc = IOModulDescription(
        std::string(libPath),
        std::string(name.GetString()),
        std::string(configuration),
        tp,
        iMaps,
        oMaps);

    if (desc.Interface().get()->Open(configuration) != miModul::IOModulResult::Ok)
    {
        return IOManagerResult::ErrorLoadDriver;
    }

    _ModulList[desc.Name()] = desc;

    return IOManagerResult::Ok;
}

IOManagerResult IOManager::ReadIOMaps(const rapidjson::Value& d,std::string key, IOMap& ioMaps,const miIOImage::IOImageOffset startOffset)
{
    if (!d.HasMember(key.c_str()))
    {
        return IOManagerResult::ErrorConfiguration;
    }
    const rapidjson::Value& iomaps = d[key.c_str()];


    if (iomaps.Size() == 0)
    {
        return  IOManagerResult::ErrorConfiguration;
    }

    for (rapidjson::SizeType i = 0; i < iomaps.Size(); i++)
    {
        int32_t id = 0;
        int32_t offset = 0;
        int32_t bitSize = 0;
        int32_t driverSpecific;
        const rapidjson::Value& map = iomaps[i];
        if (!map.HasMember("id"))
        {
            return IOManagerResult::ErrorConfiguration;
        }
        id = map["id"].GetInt();
        if (!map.HasMember("offset"))
        {
            return IOManagerResult::ErrorConfiguration;
        }
        offset = map["offset"].GetInt();
        if (!map.HasMember("bitsize"))
        {
            return IOManagerResult::ErrorConfiguration;
        }
        bitSize = map["bitsize"].GetInt();
        if (!map.HasMember("driverspecific"))
        {
            return IOManagerResult::ErrorConfiguration;
        }
        driverSpecific = map["driverspecific"].GetInt();

        miModul::IOModulIOMap nMap(id, driverSpecific, offset + startOffset, bitSize);
        ioMaps.iOMapMap[id] = nMap;
        ioMaps._BitSize = bitSize;
        bitSize += bitSize;
    }
    return IOManagerResult::Ok;
}

IOManager::IOManager(miIOImage::IOImageSize inputImageSize, miIOImage::IOImageSize outputImageSize)
	:_InputImage(inputImageSize, miIOImage::IOImageType::Input,"input")
	, _OutputImage(outputImageSize, miIOImage::IOImageType::Output,"output")
    , _Timer("buscycle",this)
    , _State(IOManagerResult::Ok)
{

}

IOManagerResult IOManager::AddIOModul(const std::string& configuration)
{
    IOManagerResult result =  ReadConfig(configuration, _ActInputOffset, _ActOutputOffset);
    if (result != IOManagerResult::Ok)
    {
        return result;
    }
    _ActInputOffset += std::prev(_ModulList.end())->second.IMap()._BitSize;
    _ActOutputOffset += std::prev(_ModulList.end())->second.OMap()._BitSize;
    return result;
}

IOManagerResult miIOManager::IOManager::AddIOModul(const miIOImage::IOImageOffset inputOffset, const miIOImage::IOImageOffset outputOffset, const std::string& configuration)
{
    return  ReadConfig(configuration, inputOffset,outputOffset);
}

IOManagerResult IOManager::RemoveIOModul(const std::string& name)
{
    return IOManagerResult();
}

IOManagerResult miIOManager::IOManager::ReadInputs()
{
    miModul::IOModulResult result = miModul::IOModulResult::Ok;
    for (const auto& n : _ModulList)
    {
        if (n.second.Interface().get() == nullptr)
        {
            continue;
        }
        for (const auto& m : n.second.IMap().iOMapMap)
        {
            result = n.second.Interface().get()->ReadInputs(_InputImage, m.second);
        }
    }
    if(result != miModul::IOModulResult::Ok)
    {
        return IOManagerResult::ErrorModulRead;
    }
    return IOManagerResult();
}

IOManagerResult miIOManager::IOManager::WriteOutputs()
{
    miModul::IOModulResult result = miModul::IOModulResult::Ok;
    for (const auto& n : _ModulList)
    {
        if (n.second.Interface().get() == nullptr)
        {
            continue;
        }
       
        for (const auto& m : n.second.OMap().iOMapMap)
        {
            result = n.second.Interface().get()->WriteOutputs(_OutputImage, m.second);
        }
    }
    if (result != miModul::IOModulResult::Ok)
    {
        return IOManagerResult::ErrorModulWrite;
    }
    return IOManagerResult();
}

IOManagerResult IOManager::StartIOCycle(int cycleTime)
{
    if (_Timer.start(cycleTime, this, 50, miutils::Schedulers::Fifo) != miutils::TimerResults::ErrorOk)
    {
        return IOManagerResult::ErrorStart;
    }
    return IOManagerResult::Ok;
}

IOManagerResult IOManager::StopIOCycle()
{
    _Timer.stop();
    return IOManagerResult::Ok;
}

IOManagerResult miIOManager::IOManager::StartIOModulCycle()
{
    for (const auto& n : _ModulList)
    {
        if (n.second.Interface().get() == nullptr)
        {
            continue;
        }
        if (n.second.Interface().get()->Start() != miModul::IOModulResult::Ok)
        {
            return IOManagerResult::ErrorModulStart;
        }
    }
    return IOManagerResult::Ok;
}

IOManagerResult miIOManager::IOManager::StopIOModulCycle()
{
    for (const auto& n : _ModulList)
    {
        if (n.second.Interface().get() == nullptr)
        {
            continue;
        }
        if (n.second.Interface().get()->Stop() != miModul::IOModulResult::Ok)
        {
            return IOManagerResult::ErrorModulStart;
        }
    }
    return IOManagerResult::Ok;
}

IOManagerResult miIOManager::IOManager::IOModulControl(const std::string& name, const std::string function, uint32_t parameter)
{
    if (_ModulList.count(name) == 0)
    {
        return IOManagerResult::ErrorModulNotFound;
    }
    if (_ModulList[name].Interface().get()->Control(name, function, parameter) != miModul::IOModulResult::ErrorControl)
    {
        return IOManagerResult::ErrorModulCtrl;
    }
    return IOManagerResult::Ok;
}

void miIOManager::IOManager::eventOccured(void* sender, const std::string& name)
{
    miModul::IOModulResult result = miModul::IOModulResult::Ok;
    if (sender == nullptr)
    {
        return;
    }
    
    for (const auto& n : _ModulList)
    {
        if (n.second.Interface().get() == nullptr)
        {
            continue;
        }
        for (const auto& m : n.second.IMap().iOMapMap)
        {
            result = n.second.Interface().get()->ReadInputs(_InputImage, m.second);
        }
        if (result != miModul::IOModulResult::Ok)
        {
            _State = IOManagerResult::ErrorModulRead;
        }
        for (const auto& m : n.second.OMap().iOMapMap)
        {
            result = n.second.Interface().get()->WriteOutputs(_OutputImage, m.second);
        }

        if (result != miModul::IOModulResult::Ok)
        {
            _State =  IOManagerResult::ErrorModulWrite;
        }
        
    }
}
