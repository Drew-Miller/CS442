
#include "controller.h"
#include "framework.h"
#include "scene.h"
#include "view.h"
#include "work_arounds.h"

#ifdef WORK_AROUND_FORCE_PTHREADS_LINK
#include <pthread.h>
#endif


#ifdef WORK_AROUND_FORCE_PTHREADS_LINK

void _do_not_call_me()
// dummy function to force libpthreads linkage (for bug fix)
{
    pthread_getconcurrency();
};

#endif

int main(int argc, char **argv)
{
    view.init(&argc, argv, argv[0]);
    controller.init();
    scene = new Scene();
    framework.mainLoop();
}
