#include <cassert>

#include "camera.h"
#include "coordinate_axes.h"
#include "car.h"
#include "color.h"
#include "controller.h"
#include "curve.h"
#include "death_star.h"
#include "n_elem.h"
#include "render_stats.h"
#include "rocket.h"
#include "scene.h"
#include "scene_object.h"
#include "shader_programs.h"
#include "teapot.h"
#include "track.h"
#include "transform.h"
#include "tube.h"
#include "view.h"
#include "wrap_cmath_inclusion.h"

using namespace std;

const Color Scene::skyColor(0.5, 0.5, 1.0);

Scene *scene;


void Scene::addLight(Light *light)
//
// adds `light` to the scene
//
{
    lights.push_back(light);
}


void Scene::addSceneObject(SceneObject *sceneObject)
//
// adds `sceneObject` to the scene
//
{
    sceneObjects.push_back(sceneObject);
}


const double Scene::cameraSpeed(void) const
{
    //
    // ASSIGNMENT (PA09)
    //
    // If `cars` is NULL (e.g. if it hasn't been defined yet), return
    // 0.0. Otherwise, return the speed of the camera used for
    // first-person mode (that of cars[0]).
    //
    // 1 line in instructor solution (YMMV)
    //
    if(cars == NULL)
      return 0.0;

    return cars[0]->speed(track); // replace (permits template to compile cleanly)
}


void Scene::display(void)
//
// draws all objects in the scene
//
{
    //
    // The argument to the draw() methods is the
    // "model-view-projection" transform (note the name change from
    // "view" transform), which is initially just the view-projection
    // transform. We'll pass this down the calling stack ultimately to
    // the render() methods, concatenating individual model transforms
    // to the modelview transform (by right-multiplying them) as we
    // descend the scene graph, which we implement in our call graph.
    //
    Transform viewProjectionTransform
        = camera.projectionTransform() * camera.viewTransform();
    renderStats.reset(); // for this frame
    for (unsigned int i = 0; i < sceneObjects.size(); i++) {
        Transform identityTransform; // world transform, initially

        sceneObjects[i]->display(viewProjectionTransform, identityTransform);
    }
    if (controller.axesEnabled) {
        Transform identityTransform;

        coordinateAxes->display(viewProjectionTransform, identityTransform);
    }
    if (controller.statsEnabled)
        renderStats.display();
    if (controller.viewHelpEnabled)
        view.displayViewHelp();
}


void Scene::step(double dtReq)
//
// moves each of the cars and the camera (for use in first person
// mode) by `du`, computing the distance `du` traveled by each car in
// time `dtReq`
//
// also moves the camera by the same amount as the first car
//
{
    //
    // Move the cars and camera by no more than this time step. If
    // `dtReq` is smaller, use it instead. If `dtReq` is larger, take
    // steps no longer than this value.
    //

    // dtMax which is a negative power of 2 reduces the likelihood of
    // roundoff problems
    const double dtMax = 1.0 / 1024.0; // resolution of ~1ms

    double dtSum = 0.0;
    while (dtSum < dtReq) {
        double dt = dtReq - dtSum; // step size
        if (dt > dtMax)
            dt = dtMax;
        for (int i = 0; i < nCars; i++) {
            Vector3 dp_du;
            (*cars[i]->path)(cars[i]->u, &dp_du);
            double ds_du = dp_du.mag();
            // A car's speed is ds/dt, which is equal to ds/du * du/dt, so...
            double du = cars[i]->speed(track) * dt / ds_du;
            if (i == 0) // assume the camera is at car[0]
                camera.move(du);
            cars[i]->move(du);
        }
        dtSum += dt;
    }
}


Scene::Scene(const Layout layout)
{
    uniformColorShaderProgram = new UniformColorShaderProgram(
            "UniformColorShaderProgram");
    eadsShaderProgram = new EadsShaderProgram();

    coordinateAxes = new CoordinateAxes();

    //
    // Copy your previous (PA08) solution here.
    //

    // set extent
    int extent_ = 32;
    camera.setExtent((double) extent_);

    // Add Lights
    addLight(new Light(whiteColor, Vector3(0, 0, -1)));
    addLight(new Light(.50 * whiteColor, Vector3(0, 1, 0)));

    // add the ground to the scene
    Ground *g = new Ground((double) extent_);
    // addSceneObject(g);

    // add the track to the scene
    Track *t = new Track(layout, g);
    if(track == NULL)
      track = t;
    addSceneObject(t);

    // add the teapot if the layout is not Layout_Trig
    if(layout != LAYOUT_TRIG){
      addSceneObject(new DeathStar());
    }


    // hardcoded color values for control over car color
    Rgb color = Rgb(0.39, 0.00, 0.39);

    cars = new Car *[nCars];

    for(int i = 0; i < nCars; i++){
      double u = (double)i / (double)nCars;
      cars[i] = new Car(color, u, t->guideCurve);
      addSceneObject(cars[i]);
    }

    camera.setPath(t->guideCurve);
}
