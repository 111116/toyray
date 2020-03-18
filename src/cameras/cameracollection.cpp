#include "cameracollection.hpp"

#include "pinholecamera.hpp"

Camera* newCamera(const Json& conf) {
	if (conf["type"] == "pinhole")
		return new PinholeCamera(conf);
	throw "unrecognized camera type";
}