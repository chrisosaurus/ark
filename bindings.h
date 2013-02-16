/* See LICENSE file or dl.segfault.net.nz/LICENSE for copyright and license details */

/***********************************
 * config for ui keyboard bindings
 ***********************************/

/* commands you can bind to Arg.v
 * these are passed to execvp so they should be formatted similar to
 * {'program', 'flag', 'arg', ... }
 * ie;
 * {'xterm', '-bg', 'black'}
 */
static const char *blahcmd[] = {"echo", "hello"};

/* key definitions */
/* Mod1Mask is alt, Mod4Mask is windows key */
#define MODKEY Mod1Mask
#define CTRL ControlMask
#define SHIFT ShiftMask

/* key bindings */
static Key keys[] = {
	/* modifider	key		function	argument */
	{ MODKEY,	XK_q,		f_quit,		{ .i=0 } },
	{ MODKEY|SHIFT,	XK_Q,		f_quit,		{ .i=1 } },
	{ MODKEY,	XK_b,		0,		{ .v=blahcmd } },
	{ MODKEY,	XK_w,		f_save,		{ .i=0 } },

	{ MODKEY,	XK_i,		f_func,		{ .f=m_prevline } },
	{ MODKEY,	XK_j,		f_func,		{ .f=m_prevchar } },
	{ MODKEY,	XK_k,		f_func,		{ .f=m_nextline } },
	{ MODKEY,	XK_l,		f_func,		{ .f=m_nextchar } },

	{ MODKEY|SHIFT,	XK_J,		f_func,		{ .f=m_prevword } },
	{ MODKEY|SHIFT,	XK_L,		f_func,		{ .f=m_nextword } },

	{ CTRL,		XK_i,		f_func,		{ .f=m_prevline } },
	{ CTRL,		XK_j,		f_func,		{ .f=m_prevchar } },
	{ CTRL,		XK_k,		f_func,		{ .f=m_nextline } },
	{ CTRL,		XK_l,		f_func,		{ .f=m_nextchar } },

	{ 0,		XK_Left,	f_func,		{ .f=m_prevchar } },
	{ 0,		XK_Right,	f_func,		{ .f=m_nextchar } },
	{ 0,		XK_Up,		f_func,		{ .f=m_prevline } },
	{ 0,		XK_Down,	f_func,		{ .f=m_nextline } },
	{ 0,		XK_BackSpace,	f_func,		{ .f=backspace } },
	{ SHIFT,	XK_BackSpace,	f_func,		{ .f=backspace } },

	{ CTRL,		XK_Left,	f_func,		{ .f=m_prevword } },
	{ CTRL,		XK_Right,	f_func,		{ .f=m_nextword } }

};
