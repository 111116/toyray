#pragma once

#include "util/jsonutil.hpp"
class BSDF;

BSDF* newMaterial(const Json& conf, const std::unordered_map<std::string, BSDF*>& bsdfref);