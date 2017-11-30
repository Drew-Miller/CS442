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
    // Copy your previous (PA08) solution here.
    //
    Point3 p;
    Vector3 vU, vW, vV;

    // get the transform from the coordinateFrame
    Transform transform = curve->coordinateFrame(v);

    // get the angle still
    double u_a = 2 * M_PI * u;

    // get vU, vW, vV and p from the transform
    p = Point3(transform.a[12], transform.a[13], transform.a[14]);
    vU = Vector3(transform.a[0], transform.a[1], transform.a[2]);
    vW = Vector3(transform.a[4], transform.a[5], transform.a[6]);
    vV = Vector3(transform.a[8], transform.a[9], transform.a[10]);

    // get p on the circle
    p = p + vU * radius * cos(u_a) + vV * radius * sin(u_a);

    // set dp_du and dp_dv
    dp_du = 2 * M_PI * (vU * radius * -sin(u_a) + vV * radius * cos(u_a));
    dp_dv = vW;

    return p;
}
