#pragma once

#include "json.hpp"

#include <string>
#include <optional>
#include <set>

class IElementDataExporter {
public:
    virtual ~IElementDataExporter() {}

    virtual void Export(std::optional<std::set<API_ElemTypeID>> inclusionFilter) = 0;
};
