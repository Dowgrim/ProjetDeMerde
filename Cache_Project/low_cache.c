#include "low_cache.h"

struct Cache_Block_Header *Get_Free_Block(struct Cache *pcache) {
	int i=0, j=pcache->nblocks;
	for(i = 0; i < j; i++) {
		// On parcoure les blocs du cache
		if(pcache->headers[i].flags & VALID) {
			// On a un bloc libre, on le retourne
			return &(pcache->headers[i]);
		}
	}
	// On a pas trouvé de bloc libre, on return alors NULL comme indiqué dans le sujet
	return NULL;
}