#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <errno.h>
#include <string.h>
#include <sys/mman.h>

#include "channel.h"

/*declaration de la fonction channel_create */
/*closed = 0 : le channel est ouvert, closed = 1 : le channel est fermer*/

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
	
	int p = pipe(canel->fd);

	if(p < 0)
		{
			return NULL;
		}
	}
	return canel;
}

/*declaration de la fonction destroy */
void channel_destroy(struct channel *channel){
	if(channel != NULL){
		free(channel);
		}
	else{
		perror("Error : DESTROY");
		}
}

/*declaration de la fonction channel_send */
int channel_send(struct channel *channel, const void *data){	

	if(channel->closed == 1){
		errno = EPIPE; 
		return -1;
	}		

	int rc = write(channel->fd[1], data, channel->eltsize);
	if( rc < 0 )
	{
		errno = EPIPE;
		return 0;
	}
			
	return 1;
}

/*declaration de la fonction channel_close */
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

/*declaration de la fonction channel_recv*/
int channel_recv(struct channel *channel, void *data){
	
	if(channel->closed == 1){
		return 0;
	}else{
	int rc = read(channel->fd[0], data, channel->eltsize);
	if( rc < 0 )
	{
		errno = EPIPE;
		return 0;
	}
		
	return 1;
	}
}
/* fin */
