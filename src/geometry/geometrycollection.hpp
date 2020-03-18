#pragma once

#include "geometry.hpp"
#include "util/jsonutil.hpp"

#include <unordered_map>
#include "triangle.hpp"
#include "trianglemesh.hpp"
#include "sphere.hpp"
#include "plane.hpp"
#include "quad.hpp"
#include "cube.hpp"

void instantiateGeometry(const Json& conf);
Primitive* newPrimitive(const Json& conf);

