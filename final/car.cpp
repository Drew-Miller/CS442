#include <assert.h>
#include <math.h> // for M_PI

#include "car.h"
#include "controller.h"
#include "scene.h"

//
// Our rollercoaster car design is a simple irregular mesh specified
// in a Wavefront "*.obj" file (default: "car.obj"). Feel free to
// modify that file.
//

// string carFname = DEFAULT_CAR_FNAME;
string carFname = ROCKET_CAR_FNAME;

Car::Car(const Rgb &baseRgb_, double initialU, const Curve *path_)
    :
      irregularMesh(NULL)
{
    coordinateAxes = new CoordinateAxes(); // add this line

    //
    // Copy your previous (PA08) solution here.
    //

    path = path_;
    u = initialU;
    baseRgb = baseRgb_;

    // Since the car's IrregularMesh doesn't need to be tessellated
    // (effectively), we can create the hedgehogs immediately.
    irregularMesh = IrregularMesh::read(carFname.c_str());

    // Since the car's IrregularMesh doesn't need to be tessellated
    // (effectively), we can create the hedgehogs immediately.
    addHedgehogs(irregularMesh);
}

void Car::display(const Transform &viewProjectionTransform,
                  Transform worldTransform)
{
    //
    // Copy your previous (PA08) setting of `modelTransform` here.
    //
    if(path == NULL) {
      modelTransform = Transform(1.0, 0.0, 0.0, 0.0,
                                 0.0, 1.0, 0.0, 0.0,
                                 0.0, 0.0, 1.0, 0.0,
                                 0.0, 0.0, 0.0, 1.0);
    }
    else{
      modelTransform = path->coordinateFrame(u);
      modelTransform.scale(.125, .125, .125);
      modelTransform.translate(0.0, 0.0, 0.2);

      // the last two transforms got my car in a weird direction,
      // so I played around and discovered:
      // x: roll, y: pitch, z: yaw
      // and use those to orientate my cars
      modelTransform.rotate(3 * M_PI / 2, Vector3(1.0, 0.0, 0.0));
      modelTransform.rotate(M_PI, Vector3(0.0, 0.0, 1.0));
    }

    if (scene->eadsShaderProgram) { // will be NULL in the template
        double specFrac = 0.25; // fraction of reflected power that's specular
        Rgb ambDiffBaseRgb = (1.0 - specFrac) * baseRgb;
        Transform identityTransform;

        scene->eadsShaderProgram->setEmittance(blackColor);
        scene->eadsShaderProgram->setAmbient(0.2 * ambDiffBaseRgb);
        scene->eadsShaderProgram->setDiffuse(0.8 * ambDiffBaseRgb);
        scene->eadsShaderProgram->setSpecular(
            Rgb(specFrac, specFrac, specFrac), 10.0);
        worldTransform *= modelTransform;
        scene->eadsShaderProgram->setModelViewProjectionMatrix(
            viewProjectionTransform * worldTransform);
        scene->eadsShaderProgram->setWorldMatrix(worldTransform);
        scene->eadsShaderProgram->setNormalMatrix(
            worldTransform.getNormalTransform());
        scene->eadsShaderProgram->start();
    }

    // `irregularMesh` will be NULL in the unmodified template.
    if (irregularMesh) {
        irregularMesh->render();
        const double quillLength = 0.04;
        displayHedgehogs(viewProjectionTransform,
            worldTransform, quillLength);
    }

    if (controller.axesEnabled)
        coordinateAxes->display(viewProjectionTransform, worldTransform);
}


const double Car::speed(const Track *track) const
{
    //
    // ASSIGNMENT (PA09)
    //
    // Return the speed of the car at its current position, using
    // Track::speed().
    //
    // 1 line in instructor solution (YMMV)
    //
    return track->speed(u); // replace (permits template to compile cleanly)
}


void Car::move(double dU)
{
    u += dU;
    // The next loop should only be executed once at most, but with
    // parametrics we should be careful.
    while (u > 1.0)
        u -= 1.0;
}
