#ifndef CONFIG_H
#define CONFIG_H

#include <semaphore.h>
#include <sys/types.h>
/* Global Variavels */
#define NAMED_PIPE "configspipe"
#define SEM_NAME "pipe_control"
#define MAX_BUFF 15


/* http://stackoverflow.com/questions/8359322/how-to-share-semaphores-between-processes-using-shared-memory */


typedef struct 
{
	int server_port;
	int max_threads;
	char scheduling[MAX_BUFF];
	char allowed[MAX_BUFF];
}config_node;



config_node  * configuracoes;

pid_t config_pid;

void pipe_comunication();
void update_values(char * , char * , int );

#endif