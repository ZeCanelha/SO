#ifndef CONFIG_H
#define CONFIG_H

#define FILENAME "..//configs/configs.txt"
#define LINE_SIZE 50
#define NAMED_PIPE "..//configs/pipe"
#include <sys/types.h>
/* Global Variavels */

typedef struct config
{
	int server_port;
	int max_threads;
	char scheduling[15];
	char allowed[15];
}config_node;
typedef config_node * config_ptr;

config_ptr configuracoes;
pid_t config_pid;


int server_port;
int max_threads;
char scheduling[15];
char allowed[15];


int read_configs();
void parse(char *);
void pipe_comunication();
void update_values(char * , char * , int );

#endif