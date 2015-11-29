#include "stdio.h"
#include "stdlib.h"
#include "pthread.h"
#include "string.h"
#include "unistd.h"
#include "queue.h"

void* producer(void* arg)
{
	queue_t* queue = (queue_t*)arg;
	size_t i = 0;
	for(i = 0; i < 100000; ++i)
	{
		push(queue, (void*)i, sizeof(i));
		if ((i % 10000) == 0) { sleep(1); }
	}
	return NULL;
}

void* consumer(void* arg)
{
	queue_t* queue = (queue_t*)arg;
	size_t i = 0;
	for(i = 0; i < 100000; ++i)
	{
		size_t i = 0;
		size_t i_size = 0;
		if (pop(queue, (void**)&i, (void*)&i_size))
		{
			/*assert(i_size == sizeof(i));*/
			fprintf(stderr, "got %d from queue\n", i);
		}
	}
	return NULL;
}

int main(int argc, char* argv[])
{
	pthread_t pro_th, con_th;
	queue_t queue;
	queue_init(&queue);

	producer(&queue);  // run on other thread
	consumer(&queue);  // run on other thread

	queue_destroy(&queue);

	return EXIT_SUCCESS;
}
