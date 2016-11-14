
#ifndef SCHEDULER_H
#define SCHEDULER_H

/* Create diferent types of scheduller */

/* Type 1: FIFO */
/* Type 2: Compressed */
/* Type 3: Static */

typedef struct request
{
	int socket_id;
	int request_type;
}new_request;


typedef struct queue_node *node_ptr;
typedef struct queue
{
	new_request data;
	node_ptr next_node;
}queue_node;

typedef struct 
{
	node_ptr head;
	node_ptr tail;
}Queue;


void create_queue(Queue * queue);
void enqueue(Queue * queue, new_request new);
new_request dequeue(Queue *queue);
int empty_queue(Queue * queue);



#endif