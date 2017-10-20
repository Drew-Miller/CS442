#include <assert.h>

#include "geometry.h"
#include "mesh.h"
#include "scene_object.h"
#include "surface.h"


void Surface::draw(SceneObject *sceneObject)
{
    if (!tessellationMesh) {
        tessellate();
        sceneObject->addHedgehogs(tessellationMesh);
    }

    tessellationMesh->render();
}

void Surface::tessellate(void)
{
    //
    // ASSIGNMENT (PA07)
    //
    // Construct the `nI` x `nJ` grid of `vertexPositions` (Point3s)
    // and their `vertexNormals` (Vector3s) on the surface and set
    // `tessellationMesh` to a new RegularMesh using them. Remember
    // that the surface itself can be invoked parametrically (as
    // "(*this)(...)").
    //
    // To construct `vertexNormals`, note that "(*this)(...)" now
    // returns partial derivative Vector3's `dp_du` and `dp_dv` and
    // that the surface normal can be constructed from these. Also
    // remember that surface normals must be of unit length.
    //
    // Use the same criteria for `u` step size (depending on closed
    // vs. non-closed) that you used for Tube::tessellate() in PA06
    // (using `wrapJ` here, of course). Also use it for the `v` step
    // size.
    //
    // For consistency with the solution (although it's really
    // arbitrary), make `i` the most rapidly-varying index.
    //
    // 17 lines in instructor solution (YMMV)
    //

    return;
}
