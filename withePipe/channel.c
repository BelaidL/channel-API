#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <errno.h>
#include <string.h>
#include <sys/mman.h>

#include "channel.h"

/**
*	channel->closed == 0 : the channel is open;
* channel->closed == 1 : the channel is closed;
**/

struct channel *channel_create(int eltsize, int size, int flags){
	
	if ((size == 0) || (flags == CHANNEL_PROCESS_SHARED)) {
        errno = ENOSYS;
        return NULL;
    }

	struct channel *c = malloc(sizeof(channel));
	if(c != NULL){
				
	c->size = size;
	c->eltsize = eltsize;
	c->closed = 0;
	
	int p = pipe(c->fd);

	if(p < 0)
		{
			return NULL;
		}
	}
	return c;
}

void channel_destroy(struct channel *channel){
	if(channel != NULL){
		free(channel);
		}
	else{
		perror("Error : DESTROY");
		}
}

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

