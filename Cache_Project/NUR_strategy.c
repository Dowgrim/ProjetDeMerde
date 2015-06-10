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
    int ib, min;
    struct Cache_Block_Header *pbh;
    struct Cache_Block_Header *pbh_min = NULL;

    /* On cherche d'abord un bloc invalide */
    if ((pbh = Get_Free_Block(pcache)) != NULL)
        return pbh;
    // initialisation du min à la valeur max du flag
    min = 4;
    for (ib = 0; ib < pcache->nblocks; ib++) {
        pbh = &pcache->headers[ib];
        // si le flag est nul, on a trouvé le bon bloc
        if (pbh->flags == 0) {
            return pbh;
        }
        // sinon on cherche la valeur du flag minimale
        else if (pbh->flags < min) {
            min = pbh->flags;
            pbh_min = pbh;
        }
        
    }

    return pbh_min;
}


void Strategy_Read(struct Cache *pcache, struct Cache_Block_Header *pbh) 
{
    // on met le premier bit à 1
    pbh->flags |= 1 << 1;
}  

void Strategy_Write(struct Cache *pcache, struct Cache_Block_Header *pbh)
{
    // on met le flag à la valeur maximale (R et M à 1)
    pbh->flags = 3;
} 

char *Strategy_Name()
{
    return "NUR";
}
