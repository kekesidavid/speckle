#pragma once

#include "json.hpp"

#include <string>

class IElementDataExporter {
public:
    virtual ~IElementDataExporter() {}

    virtual void Export(const nlohmann::json& elementsData) = 0;
};
