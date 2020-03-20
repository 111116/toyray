#include "lightcollection.hpp"
#include "util/jsonutil.hpp"
#include "util/filepath.hpp"

#include "pointlight.hpp"
#include "constlight.hpp"
#include "envmaplight.hpp"
#include "diffuselight.hpp"
#include "directionallight.hpp"


Light* newLight(const Json& conf) {
	if (conf["type"] == "point")
		return new PointLight(json2vec3f(conf["intensity"]), json2vec3f(conf["position"]));
	if (conf["type"] == "directional")
		return new DirectionalLight(json2vec3f(conf["irradiance"]), json2vec3f(conf["direction"]));
	if (conf["type"] == "constant")
		return new ConstEnvLight(json2vec3f(conf["radiance"]));
	if (conf["type"] == "envmap")
		return new EnvLight(new Image(getpath(conf["radiance"])));
	throw "unrecognized type of light source";
}