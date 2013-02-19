#ifndef llist_h
#define llist_h

/* lines are allocated with lenghts that are multiples of LINESIZE */
#define LINESIZE 80

/* linked list */
typedef struct Llist Llist;
struct Llist {
	Llist *next;
	Llist *prev;
};

/** Llist functions, they take a void * so that it can silently downcast from derived types **/

/* insert item after prev and before next
 * returns 0 on success and 1 on failure */
int ll_insert(void *item, void *prev, void *next);
/* remove node, linking node->prev and node->next
 * returns 0 on success and 1 on failure */
int ll_remove(void *node);
/* returns previous node */
void * ll_prev(void *node);
/* returns next node */
void * ll_next(void *node);

/** --- below this line needs to be moved --- **/

/* a Line of text */
typedef struct Line Line;
struct Line {
	Llist ll;
	char *con; /* contents, \0 terminated, will include any '\n's */
	int mul; /* the multiple of LINESIZE that is the capacity of this Line */
	int len; /* number of bytes stored excluding trailing \0 (not chars as utf8 characters are multibyte) */
};

/* position within the set of Lines */
typedef struct Pos Pos;
struct Pos {
	Line *line;
	int offset; /* byte offset within line (not character offset due to utf8) */
};

/* a buffer of a file */
typedef struct Buffer Buffer;
struct Buffer {
	Line *start, *end; /* first and last line */
	Line *s_start,*s_end; /* first and last lines on screen ('screen_start' and 'screen_end')*/
	Pos cur; /* current cursor position within buffer */
	char *path; /* relative path to file or 0 */
	int mod; /* if the buffer has been modified since it was last saved */
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
void m_scrolldown(Buffer *buf);
void m_scrollup(Buffer *buf);

/** llist functions **/
/* load file buf->path into buf, returns 0 on success and 1 on error */
int load(Buffer *buf);
/* save buf into file buf->path, returns 0 on success and 1 on error */
int save(Buffer *buf);
/* insert at cursor and move cursor along, returns 0 on success and 1 on error */
/* recursive specified the behavior of \n */
int insert(Buffer *buf, const char *str);
void backspace(Buffer *buf);
/* move cursor by linenum and offset */
void position_cursor(Buffer *buf, int linenum, int offset);
/* functions for converting voffsets (visual offset on screen, number of spaces occupied)
 * to ioffsers (offset into char array) (and back) using a line, int and TABWIDTH (from config.h)
 */
int vo_to_i(Line *l, int voffset);
int i_to_vo(Line *l, int offset);

/** structure management **/
Line* newline(int mul, Line *prev, Line *next);
Buffer* newbuffer(char *path);

#endif
