#pragma once

#include "ElementData.h"
#include "json.hpp"

#include <vector>
#include <set>
#include <map>
#include <optional>

class ElementDataManager
{
	
public:
	ElementDataManager();
	~ElementDataManager();
	bool IsAnythingSelected();
	std::vector<API_Guid> GetSelectedElemetIds(std::optional<std::set<API_ElemTypeID>> inclusionFilter = std::nullopt);
	std::map<API_ElemTypeID, int> GetSelectedElementTypes();
	std::vector<API_PropertyDefinition> GetElementPropertyDefinitions(const API_Guid& elemId, const API_PropertyDefinitionFilter& propertyDefinitionFilter);
	nlohmann::json GetElementDataAsJson(const API_Guid& elemId);
	nlohmann::json GetElementDataListAsJson(const std::vector<API_Guid>& elemIds);
};