#include <stdlib.h> /* malloc family */
#include <string.h> /* strlen */
#include <stdio.h> /* printf, puts */
#include <assert.h> /* assert */

#include <X11/Xlib.h>
#include <X11/keysym.h> /* XK_*, used in config.h */


/* Argument for functions bound to key presses */
typedef union {
	int i; /* FIXME what do */
	const void *v; /* FIXME document */
} Arg;

/* key bindings defined in config.h */
typedef struct {
	unsigned int mod; /* modifiers */
	KeySym keysym; /* key */
	void (*f_func)(const Arg *); /* function to call, see Bindable functions */
	const Arg arg; /* arg to pass to function */
} Key;

/** Bindable functions, can be bound to Key.f_func **/
static void f_quit(const Arg *arg);

#include "config.h" /* include our configuration */


/** Naughty naughty global state **/
Display *dpy = 0;
Window window;
Pixmap pixmap;
GC wingc, pixgc;
int running = 0; /* keep running */



/** Internal functions **/
static void i_setup(void); /* setup for running */
static void i_run(void); /* mainloop */
static void i_tidyup(void); /* tidy up before exiting */

/** XEvent handler functions **/
static void x_keypress(XEvent *e); /* handle XEvent KeyPress */

/* an array of functions LASTEvent long, each taking an XEvent *s and returning void */
/* c99 designated initialization, [event] = func */
static void (* handler[LASTEvent]) (XEvent *) = {
	[KeyPress] = x_keypress
};

/* Function definitions */
void
x_keypress(XEvent *e){
	unsigned int i;
	XKeyEvent keyevent = e->xkey;
	KeySym keysym = XKbKeycodeToKeysym(dpy, e.xkeycode.keycode, 0, 0); /* FIXME levels */

	/* FIXME 2 vs LENGTH */
	for( i=0; i < 2; ++i ){
		if( keysym == keys[i].keysym
			&& keys[i].func )
			keys[i].f_func( &(keys[i].arg) );
	}
}

void
f_quit(const Arg *arg){
	running = 0;
}

void /* setup for running */
i_setup(void){
	running = 1;
	dpy = XOpenDisplay(NULL);
}

void /* mainloop */
i_run(void){
	XEvent e;
	XSync(dpy, False);
	for( ; running ; ){
		XNextEvent(dpy, &e);
		if( handler[e.type] )
			handler[e.type](&e); /* call assosiated handler */
	}
}

void /* tidy up before exiting */
i_tidyup(void){
	/*
	XFreeGC(dpy, wingc);
	XFreeGC(dpy, pixgc);
	*/
	XCloseDisplay(dpy);
}

int
main(int argc, char *argv[]){
	/* TODO handle args */

	i_setup();
	i_run(); /* mainloop */
	i_tidyup();

	return EXIT_SUCCESS;
}
