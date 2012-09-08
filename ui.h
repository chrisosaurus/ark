#ifndef ui_h
#define ui_h

/* should the X includes be here or within ark.c ? */
#include <X11/Xlib.h>
#include <X11/keysym.h> /* XK_* */

#include "ark.h"

/* if running is set to 0, mainloop will return */
extern int running;

/* initialise */
void ui_setup();

/* tidyup */
void ui_teardown();

/* handles XEvents and dispatches to handlers */
void ui_mainloop();

#endif
