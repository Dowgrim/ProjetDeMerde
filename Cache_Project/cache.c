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
	Cache_Sync(pcache);
	close(pcache->fp);
	free(pcache);
}

//! Synchronisation du cache.
Cache_Error Cache_Sync(struct Cache *pcache) {
	int i=0, j=pcache->nblocks;
	for(i = 0; i < j; i++) {
		unsigned int f = pcache->headers[i].flags;
		if(f >= 2) {
			// Le flag M est modifié, alors on écrit dans le fichier les modif
			fwrite(*pcache->headers[i]->ibfile-recordsz, pcache->recordsz, pcache->nrecordsz, pcache->fp);
			// Puis on met le flag M à 0
			f &= ~(1 << 2);
		}
	}
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
	//fread(&irfile-recordsz, pcache->precordsz, pcache->nrecordsz, pcache->fp);
	//pcache->ibfile = irfile;
}

//! Écriture (à travers le cache).
Cache_Error Cache_Write(struct Cache *pcache, int irfile, const void *precord) {
	//fwrite(&irfile-recordsz, pcache->recordsz, pcache->nrecordsz, pcache->fp);
	//pcache->headers[?]	=> Mettre à jour flag modifié à 0 ?
}

//! Résultat de l'instrumentation.
struct Cache_Instrument *Cache_Get_Instrument(struct Cache *pcache) {
	struct *Cache_Instrument copie = pcache->instrument;
	pcache->instrument->n_reads = 0;
	pcache->instrument->n_writes = 0;
	pcache->instrument->n_hits = 0;
	pcache->instrument->n_syncs = 0;
	pcache->instrument->n_deref = 0;
	return copie;
}