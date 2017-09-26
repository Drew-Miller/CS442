#include <assert.h>

#include "car.h"
#include "controller.h"
#include "scene.h"

//
// Our rollercoaster car design is a simple irregular mesh specified
// in a Wavefront "*.obj" file (default: "car.obj"). Feel free to
// modify that file.
//

string carFname = DEFAULT_CAR_FNAME;

Car::Car(void)
    :
      irregularMesh(NULL)
{

    //
    // Copy your previous (PA03) solution here.
    //
    irregularMesh = IrregularMesh::read(carFname.c_str());

    // Since the car's IrregularMesh doesn't need to be tessellated
    // (effectively), we can create the hedgehogs immediately.
    addHedgehogs(irregularMesh);
}

void Car::display(const Transform &viewTransform)
{

    if (scene->uniformColorShaderProgram) { // will be NULL in the template

        // dark blue
        scene->uniformColorShaderProgram->setColor(Color(0.0, 0.0, 0.5));

        scene->uniformColorShaderProgram->setViewMatrix(viewTransform);
        scene->uniformColorShaderProgram->start();
    }

    // `irregularMesh` will be NULL in the unmodified template.
    if (irregularMesh) {
        irregularMesh->render();
        const double quillLength = 0.04;
        displayHedgehogs(viewTransform, quillLength);
    }

}
