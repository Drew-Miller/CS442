#ifndef INCLUDED_DEATH_STAR

//
// The "teapot" module implements the Teapot class (see below).
//

using namespace std;

#include "irregular_mesh.h"
#include "coordinate_axes.h"
#include "geometry.h"
#include "scene_object.h"
#include "shader_programs.h"
#include "surface.h"

extern string dsFname; // name of the file containing the car model

#define DEATH_STAR_FNAME "death_star_normal.obj"
#define RUST_SPHERE_FNAME "death_star_rust.obj"

class DeathStar : public SceneObject
//
// SceneObject representing the classic Newell teapot
//
{
private:
    // A Teapot is made up of a collection of BezierPatches.
    IrregularMesh *irregularMesh;
    CoordinateAxes *coordinateAxes;

public:
    DeathStar(void);

    void display(const Transform &viewProjectionTransform,
                 Transform worldTransform);
};

#define INCLUDED_DEATH_STAR
#endif // INCLUDED_DEATH_STAR
