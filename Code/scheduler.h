/* SISTEMAS OPERATIVOS 2016/2017
 *
 * JOSÉ CANELHA 2012169312 125H
 * RÚBEN TOMÁS 2012172347 100h
 *
*/

#ifndef SCHEDULER_H
#define SCHEDULER_H

#define MAX_BUFFER 15

/* Request types
 * 1 - Static
 * 2 - Compressed
*/
typedef struct request
{
	int socket_id;
	char html_file[MAX_BUFFER];
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
