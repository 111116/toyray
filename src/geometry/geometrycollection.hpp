#pragma once

#include "geometry.hpp"
#include "jsonutil.hpp"

void instantiateGeometry(const Json& conf);
Primitive* newPrimitive(const Json& conf);
