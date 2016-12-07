#ifndef CONFIG_H
#define CONFIG_H

//#include <semaphore.h>
#include <sys/types.h>
/* Global Variavels */
#define NAMED_PIPE "configspipe"
//#define SEM_NAME "pipe_control"
#define MAX_BUFF 15


/* http://stackoverflow.com/questions/8359322/how-to-share-semaphores-between-processes-using-shared-memory */


typedef struct
{
	int schedulling;
	int allowed;
	int max_threads;

}config;


pid_t config_pid;

void pipe_comunication();
void update_values(int ,int , int );

#endif
