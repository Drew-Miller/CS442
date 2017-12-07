
#include "death_star.h"
#include "controller.h"
#include "scene.h"
#include "shader_programs.h"

using namespace std;

string dsFname = DEATH_STAR_FNAME;

DeathStar::DeathStar(void)
          :
            irregularMesh(NULL)
{
  coordinateAxes = new CoordinateAxes(); // add this line

  // Since the car's IrregularMesh doesn't need to be tessellated
  // (effectively), we can create the hedgehogs immediately.
  irregularMesh = IrregularMesh::read(dsFname.c_str());

  // Since the car's IrregularMesh doesn't need to be tessellated
  // (effectively), we can create the hedgehogs immediately.
  addHedgehogs(irregularMesh);
}

void DeathStar::display(const Transform &viewProjectionTransform,
                     Transform worldTransform)
{
    Rgb surfaceColorRGB(0.563, 0.578, 0.589);

    modelTransform = Transform(2.0, 0.0, 0.0, 0.0,
                               0.0, 2.0, 0.0, -0.5,
                               0.0, 0.0, 2.0, 0.0,
                               0.0, 0.0, 0.0, 1.0);

    modelTransform.rotate(M_PI / 2, Vector3(1.0, 0.0, 0.0));


    if (scene->eadsShaderProgram) { // will be NULL in the template
        scene->eadsShaderProgram->setEmittance(blackColor);
        scene->eadsShaderProgram->setDiffuse(0.3*surfaceColorRGB);
        scene->eadsShaderProgram->setAmbient(0.3*surfaceColorRGB);
        scene->eadsShaderProgram->setSpecular(Rgb(0.3, 0.3, 0.3), 30.0);
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
