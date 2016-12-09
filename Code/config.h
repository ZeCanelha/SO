#ifndef CONFIG_H
#define CONFIG_H

//#include <semaphore.h>
#include <sys/types.h>
/* Global Variavels */
#define NAMED_PIPE "configspipe"
//#define SEM_NAME "pipe_control"
#define MAX_BUFF 15
#define MAX_FILES_ALLOWED 5
#define FILES_ALLOWED_SIZE 25


/* http://stackoverflow.com/questions/8359322/how-to-share-semaphores-between-processes-using-shared-memory */


typedef struct
{
	char schedulling[MAX_BUFF];
	int string_counter;
	char allowed[MAX_FILES_ALLOWED][FILES_ALLOWED_SIZE];
	int max_threads;
}config;

config new_configs;
pid_t config_pid;

void pipe_comunication();
void update_values();
void catch_pipe();

#endif
