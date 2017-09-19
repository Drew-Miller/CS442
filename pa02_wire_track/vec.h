#ifndef INCLUDED_VEC

//
// The "vec" module provides the Vec3 and Mat4 classes (see below).
//
// This implements host equivalents of the GLSL vec3 and mat4 classes,
// although these are double precision.
//


// We use the Vec3 class as a base class for representing 3-valued
// entities like points, vectors, and colors.

#include <iostream>
using namespace std;

//
// Floating point calculations, (e.g. Vec3 magnitudes), that result in
// quantities with absolute values less than EPSILON should be
// considered zero due to the effects of roundoff. If we ever choose
// to use (single precision) float values, this quantity should be
// increased to, say, 1.0e-6.
//
const double EPSILON = 1.0e-12;

class Vec3
//
// a 3D vector (low level)
//
{
public:
    union { // good example (IMHO) of union use: notational convenience
        struct {
            double x, y, z;
        } g;         // when used as individual [g]eometric components
        struct {
            double r, g, b;
        } c;         // when used as individual [c]olor components
        double a[3]; // when used as array
    } u;

    Vec3(void)
        { u.g.x = 0.0;   u.g.y = 0.0;   u.g.z = 0.0; };

    Vec3(const double x, const double y, const double z)
        { u.g.x = x;     u.g.y = y;     u.g.z = z; };

    Vec3(const double a[3])
        { u.a[0] = a[0]; u.a[1] = a[1]; u.a[2] = a[2]; };

    const bool isZero(void) const
    {
        return u.a[0] == 0.0 && u.a[1] == 0.0 && u.a[2] == 0.0;
    }

    Vec3 &operator+=(const Vec3 &vec)
    {
        u.a[0] += vec.u.a[0];
        u.a[1] += vec.u.a[1];
        u.a[2] += vec.u.a[2];
        return *this;
    };

    const Vec3 operator+(const Vec3 &vec) const
    {
        return Vec3(*this) += vec;
    };

    Vec3 &operator*=(const Vec3 &vec)
    {
        u.a[0] *= vec.u.a[0];
        u.a[1] *= vec.u.a[1];
        u.a[2] *= vec.u.a[2];
        return *this;
    };

    const Vec3 operator*(const Vec3 &vec) const
    {
        return Vec3(*this) *= vec;
    };

    Vec3 &operator*=(double scale)
    {
        u.a[0] *= scale;
        u.a[1] *= scale;
        u.a[2] *= scale;
        return *this;
    };

    const Vec3 operator*(double scale) const
    {
        return Vec3(*this) *= scale;
    };
};


inline Vec3 operator*(const double scale, const Vec3 &vec)
{
    return vec * scale;
};

ostream& operator<<(ostream &out, const Vec3 t);

class Mat4
//
// a 4x4 matrix (low level)
//
{

public:
    double a[16];

    inline int ij(const int i, const int j) const
    //
    // maps (i, j) (i.e., (row, column)) to the 0 - 15 index of the
    // matrix using the OpenGL (column-major) instead of C/C++
    // (row-major) convention
    //
    {
        return 4*j + i; // OpenGL order (column-major)
    }
};

ostream& operator<<(ostream &out, const Mat4 mat);


#define INCLUDED_VEC
#endif // INCLUDED_VEC
