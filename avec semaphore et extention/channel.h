#include <pthread.h>
#include <semaphore.h>

#define E_CLOSE  2
#define EALLOC 3
/*structur sem */
typedef struct sem sem;
struct sem{
	
	sem_t sem_empty;
	sem_t sem_full;
	sem_t sem_syc;
	};


/*structur du channel */
typedef struct channel channel;
struct channel{
	int closed;
	unsigned int size;
	unsigned int eltsize;
	void *data;
	void *pread;
	void *pwrite;
	sem  *semph;
	};
/* flags */
#define CHANNEL_PROCESS_SHARED 1

struct channel *channel_create(int eltsize, int size, int flags);
void channel_destroy(struct channel *channel);
int channel_send(struct channel *channel, const void *data);
int channel_close(struct channel *channel);
int channel_recv(struct channel *channel, void *data);

struct channel *channel_named_create(const char *name, int eltsize, int size);
