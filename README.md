a naive path tracer (CG course project). **under active development**

## Compile and Run

```bash
mkdir build
cd build
cmake ..
make -j
./toyray
```

## Implemented functions

##### supporting shape types:

- triangle
- triangle mesh
- sphere
- infinite plane
- square
- cube
- transformed geometries

##### supporting materials:

- lambert reflective
- mirror

##### supporting light types:

- diffuse area light
- point light
- directional light
- constant environment light
- environment map (light probe)

##### supporting input format:

- Wavefront OBJ (ASCII) for shapes
- Tungsten styled json for scene configuration

##### supporting output format:

- EXR
- BMP

## Other notes

If you decide to learn path tracing by reading this code, I recommend start by looking at the path tracing core `src/renderer.cpp`.

Scenes are available at [toyray-scenes](https://github.com/111116/toyray-scenes) made by other artists, mostly converted by [Benedikt Bitterli](https://benedikt-bitterli.me/).

