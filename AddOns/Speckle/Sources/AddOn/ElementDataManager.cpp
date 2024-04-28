#include "APIdefs.h"
#include "APIEnvir.h"
#include "ACAPinc.h"
#include "ElementDataManager.h"
#include "PropertyManager.h"
#include "NotImplementedException.h"
#include "ElementTypeToStringconverter.h"
#include "CheckError.h"

static API_Element GetElementByGuid(const API_Guid& guid)
{
	API_Element	elem{};
	elem.header.guid = guid;
	CHECK_ERROR(ACAPI_Element_Get(&elem));
	return elem;
}

ElementDataManager::ElementDataManager()
{
}

ElementDataManager::~ElementDataManager()
{
}

bool ElementDataManager::IsAnythingSelected()
{
	API_SelectionInfo	selectionInfo;
	GS::Array<API_Neig>	selNeigs;
	GSErrCode err = ACAPI_Selection_Get(&selectionInfo, &selNeigs, false, false);
	return err == NoError;
}

std::vector<API_Guid> ElementDataManager::GetSelectedElemetIds(std::optional<std::set<API_ElemTypeID>> inclusionFilter)
{
	std::vector<API_Guid> elementGuids;

	API_SelectionInfo	selectionInfo;
	GS::Array<API_Neig>	selNeigs;
	CHECK_ERROR(ACAPI_Selection_Get(&selectionInfo, &selNeigs, false, false));
	BMKillHandle((GSHandle*)&selectionInfo.marquee.coords);

	for (const API_Neig& neig : selNeigs)
	{
		API_Guid guid{};
		CHECK_ERROR(ACAPI_Selection_GetSelectedElement(&neig, &guid));
		API_Element elem = GetElementByGuid(guid);
		API_ElemTypeID elemType = elem.header.type.typeID;

		if (inclusionFilter)
		{
			const auto& filter = inclusionFilter.value();
			if (filter.find(elemType) != filter.end())
			{
				elementGuids.push_back(guid);
			}
		}
		else
		{
			elementGuids.push_back(guid);
		}
	}

	return elementGuids;
}


std::map<API_ElemTypeID, int> ElementDataManager::GetSelectedElementTypes()
{
	const auto& selectedElementGuids = GetSelectedElemetIds();
	std::map<API_ElemTypeID, int> elementTypes;

	for (const API_Guid& guid : selectedElementGuids)
	{
		API_Element element = GetElementByGuid(guid);
		API_ElemTypeID elemType = element.header.type.typeID;
		if (elementTypes.find(elemType) != elementTypes.end()) 
		{
			elementTypes[elemType]++;
		}
		else 
		{
			elementTypes[elemType] = 1;
		}
	}

	return elementTypes;
}

std::vector<API_PropertyDefinition> ElementDataManager::GetElementPropertyDefinitions(const API_Guid& elemId, const API_PropertyDefinitionFilter& propertyDefinitionFilter)
{
	std::vector<API_PropertyDefinition> propertyDefinitions;
	GS::Array<API_PropertyDefinition> definitions;
	CHECK_ERROR(ACAPI_Element_GetPropertyDefinitions(elemId, propertyDefinitionFilter, definitions));

	for (const auto& definition : definitions)
	{
		propertyDefinitions.push_back(definition);
	}

	return propertyDefinitions;
}

nlohmann::json ElementDataManager::GetElementDataAsJson(const API_Guid& elemId)
{
	nlohmann::json elementJson;
	PropertyManager propertyManager;

	const auto& element = GetElementByGuid(elemId);
	elementJson["_guid"] = APIGuidToString(elemId).ToCStr();
	ElementTypeToStringConverter conveter;
	elementJson["_type"] = conveter.ElementTypeToString(element.header.type.typeID);

	GS::UniString infoString;
	CHECK_ERROR(ACAPI_Element_GetElementInfoString(&elemId, &infoString));

	elementJson["_name"] = infoString.ToCStr();

	const auto& builtInProperties = propertyManager.GetPropertyListAsJson(elemId, GetElementPropertyDefinitions(elemId, API_PropertyDefinitionFilter_FundamentalBuiltIn));
	elementJson["built in properties"] = builtInProperties;

	// You need to create User Defined Properties in ArchiCad and assign values to them to see anything other than "null" under "user defined properties"
	const auto& userDefinedProperties = propertyManager.GetPropertyListAsJson(elemId, GetElementPropertyDefinitions(elemId, API_PropertyDefinitionFilter_UserDefined));
	elementJson["user defined properties"] = userDefinedProperties;

	return elementJson;
}

nlohmann::json ElementDataManager::GetElementDataListAsJson(const std::vector<API_Guid>& elemIds)
{
	nlohmann::json elementsDataJson;
	
	API_ProjectInfo projectInfo{};
	CHECK_ERROR(ACAPI_ProjectOperation_Project(&projectInfo));
	elementsDataJson["_project name"] = projectInfo.projectName->ToCStr();

	for (const API_Guid& elemId : elemIds)
	{
		elementsDataJson["elements"].push_back(GetElementDataAsJson(elemId));
	}

	return elementsDataJson;
}
