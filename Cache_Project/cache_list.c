#include "double_list.h"
#include <stdio.h>
#include <stdlib.h>

struct Cache_List *Cache_List_Create(){
	struct Cache_List *p=malloc(sizeof(struct Cache_List));
	p->prev=p;
	p->next=p;
	
	return p;
}
