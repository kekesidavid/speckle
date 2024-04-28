#pragma once

#include "IElementDataExporter.h"

#include <vector>
#include <string>


class JsonElementDataExporter :  public IElementDataExporter
{
public:

    void Export(const nlohmann::json& elementsData) override;
    void WriteJsonToFile(const nlohmann::json& jsonData, const std::string& filePath);
};
