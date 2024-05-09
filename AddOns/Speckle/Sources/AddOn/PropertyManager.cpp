#include "APIdefs.h"
#include "APIEnvir.h"
#include "ACAPinc.h"
#include "PropertyManager.h"
#include "NotImplementedException.h"
#include "CheckError.h"
#include "PerformanceStats.h"

#include <chrono>


PropertyManager::PropertyManager()
{
}

PropertyManager::~PropertyManager()
{
}

nlohmann::json PropertyManager::GetPropertyAsJson(const API_Guid& elemId, const API_PropertyDefinition& propertyDefinition)
{
	auto& stats = PerformanceStats::GetInstance();
	auto start = std::chrono::high_resolution_clock::now();

	nlohmann::json propertyJson;

	std::string propertyGuid = APIGuidToString(propertyDefinition.guid).ToCStr();
	std::string propertyName = propertyDefinition.name.ToLowerCase().ToCStr();
	std::string propertyValue = "";
	std::string propertyValueType = "";

	API_Property  prop = {};
	CHECK_ERROR(ACAPI_Element_GetPropertyValue(elemId, propertyDefinition.guid, prop));

	auto end = std::chrono::high_resolution_clock::now();
	std::chrono::duration<double> duration = end - start;
	stats.IncreasePropertyReadTime(duration.count());
	
	if (propertyDefinition.valueType == API_PropertyStringValueType && propertyDefinition.collectionType == API_PropertySingleCollectionType)
	{
		propertyValueType = "string";
		propertyValue = prop.value.singleVariant.variant.uniStringValue.ToCStr();	
	}
	else if (propertyDefinition.valueType == API_PropertyIntegerValueType && propertyDefinition.collectionType == API_PropertySingleCollectionType)
	{
		propertyValueType = "integer";
		propertyValue = std::to_string(prop.value.singleVariant.variant.intValue);
	}
	else if (propertyDefinition.valueType == API_PropertyRealValueType && propertyDefinition.collectionType == API_PropertySingleCollectionType)
	{
		propertyValueType = "real";
		propertyValue = std::to_string(prop.value.singleVariant.variant.doubleValue);
	}
	else
	{
		// TODO implement other cases
		throw NotImplementedException();
	}

	if (propertyValue != "")
	{
		propertyJson["guid"] = propertyGuid;
		propertyJson["name"] = propertyName;
		propertyJson["value"] = propertyValue;
		propertyJson["value type"] = propertyValueType;
	}

	return propertyJson;
}

nlohmann::json PropertyManager::GetPropertyListAsJson(const API_Guid& elemId, const std::vector<API_PropertyDefinition>& propertyDefinitionList)
{
	auto& stats = PerformanceStats::GetInstance();
	auto start = std::chrono::high_resolution_clock::now();

	nlohmann::json propertyListJson;

	for (const auto& propertyDefinition : propertyDefinitionList)
	{
		try
		{
			const auto& propertyJson = GetPropertyAsJson(elemId, propertyDefinition);
			if (!propertyJson.empty())
			{
				std::string propertyGroupName = GetGroupNameOfPropertyDefinition(propertyDefinition);
				propertyListJson[propertyGroupName].push_back(propertyJson);
			}
		}
		catch (const NotImplementedException&)
		{
			// TODO handle this
			//std::cout << ex.what();
		}
	}

	auto end = std::chrono::high_resolution_clock::now();
	std::chrono::duration<double> duration = end - start;
	stats.IncreasePropertyListBuildTime(duration.count());

	return propertyListJson;
}

std::string PropertyManager::GetGroupNameOfPropertyDefinition(const API_PropertyDefinition& propertyDefinition)
{
	API_PropertyGroup group;
	group.guid = propertyDefinition.groupGuid;
	CHECK_ERROR(ACAPI_Property_GetPropertyGroup(group));

	std::string groupName = group.name.ToCStr();
	return groupName;
}
