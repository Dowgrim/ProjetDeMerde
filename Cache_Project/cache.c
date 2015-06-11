#include "cache.h"

struct Cache *Cache_Create(const char *fic, unsigned nblocks, unsigned nrecords,
                           size_t recordsz, unsigned nderef) {
	struct Cache* pcache = malloc(sizeof(struct Cache)); 
	pcache->file = fic; 
	pcache->fp = fopen(fic, O_RDWR|O_CREAT|O_TRUNC); 
	pcache->nblocks = nblocks;
	pcache->nrecordsz = nrecordsz; 
	pcache->blocksz = nrecords*recordsz; 
	pcache->nderef = nderef; 
	pcache->pstrategy = Strategy_Create(pcache);
	// On initialise les données utiles à l'instrumentation
	struct Cache_Instrument cache_instrument = {0, 0, 0, 0, 0};
	struct *Cache_Block_Header headers = malloc(sizeof(struct Cache_Block_Header)*(nrecords));
	pcache->instrument = cache_instrument;
	pcache->headers = cache_header;
	int i;
	// On initialise les headers du cache
	for(i = 0; i < nblocks; i++) {
		pcache->headers[i].data = malloc(recordsz*nrecords);
		pcache->headers[i].ibcache = i;
		pcache->headers[i].flags = 0;
	}
	pcache->pfree = &(pcache->headers[0]);
}

//! Fermeture (destruction) du cache.
Cache_Error Cache_Close(struct Cache *pcache) {
	// On synchronise le cache et on ferme le fichier
	if(Cache_Sync(pcache)==CACHE_KO || fclose(pcache->fp))
		return CACHE_KO;
	// Libération de toute la mémoire alloué lors de la création du cache
	Strategy_Close(pcache);
	for(i = 0 ; i < pcache->nblocks ; i++){
		free(pcache->headers[i].data);
	}
	free(pcache->headers);
	free(pcache);
}

//! Synchronisation du cache.
Cache_Error Cache_Sync(struct Cache *pcache) {
	// On va parcourir tous les blocs du cache
	int i=0, j=pcache->nblocks;
	for(i = 0; i < j; i++) {
		unsigned int f = pcache->headers[i].flags;
		if(f & MODIF) {
			// Le flag M est modifié, alors on écrit dans le fichier les modif
			// On commence par mettre le curseur dans le fichier à la position ibfile
			if(fseek(pcache->fp, pcache->headers[i].ibfile * pcache->blocksz, SEEK_SET))
				return CACHE_KO;
			// On peut ensuite écrire dans le fichier à la position définie ci-dessus
			if(fputs(pcache->headers[i].data, pcache->fp)==EOF)
				return CACHE_KO;
			// Puis on met le flag M à 0
			f &= ~MODIF;
		}
	}
	// La fonction n'a pas généré d'erreur
	return CACHE_OK;
}

//! Invalidation du cache.
Cache_Error Cache_Invalidate(struct Cache *pcache) {
	int i=0, j=pcache->nblocks;
	// Tous les blocks deviennent libre, pfree est donc le premier bloc
	pcache->pfree = &(pcache->headers[0]);
	// On marque tous les blocs comme valides
	for(i = 0; i < j; i++) {
		pcache->headers[i].flags &= ~VALID;
	}
	Strategy_Invalidate(pcache);
	return CACHE_OK;
}

//! Lecture  (à travers le cache).
Cache_Error Cache_Read(struct Cache *pcache, int irfile, void *precord) {
	//fread(&pcache->data, pcache->recordsz, pcache->nrecordsz, pcache->fp);
	//pcache->ibfile = irfile;	
}

//! Écriture (à travers le cache).
Cache_Error Cache_Write(struct Cache *pcache, int irfile, const void *precord) {
	//fwrite(&irfile-recordsz, pcache->recordsz, pcache->nrecordsz, pcache->fp);
	//pcache->headers[?]	=> Mettre à jour flag modifié à 0 ?
}

//! Résultat de l'instrumentation.
struct Cache_Instrument *Cache_Get_Instrument(struct Cache *pcache) {
	struct *Cache_Instrument copie = malloc(sizeof(struct Cache_Instrument));
	copie->n_reads = pcache->instrument->n_reads;
	copie->n_writes = pcache->instrument->n_writes;
	copie->n_hits = pcache->instrument->n_hits;
	copie->n_syncs = pcache->instrument->n_syncs;
	copie->n_deref = pcache->instrument->n_deref;
	pcache->instrument = {0, 0, 0, 0, 0};
	return copie;
}
