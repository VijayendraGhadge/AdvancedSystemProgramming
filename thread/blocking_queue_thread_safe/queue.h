#include <stdlib.h>
#include <stdbool.h>
#include <pthread.h>

pthread_mutex_t mut=PTHREAD_MUTEX_INITIALIZER;     //statically initializing MUTEX
pthread_cond_t con=PTHREAD_COND_INITIALIZER;       //statically initializing condition variable

typedef struct element {
   void* data;
   size_t size;
   struct element* next;
} element_t;

typedef struct {
   element_t* head;
   element_t* tail;
} queue_t;

void queue_init(queue_t* queue)
{
   queue->head = NULL;
   queue->tail = NULL;
   return;
}

void queue_destroy(queue_t* queue)
{
   if (queue != NULL)
   {
      while(queue->head != NULL)
      {
	      element_t* temp = queue->head->next;
	      free(queue->head);
	      queue->head = temp;
      }
   }
   return;
}

bool push(queue_t* queue, void* data, size_t size)
{
   pthread_mutex_lock (&mut);    //Locking mutex
   element_t* next = malloc(sizeof(element_t));
   next->data = data;
   next->size = size;
   next->next = NULL;

   if (queue->head == NULL)
   {
      queue->head = next;
   }
   if (queue->tail != NULL)
   {
      queue->tail->next = next;
   }
   queue->tail = next;

   pthread_cond_signal(&con);       //signalling waiting condition variables
   pthread_mutex_unlock (&mut);     //unlocking mutex.

   return true;
}

bool pop(queue_t* queue, void** data, size_t* size)
{
   pthread_mutex_lock (&mut);          //locking mutex
   if(queue->head==NULL)pthread_cond_wait(&con,&mut);    //waiting for condition varible.
   element_t* top = queue->head;
      
      if (queue->tail == top)
      {
         queue->tail = NULL;
      }
      queue->head = top->next;
      *data = top->data;
      *size = top->size;
      free(top);
      pthread_mutex_unlock (&mut);     //unlocking mutex.
      return true;
}
