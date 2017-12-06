
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
    Rgb veryRedRgb(0.976, 0.051, 0.008);


    modelTransform = Transform(1.0, 0.0, 0.0, 0.0,
                               0.0, 1.0, 0.0, 0.0,
                               0.0, 0.0, 1.0, 0.0,
                               0.0, 0.0, 0.0, 1.0);

    modelTransform.scale(10, 10, 10);
    modelTransform.translate(0.0, 0.0, 0.2);


    if (scene->eadsShaderProgram) { // will be NULL in the template
        scene->eadsShaderProgram->setEmittance(blackColor);
        scene->eadsShaderProgram->setDiffuse(0.3*veryRedRgb);
        scene->eadsShaderProgram->setAmbient(0.3*veryRedRgb);
        scene->eadsShaderProgram->setSpecular(Rgb(0.3, 0.3, 0.3), 30.0);
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
}
