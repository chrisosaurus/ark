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


    /* FIXME going off course */
    /* see  http://user.xmission.com/~georgeps/documentation/tutorials/Xlib_Beginner.html */
    XColor green_color;
    Colormap colormap = DefaultColormap(dpy, 0);
    char green[] = "#00FF00";

    /* parse the color green and allocated for later use */
    XParseColor(dpy, colormap, green, &green_color);
    XAllocColor(dpy, colormap, &green_color);
    /* set foreground color of the graphics context
     * green_color.pixel is the pixel value of the color allocated
     */
    XSetForeground(dpy, gc, green_color.pixel);
    /* back on course */


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

    /* deviation: trying to draw a string */
    char *hello = "hello world";
    XDrawString(dpy, w, gc, 20, 30, hello, strlen(hello));

    /* send the drawline request to the server */
    /* NB: XNextevent performs an implicit XFlush before trying to read events */
    XFlush(dpy);

    /* wait for 10 seconds so it is displayed */
    sleep(10);
}
