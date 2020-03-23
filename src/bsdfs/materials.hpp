#pragma once

#include "util/jsonutil.hpp"
class BSDF;

BSDF* newMaterial(const Json& conf);