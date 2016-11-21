
#ifndef SCHEDULER_H
#define SCHEDULER_H

#include "ServerHttp.h"

/* Create diferent types of scheduller
	* Type 1: FIFO
	* Type 2: Compressed
	* Type 3: Static
*/

typedef struct request
{
	int socket_id;
	char html_file[MAX_BUFF];
	int request_type;
}new_request;


typedef struct node_type
{
    new_request new;
    struct node_type * next;
}queue_node;
typedef struct node_type * node_ptr;
typedef struct
{
    node_ptr  front;
    node_ptr  rear;

}Queue;



void create_queue(Queue * queue);
void enqueue(Queue * queue, new_request new, char * scheduller);
void enqueue_fifo(Queue * queue, new_request new);
void enqueue_compressed(Queue * queue, new_request new);
void enqueue_static(Queue * queue, new_request new);
new_request dequeue(Queue *queue);
int empty_queue(Queue * queue);
void destroy_queue(Queue * queue);
void print_queue(Queue * queue);



#endif
