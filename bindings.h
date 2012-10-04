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

/* key bindings */
static Key keys[] = {
	/* modifider	key	function	argument */
	{ MODKEY,	XK_q,	f_quit,		{ .i=0 } },
	{ MODKEY,	XK_b,	0,		{ .v=blahcmd } },
	{ 0,		XK_Left, f_cur, { .m=m_prevchar } },
	{ 0,		XK_Right, f_cur, { .m=m_nextchar } },
	{ 0,		XK_Up, f_cur, { .m=m_prevline } },
	{ 0,		XK_Down, f_cur, { .m=m_nextline } }
};
