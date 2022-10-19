#include "IOModulBase.h"

miModul::IOModulResult miModul::IOModulBase::ReadModulConfiguration(const std::string& configuration)
{
	rapidjson::Document d;
	d.Parse(configuration.c_str());
	if (!d.HasMember("mimoduldescription"))
	{
		_State = IOModulResult::ErrorConf;
		return _State;
	}
	const rapidjson::Value& mimoduldescription = d["mimoduldescription"];

	if (!mimoduldescription.HasMember("name"))
	{
		_State = IOModulResult::ErrorConf;
		return _State;
	}
	const rapidjson::Value& name = mimoduldescription["name"];
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
			ReadDriverSpecificInputConfig(inputs[i]);
		}
	}

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
			ReadDriverSpecificInputConfig(outputs[i]);
		}
	}
	return IOModulResult();
}

