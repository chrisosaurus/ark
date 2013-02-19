#include <stdlib.h> /* calloc, free, calloc */
#include <string.h> /* strlen */
#include "ui.h"
#include "ark.h"
#include "config.h"
#include "bindings.h"

#pragma GCC diagnostic ignored "-Wunused-parameter"

/* macros */
#define LENGTH(x) (sizeof x / sizeof x[0])

/* internal functions */
static void /* draw the llist to the pixmap */
draw(ui_window *uiw){
	int x=uiw->startx, y=uiw->starty;
	Line *l;
	int i=0, vi=0; /* index into l->con, visual index */
	int cpy=0, cpx=0; /* cur pos y, cur pos x */

	/* clear our pixmap */
	XSetForeground(uiw->dpy, uiw->pixgc, uiw->white_color);
	XFillRectangle(uiw->dpy, uiw->pixmap, uiw->pixgc, 0, 0, uiw->wa.width, uiw->wa.height);
	XSetForeground(uiw->dpy, uiw->pixgc, uiw->black_color);

	/* local x used within the drawing loop */
	int localx;

	/* draw all the things */
	for( l=uiw->buf->s_start; l; l=ll_next(l) ){
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
		if( ! (y+uiw->hoffset < uiw->height) ){
			uiw->buf->s_end = ll_prev(l); /* FIXME should it be prev or l? last line, or one past ? */
			break;
		}

		localx = x;

		/* we go around for i = l->len as this is a valid position for the cur and special cases are bad */
		for( i=0, vi=0; i <= l->len; ++i ){
			if( i == uiw->buf->cur.offset && l == uiw->buf->cur.line ){
				cpx = localx;
				cpy = y-uiw->fascent;
			}

			if( l->con[i] == '\t' ){
				do {
					XDrawString( uiw->dpy, uiw->pixmap, uiw->pixgc, localx, y, tabchar, 1 );
					localx += uiw->woffset;
				} while( ++vi % tabwidth );
				continue;
			} else if( l->con[i] != '\0' ){
				XDrawString( uiw->dpy, uiw->pixmap, uiw->pixgc, localx, y, &l->con[i], 1 );
			}

			localx += uiw->woffset;
			++vi;
		}

		y += uiw->hoffset;
	}

	/* only draw cur if it is on the screen */
	if( cpx ){
		/* draw the cur line, offset the x so we don't intersect any characters */
		/* cpy and cpy+hoffset for unfilled-dumbells, cpy-1 and cpy+hoffset+1 for filled-dumbells */
		XDrawLine( uiw->dpy, uiw->pixmap, uiw->pixgc, cpx-1, cpy-1, cpx-1, cpy+uiw->hoffset+1 );
		/* draw the 'dumbells' */
		XDrawRectangle( uiw->dpy, uiw->pixmap, uiw->pixgc, cpx-2, cpy-2, 2, 2 );
		XDrawRectangle( uiw->dpy, uiw->pixmap, uiw->pixgc, cpx-2, cpy+uiw->hoffset, 2, 2 );
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
display(ui_window *uiw){
	/* TODO FIXME wa.{width, height} VS {width, height} */
	XCopyArea( uiw->dpy, uiw->pixmap, uiw->xwindow, uiw->wingc, 0, 0, uiw->width, uiw->height, 0, 0 );
	XFlush(uiw->dpy);
}

/** event handlers **/
static void
keypress(ui_window *uiw, XEvent *e){
	unsigned int i; /* for loop count */
	char buf[32];
	int len = 0;
	KeySym keysym;
	Status status;
	XKeyEvent keyevent = e->xkey;

	len = XmbLookupString(uiw->xic, &keyevent, buf, sizeof buf, &keysym, &status);
	buf[len] = '\0'; /* XmbLookupString doesn't null terminate */

	if( status == XBufferOverflow )
		return;

	/* handle bindings */
	for( i=0; i < LENGTH(keys); ++i){
		if( keysym == keys[i].keysym && keyevent.state == keys[i].mods && keys[i].f_func ){
			keys[i].f_func( uiw, &(keys[i].arg) );
			draw(uiw);
			display(uiw);
			return; /* if we don't return here, bindings like BackSpace will be inserted below */
		}
	}

	/* handle inserting */
    /* only insert if we are not pressing Mod1 (alt), Mod4 (win), or Control */
	if( !(keyevent.state & (Mod1Mask | Mod4Mask | ControlMask) ) && len ){
#pragma GCC diagnostic ignored "-Wformat"
		printf("INSERTING : char (%s), dec (%d)\n", buf, buf); /* FIXME debugging */
#pragma GCC diagnostic warning "-Wformat"
		insert(uiw->buf, buf);
		draw(uiw);
		display(uiw);
	}
}

static void /* converts an x,y pair to a cur position */
buttonpress(ui_window *uiw, XEvent *e){
	int x=e->xbutton.x, y=e->xbutton.y;
	x -= uiw->startx;
	y -= uiw->starty - uiw->fascent;
	int linenum = y / uiw->hoffset;
	int offset = x / uiw->woffset;

	switch( e->xbutton.button ){
		case 1: /* left button */
			position_cursor(uiw->buf, linenum, offset);
			break;
		case 2: /* middle mouse button */
			break;
		case 3: /* right mouse button */
			break;
		case 4: /* scroll up */
			m_scrollup(uiw->buf);
			break;
		case 5: /* scroll down */
			m_scrolldown(uiw->buf);
			break;
		default:
			printf("%d\n", e->xbutton.button );
	}

	draw(uiw);
	display(uiw);
}

static void
configure(ui_window *uiw, XEvent *e){
	/* we could get the width and height from e.configure, but we need the depth from the window anyway */
	XGetWindowAttributes(uiw->dpy, uiw->xwindow, &(uiw->wa));
	uiw->width = uiw->wa.width;
	uiw->height = uiw->wa.height;

	/* free old pixmap and assosiated graphics context */
	XFreeGC(uiw->dpy, uiw->pixgc);
	XFreePixmap(uiw->dpy, uiw->pixmap);

	/* create a new pixmap and graphics context */
	uiw->pixmap = XCreatePixmap(uiw->dpy, uiw->xwindow, uiw->wa.width, uiw->wa.height, uiw->wa.depth);
	uiw->pixgc = XCreateGC(uiw->dpy, uiw->pixmap, 0, NULL);

	/* FIXME TODO now what ? */
	/* should we cause a draw */
	draw(uiw);
	display(uiw);
}

static void /* window display information has been lost, need to re-copy */
expose(ui_window *uiw, XEvent *e){
	display(uiw);
}


/* array of handlers indexed by XEvent.type */
static void (* handler[LASTEvent]) (ui_window *uiw, XEvent *) = {
	[KeyPress] = keypress,
	[ButtonPress] = buttonpress,
	[ConfigureNotify] = configure,
	[Expose] = expose
};

/** internal functions **/

/** external interface **/
void /* cause ui_mainloop to return */
ui_stop(ui_window *uiw){
	uiw->running = 0;
}

ui_window * /* allocated and initialized ui_window * OR null */
ui_setup(Buffer *buffer){
	ui_window *uiw = calloc(1, sizeof(*uiw));
	if( ! uiw )
		return 0; /* FIXME TODO XXX error with calloc, crap */

	uiw->buf = buffer;
	uiw->running = 1;

	/* FIXME TODO XXX maybe the following should be user configurable */
	uiw->width = 400;
	uiw->height = 400;
	uiw->startx = 5;
	uiw->starty = 15;
	/* NB: due to calloc (<3) all other variables are 0 */

	/* open connection, create an input method and context */
	uiw->dpy = XOpenDisplay(NULL);
	uiw->xim = XOpenIM(uiw->dpy, NULL, NULL, NULL);
	uiw->xic = XCreateIC(uiw->xim, XNInputStyle, XIMPreeditNothing | XIMStatusNothing, XNClientWindow, uiw->xwindow, NULL);

	/* get black and white colours */
	uiw->black_color = BlackPixel(uiw->dpy, DefaultScreen(uiw->dpy));
	uiw->white_color = WhitePixel(uiw->dpy, DefaultScreen(uiw->dpy));


	/* create window and get attributes */
	uiw->xwindow = XCreateSimpleWindow(uiw->dpy, DefaultRootWindow(uiw->dpy), 0, 0, uiw->width, uiw->height, 0, uiw->black_color, uiw->white_color);
	XGetWindowAttributes(uiw->dpy, uiw->xwindow, &(uiw->wa));

	/* define our pixmap */
	uiw->pixmap = XCreatePixmap(uiw->dpy, uiw->xwindow, uiw->wa.width, uiw->wa.height, uiw->wa.depth);

	/* create our graphic contexts */
	uiw->wingc = XCreateGC(uiw->dpy, uiw->xwindow, 0, NULL);
	uiw->pixgc = XCreateGC(uiw->dpy, uiw->pixmap, 0, NULL);

	/* select the events we are interested in */
	XSelectInput(uiw->dpy, uiw->xwindow,
				StructureNotifyMask /* MapNotify, ConfigureNotify */
				| KeyPressMask /* KeyPress, KeyRelease */
				| ExposureMask /* Expose */
				| EnterWindowMask /* EnterNotify */
				| FocusChangeMask /* FocusIn */
				| KeymapStateMask /* KeymapNotify (follows EnterNotify and FocusIn events) */
				| ButtonPressMask /* ButtonPress */
		);

	/* set the title */
	if( uiw->buf->path ){
		/* \0 + strlen("ark ") + strlen(buf->path) */
		int len = 1 + 4 + strlen(uiw->buf->path);
		uiw->title = calloc( len, sizeof(char) );
		snprintf(uiw->title, len, "ark %s", uiw->buf->path);
	} else
		uiw->title = "ark";
	XStoreName(uiw->dpy, uiw->xwindow, uiw->title);

	/* get font information */
	/* get text information */
	XFontStruct *xfs = XQueryFont(uiw->dpy, XGContextFromGC(uiw->pixgc));
	XCharStruct xcs; /* FIXME TODO wtf is this used for? XFontStruct contains these, which char is this for? etc? */
	int dir; /* direction return */
	/* this assumes that our strings are all similar, could do per string */
	XTextExtents( xfs, "aAzZ1", 5, &dir, &(uiw->fascent), &(uiw->fdescent), &xcs);
	/* calculate offsets */
	uiw->hoffset = uiw->fdescent + uiw->fascent; /* height offset, ascent + descent */
	uiw->woffset = XTextWidth( xfs, "Z", 1 );

	return uiw;
}

void /* tidy up and free all needed elements of uiw, before freeing ui_window */
ui_teardown(ui_window *uiw){
	XFreePixmap(uiw->dpy, uiw->pixmap);
	XFreeGC(uiw->dpy, uiw->wingc); /* may fail but do we care? */
	XFreeGC(uiw->dpy, uiw->pixgc);
	XCloseDisplay(uiw->dpy);
	free(uiw->title);
	free(uiw);
}

void
ui_mainloop(ui_window *uiw){ /* Map (display) the window and begin a running loop, can be stopped via ui_stop. Re-entrant. */
	XEvent e;

	uiw->running = 1;
	XMapWindow(uiw->dpy, uiw->xwindow);
	draw(uiw);
	display(uiw);
	for( ; uiw->running ; ){
		XNextEvent(uiw->dpy, &e);
		if( handler[e.type] )
			handler[e.type](uiw, &e);
	}
	XUnmapWindow(uiw->dpy, uiw->xwindow);
}

