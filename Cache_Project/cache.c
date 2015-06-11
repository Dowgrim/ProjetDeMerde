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
			Cache_Replace_In_File(&pcache, pcache->headers[i]);
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
	if(Cache_Access(pcache, irfile, precord, 0)==CACHE_KO)
		return CACHE_KO;
	// On synchronise si besoin
	pcache->instrument.n_reads++;
	if(pcache->instrument.n_reads+pcache->instrument.n_writes % NSYNC == 0)
		Cache_Sync(pcache);
	Strategy_Read(pcache);
	return CACHE_OK;
}

//! Écriture (à travers le cache).
Cache_Error Cache_Write(struct Cache *pcache, int irfile, const void *precord) {
	if(Cache_Access(pcache, irfile, precord, 1)==CACHE_KO)
		return CACHE_KO;
	// On synchronise si besoin
	pcache->instrument.n_writes++;
	if(pcache->instrument.n_reads+pcache->instrument.n_writes % NSYNC == 0)
		Cache_Sync(pcache);
	Strategy_Write(pcache);
	return CACHE_OK;
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

//! Remplacement de contenu dans le fichier
Cache_Error Cache_Replace_In_File(struct Cache *pcache, struct Cache_Block_Header block) {
	// On commence par mettre le curseur dans le fichier à la position ibfile
	if(fseek(pcache->fp, pcache->headers[i].ibfile * pcache->blocksz, SEEK_SET))
		return CACHE_KO;
	// On peut ensuite écrire dans le fichier à la position définie ci-dessus
	if(fputs(block.data, pcache->fp)==EOF)
		return CACHE_KO;
	return CACHE_OK;
}

//! Accès en lecture ou en écriture
Cache_Error Cache_Access(struct Cache *pcache, int irfile, const void *precord, int mode) {
	// Etape 1 : initialisation des variables nécessaires 
	char *buffer = (char *)precord; 
	struct Cache_Block_Header block = NULL;
	// Etape 2 : on va chercher si le bloc associé à irfile est déjà dans le cache
	int index = irfile/pcache->nblocks, i, j = nblocks;
	for(i=0; i<j, i++) {
		// Si le bloc est celui qu'on chercher et qu'il n'a pas été libéré, on affecte block
		if(pcache->headers[i].ibfile==index && (pcache->headers[i].flags & VALID == 1)) {
			block = &(pcache->headers[i]);
		}
	}
	// Etape 3 : on agit selon le cas, si le bloc est déjà dans le cache ou non
	if(block==NULL) {
		// Si le bloc n'est pas dans le cache, on utilise la stratégie de remplacement
		block = Strategy_Replace_Block();
		// Si ce bloc a été modifié, on enregistre les changements dans le fichier
		if(block->flags & MODIF == 1) 
			Cache_Replace_In_File(&pcache, block);
		// On récupère le nouveau bloc dans le fichier
		index = irfile;
		if(fseek(pcache->fp, pcache->headers[i].ibfile * pcache->blocksz, SEEK_SET))
			return CACHE_KO;
		if(fgets(block->data,pcache->blocksz,pcache->fp)==NULL)
			return CACHE_KO;
		// Mise à jour de l'index du fichier correspond à ce qu'on met dans le bloc
		block->ibfile = index;
		// Le bloc est marqué comme non libre et non modifié
		block->flags |= VALID;
		block->flags &= ~MODIF;
	}
	else {
		// Si le bloc est déjà dans le cache, c'est bon; augmentation du hit rate
		pcache->instrument.n_hits++;
	}
	// Etape 4 : on agit selon la mode
	if(mode==0) {
		// Fonction read; on écrit dans le buffer 
		sprintf(buffer,block->data);
	}
	else if(mode==1) {
		// Fonction write; on écrit depuis le buffer 
		sprintf(block->data,buffer);
	}
	return CACHE_OK;
}