#include <stdio.h> /* perror */
#include <stdlib.h> /* calloc, realloc */

#include "llist.h"

/* insert item after pre and before next,
 * return 0 on success and 1 on failure */
int ll_insert(void *item, void *prev, void *next){
	ll_remove(item);
	Llist *li = item;
	Llist *lp = prev;
	Llist *ln = next;

	if( ! li ) return 1;

	if( lp )
		lp->next = li;
	if( ln )
		ln->prev = li;

	li->prev = lp;
	li->next = ln;
	return 0;
}
/* remove node, linking node->prev and node->next
 * returns 0 on success and 1 on failure */
int ll_remove(void *node){
	Llist *li = node;
	if( ! li ) return 1;
	if( !li->next && !li->prev ) return 1; /* nothing to remove from */

	if( li->prev )
		li->prev->next = li->next;
	if( li->next )
		li->next->prev = li->prev;

	/* some nice tidying up, makes sure that ln doesn't think it's still in the list */
	li->next = 0;
	li->prev = 0;
	return 0;
}
/* returns previous node or 0*/
void * ll_prev(void *node){
	Llist *ln = node;
	return ln->prev;
}
/* returns next node or 0 */
void * ll_next(void *node){
	Llist *ln = node;
	return ln->next;
}

