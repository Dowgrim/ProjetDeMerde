#include <assert.h>

#include "strategy.h"
#include "low_cache.h"
#include "time.h"


void deref(struct Cache *pcache)
{
    // on met le bit REFER à 0 dans tous les blocs
    for (int i = 0; i < pcache->nblocks; i++)
        pcache->headers[i].flags &= ~REFER;
    // ràz du compteur
    pcache->pstrategy = 0;
    // on incrémente le compteur
    ++pcache->instrument.n_deref;
}

// calcul de la valeur de rm : 2 * R + M
int valuate_rm(struct Cache_Block_Header *bloc) {
    int rm = 0;
    return rm = (2 * (bloc->flags & REFER)) + (bloc->flags & MODIF);
}

void *Strategy_Create(struct Cache *pcache) 
{
}

void Strategy_Close(struct Cache *pcache)
{
}

void Strategy_Invalidate(struct Cache *pcache)
{
    deref(pcache);
}

struct Cache_Block_Header *Strategy_Replace_Block(struct Cache *pcache) 
{
    // initialisation du min à la valeur max du flag (une valeur qui ne peut être atteinte)
    int min = 4;
    struct Cache_Block_Header *pbh;
    // on regardera la valeur du flag le plus petit
    struct Cache_Block_Header *pbh_min = NULL;

    /* On cherche d'abord un bloc invalide */
    if ((pbh = Get_Free_Block(pcache)) != NULL)
        return pbh;
    for (int i = 0; i < pcache->nblocks; i++) {
        pbh = &pcache->headers[i];
        // si le flag est nul, on a trouvé le bon bloc
        if (valuate_rm(pbh) == 0) {
            return pbh;
        }
        // sinon on cherche la valeur du flag minimale
        else if (min == 4 || valuate_rm(pbh) < min) {
            min = pbh->flags;
            pbh_min = pbh;
        }   
    }
    return pbh_min;
}

void Strategy_Read(struct Cache *pcache, struct Cache_Block_Header *pbh) 
{
    // réinitialisation du compteur si on dépasse la limite de nderef
    // (on a fait ndref accès)
    if (++pcache->pstrategy >= pcache->nderef)
        deref(pcache);

    // on met le bit REFER à 1 (on a accédé au bloc)
    pbh->flags |= REFER;
}  

void Strategy_Write(struct Cache *pcache, struct Cache_Block_Header *pbh)
{
    if ((++pcache->pstrategy) >= pcache->nderef)
        deref(pcache);

    pbh->flags |= REFER;
} 

char *Strategy_Name()
{
    return "NUR";
}
