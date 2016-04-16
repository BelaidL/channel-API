#include <pthread.h>
#include <semaphore.h>

#define E_CLOSE  2
#define EALLOC 3
/*structur du channel */

typedef struct channel channel1;
struct channel{
	int closed;
	unsigned int size;
	unsigned int eltsize;
	//unsigned int count;
	void *data;
	void *pread;
	void *pwrite;
	pthread_mutex_t verrou;
	pthread_cond_t full , empty;
	sem_t sem_empty;
	sem_t sem_full;
	};
/* flags */
#define CHANNEL_PROCESS_SHARED 1

struct channel *channel_create(int eltsize, int size, int flags);
void channel_destroy(struct channel *channel);
int channel_send(struct channel *channel, const void *data);
int channel_close(struct channel *channel);
int channel_recv(struct channel *channel, void *data);

