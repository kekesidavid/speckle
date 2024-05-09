#pragma once

#include "APIdefs.h"
#include "json.hpp"

#include <vector>
#include <set>
#include <map>
#include <optional>

class PropertyManager
{
	
public:
	PropertyManager();
	~PropertyManager();
	nlohmann::json GetPropertyAsJson(const API_Guid& elemId, const API_PropertyDefinition& propertyDefinition);
	nlohmann::json GetPropertyListAsJson(const API_Guid& elemId, const std::vector<API_PropertyDefinition>& propertyDefinitionList);
	nlohmann::json GetPropertyListAsJson2(const API_Guid& elemId, const std::vector<API_PropertyDefinition>& propertyDefinitionList);
	std::string GetGroupNameOfPropertyDefinition(const API_PropertyDefinition& propertyDefinition);
};