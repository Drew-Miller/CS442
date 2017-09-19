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
    // ASSIGNMENT (PA03)
    //
    // This will be the initial "car" for the rollercoaster.
    //
    // Here is the step:
    //
    // - Call `IrregularMesh::read()` to read in the model in
    //   `carFname` (cast to a C string) and assign it to
    //   `irregularMesh`.
    //
    // 4 lines in instructor solution (YMMV)
    //
    irregularMesh = IrregularMesh::read(carFname.c_str());
}

void Car::display(const Transform &viewTransform)
{

    if (scene->uniformColorShaderProgram) { // will be NULL in the template

        // dark blue
        scene->uniformColorShaderProgram->setColor(Color(0.0, 0.0, 0.5));

        scene->uniformColorShaderProgram->start();
    }

    // `irregularMesh` will be NULL in the unmodified template.
    if (irregularMesh) {
        irregularMesh->render(viewTransform);
    }

}
