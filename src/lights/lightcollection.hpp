#pragma once

#include "light.hpp"
#include "util/jsonutil.hpp"

Light* newLight(const Json& conf);