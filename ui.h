#ifndef ui_h
#define ui_h

#include <stdio.h>

/* should the X includes be here or within ark.c ? */
#include <X11/Xlib.h>
#include <X11/keysym.h> /* XK_* */
#include <X11/XKBlib.h> /* XkbKeycodeToKeysym */

#include "llist.h"

typedef struct ui_window {
	Display *dpy; //=0;
	Window xwindow;
	XWindowAttributes wa;
	Pixmap pixmap;
	GC wingc, pixgc;
	int width, height; //=400, height=400; /* default dimensions */
	int black_color, white_color;
	char *title; /* allocated in setup */
	int startx, starty; /* startx=5, starty=15 starting x and y positions */
	int hoffset, woffset, fascent, fdescent;/* all set to 0; height and width offset for font, font ascent and descent */
	int running; /* 0, set to 1 in mainloop, set to 0 in stop */
	Buffer *buf;
	XIM xim; /* X Input Method */
	XIC xic; /* X Input Context */
} ui_window;


typedef union Arg Arg;
union Arg{
	int i;
	void *v;
	void (*f)(Buffer*); /* function to execute */
};

typedef struct Key Key;
struct Key{ /* structure used in config.h to bind a function to a key */
	int mods; /* modifiers */
	KeySym keysym; /* keysym to bind to */
	void (*f_func) (ui_window *uiw, const Arg *arg); /* function to call */
	const Arg arg; /* argument to pass */
};

/* calling this will cause ui_mainloop to return */
void ui_stop(ui_window *uiw);

/* allocated and initialise an ui_window, return it's address OR null on failure */
ui_window * ui_setup(Buffer *buffer);

/* tidyup x state, free all needed ui_window properties, and then finally free ui_window */
void ui_teardown(ui_window *uiw);

/* handles XEvents and dispatches to handlers */
void ui_mainloop(ui_window *uiw);

#endif
