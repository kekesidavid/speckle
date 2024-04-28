#include "APIdefs.h"
#include "DGModule.hpp"
#include "DGFileDialog.hpp"
#include "JsonElementDataExporter.h"
#include "json.hpp"

#include <iostream>
#include <fstream>
#include <optional>

static std::optional<std::string> GetSaveLocation()
{
	DG::FileDialog dialog(DG::FileDialog::Save);
	dialog.SetTitle("Select destination file");

	FTM::FileTypeManager manager("JSON files");
	FTM::FileType type(nullptr, "json", 0, 0, 0);
	FTM::TypeID id = FTM::FileTypeManager::SearchForType(type);

	if (id == FTM::UnknownType) 
	{
		id = manager.AddType(type);
	}

	dialog.AddFilter(id);

	IO::Location location;
	if (dialog.Invoke()) 
	{
		location = dialog.GetSelectedFile();
		std::string locationString = location.ToDisplayText().ToCStr();
		return locationString;
	}
	else 
	{
		return std::nullopt;
	}
}


void JsonElementDataExporter::Export(const nlohmann::json& elementsData)
{
	const auto& filePath = GetSaveLocation();
	if (filePath)
	{
		try
		{
			WriteJsonToFile(elementsData, filePath.value());
		}
		catch (const std::exception& ex)
		{
			std::string msg = ex.what();
			DG::ErrorAlert(msg.c_str(), "", "OK");
		}
	}
}

void JsonElementDataExporter::WriteJsonToFile(const nlohmann::json& jsonData, const std::string& filePath)
{
    std::ofstream outputFile(filePath);

    if (!outputFile.is_open()) {
        std::cerr << "Error: Unable to open file for writing: " << filePath << std::endl;
        return;
    }

    outputFile << std::setw(4) << jsonData << std::endl;

    outputFile.close();

	std::string msg = "JSON data successfully written to file: " + filePath;
    DG::InformationAlert(msg.c_str(), "", "OK");
}
