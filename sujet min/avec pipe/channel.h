#include <pthread.h>

/*structur du channel */

typedef struct channel channel1;
struct channel{
	int closed;
	unsigned int size;
	unsigned int eltsize;
	int fd[2];
	};
/* flags */
#define CHANNEL_PROCESS_SHARED 1
#define E_CLOSE  2

struct channel *channel_create(int eltsize, int size, int flags);
void channel_destroy(struct channel *channel);
int channel_send(struct channel *channel, const void *data);
int channel_close(struct channel *channel);
int channel_recv(struct channel *channel, void *data);

