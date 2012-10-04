#include <stdlib.h> /* calloc, free, malloc */
#include <string.h> /* strlen */
#include "ui.h"
#include "ark.h"
#include "config.h"
#include "bindings.h"

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
static char *title; /* allocated in setup */
static int startx=5, starty=15; /* starting x and y positions */
static int hoffset=0, woffset=0, fascent=0, fdescent=0;/* height and width offset for font, font ascent and descent */

/* set by ui_setup and ui_stop, if 0 then ui_mainloop will return */
static int running = 0;

/* buffer, shared with ark */
static Buffer *buf;


/* internal functions */
static void /* draw the llist to the pixmap */
draw(){
	int x=startx, y=starty;
	Line *l;
	int i=0, vi=0, lc=0; /* index into l->contents, visual index, line count */
	int cpy=0, cpx=0; /* cursor pos y, cursor pos x */

	/* clear our pixmap */
	XSetForeground(dpy, pixgc, white_color);
	XFillRectangle(dpy, pixmap, pixgc, 0, 0, wa.width, wa.height);
	XSetForeground(dpy, pixgc, black_color);

	/* local x used within the drawing loop */
	int localx;

	/* draw all the things */
	for( l=buf->sstart, lc=0; l; l=l->next, ++lc ){
		/*
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
		* 		record vlines, vwidth
		*/

		/* stop if drawing another line would go off the window */
		if( ! (y+hoffset < height) ){
			buf->send = l->prev; /* FIXME should it be prev or l? last line, or one past ? */
			break;
		}

		localx = x;

		/* we go around for i = l->len as this is a valid position for the cursor and special cases are bad */
		for( i=0, vi=0; i <= l->len; ++i ){
			if( i == buf->cursor.offset && l == buf->cursor.line ){
				cpx = localx;
				cpy = y-fascent;
			}

			if( l->contents[i] == '\t' ){
				do {
					XDrawString( dpy, pixmap, pixgc, localx, y, tabchar, 1 );
					localx += woffset;
				} while( ++vi % tabwidth );
				continue;
			} else if( l->contents[i] != '\0' ){
				XDrawString( dpy, pixmap, pixgc, localx, y, &l->contents[i], 1 );
			}

			localx += woffset;
			++vi;
		}

		y += hoffset;
	}

	/* only draw cursor if it is on the screen */
	if( cpx ){
		/* draw the cursor line, offset the x so we don't intersect any characters */
		/* cpy and cpy+hoffset for unfilled-dumbells, cpy-1 and cpy+hoffset+1 for filled-dumbells */
		XDrawLine( dpy, pixmap, pixgc, cpx-1, cpy-1, cpx-1, cpy+hoffset+1 );
		/* draw the 'dumbells' */
		XDrawRectangle( dpy, pixmap, pixgc, cpx-2, cpy-2, 2, 2 );
		XDrawRectangle( dpy, pixmap, pixgc, cpx-2, cpy+hoffset, 2, 2 );
	}

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
	 * consider not wrapping, horizontal scroll
	 */
	/* TODO */
}

static void /* copy the pixmap to the window */
display(){
	/* TODO FIXME wa.{width, height} VS {width, height} */
	XCopyArea( dpy, pixmap, window, wingc, 0, 0, width, height, 0, 0 );
	XFlush(dpy);
}

static char* /* return a string of this KeySym, caller must free returned char* */
keysym_to_charp(KeySym keysym){
	char *cp = calloc(sizeof(char), 2);
	/* need a nicer mapping */
	switch(keysym){
		case XK_Return:
			cp[0] = '\n';
			break;
		case XK_Tab:
			cp[0] = '\t';
			break;
		case XK_BackSpace:
			backspace(buf);
			break;
		default:
		 	/* Latin 1, see /usr/include/X11/keysymdef.h
		 	*  need to extend later to be unicode aware
		 	*/
			if( keysym < 0x0100 )
				cp[0] = keysym;
			break;
	}
	return cp;
}

/* event handlers */
static void
keypress(XEvent *e){
	unsigned int i; /* ? */
	XKeyEvent keyevent = e->xkey;
	/* we don't take level in to account here as we check for modifiers in the loop below */
	KeySym keysym = XKeycodeToKeysym(dpy, keyevent.keycode, 0);

	for( i=0; i < LENGTH(keys); ++i){
		if( keysym == keys[i].keysym && keyevent.state == keys[i].mods && keys[i].f_func ){
			keys[i].f_func( &(keys[i].arg) );
			draw();
			display();
			return;
		}
	}

	if( keyevent.keycode == 0xffe9 ){
		printf("keycode is shift\n");
	}
	if( ! keyevent.state || keyevent.state & ShiftMask ){
		keysym = XKeycodeToKeysym(dpy, keyevent.keycode, (keyevent.state & ShiftMask));
		/* FIXME TODO This is all testing... */
		/* this doesnt deal with newlines */
		/* see /usr/include/X11/keysymdef.h */

		char *str = keysym_to_charp(keysym);
		insert(buf, str);
		free(str);
		draw();
		display();
	}
}

static void /* converts an x,y pair to a cursor position */
buttonpress(XEvent *e){
	int x=e->xbutton.x, y=e->xbutton.y;
	x -= startx;
	y -= starty - fascent;
	int linenum = y / hoffset;
	int offset = x / woffset;
	int i;

	switch( e->xbutton.button ){
		case 1: /* left button */
			position_cursor(buf, linenum, offset);
			break;
		case 2: /* middle mouse button */
			break;
		case 3: /* right mouse button */
			break;
		case 4: /* scroll up */
			/* FIXME temp, this logic should be elsewhere */
			for( i=0; i<scrolldistance; ++i ){
				if( buf->sstart->prev )
					buf->sstart = buf->sstart->prev;
				else
					break;
			}
			break;
		case 5: /* scroll down */
			/* FIXME temp, this logic should be elsewhere */
			for( i=0; i<scrolldistance; ++i ){
				if( buf->sstart->next )
					buf->sstart = buf->sstart->next;
				else
					break;
			}
			break;
	}

	draw();
	display();
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
	draw();
	display();
}

/* array of handlers indexed by XEvent.type */
static void (* handler[LASTEvent]) (XEvent *) = {
	[KeyPress] = keypress,
	[ButtonPress] = buttonpress,
	[ConfigureNotify] = configure
};

/** internal functions **/

/** external interface **/
void /* cause ui_mainloop to return */
ui_stop(){
	running = 0;
}

void
ui_setup(Buffer *buffer){
	buf = buffer;
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

	/* select the events we are interested in */
	XSelectInput(dpy, window,
				StructureNotifyMask /* MapNotify, ConfigureNotify */
				| KeyPressMask /* KeyPress, KeyRelease */
				| ExposureMask /* Expose */
				| EnterWindowMask /* EnterNotify */
				| FocusChangeMask /* FocusIn */
				| KeymapStateMask /* KeymapNotify (follows EnterNotify and FocusIn events) */
				| ButtonPressMask /* ButtonPress */
		);

	/* set the title */
	if( buf->path ){
		/* \0 + strlen("ark ") + strlen(buf->path) */
		int len = 1 + 4 + strlen(buf->path);
		title = malloc( len );
		snprintf(title, len, "ark %s", buf->path);
	} else
		title = "ark";
	XStoreName(dpy, window, title);

	/* get font information */
	/* get text information */
	XFontStruct *xfs = XQueryFont(dpy, XGContextFromGC(pixgc));
	XCharStruct ch; /* FIXME TODO wtf is this used for? XFontStruct contains these, which char is this for? etc? */
	int dir; /* direction return */
	/* this assumes that our strings are all similar, could do per string */
	XTextExtents( xfs, "aAzZ1", 5, &dir, &fascent, &fdescent, &ch);
	/* calculate offsets */
	hoffset = fdescent + fascent; /* height offset, ascent + descent */
	woffset = XTextWidth( xfs, "Z", 1 );

	/* map the window */
	XMapWindow(dpy, window);
}

void
ui_teardown(){
	XFreePixmap(dpy, pixmap);
	XFreeGC(dpy, wingc); /* may fail but do we care? */
	XFreeGC(dpy, pixgc);
	XCloseDisplay(dpy);
	free(title);
}

void
ui_mainloop(){
	XEvent e;
	draw();
	display();
	for( ; running ; ){
		XNextEvent(dpy, &e);
		if( handler[e.type] )
			handler[e.type](&e);
	}
}

