#include "APIEnvir.h"
#include "ACAPinc.h"
#include "ExportDialog.h"
#include "ElementDataManager.h"
#include "JsonElementDataExporter.h"

#include <chrono>

static void ShowNothingSelectedWarning()
{
	DG::WarningAlert("Nothing is selected. Please select items to export!", "", "OK");
}

enum class PropertyValueReadMode { OneByOne, Batched };

static GSErrCode GetAllPropertyValuesBatched(const API_Guid& elemGuid, GS::Array<API_PropertyValue>& values)
{
	GS::Array<API_PropertyDefinition> definitions;
	GSErrCode error = ACAPI_Element_GetPropertyDefinitions(elemGuid, API_PropertyDefinitionFilter_FundamentalBuiltIn, definitions);
	if (error == NoError)
	{
		GS::Array<API_Property>  properties;
		error = ACAPI_Element_GetPropertyValues(elemGuid, definitions, properties);
		if (error == NoError) 
		{
			for (UInt32 i = 0; i < properties.GetSize(); i++) 
			{
				if (properties[i].isDefault) 
				{
					values.Push(properties[i].definition.defaultValue.basicValue);
				}
				else 
				{
					values.Push(properties[i].value);
				}
			}
		}
	}

	return error;
}

static GSErrCode GetAllPropertyValuesOneByOne(const API_Guid& elemGuid, GS::Array<API_PropertyValue>& values)
{
	GS::Array<API_PropertyDefinition> definitions;
	GSErrCode error = ACAPI_Element_GetPropertyDefinitions(elemGuid, API_PropertyDefinitionFilter_FundamentalBuiltIn, definitions);
	if (error == NoError) 
	{
		GS::Array<API_Property>  properties;

		for (const auto& def : definitions)
		{
			API_Property prop;
			ACAPI_Element_GetPropertyValue(elemGuid, def.guid, prop);
			if (prop.isDefault)
			{
				values.Push(def.defaultValue.basicValue);
			}
			else
			{
				values.Push(prop.value);
			}
		}
	}

	return error;
}

static bool Test(bool testing, PropertyValueReadMode mode)
{
	if (!testing)
		return false;

	ElementDataManager elementDataManager;

	if (!elementDataManager.IsAnythingSelected())
	{
		ShowNothingSelectedWarning();
		return true;
	}

	auto start = std::chrono::high_resolution_clock::now();

	const auto& elemIds = elementDataManager.GetSelectedElemetIds();
	for (const auto& i : elemIds)
	{
		GS::Array<API_PropertyValue> values;
		if (mode == PropertyValueReadMode::Batched)
		{
			GetAllPropertyValuesBatched(i, values);
		}
		else
		{
			GetAllPropertyValuesOneByOne(i, values);
		}
	}

	auto end = std::chrono::high_resolution_clock::now();
	std::chrono::duration<double> duration = end - start;

	return true;
}

static void	ShowExportDialog()
{
	if (Test(false, PropertyValueReadMode::OneByOne)) return;

	try
	{
		ElementDataManager elementDataManager;

		if (!elementDataManager.IsAnythingSelected())
		{
			ShowNothingSelectedWarning();
			return;
		}

		auto elementTypes = elementDataManager.GetSelectedElementTypes();

		std::set<API_ElemTypeID> inclusionfilter;
		ExportDialog dialog(elementTypes, &inclusionfilter);
		bool result = dialog.Invoke();

		if (result)
		{
			JsonElementDataExporter exporter;
			exporter.Export(inclusionfilter);
		}
	}
	catch (const std::exception& ex)
	{
		// TODO more sophisticated error handling
		std::string msg = ex.what();
		DG::ErrorAlert(msg.c_str(), "", "OK");
	}
}

static void	QuickExport()
{
	if (Test(true, PropertyValueReadMode::Batched)) return;

	try
	{
		ElementDataManager elementDataManager;

		if (!elementDataManager.IsAnythingSelected())
		{
			ShowNothingSelectedWarning();
			return;
		}

		JsonElementDataExporter exporter;
		exporter.Export(std::nullopt);
	}
	catch (const std::exception& ex)
	{
		// TODO more sophisticated error handling
		std::string msg = ex.what();
		DG::ErrorAlert(msg.c_str(), "", "OK");
	}
}

GSErrCode __ACENV_CALL MenuCommandHandler(const API_MenuParams *menuParams)
{
	switch (menuParams->menuItemRef.menuResID) 
	{
		case 32500:
			switch (menuParams->menuItemRef.itemIndex) 
			{
				case 1:
					ShowExportDialog();
					break;
				case 2:		
					QuickExport();
					break;
				default:				
					break;
			}
			break;
		default:
			break;
	}

	return NoError;
}

API_AddonType __ACENV_CALL CheckEnvironment(API_EnvirParams* envir)
{
	RSGetIndString(&envir->addOnInfo.name, 32000, 1, ACAPI_GetOwnResModule());
	RSGetIndString(&envir->addOnInfo.description, 32000, 2, ACAPI_GetOwnResModule());

	return APIAddon_Normal;
}

GSErrCode __ACENV_CALL RegisterInterface(void)
{
	GSErrCode err = ACAPI_MenuItem_RegisterMenu(32500, 32510, MenuCode_UserDef, MenuFlag_Default);

	if (err != NoError)
		ACAPI_WriteReport("RegisterInterface() ACAPI_Register_Menu failed\n", false);

	return err;
}

GSErrCode __ACENV_CALL Initialize(void)
{
	GSErrCode err = ACAPI_MenuItem_InstallMenuHandler(32500, MenuCommandHandler);

	if (err != NoError)
		ACAPI_WriteReport("Initialize() ACAPI_Install_MenuHandler failed\n", false);
	return err;
}		

GSErrCode __ACENV_CALL	FreeData(void)
{
	return NoError;
}
