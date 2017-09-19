//#REMOVE - remove later
//#HERE - current problem
//#LATER - something else to do

#include <vector>
#include <iostream>
#include <fstream>
#include <math.h>

#include "circle.h"
#include "n_elem.h"
#include "poly_line.h"
#include "wrap_cmath_inclusion.h"

// define PI value
#define PI 3.14159265358979323846

int getNFromR(double r){
  /// contants given
  double p = .00119;
  double d = p / 2;
  double x = r - d;

  // variables to compute
  double theta, theta2;
  int n;

  // find side and theta based on r
  theta2 = acos(x/r);
  theta = theta2 * 2;

  // then find n
  n = (2*PI/theta);

  return n;
}

// function to create a vector and return it based on r
vector<Point2> getPoint2Vector(double r){
  int i;
  int n = getNFromR(r);
  double theta = 2 * PI / n;

  vector<Point2> v;
  Point2 p;

  // loop through the coordinates and determine the points
  for(i = 0; i < n; i++){
    // create the coordinates
    p = Point2(r * cos(theta * i), r * sin(theta * i));
    // push them onto the vector
    v.push_back(p);
  }

  return v;
}


void Circle::draw(void)
{
    // If the circle has not been tessellated, tessellate it.
    if (!tessellationPolyLine)
        tessellate();

    // Now draw it.
    tessellationPolyLine->render();
}


void Circle::tessellate(void)
//
// tessellates the circle
//
{
    //
    // ASSIGNMENT (PA01)
    //
    // Replace the square PolyLine code below with code that uses the
    // formula you derived to set `tessellationPolyLine` to a PolyLine
    // representing a polygon with the minimum number of sides to be
    // within a half-pixel of a circle of the given `radius`.
    //
    // PolyLine() copies `vertexPosition`, so if you use dynamic
    // storage (and you should), remember to clean up after yourself.
    //
    // 19 lines in instructor solution (YMMV)
    //
    vector<Point2> v;

    v = getPoint2Vector(radius);
    tessellationPolyLine = new PolyLine(&v[0],
                                        v.size(), true);
}
