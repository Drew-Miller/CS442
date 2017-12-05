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

#define DEATH_STAR_FNAME "death_star.obj"

class DeathStar : public SceneObject
//
// SceneObject representing the classic Newell teapot
//
{
    // A Teapot is made up of a collection of BezierPatches.
    IrregularMesh *irregularMesh;

    // Each patch will be made of the same material and will get an nI
    // x nJ tessellation.
    static const int nI = 8;
    static const int nJ = 8;
    Vector3 offset; // displacement from the modeling coordinate origin

public:
    DeathStar(void);

    void display(const Transform &viewProjectionTransform,
                 Transform worldTransform);
};

#define INCLUDED_DEATH_STAR
#endif // INCLUDED_DEATH_STAR
