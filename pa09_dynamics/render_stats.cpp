#include <string>
#include <sstream>

#include "clock.h"
#include "controller.h"
#include "n_elem.h"
#include "render_stats.h"
#include "scene.h"
#include "view.h"
#include "wrap_cmath_inclusion.h"


RenderStats renderStats;

void RenderStats::display()
{
    double thisFrameTime = clock_.read() - startTime; // in seconds
    //
    // If frame time averaging is used, assign exponential weights to
    // previous frames:
    //
    // tMean[i] = a*t[i] + a*w * t[i-1] + a*w**2 * t[i-2] + ....
    //
    // which is just
    //
    //          = a*t[i] + w*tMean[i-1]
    //
    // Given that sum_{i=0}^inf (a * w**i) must == 1, we get
    //
    //     a = (1 - w)
    //
    // assuming 0 < w < 1. To define w, we choose a value of n for
    // which the relative contribution to the sum is 1/2, i.e.
    //
    //     w**n = 1/2
    //
    // and solve (approximately) for w (`weightPrevious`).
    //
    const int halfWeightFrame = 100;
    double weightPrevious = pow(0.5, 1.0 / halfWeightFrame);

    double meanFrameTimeUsec; // in microseconds
    double frameRate;
    double triangleRate;

    if (resetFrameTimer) {
        meanFrameTime = thisFrameTime;
        resetFrameTimer = false;
    } else {
        meanFrameTime =
            (1 - weightPrevious) * thisFrameTime  // = a*t[i]
            + weightPrevious * meanFrameTime;     // = w*tMean[i-1]
    }
    meanFrameTimeUsec = 1.e6 * meanFrameTime; // in microseconds
    frameRate = 1.0 / meanFrameTime;
    triangleRate = (ctTrianglesInIrregularMeshes + ctTrianglesInRegularMeshes)
        / meanFrameTime;
    // display speed in m/s
    double observerSpeed = scene->cameraSpeed() * METERS_PER_LENGTH_UNIT;

    struct {
        string tag;
        bool isValid;
        int prec;      // -1 -> int, >= 0 -> = precision
        void *value;
    } lineSpecs[] = {
        { "frame number",             true, -1, &frameNumber },
        { "vertices",                 true, -1, &ctVertices },
        { "lines",                    true, -1, &ctLines },
        { "line strips",              true, -1, &ctLineStrips },
        { "triangles (in irregular meshes)",
                                      true, -1, &ctTrianglesInIrregularMeshes },
        { "triangles (in regular meshes)",
                                      true, -1, &ctTrianglesInRegularMeshes },
        { "triangle strips",          true, -1, &ctTriangleStrips },
        { "mean frame time (usec)",    true, 1, &meanFrameTimeUsec },
        { "frames/sec",                true, 1, &frameRate },
        { "triangles/sec",             true, 1, &triangleRate },
        { "observer speed (m/s)",      controller.useFirstPerson,
                                             1, &observerSpeed },
    };
    int maxLines = N_ELEM(lineSpecs);
    int nLines = 0;

    string *lines = new string[maxLines];
    for (int i = 0; i < maxLines; i++) {
        ostringstream sstrm;

        if (!lineSpecs[i].isValid)
            continue;
        sstrm << fixed;
        sstrm.width(10);
        if (lineSpecs[i].prec >= 0) {
            sstrm.precision(lineSpecs[i].prec);
            sstrm << *static_cast<double *>(lineSpecs[i].value);
        } else {
            sstrm.precision(1);
            sstrm << *static_cast<int *>(lineSpecs[i].value);
        }
        sstrm << " " << lineSpecs[i].tag;
        lines[nLines++] = sstrm.str();
    }

    view.renderSupertext(-0.90, 0.90, lines, nLines);
    delete [] lines;
}


void RenderStats::resetTimeAveraging(void)
{
    resetFrameTimer = true;
}


void RenderStats::reset(void)
//
// Clear the all counts and mark the time. This is typically done at
// the start of a frame.
//
{
    ctVertices = 0;
    ctLines = 0;
    ctLineStrips = 0;
    ctTrianglesInIrregularMeshes = 0;
    ctTrianglesInRegularMeshes = 0;
    ctTriangleStrips = 0;
    startTime = clock_.read();
}
