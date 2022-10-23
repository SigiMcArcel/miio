#include <dlfcn.h>
#include <mi/miio/IOManager.h>
#include <fstream>
#include <iostream>
#include <sstream> //std::stringstream

using namespace miIOManager;

IOManagerResult miIOManager::IOManager::ReadConfig(
    const std::string& descriptionName
    , miIOImage::IOImageOffset startInputOffset
    , miIOImage::IOImageOffset startOutputOffset
    , const std::string& driverspecific
    , IOModulDescription& desc)
{
    rapidjson::Document d;
    IOManagerResult result = IOManagerResult::Ok;
    miIOImage::IOImageOffset bitSizeIn = 0;
    miIOImage::IOImageSize bitSizeOut = 0;
    IOModulValues iValues;
    IOModulValues oValues;
    std::string descriptionPath = _IOModulDescriptionPath + std::string("/") + descriptionName + std::string(".json");

    std::ifstream inFile;
    std::stringstream strStream;
    inFile.open(descriptionPath.c_str()); //open the input file
    if (!inFile.good())
    {
        inFile.close();
        return IOManagerResult::ErrorModulFileNotFound;
    }
    strStream << inFile.rdbuf(); //read the file
    std::string jsonString = strStream.str(); //str holds the content of the file

    d.Parse(jsonString.c_str());
    if (!d.HasMember("mimoduldescription"))
    {
        return IOManagerResult::ErrorLoadDriver;
    }
    const rapidjson::Value& mimoduldescription = d["mimoduldescription"];

   
    if (!mimoduldescription.HasMember("drivername"))
    {
        return IOManagerResult::ErrorConfiguration;
    }
    const rapidjson::Value& drivername = mimoduldescription["drivername"];

    if (!mimoduldescription.HasMember("modulconf"))
    {
        return IOManagerResult::ErrorConfiguration;
    }
    const rapidjson::Value& modulconf = mimoduldescription["modulconf"];

    if (!modulconf.HasMember("inputbitsize"))
    {
        return IOManagerResult::ErrorConfiguration;
    }
    bitSizeIn = modulconf["inputbitsize"].GetInt();

    if (!modulconf.HasMember("outputbitsize"))
    {
        return IOManagerResult::ErrorConfiguration;
    }
    bitSizeOut = modulconf["outputbitsize"].GetInt();

    if (bitSizeIn > 0)
    {
        if (!modulconf.HasMember("inputs"))
        {
            return IOManagerResult::ErrorConfiguration;
        }
        const rapidjson::Value& ivalues = modulconf["inputs"];

        if (ivalues.Size() == 0)
        {
            return  IOManagerResult::ErrorConfiguration;
        }

        for (rapidjson::SizeType i = 0; i < ivalues.Size(); i++)
        {
            IOModulValue value;
            result = ReadIOModuleValueConfig(ivalues[i], value);
            if (result != IOManagerResult::Ok)
            {
                return result;
            }
            iValues[value.Name()] = value;
        }
    }
    if (bitSizeOut > 0)
    {
        if (!modulconf.HasMember("outputs"))
        {
            return IOManagerResult::ErrorConfiguration;
        }
        const rapidjson::Value& ovalues = modulconf["outputs"];

        if (ovalues.Size() == 0)
        {
            return  IOManagerResult::ErrorConfiguration;
        }

        for (rapidjson::SizeType i = 0; i < ovalues.Size(); i++)
        {
            IOModulValue value;
            result = ReadIOModuleValueConfig(ovalues[i], value);
            if (result != IOManagerResult::Ok)
            {
                return result;
            }
            oValues[value.Name()] = value;
        }
    }


    const rapidjson::Value& libname = mimoduldescription["libname"];
    if (!mimoduldescription.HasMember("libname"))
    {
        return IOManagerResult::ErrorConfiguration;
    }
    std::string libPath = std::string("lib") + std::string(libname.GetString()) + std::string(".so");

    
    if (result != IOManagerResult::Ok)
    {
        return result;
    }
    miModul::IOModul modul = miModul::IOModul(libname.GetString());
    if (!modul.Valid())
    {
        return IOManagerResult::ErrorLoadDriver;
    }
    desc = IOModulDescription(
        std::string(libPath),
        std::string(drivername.GetString()),
        std::string(jsonString),
        driverspecific,
        modul,
        iValues,
        oValues,
        bitSizeIn,
        bitSizeOut);

    return IOManagerResult::Ok;
}

IOManagerResult IOManager::ReadIOModuleValueConfig(const rapidjson::Value& d,IOModulValue& moduleValue)
{
    std::string name;
    miIOImage::IOImageOffset bitOffset;
    miIOImage::IOImageSize bitSize;

    if (!d.HasMember("name"))
    {
        return IOManagerResult::ErrorConfiguration;
    }
    name = std::string(d["name"].GetString());
    if (!d.HasMember("bitoffset"))
    {
        return IOManagerResult::ErrorConfiguration;
    }
    bitOffset = d["bitoffset"].GetInt();
    if (!d.HasMember("bitsize"))
    {
        return IOManagerResult::ErrorConfiguration;
    }
    bitSize = d["bitsize"].GetInt();
    
    moduleValue = IOModulValue(name, bitOffset, bitSize);

    return IOManagerResult::Ok;
}

IOManager::IOManager(miIOImage::IOImageSize inputImageSize, miIOImage::IOImageSize outputImageSize)
	:_InputImage(inputImageSize, miIOImage::IOImageType::Input,"input")
	, _OutputImage(outputImageSize, miIOImage::IOImageType::Output,"output")
    , _Timer("buscycle",this)
    , _State(IOManagerResult::Ok)
{

}

IOManagerResult IOManager::AddIOModul(
     const std::string& instanceName
    ,const std::string& descriptionName
    ,const std::string& driverspecific)
{
    IOManagerResult result = AddIOModul(instanceName, descriptionName, driverspecific,_ActInputOffset,_ActOutputOffset);
    if (result != IOManagerResult::Ok)
    {
        return result;
    }
    _ActInputOffset += std::prev(_ModulList.end())->second.IOModulInputBitSize();
    _ActOutputOffset += std::prev(_ModulList.end())->second.IOModulOutputBitSize();
    return IOManagerResult();
}

IOManagerResult miIOManager::IOManager::AddIOModul(
    const std::string& instanceName
    ,const std::string& descriptionFile
    ,const std::string& driverspecific
    ,const miIOImage::IOImageOffset inputOffset
    ,const miIOImage::IOImageOffset outputOffset)
{
    IOModulDescription desc;
    IOManagerResult result = ReadConfig(descriptionFile, inputOffset, inputOffset,driverspecific,desc);
    if (result != IOManagerResult::Ok)
    {
        return result;
    }
    if (desc.IOModul().Open(descriptionFile,desc.DriverSpecific()) != miModul::IOModulResult::Ok)
    {
        return IOManagerResult::ErrorLoadDriver;
    }

    _ModulList[desc.InstanceName()] = desc;
    return IOManagerResult();
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
        if (!n.second.IOModul().Valid())
        {
            continue;
        }
        for (const auto& m : n.second.IOModulInputValues())
        {
            result = n.second.IOModul().ReadInputs(_InputImage,m.second.Offset(), m.second.Size());
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
        if (n.second.IOModul().Valid())
        {
            continue;
        }
       
        for (const auto& m : n.second.IOModulOutputValues())
        {
            result = n.second.IOModul().WriteOutputs(_OutputImage,m.second.Offset(), m.second.Size());
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
   
    if (_Timer.Start(cycleTime, this, 50, miutils::Schedulers::Fifo) != miutils::TimerResults::ErrorOk)
    {
        return IOManagerResult::ErrorStart;
    }
    return IOManagerResult::Ok;
}

IOManagerResult IOManager::StopIOCycle()
{
    _Timer.Stop();
    return IOManagerResult::Ok;
}

IOManagerResult miIOManager::IOManager::StartIOModulCycle()
{
    for (const auto& n : _ModulList)
    {
        if (!n.second.IOModul().Valid())
        {
            continue;
        }
        if (n.second.IOModul().Start() != miModul::IOModulResult::Ok)
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
        if (!n.second.IOModul().Valid())
        {
            continue;
        }
        if (n.second.IOModul().Stop() != miModul::IOModulResult::Ok)
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
    if (_ModulList[name].IOModul().Control(name, function, parameter) != miModul::IOModulResult::ErrorControl)
    {
        return IOManagerResult::ErrorModulCtrl;
    }
    return IOManagerResult::Ok;
}

void miIOManager::IOManager::eventOccured(void* sender, const std::string& name)
{
    miIOManager::IOManagerResult result = miIOManager::IOManagerResult::Ok;
    miIOManager::IOManager* manager = reinterpret_cast<IOManager*>(sender);
    if (sender == nullptr)
    {
        _State = miIOManager::IOManagerResult::ErrorModulRead;
        return;
    }

    result = manager->ReadInputs();
    if (result != miIOManager::IOManagerResult::Ok)
    {
        _State = result;
    }
    if (result != manager->WriteOutputs())
    {
        _State = result;
    }
}
