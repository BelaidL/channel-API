struct channel{
	int closed; // si le channel est fermé
	unsigned int size; // le nombre des elements
	unsigned int eltsize; // la taille de chaque element
	void *data; // le pointeur du debut de la zone memoire 
	void *pread; // un pointeur utilisé pour lire dans la zone 
	void *pwrite;// un pointeur pour ecrir dans la zone 
	pthread_mutex_t verrou; // un verrou pour géré les lectures et les ecriturs 
	};

struct channel *channel_create(int eltsize, int size, int flags){
 on crré un channel et on enitialisent les champs 
 on allou la mem pour data de taille eltsize * size
 on enitialise pread et pwrite
 on retiurne le channel crée 

}
void channel_destroy(struct channel *channel){
 detruire le channel on libérent la mem de data 
}


int channel_send(struct channel *channel, const void *data){
si le channel est fermé on positionne errno et on retourne -1
sinon 
	si le channel n'est pas plein on ecrit l'élement
	sinon on bloque jusque la libération d'un l'élément

}

int channel_close(struct channel *channel){
 on verifie channel->closeed 
	si c'est 1 : on retourne 0
	si c'est 0 : on retourne 1
	sinon on on positionne errno retourne -1	
}

int channel_recv(struct channel *channel, void *data){
 si le channel est fermé on positionne errno et on retourne 0
 sinon 
	si le channel n'est pas vide on ecrit l'élement
	sinon on bloque jusque la libération d'un l'élément
}


et kami et dia vent faire le reste vous devez géré les semaphores 


et seddik vas se reposé "ya3yaaaaaaaaaaaaaaaaaaaaaaaa"


