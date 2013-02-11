#ifndef ark_h
#define ark_h

#include "ui.h"

/** bindable Functions **/
void f_quit(ui_window *uiw, const Arg *arg); /* if arg->i then will quit regardless of changes, otherwise will not quit until changes are saved */
void f_func(ui_window *uiw, const Arg *arg); /* execute function in arg->f */
void f_save(ui_window *uiw, const Arg *arg); /* write buffer contents to buffer path */

#endif
