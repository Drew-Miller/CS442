#include "coordinate_axes.h"
#include "geometry.h"
#include "poly_line.h"
#include "scene.h"
#include "transform.h"


CoordinateAxes::CoordinateAxes(void)
{
    Point3 pXs[] = { Point3(0, 0, 0), Point3(1, 0, 0) };
    Point3 pYs[] = { Point3(0, 0, 0), Point3(0, 1, 0) };
    Point3 pZs[] = { Point3(0, 0, 0), Point3(0, 0, 1) };

    xAxis = new PolyLine(pXs, 2, false);
    yAxis = new PolyLine(pYs, 2, false);
    zAxis = new PolyLine(pZs, 2, false);
}


void CoordinateAxes::display(const Transform &viewTransform)
//
// draws red, green, and blue unit-length lines for the x, y, and z
// axes
//
{
    // set transform

    scene->uniformColorShaderProgram->setColor(redColor);
    scene->uniformColorShaderProgram->start();

    xAxis->render(viewTransform);

    scene->uniformColorShaderProgram->setColor(greenColor);
    scene->uniformColorShaderProgram->start();
    yAxis->render(viewTransform);

    scene->uniformColorShaderProgram->setColor(blueColor);
    scene->uniformColorShaderProgram->start();
    zAxis->render(viewTransform);
}
