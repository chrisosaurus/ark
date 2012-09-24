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


/** Movement functions **/
Pos m_startofline(Pos pos);
Pos m_endofline(Pos pos);
Pos m_startoffile(Pos pos);
Pos m_endoffile(Pos pos);
Pos m_prevchar(Pos pos);
Pos m_nextchar(Pos pos);
Pos m_prevline(Pos pos);
Pos m_nextline(Pos pos);
Pos m_prevword(Pos pos);
Pos m_nextword(Pos pos);

/** llist functions **/
void ll_setup();
void ll_teardown();
Pos load(Pos into, FILE *f);
void save(Pos from, Pos to, FILE *f);
Pos insert(Pos pos, const char *str);
Line* newline(int mul, Line *prev, Line *next);

#endif
