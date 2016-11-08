#ifndef CONFIG_H
#define CONFIG_H

#define FILENAME "..//configs/configs.txt"
#define LINE_SIZE 50

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

#endif