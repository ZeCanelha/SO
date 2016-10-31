#ifndef CONFIG_H
#define CONFIG_H

#define FILENAME "..//configs/configs.txt"
#define LINE_SIZE 50

/* Global Variavels */

int server_port;
int max_threads;
char scheduling[15];
char allowed[15];


int read_configs();
void parse(char *);

#endif