#include <cassert>

#include <iostream>
#include <string>
using namespace std;

#include "check_gl.h"
#include "color.h"
#include "controller.h"
#include "framework.h"
#include "render_stats.h"
#include "scene.h"
#include "view.h"
#include "work_arounds.h"

View view;

void View::display(void)
//
// redraws the view, starting from a blank screen
//
{
    CHECK_GL(glClearColor(
                 static_cast<GLclampf>(scene->skyColor.u.c.r),
                 static_cast<GLclampf>(scene->skyColor.u.c.g),
                 static_cast<GLclampf>(scene->skyColor.u.c.b), 1.0));
    CHECK_GL(glClear(GL_COLOR_BUFFER_BIT));
    CHECK_GL(glPolygonMode(GL_FRONT_AND_BACK, GL_LINE));
    scene->display();
    renderStats.frameNumber++;
    framework.swapBuffers();
}


#include "n_elem.h"
void View::displayViewHelp(void)
{
    string orthographicHelpLines[] = {
        "key             camera motion",
        "---             ----------------------",
        "r               reset view",
        "Up              rotate +1 deg around x",
        "Down            rotate -1 deg around x",
        "Left            rotate -1 deg around y",
        "Right           rotate +1 deg around y",
        "Ctrl-Left       rotate +1 deg around z",
        "Ctrl-Right      rotate -1 deg around z",
        "Shift-Left      move left",
        "Shift-Right     move right",
        "Shift-Up        move up",
        "Shift-Down      move down",
        "Wheel-Up or +   zoom in",
        "Wheel-Down or - zoom out",
    };
    int nOrthographicHelpLines = N_ELEM(orthographicHelpLines);

    renderSupertext(-0.90, 0.0, orthographicHelpLines, nOrthographicHelpLines);
}


const void View::renderSupertext(double xFrac, double yFrac,
                                 string lines[], int nLines) const
{
    Color textColor = yellowColor;

    //
    // For now, we fall back on the default OpenGL shader program so
    // we can set the text color with glColor(). Eventually, we should
    // use a constant-color shader here, but that will require
    // knowledge of how (for instance) glutBitmapCharacter()
    // determines its pixel color.
    //
    ShaderProgram::disableCurrent();

    //
    // Enable a screen-specific coordinate system to work with
    // draws.  The modelview and projection transform matrix
    // push/pops are necessary to deal with the changing raster
    // position and viewing volume.
    //
    CHECK_GL(glRasterPos2d(0.0, 0.0));
    CHECK_GL(glMatrixMode(GL_MODELVIEW));
    CHECK_GL(glLoadIdentity());
	CHECK_GL(glMatrixMode(GL_PROJECTION));
    CHECK_GL(glLoadIdentity());
    //
    // Disable depth testing to make sure the supertext appears on
    // top of whatever else is being drawn and restore the default
    // when we're done.
    //
    // Also disable lighting and texturing so that the text appears
    // with the specified color.
    //
    CHECK_GL(glPushAttrib(GL_DEPTH_BUFFER_BIT
                          | GL_LIGHTING_BIT
                          | GL_TEXTURE_BIT));
    CHECK_GL(glDisable(GL_DEPTH_TEST));
    CHECK_GL(glDisable(GL_LIGHTING));
    CHECK_GL(glDisable(GL_TEXTURE_2D));
    CHECK_GL(glColor3d(textColor.u.c.r, textColor.u.c.g, textColor.u.c.b));
    while (nLines--) {
        CHECK_GL(glRasterPos2d(xFrac, yFrac));
        for (unsigned int i = 0; (*lines)[i]; i++) {
            framework.bitmapCharacter(framework.FRAMEWORK_BITMAP_9_BY_15,
                                      (*lines)[i]);
        }
        lines++;
        yFrac -= 0.05; // empirical: could be done with more font awareness
    }
    //
    // Undo the attribute push we made above for depth testing.
    //
    CHECK_GL(glPopAttrib());

    CHECK_GL(glMatrixMode(GL_PROJECTION));

	CHECK_GL(glMatrixMode(GL_MODELVIEW));
#ifdef WORK_AROUND_GLUT_CHARS_NEED_FLUSH_BUG
    framework.flushText();
#endif
}


void View::reset(void)
{
    camera.reset(canvasWidth, canvasHeight);
}


void View::reshape(int newCanvasWidth, int newCanvasHeight)
//
// responds to a resize request
//
{
    camera.reshape(newCanvasWidth, newCanvasHeight);
    view.canvasWidth  = newCanvasWidth;
    view.canvasHeight = newCanvasHeight;
    CHECK_GL(glViewport(0, 0, view.canvasWidth, view.canvasHeight));
}


void View::setFullScreen(bool makeItFull)
{
    if (makeItFull) {
        savedCanvasWidth = canvasWidth;
        savedCanvasHeight = canvasHeight;
        framework.fullScreen();
    } else
        framework.reshapeWindow(savedCanvasWidth, savedCanvasHeight);
}
