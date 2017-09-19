//
// This file will deal with all single-parametric curves.
//

#include "curve.h"
#include "geometry.h"
#include "poly_line.h"
#include "wrap_cmath_inclusion.h"
#include <math.h>

void TrigonometricCurve::draw(const Transform &viewTransform)
//
// If necessary, tessellate the Curve and then draw it (in
// the given color).
//
{
    //
    // ASSIGNMENT (PA02)
    //
    // Do the following:
    //
    // - If `tessellationPolyLine` is NULL, call `tessellate()` to
    //   create it.
    //
    // - Invoke the `render()` method on the tessellation.
    //
    // This will be very similar to Circle::draw(), but you'll need to
    // pass `viewTransform` to `render()` to implement 3D viewing. How
    // this actually works will be covered in a later unit.
    //
    // 3 lines in instructor solution (YMMV)
    //
    if(!tessellationPolyLine){
        tessellate();
    }

    tessellationPolyLine->render(viewTransform);
}


const Point3 TrigonometricCurve::operator()(const double u) const
//
// returns ths value of the curve at the parameter u (0 <= u <= 1).
//
{
    //
    // ASSIGNMENT (PA02)
    //
    // Compute (the Vec3) angle 2*`M_PI`*(`freq` * `u` + `phase`) and
    // return a Point3 which is the value of computing the cosine of
    // each of its components, weighted by the corresponding
    // components of `mag`, then add `offset` to the returned result.
    //
    // To understand the convenience of operator overloading, use no
    // for loops to do this.
    //
    // `M_PI` comes from the standard math ("math.h") library. Never
    // declare your own value of pi (or e, for that matter).
    //
    // 5 lines in instructor solution (YMMV)
    //
    double x, y, z;
    Vec3 a;
    Point3 p;

    a = 2 * M_PI * (freq * u + phase);

    x = mag.u.g.x * cos(a.u.g.x) + offset.u.g.x;
    y = mag.u.g.y * cos(a.u.g.y) + offset.u.g.y;
    z = mag.u.g.z * cos(a.u.g.z) + offset.u.g.z;

    p = Point3(x, y, z);
    return p; // replace (permits template to compile cleanly)
}


void TrigonometricCurve::tessellate(void)
//
// tessellates the trigonometric curve
//
{
    //
    // ASSIGNMENT (PA02)
    //
    // Add code to create an `nI`-element PolyLine using the curve.
    // Note that (*this)(u) returns a point along the curve and that
    // the curve is closed. (We don't want the car to fall off the
    // tracks!) Also, `u` varies from 0.0 to (`nI` - 1)/`nI` in steps
    // of 1.0/`nI`. (The point at `u` == 1.0 would be identical to
    // that at `u` == 0.0, and we avoid redundant positions throughout
    // coaster.)
    //
    // 9 lines in instructor solution (YMMV)
    //

    //CHANGE
    // Create the parametric curve
    trackTrigonometricCurve = new TrigonometricCurve(mag, freq, phase, offset, nRailSegments);

    // Create the supports
    double supportInterval = 1.0 / nSupports;

    double u;
    int count;

	// Create vertices
	Point3 vertices[nSupports][2];

    for (count = 0, u = 0.0; u < (1.0 - supportInterval); u += supportInterval, count++)
    {
        // Get the height at the u position
        Point3 value = (*trackTrigonometricCurve)(u);

		// Get the ground and height
		Point3 ground(value.u.g.x, value.u.g.y, 0);
		Point3 height(value.u.g.x, value.u.g.y, value.u.g.z);

		vertices[count][0] = ground;
		vertices[count][1] = height;
    }

	supportLines = new Lines(vertices, nSupports);
}
