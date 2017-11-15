#include <iostream>
using namespace std;

#include "curve.h"
#include "geometry.h"
#include "scene.h"
#include "tube.h"
#include "wrap_cmath_inclusion.h"

const Point3 Tube::operator()(const double u, const double v,
        Vector3 &dp_du, Vector3 &dp_dv) const
//
// returns a Point3 on the surface of the tube. `u` maps to the
// aziumuthal angle around the tube. `v` maps to the axial position
// along the curve determined by the of the guiding curve. As
// parameters. They both vary from 0 to 1.
//
{
    //
    // ASSIGNMENT (PA08)
    //
    // Modify your previous (PA07) solution to get and use the
    // transform that `curve->coordinateFrame(v)` now returns. The
    // cleanest way to do this is construct a Point3 on a circle in
    // the UV plane (i.e., in model coordinates) and apply the
    // transform to it. Also apply it to model coordinate tangent
    // vectors to get (tangential) `dp_du` and (axial) `dp_dv`
    // vectors. (Think: How would you define the tangents to a
    // cylinder in model coordinates?)
    //
    // Note that, unlike normal vectors, tangent vectors transform
    // like any other vectors.
    //
    // 7 lines in instructor solution (YMMV)
    //

    Point3 p;

    // get the transform from the coordinateFrame
    Transform transform = curve->coordinateFrame(v);

    // get the angle still
    double u_rad = 2 * M_PI * u;

    Point3 onCircle = Point3(radius * cos(u_rad), radius * sin(u_rad), 0.0);

    Vector3 vU(-1 * radius * sin(u_rad), radius * cos(u_rad), 0.0);
    Vector3 vV(0, 0, 1);


    // derive p with respect to u, 2 * pi * u = u_rad
    p = transform * onCircle;
    dp_du = transform * vU;
    dp_dv = transform * vV;

    return p; // replace (permits template to compile cleanly)
}
