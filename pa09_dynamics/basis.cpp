#include <assert.h>

#include "basis.h"

const void BezierBasis::operator()(const double u,
    double bs[4], double (*db_dus)[4], double (*d2b_du2s)[4]) const
// returns the values of the 4 cubic Bezier basis functions, otherwise
// known as the Bernstein polynomials and (if `db_dus` is not NULL)
// their derivatives.
{
    //
    // ASSIGNMENT (PA09)
    //
    // If coaster required the second derivatives of the Bezier basis
    // (for some unknown reason), this would be the place to also do
    // the following:
    //
    // - Add code to evaluate the *second* derivatives of the 4
    //   Bernstein polynomials at `u` and put the results in
    //   (*d2b_du2s)`. (Don't bother if the pointer is NULL.)
    //
    // But since coaster does not (and, at this writing, will not)
    // require them, you don't need to support this. Instead, simply
    // assert that `d2b_du2s` is NULL so you'll know if you ever need
    // to add it again should you reuse this code for another
    // project.
    //
    // Again, you would be welcome to look at
    // `UniformCubicBSplineBasis::operator()` as an example.
    //
    // 13 lines in instructor solution (YMMV)
    //

    // pretty much same as below function without the division
    double u2 = u * u;
    double u3 = u * u * u;

    // bo = (1-t) ^ 3
    // b1 = 3t (1-t)^2
    // b2 = 3t^2 (1-t)
    // b3 = t^3
    // expanded from those functions:
    bs[0] = 1 - 3 * u + 3 * u2 - u3;
    bs[1] = 3 * u - 6 * u2 + 3 * u3;
    bs[2] = 3 * u2 - 3 * u3;
    bs[3] = u3;

    // if we want the derivative also
    if(db_dus){
      // derivatives
      (*db_dus)[0] = -3 + 6 * u - 3 * u2;
      (*db_dus)[1] = 3 - 12 * u + 9 * u2;
      (*db_dus)[2] = 6 * u - 9 * u2;
      (*db_dus)[3] = 3 * u2;
    }
    // check if we need to take the second derivative
    if(d2b_du2s){
      // second derivatives
      (*d2b_du2s)[0] = 6 - 6 * u;
      (*d2b_du2s)[1] = -12 + 18 * u;
      (*d2b_du2s)[2] = 6 - 18 * u;
      (*d2b_du2s)[3] = 6 * u;
    }
}

const void UniformCubicBSplineBasis::operator()(
    const double u, double bs[4],
    double (*db_dus)[4], double (*d2b_du2s)[4]) const
//
// returns the values of the 4 uniform cubic (aka "N_3") B-spline
// basis functions and (if `db_dus` is not NULL) their derivatives.
// For the time being, we're leaving out the knot sequence details.
//
{
    //
    // These polynomials are a little more difficult to find, so I'll
    // just provide the solution. You can use it as a model for
    // BezierBasis.
    //
    double u2 = u*u;
    double u3 = u2*u;

    //
    // These are the bases ...
    //
    bs[0]    = (-1*u3 + 3*u2 - 3*u + 1) / 6.0;
    bs[1]    = ( 3*u3 - 6*u2       + 4) / 6.0;
    bs[2]    = (-3*u3 + 3*u2 + 3*u + 1) / 6.0;
    bs[3]    = ( 1*u3                 ) / 6.0;

    if (db_dus) {
        //
        // ... and these are their derivatives wrt u. (Notice how the
        // notation lets you confirm this by inspection.)
        //
        (*db_dus)[0] = (-3*u2 +  6*u - 3) / 6.0;
        (*db_dus)[1] = ( 9*u2 - 12*u    ) / 6.0;
        (*db_dus)[2] = (-9*u2 +  6*u + 3) / 6.0;
        (*db_dus)[3] = ( 3*u2           ) / 6.0;
    }
    //
    // ASSIGNMENT (PA09)
    //
    // Add code that, iff `d2b_du2s` is not NULL, evaluates the
    // *second* (linear) derivatives of the 4 cubic B-spline basis
    // functions at `u` and put the results in `(*d2b_du2s)`.
    //
    // Look at the first derivative calculation above as a guideline
    // for a readable layout of the code.
    //
    if (d2b_du2s) {
        //
        // ... and these are their derivatives wrt u. (Notice how the
        // notation lets you confirm this by inspection.)
        //
        (*d2b_du2s)[0] = (-6*u +  6       ) / 6.0;
        (*d2b_du2s)[1] = ( 18*u - 12      ) / 6.0;
        (*d2b_du2s)[2] = (-18*u +  6      ) / 6.0;
        (*d2b_du2s)[3] = ( 6*u            ) / 6.0;
    }
}
