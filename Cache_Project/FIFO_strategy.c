#include <assert.h>

#include "strategy.h"
#include "low_cache.h"
#include "time.h"

void *Strategy_Create(struct Cache *pcache) 
{
    // srand((unsigned int)time(NULL));
    return NULL;
}

void Strategy_Close(struct Cache *pcache)
{
}

void Strategy_Invalidate(struct Cache *pcache)
{
}

struct Cache_Block_Header *Strategy_Replace_Block(struct Cache *pcache) 
{
    int ib;
    struct Cache_Block_Header *pbh;

    /* On cherche d'abord un bloc invalide */
    if ((pbh = Get_Free_Block(pcache)) != NULL) return pbh;

    /* Sinon on tire un numéro de bloc au hasard */
    ib = RANDOM(0, pcache->nblocks);
    return &pcache->headers[ib];
}


void Strategy_Read(struct Cache *pcache, struct Cache_Block_Header *pbh) 
{
}  

void Strategy_Write(struct Cache *pcache, struct Cache_Block_Header *pbh)
{
} 

char *Strategy_Name()
{
    return "FIFO";
}
