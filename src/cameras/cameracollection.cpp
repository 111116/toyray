#include "cameracollection.hpp"

#include "pinholecamera.hpp"
#include "thinlenscamera.hpp"

Camera* newCamera(const Json& conf) {
	if (conf["type"] == "pinhole")
		return new PinholeCamera(conf);
	if (conf["type"] == "thinlens")
		return new ThinlensCamera(conf);
	throw "unrecognized camera type";
}