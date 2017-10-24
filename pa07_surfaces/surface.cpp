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
    Point3 *vertexPositions = new Point3[nJ * nI];
    Vector3 *vertexNormals = new Vector3[nJ * nI];


    double v = 0.0;
    for(int j = 0; j < nJ; j++){
      double u = 0.0;

      for(int i = 0; i < nI; i++){
        Vector3 tangentU, tangentV;
        Point3 p = (*this)(u, v, tangentU, tangentV);

        Vector3 n = (tangentV.cross(tangentU)).normalized();

        vertexPositions = p;
        vectexNormals = n;

        // increment u
        u += 1.0 / (nI + wrapI - 1);
      }

      v += 1.0 / (nJ + wrapJ - 1);
    }

    tessellationMesh = new RegularMesh(vertexPositions, vertexNormals, nI, nJ, true, false);

    delete[] vertexPositions;
    delete[] vertexNormals;
}
