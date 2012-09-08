#include "ui.h"

/** state variables **/
Display *dpy=0;
Window window;
Pixmap pixmap;
GC wingc, pixgc;
int width=400, height=400; /* default dimensions */
int running = 0;

/* event handlers */
void keypress(XEvent *e){}
/* array of handlers indexed by XEvent.type */
static void (* handler[LASTEvent]) (XEvent *) = {
	[KeyPress] = keypress
};

void
ui_setup(){
	running = 1; /* FIXME this may need to be visible from within ark.c, commands may want to quit, should we really set this? */
	dpy = XOpenDisplay(NULL);
}

void
ui_teardown(){
	XFreeGC(dpy, wingc); /* may fail but do we care? */
	XFreeGC(dpy, pixgc);
	XCloseDisplay(dpy);
}

void
ui_mainloop(){
	XEvent e;
	for( ; running ; ){
		XNextEvent(dpy, &e);
		if( handler[e.type] )
			handler[e.type](&e);
	}
}

