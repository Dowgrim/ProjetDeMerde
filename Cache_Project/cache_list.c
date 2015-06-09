#include "cache_list.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

struct Cache_List *Cache_List_Create(){

	struct Cache_List *p=malloc(sizeof(struct Cache_List));
	p->prev=p;
	p->next=p;
	
	return p;
}

void Cache_List_Delete(struct Cache_List *list){
	
	struct Cache_List *pcurr;
	
	for (pcurr = list->next; pcurr != list->pheader; pcurr = pcurr->next)
    { }
	
	if (pcurr != list->pheader)
    {
        pcurr->prev->next = pcurr->next;
        pcurr->next->prev = pcurr->prev;
        free(pcurr);
    }
}

int main(){

	struct Cache_List *d0=Cache_List_Create();
	dlist_insert(d0,10);
	dlist_insert(d0,6);
	dlist_insert(d0,3);
	dlist_insert(d0,7);
	dlist_insert(d0,9);
}
