#include "scheduler.h"

void enqueue(Queue * queue, new_request new , char * scheduller)
{
  /* In order to guarantee success in strmcmp, put the string in upercases */

  to_upper_case(scheduller);

  if ( strcmp(scheduller,"FIFO") == 0 )
  {
    enqueue_fifo(queue,new);
  }
  else if ( strcmp(scheduller,"STATIC") == 0 )
  {
    enqueue_static(queue,new);
  }
  else if ( strcmp(scheduller,"COMPRESSED") == 0 )
  {
    enqueue_compressed(queue,new);
  }
  else
  {
    printf("Error: Schedulling type not valid.\n");
  }
}

void create_queue( Queue * queue )
{
  queue->front = NULL;
  queue->rear = NULL;
}

int empty_queue (Queue *queue)
{
    return (queue->front == NULL ? 1 : 0);
}


void destroy_queue (Queue *queue)
{
    node_ptr temp_ptr;
    while (empty_queue (queue) == 0)
    {
        temp_ptr = queue->front;
        queue->front = queue->front->next;
        free (temp_ptr);
    }
    queue->rear = NULL;
}

void enqueue_fifo ( Queue * queue, new_request new)
{
    node_ptr temp_ptr;
    temp_ptr = (node_ptr) malloc (sizeof (queue_node));
    if (temp_ptr != NULL)
    {
        temp_ptr->new = new;
        temp_ptr->next = NULL;
    }
    if (empty_queue (queue) == 1)
        queue->front = temp_ptr;
    else
        queue->rear->next = temp_ptr;
    queue->rear = temp_ptr;
}

void enqueue_static( Queue * queue , new_request new)
{
  node_ptr temp_ptr;
  node_ptr ant;
  node_ptr head;
  head = queue->front;

  temp_ptr = ( node_ptr ) malloc ( sizeof ( queue_node));
  if ( temp_ptr != NULL )
  {
    temp_ptr->new = new;
    temp_ptr->next = NULL;

    if ( empty_queue(queue) == 1 )
    {
      queue->front = temp_ptr;
      queue->rear = temp_ptr;
      /* If the request_type is static, add at the end */
      if ( new.request_type == 1 )
      {
        queue->rear->next = temp_ptr;
        queue->rear = temp_ptr;
      }
      /* If the request type is not-static, add the node after all non-static nodes*/
      else
      {
        while( head != NULL && (head->new.request_type ) == 2)
        {
          ant = head;
          head = head->next;
        }
        ant->next = temp_ptr;
        temp_ptr->next = head;
      }
    }
  }
}

void enqueue_compressed( Queue * queue , new_request new)
{
  node_ptr temp_ptr;
  node_ptr ant;
  node_ptr head;
  head = queue->front;

  temp_ptr = ( node_ptr ) malloc ( sizeof ( queue_node));
  if ( temp_ptr != NULL )
  {
    temp_ptr->new = new;
    temp_ptr->next = NULL;

    if ( empty_queue(queue) == 1 )
    {
      queue->front = temp_ptr;
      queue->rear = temp_ptr;
      /* If the request_type is compressed, add at the end */
      if ( new.request_type == 2 )
      {
        queue->rear->next = temp_ptr;
        queue->rear = temp_ptr;
      }
      /* If the request type is static, add the node after all non-static nodes*/
      else
      {
        while( head != NULL && (head->new.request_type  == 1) )
        {
          ant = head;
          head = head->next;
        }
        ant->next = temp_ptr;
        temp_ptr->next = head;
      }
    }
  }
}

void print_queue(Queue * queue){
    node_ptr temp;
    temp=queue->front;
    while(temp!=NULL)
    {
        printf("Socket id: %d\nHTML file: %s", temp->new.socket_id, temp->new.html_file);
        temp=temp->next;
    }
}

new_request dequeue (Queue *queue)
{
  node_ptr temp_ptr;
  new_request request;
  if (empty_queue (queue) == 0)
  {
    temp_ptr = queue->front;
    request = temp_ptr->new;
    queue->front = queue->front->next;
    if (empty_queue (queue) == 1)
      queue->rear = NULL;
    free (temp_ptr);
    return (request);
  }
}
