#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <errno.h>
#include <string.h>
#include <sys/mman.h>
#include <semaphore.h>

#include "channel.h"

/*declaration de la methode channel_create */
/* le channel est ouvert ---> closed = 0 si il est fermer closed = 1 */

struct channel *channel_create(int eltsize, int size, int flags){

	if ( ( size == 0 ) || ( flags == CHANNEL_PROCESS_SHARED ) ) {
        errno = ENOSYS;
        return NULL;
    }

	struct channel *canel = malloc(sizeof(channel1));
	if(canel != NULL){
		
			
	canel->size = size;
	canel->eltsize = eltsize;
	canel->closed = 0;

	canel->data = mmap(NULL, eltsize*size, PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_SHARED, -1, 0);

	if(canel->data == MAP_FAILED)
		{
			errno = EALLOC;
			return NULL;
		}

	canel->pread = canel->data;
	canel->pwrite = canel->data;

	sem_init(&canel->sem_full, CHANNEL_PROCESS_SHARED, canel->size); 
    sem_init(&canel->sem_empty, CHANNEL_PROCESS_SHARED, 0);
	pthread_mutex_init(&canel->verrou, NULL);
	}
	return canel;
}
/*declaration de la methode destroy */
void channel_destroy(struct channel *channel){
	if(channel != NULL){
		free(channel);
		}
	else{
		perror("Error : DESTROY");
		}
}
/*declaration de la methode channel_send */
int channel_send(struct channel *channel, const void *data){

	if(channel->closed == 1){
		errno = EPIPE; 
		return -1;
		
	}	

    int rc = sem_wait(&channel->sem_full);
    if ( rc < 0 ) {
        perror("sem_wait");
        exit(EXIT_FAILURE);
    }
		pthread_mutex_lock(&channel->verrou);
		memcpy(channel->pwrite, data, channel->eltsize);

		if(channel->pwrite == (channel->data +(channel->eltsize * (channel->size - 1)))){
			channel->pwrite = channel->data;
		}else{
			channel->pwrite = channel->pwrite + channel->eltsize;
		}
	pthread_mutex_unlock(&channel->verrou);
	rc = sem_post(&channel->sem_empty);
	
    return 1;				
}
/*declaration de la methode channel_close */
int channel_close(struct channel *channel){
	switch(channel->closed){
	case 1 :
						return 0;
	case 0 :{
					  channel->closed = 1;
					  return 1;
					}
default : {
						errno = E_CLOSE;	
						return -1;
					}
		}
}
/*declaration de la methode channel_recv*/
int channel_recv(struct channel *channel, void *data){
	
 
	if(channel->closed == 1){
		return 0;
	}
	
    int rc = sem_wait(&channel->sem_empty);
    if ( rc < 0 ) {
        perror("sem_post");
        exit(EXIT_FAILURE);
    }
		pthread_mutex_lock(&channel->verrou);
		memcpy(data, channel->pread, channel->eltsize);
		
		if(channel->pread == (channel->data +(channel->eltsize * (channel->size - 1)))){
			channel->pread = channel->data;
		}else{
			channel->pread = channel->pread + channel->eltsize;
		}

		pthread_mutex_unlock(&channel->verrou);
		rc = sem_post(&channel->sem_full);
	
	return 1;
}
/*end*fin*/
