#include <iostream>
#include <cstdlib>
using namespace std;
#include <getopt.h>

#include "car.h"
#include "controller.h"
#include "framework.h"
#include "scene.h"
#include "view.h"
#include "work_arounds.h"

#ifdef WORK_AROUND_FORCE_PTHREADS_LINK
#include <pthread.h>
#endif


#define DEFAULT_SKY_BOX "sky_box_0.rgb"


static void help(char *progname)
{
    cout <<
        "syntax: " << progname << " [<option>]*\n"
        "where <option> is any of:\n"
        "  -c <filename>  use <filename> as car model (obj format)\n"
        "                 (default: \"" DEFAULT_CAR_FNAME "\")\n"
        "  -h             (this) help message\n"
        "  -l <name>      selects track layout, where <name> is one of:\n"
        ;
    for (unsigned int i = 0; i < nTagOfLayout; i++) {
        cout << "                " << tagOfLayout[i].tag << "\n";
    }
    cout <<
        "  -s <filename>  use <filename> as sky box"
        "                 (default: \"" DEFAULT_SKY_BOX "\")\n";
}

#ifdef WORK_AROUND_FORCE_PTHREADS_LINK

void _do_not_call_me()
// dummy function to force libpthreads linkage (for bug fix)
{
    pthread_getconcurrency();
};

#endif

int main(int argc, char **argv)
{
    int ch;
    string skyBoxFname = DEFAULT_SKY_BOX;
    int status;
    // PA11 - track.h
    // Layout layout = LAYOUT_BSPLINE; // the default
    Layout layout = LAYOUT_BSPLINE; // the default

    while ((ch = getopt(argc, argv, "c:l:s:h")) != -1) {
        switch (ch) {

        case 'c':
            carFname = optarg;
            break;

        case 'l':
            status = parseLayout(optarg, layout);
            assert(status);
            break;

        case 's':
            skyBoxFname = optarg;
            break;

        case 'h':
            help(argv[0]);
            exit(EXIT_SUCCESS);
            break;

        default:
            cerr << "unknown command line option -- exiting\n";
            exit(EXIT_FAILURE);
        }
    }

    view.init(&argc, argv, argv[0]);
    controller.init();
    scene = new Scene(layout, skyBoxFname);
    // hack: addTies() depends on `scene` being defined
    scene->track->addTies();
    framework.mainLoop();
}
