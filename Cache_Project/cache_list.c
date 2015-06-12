#include "cache_list.h"
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "low_cache.h"

/*! Création d'une liste de blocs */
struct Cache_List *Cache_List_Create(){
	
	//assigner la mémoire
	struct Cache_List *racine = malloc ( sizeof *racine );
	if ( racine != NULL ){
		racine->prev = racine;
		racine->next = racine;
	}
	return racine;
}

/*! Destruction d'une liste de blocs */
void Cache_List_Delete(struct Cache_List *list){

	struct Cache_List *pcurr = list;
	
	//Parcourir la liste
	for(pcurr = list->next ; pcurr != list; pcurr = pcurr->next){
		free(pcurr);		
	}
	free(list);
}

/*! Insertion d'un élément à la fin */
void Cache_List_Append(struct Cache_List *list, struct Cache_Block_Header *pbh){
    
    //augmenter la taille de la mémoire allouée
    struct Cache_List *pnew = malloc(sizeof(struct Cache_List));
	struct Cache_List *pcurr = list;
	
    pnew->pheader = pbh;

    pnew->prev = pcurr->prev;
   	pcurr->prev->next = pnew;
    pcurr->prev = pnew;
    pnew->next = pcurr;
}

/*! Insertion d'un élément au début*/
void Cache_List_Prepend(struct Cache_List *list, struct Cache_Block_Header *pbh){

	//augmenter la taille de la mémoire allouée
	struct Cache_List *pnew = malloc(sizeof(struct Cache_List));
	Cache_List_Append(list, pbh);
	list = pnew;
}

/*! Retrait du premier élément */
struct Cache_Block_Header *Cache_List_Remove_First(struct Cache_List *list){
	
	Cache_Block_Header *hpcurr = NULL;
	
	//On vérifie si la liste n'est pas vide
	if(!Cache_List_Is_Empty(list)){
		hpcurr = Cache_List_Remove(list, list->pheader);
	}

	return hpcurr;
}

/*! Retrait du dernier élément */
struct Cache_Block_Header *Cache_List_Remove_Last(struct Cache_List *list){

	Cache_Block_Header *hpcurr = NULL;
    
    //On vérifie si la liste n'est pas vide
	if(!Cache_List_Is_Empty(list)){
		hpcurr = Cache_List_Remove(list, list->prev->pheader);
	}
	
	return hpcurr;
}

/*! Retrait d'un élément quelconque */
struct Cache_Block_Header *Cache_List_Remove(struct Cache_List *list, struct Cache_Block_Header *pbh){

	struct Cache_List *pcurr = NULL;
	Cache_Block_Header *hpreturn = NULL;
      
    //Parcourir la liste	
  	for (pcurr = list->next; pcurr != list;  pcurr = pcurr->next){
		if( pcurr->pheader == pbh ){
			pcurr->prev->next = pcurr->next;
			pcurr->next->prev = pcurr->prev;
			free(pcurr);
			hpreturn = pbh;
    	}			
	}
	return hpreturn;
}

/*! Remise en l'état de liste vide */
void Cache_List_Clear(struct Cache_List *list){

	struct Cache_List *pcurr = NULL;
	struct Cache_List *pprec = NULL;
	
	//Parcourir la liste
	for (pprec = list, pcurr = list->next; pcurr != list; pprec = pcurr, pcurr = pcurr->next){
		free(pprec);
	}			
	list->next = list;
	list->prev = list;
}

/*! Test de liste vide */
bool Cache_List_Is_Empty(struct Cache_List *list){

	return list->next == list;
}

/*! Transférer un élément à la fin */
void Cache_List_Move_To_End(struct Cache_List *list,struct Cache_Block_Header *pbh){

	Cache_List_Append(list, Cache_List_Remove(list, pbh));
}

/*! Transférer un élément  au début */
void Cache_List_Move_To_Begin(struct Cache_List *list,struct Cache_Block_Header *pbh){
	
	Cache_List_Prepend(list, Cache_List_Remove(list, pbh));
}
