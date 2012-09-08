#ifndef ui_h
#define ui_h

#include <stdio.h>

/* should the X includes be here or within ark.c ? */
#include <X11/Xlib.h>
#include <X11/keysym.h> /* XK_* */

#include "ark.h"

/* calling this will cause ui_mainloop to return */
void ui_stop();

/* initialise */
void ui_setup();

/* tidyup */
void ui_teardown();

/* handles XEvents and dispatches to handlers */
void ui_mainloop();

#endif
