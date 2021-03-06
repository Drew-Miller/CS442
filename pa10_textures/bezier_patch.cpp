#include "bezier_patch.h"

const Point3 BezierPatch::operator()(const double u, const double v,
                                     Vector3 &dp_du, Vector3 &dp_dv) const
{
    //
    // Copy your previous (PA07) solution here.
    //
    double u_bs[4], v_bs[4], db_dus[4], db_dvs[4];

    // get the u values and the dp_du and dp_dv values
    basis(u, u_bs, &db_dus);
    basis(v, v_bs, &db_dvs);

    // starting point
    Point3 p(0,0,0);

    // set the references
    dp_du = Vector3();
    dp_dv = Vector3();

    // set the control points fo the BezierPatch
    // cvs[4][4], iterate through all coordiantes and set
    // from data from basis
    for(int i = 0; i < 4; i++ ){
      for(int j = 0; j < 4; j++){
        Point3 cv = cvs[i][j];

        // offset the point from the basis
        p += cv * u_bs[i] * v_bs[j];

        // have to change the derivates as well
        // These are point3s
        Point3 offset_u = cv * db_dus[i] * v_bs[j];
        Point3 offset_v = cv * u_bs[i] * db_dvs[j];

        // easy conversion to vec3
        dp_du += Vector3(offset_u.u.a);
        dp_dv += Vector3(offset_v.u.a);
      }
    }

    return p; // replace (permits template to compile cleanly)
}
