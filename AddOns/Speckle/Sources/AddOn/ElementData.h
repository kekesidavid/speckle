#pragma once

#include "APIdefs.h"

#include <string>

struct ElementData 
{
    API_Guid guid;
    API_ElemTypeID elemType;

    ElementData(const API_Guid g, API_ElemTypeID& t) : guid(g), elemType(t) {}
};
