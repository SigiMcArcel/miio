#include <fstream>
#include <iostream>
#include <sstream> //std::stringstream
#include "IOModulBase.h"

miModul::IOModulResult miModul::IOModulBase::ReadModulConfiguration(const std::string& descriptionFile)
{
	rapidjson::Document d;
	std::string descriptionPath;
	std::ifstream inFile;
	std::stringstream strStream;
	descriptionPath.append(_IOModuleDescriptionPath);
	descriptionPath.append(std::string("/"));
	descriptionPath.append(descriptionFile);
	descriptionPath.append(std::string(".json"));
	
	std::cerr << "mimodule opens " << descriptionPath << std::endl;
	inFile.open(descriptionPath.c_str()); //open the input file
	if (!inFile.good())
	{
		inFile.close();
		std::cerr << __PRETTY_FUNCTION__ << " :DescriptionPath Not found " << descriptionPath << std::endl;
		return IOModulResult::ErrorConfigurationFileNotFound;
	}
	strStream << inFile.rdbuf(); //read the file
	std::string jsonString = strStream.str(); //str holds the content of the file
	d.Parse(jsonString.c_str());
	if (!d.HasMember("mimoduldescription"))
	{
		_State = IOModulResult::ErrorConf;
		return _State;
	}
	const rapidjson::Value& mimoduldescription = d["mimoduldescription"];

	if (!mimoduldescription.HasMember("drivername"))
	{
		_State = IOModulResult::ErrorConf;
		return _State;
	}
	const rapidjson::Value& name = mimoduldescription["drivername"];
	if (std::string(name.GetString()) != _Name)
	{
		_State = IOModulResult::ErrorDescriptionNotMatch;
		return _State;
	}

	if (!mimoduldescription.HasMember("modulconf"))
	{
		return IOModulResult::ErrorConf;
	}
	const rapidjson::Value& modulconf = mimoduldescription["modulconf"];

	std::cerr << "mimodule inputs " << std::endl;
	if (modulconf.HasMember("inputs"))
	{
		const rapidjson::Value& inputs = modulconf["inputs"];
		if (inputs.Size() == 0)
		{
			_State = IOModulResult::ErrorConf;
			return _State;
		}
		for (rapidjson::SizeType i = 0; i < inputs.Size(); i++)
		{
			_State = ReadDriverSpecificInputConfig(inputs[i]);
			return _State;
		}
	}
	std::cerr << "mimodule outputs " << std::endl;
	if (modulconf.HasMember("outputs"))
	{
		const rapidjson::Value& outputs = modulconf["outputs"];
		if (outputs.Size() == 0)
		{
			_State = IOModulResult::ErrorConf;
			return _State;
		}

		for (rapidjson::SizeType i = 0; i < outputs.Size(); i++)
		{
			_State = ReadDriverSpecificInputConfig(outputs[i]);
			return _State;
		}
	}
	return IOModulResult();
}

