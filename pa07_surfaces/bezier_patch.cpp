#include "bezier_patch.h"

const Point3 BezierPatch::operator()(const double u, const double v,
                                     Vector3 &dp_du, Vector3 &dp_dv) const
{
    //
    // ASSIGNMENT (PA07)
    //
    // Make this function return a Point3 lying on patch at the
    // parametric coordinates (`u`, `v`). Call `basis()` for both `u`
    // and `v`.
    //
    // 15 lines in instructor solution (YMMV)
    //
    double u_bs[4], v_bs[4], db_dus[4], db_dvs[4];
    Point3 p(0, 0, 0);


    // get the u values and the dp_du and dp_dv values
    basis(u, u_bs, db_dus);
    basis(v, v_bs, db_dvs);

    // set the control points fo the BezierPatch
    // cvs[4][4], iterate through all coordiantes and set
    // from data from basis
    for(int i = 0; i < 4; i++ ){
      for(int j = 0; j < 4; j++){
        Point3 cv = cvs[i][j];

        // offset the point from the basis
        p += cv * u_bs[i] * v_bs[j];

        // have to change the derivates as well
        dp_du += db_dus[i] * cv;
        dp_dv += db_dvs[j] * cv;
      }
    }

    return p; // replace (permits template to compile cleanly)
}
