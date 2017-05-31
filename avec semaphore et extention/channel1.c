#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <errno.h>
#include <string.h>
#include <sys/mman.h>
#include <semaphore.h>
#include <sys/stat.h>
#include <inttypes.h>
#include <fcntl.h>

#include "channel.h"

/*declaration de la methode channel_create */
/* le channel est ouvert ---> closed = 0 si il est fermer closed = 1 */

struct channel *channel_create(int eltsize, int size, int flags){
	int flag;
	

	struct channel *canel = malloc(sizeof(channel));
	if(canel == NULL){
		return NULL;
			}
	canel->size = size;
	canel->eltsize = eltsize;
	canel->closed = 0;
	if(flags == CHANNEL_PROCESS_SHARED ){
		flag = MAP_SHARED;
	}else
		{
			flag = MAP_PRIVATE;
		}

	if ( ( size != 0 )) {
        canel->data = mmap(NULL, eltsize*size, PROT_READ | PROT_WRITE, MAP_ANONYMOUS | flag, -1, 0);
    }
	
	canel->semph = mmap(NULL, sizeof(sem), PROT_READ | PROT_WRITE, MAP_ANONYMOUS | flag, -1, 0);
	
	if(canel->data == MAP_FAILED)
		{
			errno = EALLOC;
			return NULL;
		}

	canel->pread = canel->data;
	canel->pwrite = canel->data;

   sem_init(&canel->semph->sem_full, CHANNEL_PROCESS_SHARED, canel->size); 
   
	if(size != 0){
	sem_init(&canel->semph->sem_empty, CHANNEL_PROCESS_SHARED, 0);
	sem_init(&canel->semph->sem_syc, CHANNEL_PROCESS_SHARED, 1);
	}else{
		
		sem_init(&canel->semph->sem_syc, CHANNEL_PROCESS_SHARED, 0);
		}
	return canel;
}

struct channel *channel_named_create(const char *name, int eltsize, int size){
	 int fd;    
    int rc;
    int size_buf, nbr_page_buf;
	 int new_size;
    struct stat st;
    struct channel *canel;
    int old_size;
    
    if ( size == 0 ) {
        errno = ENOSYS;
        return NULL;
    }

    size_buf = eltsize*size;
	 nbr_page_buf = size_buf / sysconf(_SC_PAGE_SIZE);

	 if( (size_buf % sysconf(_SC_PAGE_SIZE)) != 0 ){
	 	nbr_page_buf += 1;
	 }
                    
    fd = shm_open(name, O_CREAT | O_RDWR, 0666);
    if ( fd < 0 ) {
		  perror("shm_open");
        return NULL;
    }

    fstat(fd, &st);
    old_size = st.st_size;
                                
	 if(old_size == 0){
    rc = ftruncate(fd, sysconf(_SC_PAGE_SIZE)*(nbr_page_buf+2));
    if(rc < 0){
		  perror("ftruncate");
        shm_unlink(name);
        return NULL;
     }
    }

    fstat(fd, &st);
    new_size = sysconf(_SC_PAGE_SIZE)*(nbr_page_buf+2);
    
    canel = mmap(NULL, st.st_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if ( canel == MAP_FAILED ) {
		  perror("mmap_failed");
        return NULL;
    }


    canel->semph = mmap(NULL, sizeof(sem), PROT_READ | PROT_WRITE,
		    MAP_SHARED, fd, sysconf(_SC_PAGE_SIZE));
    if ( canel->semph == MAP_FAILED ) {
		  perror("mmap_failed");
        return NULL;
    }

    canel->data = mmap(NULL, size_buf, PROT_READ | PROT_WRITE,
		    MAP_SHARED, fd, sysconf(_SC_PAGE_SIZE)*2);
    if ( canel->data == MAP_FAILED ) {
		  perror("mmap_failed");
        return NULL;
    }

    canel->size = size;
    canel->eltsize = eltsize;
    canel->closed = 0;
    
    canel->pread = canel->data;
    canel->pwrite = canel->data;

/*initialiser les semaphores par le premier ps qui cree le fichier */
    if ( old_size != new_size ) {
    sem_init(&canel->semph->sem_full, CHANNEL_PROCESS_SHARED, canel->size); 
    sem_init(&canel->semph->sem_empty, CHANNEL_PROCESS_SHARED, 10);
    sem_init(&canel->semph->sem_syc, CHANNEL_PROCESS_SHARED, 1);
    }
	return canel;
		
}


/*declaration de la methode destroy */
void channel_destroy(struct channel *channel){
	if(channel != NULL){
		munmap(&channel->data, (channel->size * channel->eltsize));
		munmap(&channel->semph, sizeof(sem));
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
	 
	 if(channel->size != 0 ){
    int rc = sem_wait(&channel->semph->sem_full);
    if ( rc < 0 ) {
        perror("sem_wait");
        exit(EXIT_FAILURE);
    }
		sem_wait(&channel->semph->sem_syc);
		memcpy(channel->pwrite, data, channel->eltsize);

		if(channel->pwrite == (channel->data +(channel->eltsize * (channel->size - 1)))){
			channel->pwrite = channel->data;
		}else{
			channel->pwrite = channel->pwrite + channel->eltsize;
		}
	sem_post(&channel->semph->sem_syc);
	rc = sem_post(&channel->semph->sem_empty);
	}else{
	 //sync
	 int rc = sem_wait(&channel->semph->sem_full);
    if ( rc < 0 ) {
        perror("sem_wait");
        exit(EXIT_FAILURE);
    }
	 
	 memcpy(channel->data, data, channel->eltsize);
	 sem_post(&channel->semph->sem_syc);
	}
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

	 if(channel->size != 0 ){
    int rc = sem_wait(&channel->semph->sem_empty);
    if ( rc < 0 ) {
        perror("sem_post");
        exit(EXIT_FAILURE);
    }

		sem_wait(&channel->semph->sem_syc);
		memcpy(data, channel->pread, channel->eltsize);
		
		if(channel->pread == (channel->data +(channel->eltsize * (channel->size - 1)))){
			channel->pread = channel->data;
		}else{
			channel->pread = channel->pread + channel->eltsize;
		}

		sem_post(&channel->semph->sem_syc);
		rc = sem_post(&channel->semph->sem_full);
	}else{
			 //sync
			 channel->data = data;
			 int rc = sem_post(&channel->semph->sem_full);

			 rc = sem_wait(&channel->semph->sem_syc);
			 if ( rc < 0 ) {
				  perror("sem_post");
				  exit(EXIT_FAILURE);
			 }
			
		}
	return 1;
}
/*end*fin*/
