#include <X11/Xlib.h> /* Every Xlib program must include this */
#include <assert.h> /* FIXME testing return values the lazy way */
#include <string.h>

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

    Pixmap pixmap = XCreatePixmap(dpy, w, 100, 100, 1); /* FIXME should I use w or DefaultRootWindow(dpy) for offscreen? */
    GC pgc = XCreateGC(dpy, pixmap, 0, NIL);
    GC wgc = XCreateGC(dpy, pixmap, 0, NIL);


    /* see  http://user.xmission.com/~georgeps/documentation/tutorials/Xlib_Beginner.html */
    XColor blue_color;
    Colormap colormap = DefaultColormap(dpy, 0);
    char blue[] = "#0000FF";

    /* parse the color green and allocated for later use */
    XParseColor(dpy, colormap, blue, &blue_color);
    XAllocColor(dpy, colormap, &blue_color);
    /* set foreground color of the graphics context
     * green_color.pixel is the pixel value of the color allocated
     */
    XSetForeground(dpy, gc, blue_color.pixel);
    XSetForeground(dpy, pgc, blue_color.pixel);

    XMapWindow(dpy, w);

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

    /* create a pixmap, draw to it, and swap it to window */
    char *string = "hello world I am some awesome text";
    //int width = XTextWidth(None, string, strlen(string)); // FIXME segfaults
    XFillRectangle(dpy, pixmap, wgc, 0, 0, 100, 100);
    XDrawString(dpy, pixmap, pgc, 20, 30, string, strlen(string));
    XCopyPlane(dpy, pixmap, w, gc, 0, 0, 100, 100, 0, 0, 1);
    XSync(dpy, False);

    /* send the drawline request to the server */
    /* NB: XNextevent performs an implicit XFlush before trying to read events */
    XFlush(dpy);

    /* wait for 10 seconds so it is displayed */
    sleep(10);
}

/* NOTES from http://fixunix.com/x/346539-write-xlib-textbox-application-without-widgets.html
 1. measure the text with XTextWidth()

 2. use XCreateWindow to create a window or subwindow with padding for
 around the text.

 3. create a Pixmap with XCreatePixmap() that is the size of the window.

 4. use XFillRectangle to draw a background for the Pixmap.

 5. XDrawText(), or XDrawString, or XDrawImageString on the Pixmap.

 6. Use XCopyArea to copy the Pixmap to the Window.

 7. call XMapWindow

 8. call XFlush

 9. save the Pixmap and copy the Pixmap to the Window whenever you
 receive an Expose event.
 */
