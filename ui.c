#include "ui.h"
#include "config.h"

/* macros */
#define LENGTH(x) (sizeof x / sizeof x[0])

/** state variables **/
static Display *dpy=0;
static Window window;
static XWindowAttributes wa;
static Pixmap pixmap;
static GC wingc, pixgc;
static int width=400, height=400; /* default dimensions */
static int black_color, white_color;

/* set by ui_setup and ui_stop, if 0 then ui_mainloop will return */
static int running = 0;

/* internal functions */
static void /* draw the llist to the pixmap */
draw(){
	/* x=startx, y=starty
	 * pos = {line, 0}
	 * for( ; pos.line; ){
	 * 	if not y + TextHeight( &pos.line[pos.offset] ) < height
	 * 		break
	 * 	if x + TextWidth( &pos.line[pos.offset] ) < width
	 * 		draw line
	 * 		y += TextHeight( &pos.line[pos.offset] )
	 * 		pos.line = pos.line.next
	 * 		pos.offset = 0
	 * 	else
	 * 		find highest offset that will fit within width
	 * 		draw [pos - offset]
	 * 		y += TextHeight( [pos - offset] ) + descent + ascent
	 * 		pos = line, offset
	 * 		record vlines
	 *
	 * consider Vlines and Vwidth (longest number of chars we have drawn so far)
	 */
	/* TODO */
}

static void /* copy the pixmap to the window */
display(){
	/* TODO */
}

/* event handlers */
static void
keypress(XEvent *e){
	unsigned int i; /* ? */
	XKeyEvent keyevent = e->xkey;
	/* we don't take level in to account here as we check for modifiers in the loop below */
	KeySym keysym = XKeycodeToKeysym(dpy, keyevent.keycode, 0);

	for( i=0; i < LENGTH(keys); ++i){
		if( keysym == keys[i].keysym && keyevent.state == keys[i].mods && keys[i].f_func )
			keys[i].f_func( &(keys[i].arg) );
	}
}

static void
configure(XEvent *e){
	/* we could get the width and height from e.configure, but we need the depth from the window anyway */
	XGetWindowAttributes(dpy, window, &wa);
	width = wa.width;
	height = wa.height;

	/* free old pixmap and assosiated graphics context */
	XFreeGC(dpy, pixgc);
	XFreePixmap(dpy, pixmap);

	/* create a new pixmap and graphics context */
	pixmap = XCreatePixmap(dpy, window, wa.width, wa.height, wa.depth);
	pixgc = XCreateGC(dpy, pixmap, 0, NULL);

	/* FIXME TODO now what ? */
	/* should we cause a draw */
}

/* array of handlers indexed by XEvent.type */
static void (* handler[LASTEvent]) (XEvent *) = {
	[KeyPress] = keypress,
	[ConfigureNotify] = configure
};

/** internal functions **/

/** external interface **/
void /* cause ui_mainloop to return */
ui_stop(){
	running = 0;
}

void
ui_setup(){
	running = 1;

	/* open connection and get black and white colours */
	dpy = XOpenDisplay(NULL);
	black_color = BlackPixel(dpy, DefaultScreen(dpy));
	white_color = WhitePixel(dpy, DefaultScreen(dpy));

	/* create window and get attributes */
	window = XCreateSimpleWindow(dpy, DefaultRootWindow(dpy), 0, 0, width, height, 0, black_color, white_color);
	XGetWindowAttributes(dpy, window, &wa);

	/* define out pixmap */
	pixmap = XCreatePixmap(dpy, window, wa.width, wa.height, wa.depth);

	/* create our graphic contexts */
	wingc = XCreateGC(dpy, window, 0, NULL);
	pixgc = XCreateGC(dpy, pixmap, 0, NULL);

	/* clear our pixmap, FIXME do this inside a draw function instead */
	XSetForeground(dpy, pixgc, white_color);
	XFillRectangle(dpy, pixmap, pixgc, 0, 0, wa.width, wa.height);
	XSetForeground(dpy, pixgc, black_color);

	/* select the events we are interested in */
	XSelectInput(dpy, window, 
				StructureNotifyMask /* MapNotify, ConfigureNotify */
				| KeyPressMask /* KeyPress, KeyRelease */
				| ExposureMask /* Expose */
				| EnterWindowMask /* EnterNotify */
				| FocusChangeMask /* FocusIn */
				| KeymapStateMask /* KeymapNotify (follows EnterNotify and FocusIn events) */
		);

	/* map the window */
	XMapWindow(dpy, window);
}

void
ui_teardown(){
	XFreePixmap(dpy, pixmap);
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

