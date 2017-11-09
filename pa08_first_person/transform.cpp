#include "geometry.h"
#include "transform.h"
#include "wrap_cmath_inclusion.h"


const Transform Transform::getNormalTransform(void) const
{
    Transform result = this->inverse().transpose();
    // set 3rd row and column to match identity values
    for (int i = 0; i <= 3; i++) {
        result.a[ij(3,i)] = result.a[ij(i,3)] = (i == 3);
    }
    return result;
};


const Point3 Transform::operator*(const Point3 &point3) const
{
    Point3 result;

    // We don't need i == 3 as 4th dimension of resulting point is
    // implicitly 1.
    for (int i = 0; i < 3; i++) {
        // As the 4th dimension of `point3` is implicitly 1. We can
        // initialize the sum to the last column of a[].
        result.u.a[i] = a[ij(i,3)];
        for (int j = 0; j < 3; j++)
            result.u.a[i] += a[ij(i,j)] * point3.u.a[j];
    }
    return result;
};


const Vector3 Transform::operator*(const Vector3 &vector3) const
{
    Vector3 result;

    // We don't need i == 3 as 4th dimension of resulting vector is
    // implicitly 0.
    for (int i = 0; i < 3; i++) {
        result.u.a[i] = 0.0;
        // We don't need j == 3 as 4th dimension of
        // `vector3`vector is implicitly 0.
        for (int j = 0; j < 3; j++)
            result.u.a[i] += a[ij(i,j)] * vector3.u.a[j];
    }
    return result;
};


void Transform::rotate(const double angle, const Vector3 &direction)
//
// rotates the transform by `angle` around `direction`
//
{
    //
    // This is the hardest of the `Transform::*()` methods. There's no
    // need to modify this method. Note that it operates on the
    // transform "in-place".
    //
    // Remember to test your results by compiling the "transform_t"
    // self test ("-DTEST"), code for which is included at the end of
    // this file.
    //
    Vector3 vU = direction.normalized();
    double cosA = cos(angle);
    double sinA = sin(angle);
    Matrix4 rotationMatrix;
    double s[3][3] = {
        {         0, -vU.u.g.z,  vU.u.g.y },
        {  vU.u.g.z,         0, -vU.u.g.x },
        { -vU.u.g.y,  vU.u.g.x,         0 },
    };

    // Construct the `rotationMatrix` according to the OpenGL
    // documentation.
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            double I_ij = (i == j); // identity matrix
            if (i >= 3 || j >= 3) {
                rotationMatrix.a[ij(i, j)] = I_ij;
            } else {
                double u_uT_ij = vU.u.a[i]*vU.u.a[j];
                rotationMatrix.a[ij(i, j)] = u_uT_ij
                    + cosA * (I_ij - u_uT_ij) + sinA * s[i][j];
            };
        };
    };
    (*this) = (*this) * rotationMatrix;
}


void Transform::scale(const Vector3 &factor)
//
// scales the transform by `factor`
//
{
    //
    // Scale factors of (near) zero are almost certainly errors.
    // Remove these assertions if you ever find a good reason for
    // allowing them.
    //
    assert(fabs(factor.u.a[0]) > EPSILON);
    assert(fabs(factor.u.a[1]) > EPSILON);
    assert(fabs(factor.u.a[2]) > EPSILON);

    Matrix4 scaleMatrix(
        factor.u.a[0], 0.0,           0.0,           0.0,
        0.0,           factor.u.a[1], 0.0,           0.0,
        0.0,           0.0,           factor.u.a[2], 0.0,
        0.0,           0.0,           0.0,           1.0
        );
    (*static_cast<Matrix4 *>(this)) = (*this) * scaleMatrix;
}


void Transform::translate(const Vector3 offset) {
    Matrix4 translateMatrix(
        1.0, 0.0, 0.0, offset.u.a[0],
        0.0, 1.0, 0.0, offset.u.a[1],
        0.0, 0.0, 1.0, offset.u.a[2],
        0.0, 0.0, 0.0, 1.0
        );
    (*this) *= translateMatrix;
}


#ifdef TEST
#include <iostream>
using namespace std;
#include <cstdlib>
#include <cstring>

int main(int argc, char **argv)
{
    Transform t;

    int i = 1;
    while (i < argc) {
        if (strcmp(argv[i], "-r") == 0) {
            // "r angle aX aY aZ" rotates by `angle` (in radians)
            // about (aX, aY, aZ)
            double angle = strtod(argv[i+1], NULL);
            double aX = strtod(argv[i+2], NULL);
            double aY = strtod(argv[i+3], NULL);
            double aZ = strtod(argv[i+4], NULL);
            i += 5;
            t.rotate(angle, aX, aY, aZ);
        } else if (strcmp(argv[i], "-s") == 0) {
            // "s sX sY sZ" scales the transform by (sX, sY, sZ)
            double sX = strtod(argv[i+1], NULL);
            double sY = strtod(argv[i+2], NULL);
            double sZ = strtod(argv[i+3], NULL);
            i += 4;
            t.scale(sX, sY, sZ);
        } else if (strcmp(argv[i], "-t") == 0) {
            // "t tX tY tZ" translates the transform by (tX, tY, tZ)
            double tX = strtod(argv[i+1], NULL);
            double tY = strtod(argv[i+2], NULL);
            double tZ = strtod(argv[i+3], NULL);
            i += 4;
            t.translate(tX, tY, tZ);
        } else {
            cerr << "unknown operation '" << argv[i] << "' -- exiting\n";
            exit(EXIT_FAILURE);
        }
    }

    cout << "after transformations, the identity transform becomes:\n\n";

    cout << "t:\n";
    cout << t << "\n";

    cout << "t.inverse():\n";
    cout << t.inverse() << "\n";

    cout << "t.inverse() * t: (should be ~= I)\n";
    cout << t.inverse() * t << "\n";

    cout << "t * t.inverse(): (should be ~= I)\n";
    cout << t * t.inverse() << "\n";

    cout << "t.transpose():\n";
    cout << t.transpose() << "\n";

    cout << "t.inverse().transpose():\n";
    cout << t.inverse().transpose() << "\n";
}
#endif // TEST
