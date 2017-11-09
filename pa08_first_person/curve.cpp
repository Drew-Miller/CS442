//
// This file will deal with all single-parametric curves.
//

#include "curve.h"
#include "geometry.h"
#include "poly_line.h"
#include "wrap_cmath_inclusion.h"

#include <iostream>

const Point3 BSplineCurve::operator()(const double u, Vector3 *dp_du) const
{
    assert(0.0 <= u && u <= 1.0);
    int nKnot = ( isClosed ? nCvs : nCvs - 3 );

    //  map `u` to [ 0, nKnot ]
    double t = nKnot * u; // between 0 and nKnot, inclusive

    // `iKnot` is the integer part of t. It will determine the first
    // index of `cvs` that controls the part of the curve we're
    // interested in.
    int iKnot = (int) t; // truncates (always, since t >= 0)
    // Note that if `u` is 1.0, `iKnot` could now equal `nKnot` which
    // could cause trouble if we use it (with a value of 0 to 3,
    // inclusive added to it) to index `cvs`. The solution for this
    // depends on whether the curve is closed or not, and we'll take
    // care of it below.

    t -= iKnot; // `t` retains the fractional part

    if (!isClosed && iKnot == nKnot) {
        // If we don't (need to) use the modulus in the evaluation
        // loop below we really want `iKnot` less than `nKnot`, so if
        // iKnot is nCvs, decrement it and increment `t` (which is
        // almost certainly 0) by 1. The evaluation should be the same
        // because of continuity.
        assert(t == 1.0); // sanity check: the only time this should happen.
        iKnot = nKnot - 1;
        t = 1.0;
    }

    // compute the bases functions and, optionally, their derivatives
    double bs[4], db_dts[4];
    basis(t, bs, ( dp_du ? &db_dts : NULL ));

    Point3 p(0,0,0), dp_dt(0,0,0);
    for (int i = 0; i < 4; i++) {
        int j;

        if (isClosed)
            j = (iKnot + i) % nCvs; // the modulus "wraps" the `cvs` indices
        else
            j = iKnot + i;

        assert(0 <= j && j < nCvs);
        p += bs[i] * cvs[j];
        if (dp_du)
            dp_dt += db_dts[i] * cvs[j];
    }
    if (dp_du) {
        // effectively, `nKnot` is dt/du
        *dp_du = Vector3(dp_dt.u.a) * nKnot; // derivative of spline is a vector
    }
    return p;
}


const Transform Curve::coordinateFrame(const double u) const
{
    //
    // ASSIGNMENT (PA08)
    //
    // Modify the return of this function to return a Transform of the
    // local coordinate frame to NDC coordinates. All of its elements
    // can be taken from your PA06 work.
    //
    // Everywhere that calls this method will have to be modified to
    // use the Transform. (See OffsetCurve::operator() below for an
    // example.)
    //
    // 13 lines in instructor solution (YMMV)
    //

    // get dp_du from the constructor
    Vector3 tangent;
    // get the point and the tangent vector from the curve
    Point3 p = (*this)(u, &tangent);

    // get vW and normalize it
    Vector3 vW = tangent.normalized();
    Vector3 vU = tangent.cross(vNeverParallel).normalized();
    Vector3 vV = vU.cross(vW).normalized();

    // I guess loops?
    // couldn't get this to work, just hardcoded the constructor
    /*
    Transform transform();

    for(int i = 0; i < 3){
      transform.a[i + 0 ] = vU.u.a[i];
      transform.a[i + 4 ] = vV.u.a[i];
      transform.a[i + 8 ] = vW.u.a[i];
      transform.a[i + 12] = p.u.a[i];
    }

    transform.a[3 ] = 0.0;
    transform.a[7 ] = 0.0;
    transform.a[11] = 0.0;
    transform.a[15] = 1.0;
    */

    // pass in the values into the transform matrix
    Transform transform = Transform(vU.u.g.x, vW.u.g.x, vV.u.g.x, p.u.g.x,
                                    vU.u.g.y, vW.u.g.y, vV.u.g.y, p.u.g.y,
                                    vU.u.g.z, vW.u.g.z, vV.u.g.z, p.u.g.z,
                                    0.0,      0.0,      0.0,      1.0);
                                    // 0 for vectors, 1 for points

    return transform; // replace (permits template to compile cleanly)
}


const Point3 LineSegment::operator()(const double u, Vector3 *dp_du) const
{
    if (dp_du)
        (*dp_du) = p1 - p0;
    // simple linear interpolation of the endpoints
    return p0 + u * (p1 - p0);
}


const Point3 OffsetCurve::operator()(const double u, Vector3 *dp_du) const
{
    Point3 p;

    (*frameCurve)(u, dp_du);
    Transform transform = frameCurve->coordinateFrame(u);
    // offset is a vector, but we want to transform it like a point,
    // so...
    return transform * (Point3(0,0,0) + offset);
}


const Point3 TrigonometricCurve::operator()(const double u,
                                            Vector3 *dp_du) const
//
// returns ths position of the curve at the parameter `u`
// (0 <= `u` <= 1).  If `dp_du` is not NULL, it will be set to the
// path derivative vector at `u`.
//
// Note: the path derivative is *not* normalized. If the components
// of the path had dimensions of length (i.e. the path was a
// position) and `u` had dimensions of time, the path derivative
// would be a velocity. If the caller wants a unit-length
// (dimensionless) tangent vector, it is up to them to normalize
// `dp_du` upon its return.
//
{
    //
    // Copy your previous (PA06) solution here.
    //
    Vec3 a = 2 * M_PI * (freq * u + phase);

    // Null Check
    if(dp_du){
    // Derivative: p = x * cos(2 * PI * (frequency * u + phase));
    //            dp/du = x * 2 * PI * frequency * -sin(2 * PI * (frequency * u + phase));
    //         OR dp/du = x * 2 * PI * frequency * -sin(a.u.g.x), a = 2 * PI * (frequency * u + phase)
    //            substitute variables as needed for x,y,z
      Vec3 iSin(-sin(a.u.g.x), -sin(a.u.g.y), -sin(a.u.g.z));

      dp_du->u.g.x = mag.u.g.x * iSin.u.g.x * freq.u.g.x * 2 * M_PI;
      dp_du->u.g.y = mag.u.g.y * iSin.u.g.y * freq.u.g.y * 2 * M_PI;
      dp_du->u.g.z = mag.u.g.z * iSin.u.g.z * freq.u.g.z * 2 * M_PI;
    }

    double x = mag.u.g.x * cos(a.u.g.x) + offset.u.g.x;
    double y = mag.u.g.y * cos(a.u.g.y) + offset.u.g.y;
    double z = mag.u.g.z * cos(a.u.g.z) + offset.u.g.z;

    return Point3(x, y, z);
}
