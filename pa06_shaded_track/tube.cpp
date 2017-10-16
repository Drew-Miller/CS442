#include <iostream>
using namespace std;

#include "curve.h"
#include "geometry.h"
#include "tube.h"
#include "wrap_cmath_inclusion.h"


void Tube::draw(SceneObject *sceneObject)
{
    //
    // ASSIGNMENT (PA06)
    //
    // Add about code that:
    //
    // - checks if `tessellationMesh` is NULL and, if it is,
    //
    //   + calls `tessellate()` to create it
    //
    //   + calls `sceneObject->addHedgehogs(tessellationMesh)`
    //
    // - renders the tessellation
    //
    // Use TrigonometricCurve::draw() as a guideline.
    //
    // Note: This method will go away when it is more generalized in
    // the next assignment.
    //
    // 4 lines in instructor solution (YMMV)
    //
    if(!tessellationMesh){
      tessellate();
      sceneObject->addHedgehogs(tessellationMesh);
    }

    tessellationMesh->render();
}


void Tube::tessellate(void)
{
    //
    // ASSIGNMENT (PA06)
    //
    // Add code to create an `nI` x `nJ` mesh using a trigonometric
    // parameterization. `nI` is along the angular dimension and `nJ`
    // is along the path. Iff `closed` is true, the tube is
    // continuous.
    //
    // Use `curve->coordinateFrame()` to get the position `p` and
    // `vU`, `vV`, and `vW` basis vectors and generate the
    // `vertexPositions[]` Point3's and vertexNormals[] Vector3's from that
    // based on the extrusion formula we derived in class. Remember
    // that the normal to tube with a circular cross section is just
    // the (unit) radial vector.
    //
    // If the tube is closed, use a `u` step size of 1/`nJ`. If is
    // open, use a `u` step size of 1/(`nJ`-1). The former ensures
    // that the triangles defining the last segment of the tube will
    // not be degenerate.
    //
    // Once `vertexPositions[]` are set, return a RegularMesh
    // constructed from them.
    //
    // Note: This method will go away when it is more generalized in
    // the next assignment.
    //
    // 16 lines in instructor solution (YMMV)
    //
    Point3 *vertexPositions = new Point3[nJ * nI];
    Vector3 *vertexNormals = new Vector3[nJ * nI];

    for(int i = 0; i < nI; i++){
      double u = 0.0;

      for(int j = 0; j < nJ; j++){
        Point3 p;
        Vector3 vU, vV, vW;

        double theta = 2 * M_PI * i / nI;

        curve->coordinateFrame(u, p, vU, vV, vW);

        // Vertex Position
        // Get the point along the edge of the line
        // Q = P + R*cos(theta)*U + R*sin(theta)*V
        Point3 q = p + radius * cos(theta) * vU + radius * sin(theta) * vV;
        // get the vertex normal
        Vector3 n = q - p;

        vertexPositions[j * nI + i] = q;
        vertexNormals[j * nI + i] = n;

        // increment u
        if(isClosed)
          u += 1.0 / nJ;
        else
          u += 1.0 / (nJ - 1.0);
      }
    }

    tessellationMesh = new RegularMesh(vertexPositions, vertexNormals, nI, nJ, true, false);

    delete[] vertexPositions;
    delete[] vertexNormals;
}
