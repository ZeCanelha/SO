#ifndef CONFIG_H
#define CONFIG_H

#include <sys/types.h>
/* Global Variavels */
#define NAMED_PIPE "configspipe"


typedef struct config
{
	int server_port;
	int max_threads;
	char scheduling[15];
	char allowed[15];
}config_node;
typedef config_node * config_ptr;


typedef struct 
{
	int a;
	int b;
}teste;


config_ptr configuracoes;
pid_t config_pid;

void pipe_comunication();
void update_values(char * , char * , int );

#endif