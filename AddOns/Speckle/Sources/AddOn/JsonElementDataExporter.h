#pragma once

#include "IElementDataExporter.h"

#include <vector>
#include <string>


class JsonElementDataExporter :  public IElementDataExporter
{
public:

    void Export(std::optional<std::set<API_ElemTypeID>> inclusionFilter) override;
    void WriteJsonToFile(const nlohmann::json& jsonData, const std::string& filePath);
};
