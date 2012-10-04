#ifndef llist_h
#define llist_h

#include <stdio.h>

/* lines are allocated with lenghts that are multiples of LINESIZE */
#define LINESIZE 80

/* a Line of text */
typedef struct Line Line;
struct Line{
	char *contents; /* \0 terminated, will include any '\n's */
	int mul; /* the multiple of LINESIZE that is the capacity of this Line */
	int len; /* number of bytes stored excluding trailing \0 (not chars as utf8 characters are multibyte) */
	Line *next;
	Line *prev;
};

/* position within the set of Lines */
typedef struct Pos Pos;
struct Pos{
	Line *line;
	int offset; /* byte offset within line (not character offset due to utf8) */
};

/* a buffer of a file */
typedef struct Buffer Buffer;
struct Buffer{
	Line *start, *end; /* first and last line */
	Line *sstart,*send; /* first and last lines on screen */
	Pos cursor; /* current cursor position within buffer */
	char *path; /* relative path to file or 0 */
};

/** Movement functions, adjust buf->cursor **/
void m_startofline(Buffer *buf);
void m_endofline(Buffer *buf);
void m_startoffile(Buffer *buf);
void m_endoffile(Buffer *buf);
void m_prevchar(Buffer *buf);
void m_nextchar(Buffer *buf);
void m_prevline(Buffer *buf);
void m_nextline(Buffer *buf);
void m_prevword(Buffer *buf);
void m_nextword(Buffer *buf);

/** llist functions **/
/* load file buf->path into buf, returns 0 on success and 1 on error */
int load(Buffer *buf);
/* save buf into file buf->path, returns 0 on success and 1 on error */
int save(Buffer *buf);
/* insert at cursor and move cursor along, returns 0 on success and 1 on error */
/* recursive specified the behavior of \n */
int insert(Buffer *buf, const char *str);
void backspace(Buffer *buf);
/* select linenum,offset from within screen */
void select(Buffer *buf, int linenum, int offset);
/* functions for converting voffsets (visual offset on screen, number of spaces occupied)
 * to ioffsers (offset into char array) (and back) using a line, int and TABWIDTH (from config.h)
 */
int vo_to_i(Line *l, int voffset);
int i_to_vo(Line *l, int offset);

/** structure management **/
Line* newline(int mul, Line *prev, Line *next);
Buffer* newbuffer(char *path);

#endif
