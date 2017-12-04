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
    // Copy your previous (PA07) solution here.
    //
    Point3 *vertexPositions = new Point3[nJ * nI];
    Vector3 *vertexNormals = new Vector3[nJ * nI];

    double v = 0.0;

    // slower incrementer
    for(int j = 0; j < nJ; j++){
      double u = 0.0;

      // most rapidly-varying incrementer
      for(int i = 0; i < nI; i++){
        Vector3 tangentU, tangentV;

        // get point
        Point3 p = (*this)(u, v, tangentU, tangentV);

        // get n from tU and tV
        Vector3 n = (tangentV.cross(tangentU)).normalized();

        // set them in the arrays
        vertexPositions[j * nI + i] = p;
        vertexNormals[j * nI + i] = n;

        // increment u
        u += 1.0 / (nI + wrapI - 1);
      }
      // increment v
      v += 1.0 / (nJ + wrapJ - 1);
    }

    // mesh up
    tessellationMesh = new RegularMesh(vertexPositions, vertexNormals, nI, nJ, wrapI, wrapJ);

    // delete em
    delete[] vertexPositions;
    delete[] vertexNormals;
}
