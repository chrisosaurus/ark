#include <X11/Xlib.h> /* Every Xlib program must include this */
#include <assert.h> /* FIXME testing return values the lazy way */

#define NIL (0)
/* following http://tronche.com/gui/x/xlib-tutorial/2nd-program-anatomy.html */
int main(){
    Display *dpy = XOpenDisplay(NIL);
    assert(dpy);

    /* need some colours */
    int black_color = BlackPixel(dpy, DefaultScreen(dpy));
    int white_color = WhitePixel(dpy, DefaultScreen(dpy));

    /* create the window
     * dpy is display connection
     * DefaultRootWindow(dpy) 'parent window' of the window we are creating
     * 0, 0 These are coordinates of the upper left corner of teh window
     * 200, 100 these are the width an height of the window (in pixels)
     * 0 border width
     * black_color, white_color these are the colours of the window border and window's background
     */
    Window w = XCreateSimpleWindow(dpy, DefaultRootWindow(dpy), 0, 0, 200, 100, 0, black_color, white_color);
    /* we want to get MapNotify events */
    XSelectInput(dpy, w, StructureNotifyMask);
    /* "Map" the window (that is, make it appear on the screen */
    XMapWindow(dpy, w);

    /* create a "graphics context" */
    GC gc = XCreateGC(dpy, w, 0, NIL);

    /* wait for the MapNotify event */
    XEvent e;
    for(;;){
        XNextEvent(dpy, &e);
        if( e.type == MapNotify )
            break;
    }

    /* draw a line */
    /* from (10,60), to (180,20) */
    XDrawLine(dpy, w, gc, 10, 60, 180, 20);

    /* send the drawline request to the server */
    /* NB: XNextevent performs an implicit XFlush before trying to read events */
    XFlush(dpy);

    /* wait for 10 seconds so it is displayed */
    sleep(10);
}
