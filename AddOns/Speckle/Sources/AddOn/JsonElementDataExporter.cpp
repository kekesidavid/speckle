#include "APIdefs.h"
#include "DGModule.hpp"
#include "DGFileDialog.hpp"
#include "JsonElementDataExporter.h"
#include "json.hpp"
#include "ElementDataManager.h"
#include "PerformanceStats.h"

#include <iostream>
#include <fstream>
#include <optional>
#include <chrono>

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

void JsonElementDataExporter::Export(std::optional<std::set<API_ElemTypeID>> inclusionFilter)
{
	const auto& filePath = GetSaveLocation();

	auto& stats = PerformanceStats::GetInstance();
	auto start = std::chrono::high_resolution_clock::now();

	if (filePath)
	{
		try
		{
			ElementDataManager elementDataManager;			
			const auto& selectedElementIds = elementDataManager.GetSelectedElemetIds(inclusionFilter);	
			const auto& elementJsonData = elementDataManager.GetElementDataListAsJson(selectedElementIds);

			if (elementJsonData.empty())
			{
				DG::ErrorAlert("Nothing to write, elementJsonData was empty!", "", "OK");
				return;
			}

			WriteJsonToFile(elementJsonData, filePath.value());
		}
		catch (const std::exception& ex)
		{
			std::string msg = ex.what();
			DG::ErrorAlert(msg.c_str(), "", "OK");
		}
	}
	else
	{
		// no filePath given, do nothing
	}

	auto end = std::chrono::high_resolution_clock::now();
	std::chrono::duration<double> duration = end - start;
	stats.IncreaseExportTime(duration.count());

	stats.PrintStats();
	stats.Clear();
}

void JsonElementDataExporter::WriteJsonToFile(const nlohmann::json& jsonData, const std::string& filePath)
{
	auto& stats = PerformanceStats::GetInstance();
	auto start = std::chrono::high_resolution_clock::now();

    std::ofstream outputFile(filePath);

    if (!outputFile.is_open()) 
	{
		std::string msg = "Error: Unable to open file for writing: " + filePath;
		DG::ErrorAlert(msg.c_str(), "", "OK");
        return;
    }

    outputFile << std::setw(4) << jsonData << std::endl;
    outputFile.close();

	auto end = std::chrono::high_resolution_clock::now();
	std::chrono::duration<double> duration = end - start;
	stats.IncreaseFileIOTime(duration.count());

	std::string msg = "JSON data successfully written to file: " + filePath;
    DG::InformationAlert(msg.c_str(), "", "OK");
}
