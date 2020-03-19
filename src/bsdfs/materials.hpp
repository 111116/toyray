#pragma once

#include "bsdf.hpp"
#include "util/jsonutil.hpp"

BSDF* newMaterial(const Json& conf);