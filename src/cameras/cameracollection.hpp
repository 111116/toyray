#pragma once

#include "util/jsonutil.hpp"
#include "camera.hpp"

Camera* newCamera(const Json& conf);