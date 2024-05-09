#include "APIEnvir.h"
#include "ACAPinc.h"
#include "ExportDialog.h"
#include "ElementDataManager.h"
#include "JsonElementDataExporter.h"

static void ShowNothingSelectedWarning()
{
	DG::WarningAlert("Nothing is selected. Please select items to export!", "", "OK");
}

static void	ShowExportDialog()
{
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
