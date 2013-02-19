#ifndef llist_h
#define llist_h

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

#endif
