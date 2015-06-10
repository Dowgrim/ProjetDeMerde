#include "cache.h"

struct Cache *Cache_Create(const char *fic, unsigned nblocks, unsigned nrecords,
                           size_t recordsz, unsigned nderef) {
	struct Cache* pcache; 
	pcache->file = fic; 
	pcache->fp = fopen(fic, O_RDWR|O_CREAT|O_APPEND); 
	pcache->nblocks = nblocks; 
	pcache->nrecordsz = nrecordsz; 
	pcache->blocksz = nrecords*recordsz; 
	pcache->nderef = nderef; 
	struct Cache_Instrument cache_instrument = {0, 0, 0, 0, 0};
}

//! Fermeture (destruction) du cache.
Cache_Error Cache_Close(struct Cache *pcache) {
	close(pcache->fp);
	free(pcache);
}

//! Synchronisation du cache.
Cache_Error Cache_Sync(struct Cache *pcache) {

}

//! Invalidation du cache.
Cache_Error Cache_Invalidate(struct Cache *pcache) {
	int i=0, j=pcache->nblocks;
	for(i = 0; i < j; i++) {
		pcache->headers[i].flags = 0;
	}
}

//! Lecture  (à travers le cache).
Cache_Error Cache_Read(struct Cache *pcache, int irfile, void *precord) {

}

//! Écriture (à travers le cache).
Cache_Error Cache_Write(struct Cache *pcache, int irfile, const void *precord) {

}

//! Résultat de l'instrumentation.
struct Cache_Instrument *Cache_Get_Instrument(struct Cache *pcache) {

}