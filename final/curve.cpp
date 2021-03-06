//
// This file will deal with all single-parametric curves.
//

#include "curve.h"
#include "geometry.h"
#include "poly_line.h"
#include "scene.h"
#include "wrap_cmath_inclusion.h"


const double Curve::dS(const double u, const double du) const
//
// performs a single step of a Simpson's rule integration of the curve
// length differentials ("dp_du") over [ u, u+du ], returning the
// distance increment that corresponds to the parametric increment.
//
// This is about 50% less efficient than retaining previous kernel
// evaluations (see Curve::length()), but until C++ provides us with
// generators (for integration), it's the easiest way to do this.
//
{
    double h = du / 2.0;
    Vector3 dp_du[3];

    (*this)(u,       &dp_du[0]);
    (*this)(u +   h, &dp_du[1]);
    (*this)(u + 2*h, &dp_du[2]);
    //
    // Since the intergrand does not depend on the previous values of
    // itself (dp_du), Simpson's rule integration is the equivalent
    // of the ever-popular 4th order Runge-Kutta (aka "RK4").
    //
    return du * (dp_du[0].mag() + 4.0 * dp_du[1].mag() + dp_du[2].mag()) / 6.0;
}


const double Curve::length(void) const
//
// returns the length of the curve in NDC units
//
{
    double result = 0.0;
    int nIntervals = 512;
    // Simpson's rule uses approximately 2 samples per interval.
    int nSamples = 2 * nIntervals + 1; // guaranteed odd
    double h = 1.0 / (nSamples - 1);
    for (int i = 0; i < nSamples; i++) {
        // Simpson's rule should be good enough
        Vector3 dp_du;
        (*this)(i * h, &dp_du);
        double wt = ( (i == 0 || i == nSamples - 1)
                      ? 1.0 // at endpoints, otherwise...
                      : ( i % 2
                          ? 4.0 // at odd i
                          : 2.0 // at even i
                          ) );
        result += wt * dp_du.mag();
    }
    result *= h / 3.0;
    //
    // Enable the following to compare fast (this method) and slow
    // (dS()) integrations. (As we don't call length() very often,
    // this will cause very little overhead, but disable it if that
    // becomes untrue and for some reason you don't want to cache the
    // length() result.)
    //
    // This is also an example of how to use `dS()`.
    //
# if 0 // enable for testing
    double testResult = 0.0;
    double du = 1.0 / nIntervals;
    for (int i = 0; i < nIntervals; i++)
        testResult += dS(i * du, du);
    assert(fabs(result - testResult) < EPSILON);
# endif
    return result;
}


const double Curve::zMax(void) const
//
// returns the maximum z value along the curve
//
{
    //
    // ASSIGNMENT (PA09)
    //
    // Move along the parametric curve looking for the maximum z
    // value, which you return. Choose at least 1000 steps, but don't
    // overdo it.
    //
    // (A cleverer, faster, and more reliable way would be to look for
    // extrema of the z component, but this would require skills in
    // numerical analysis, a good elective for computer graphicists to
    // take for reasons like this, but we don't assume it here.)
    //
    int steps = 1000;
    double zMax = 0.0;
    Point3 p;
    double u = 0.0;

    // find z max along the curve
    for(int i = 0; i < steps; i++){
        u = (double)i / steps;
        p = (*this)(u, NULL);

        if(p.u.g.z > zMax)
          zMax = p.u.g.z;
    }

    return zMax; // replace (permits template to compile cleanly)
}


const Point3 BSplineCurve::operator()(const double u, Vector3 *dp_du,
    Vector3 *d2p_du2) const
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
    double bs[4], db_dts[4], d2b_dt2s[4];
    basis(t, bs, ( dp_du ? &db_dts : NULL ), ( d2p_du2 ? &d2b_dt2s : NULL ));

    Point3 p(0,0,0), dp_dt(0,0,0), d2p_dt2(0,0,0);
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
        if (d2p_du2)
            d2p_dt2 += d2b_dt2s[i] * cvs[j];
    }
    if (dp_du) {
        // effectively, `nKnot` is dt/du
        *dp_du = Vector3(dp_dt.u.a) * nKnot; // derivative of spline is a vector
    }
    if (d2p_du2) {
        // effectively, `nKnot * nKnot` is d2t/du2

        // derivative of spline is a vector
        *d2p_du2 = Vector3(d2p_dt2.u.a) * nKnot * nKnot;
    }
    return p;
}


const Transform Curve::coordinateFrame(const double u) const
{
    //
    // ASSIGNMENT (PA09)
    //
    // Modify your previous (PA08) solution as follows:
    //
    // - If the curve is dynamic (`frameIsDynamic`), compute a local
    //   `vNeverParallel` which is a vector `c - g`, where
    //
    //   + `g` is the acceleration due to gravity
    //
    //     a vector in the -z direction with a magnitude of `gravAccel`
    //
    //   + `c` is the centripetal acceleration, which is
    //
    //       d2p_du2 * (speed / ds_du)**2
    //
    //     where:
    //
    //       * `d2p_du2` the second path derivative (a vector) is
    //         given by the curve (via *this()).
    //
    //       * `speed` is given by `scene->track->speed(u)`
    //
    //       * `ds_du` is the magnitude of `dp_du`, which is also
    //          provided by the curve.
    //
    // - If the curve is not dynamic, continue to use the given
    //   `vNeverParallel`.
    //
    // 25 lines in instructor solution (YMMV)
    //

    Vector3 neverParallel;

    if(frameIsDynamic){
        Vector3 g = Vector3(0.0, 0.0, -1 * gravAccel);

        Vector3 dp_du, d2p_du2;

        (*this)(u, &dp_du, &d2p_du2);

        double speed = scene->track->speed(u);

        double ds_du = dp_du.mag();

        Vector3 c = d2p_du2 * pow((speed / ds_du), 2);

        neverParallel = c - g;
    }
    else {
        neverParallel = vNeverParallel;
    }

    // get dp_du from the constructor
    Vector3 tangent;
    // get the point and the tangent vector from the curve
    Point3 p = (*this)(u, &tangent);

    // get vW and normalize it
    Vector3 vW = tangent.normalized();
    Vector3 vU = tangent.cross(neverParallel).normalized();
    Vector3 vV = vU.cross(vW).normalized();

    // pass in the values into the transform matrix
    Transform transform = Transform(vU.u.g.x, vW.u.g.x, vV.u.g.x, p.u.g.x,
                                    vU.u.g.y, vW.u.g.y, vV.u.g.y, p.u.g.y,
                                    vU.u.g.z, vW.u.g.z, vV.u.g.z, p.u.g.z,
                                    0.0,      0.0,      0.0,      1.0);
                                    // 0 for vectors, 1 for points

    return transform; // replace (permits template to compile cleanly)
}


const Point3 LineSegment::operator()(const double u, Vector3 *dp_du,
    Vector3 *d2p_du2) const
{
    if (dp_du)
        (*dp_du) = p1 - p0;
    if (d2p_du2) // linear, so 2nd derivative is zero
        (*d2p_du2) = Vector3(0.0, 0.0, 0.0);
    // simple linear interpolation of the endpoints
    return p0 + u * (p1 - p0);
}


const Point3 OffsetCurve::operator()(const double u,
                                     Vector3 *dp_du, Vector3 *d2p_du2) const
{
    Point3 p;

    (*frameCurve)(u, dp_du, d2p_du2);
    Transform transform = frameCurve->coordinateFrame(u);
    // offset is a vector, but we want to transform it like a point,
    // so...
    return transform * (Point3(0,0,0) + offset);
}


const Point3 TrigonometricCurve::operator()(const double u, Vector3 *dp_du,
    Vector3 *d2p_du2) const
//
// returns ths position of the curve at the parameter `u` (0 <= `u`
// <= 1).  If `dp_du` is not NULL, it will be set to the path
// derivative vector at `u`. If `d2p_du2` is not NULL, it will be set
// the the second path derivative vector.
//
// Note: the path derivatives are *not* normalized. If the components
// of the path had dimensions of length (i.e. the path was a
// position) and `u` had dimensions of time, the path derivatives
// `dp_du` and `d2p_du2` would be a velocity and an acceleration,
// respectively. If the caller wants a unit-length (dimensionless)
// vector, it is up to them to normalize a derivative upon its
// return.
//
{
    //
    // ASSIGNMENT (PA09)
    //
    // Enhance your previous (PA06) solution to set `*d2p_du2` to the
    // (second) path derivative as per the function header. Use your
    // knowledge of calculus to compute it.
    //
    // 15 lines in instructor solution (YMMV)
    //
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
    // second derivative
    if(d2p_du2){
      double pi2 = M_PI * M_PI;

      Vec3 iCos(-cos(a.u.g.x), -cos(a.u.g.y), -cos(a.u.g.z));

      d2p_du2->u.g.x = mag.u.g.x * iCos.u.g.x * freq.u.g.x * freq.u.g.x * 4 * pi2;
      d2p_du2->u.g.y = mag.u.g.y * iCos.u.g.y * freq.u.g.y * freq.u.g.y * 4 * pi2;
      d2p_du2->u.g.z = mag.u.g.z * iCos.u.g.z * freq.u.g.z * freq.u.g.z * 4 * pi2;
    }


    double x = mag.u.g.x * cos(a.u.g.x) + offset.u.g.x;
    double y = mag.u.g.y * cos(a.u.g.y) + offset.u.g.y;
    double z = mag.u.g.z * cos(a.u.g.z) + offset.u.g.z;

    return Point3(x, y, z);
}
