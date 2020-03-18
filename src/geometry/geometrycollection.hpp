#pragma once

#include "geometry.hpp"
#include "util/jsonutil.hpp"

void instantiateGeometry(const Json& conf);
Primitive* newPrimitive(const Json& conf);

