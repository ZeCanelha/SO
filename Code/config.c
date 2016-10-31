#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#define FILENAME "..//configs/configs.txt"
#define LINE_SIZE 50

FILE *fp;

int server_port;
int max_threads;
char scheduling[15];
char allowed[15];


int read_configs();
void parse(char *);

int main(int argc, char const *argv[])
{
	
	int ret_val;
	printf("Reading config..\n");
	ret_val = read_configs();
	printf("SERVERPORT: %d\n", server_port);
	printf("MAX_THREADS: %d\n", max_threads);
	printf("SCHEDULING: %s\n", scheduling);
	printf("ALLOWED: %s\n", allowed);

	printf("%d\n", ret_val);
	
	return 0;
}

int read_configs()
{
	
	char line[LINE_SIZE];

	fp = fopen(FILENAME, "r");

	if ( fp == NULL )
	{
		fprintf(stderr, "Error: could not open config file\n");
		return 1;
	}

	while(fgets(line,LINE_SIZE,fp))
	{
		parse(line);

	}
	fclose(fp);
	return 0;
}

void parse( char * line)
{
	char *temp =(char*)malloc(LINE_SIZE * sizeof(char));
	char *line_temp = (char*)malloc(LINE_SIZE * sizeof(char));
	strcpy(line_temp,line);


	temp = strtok(line_temp, "=");

	if ( strcmp(temp,"SERVERPORT") == 0)
	{
		temp = strtok(NULL,"\n");
		server_port = (int)atoi(temp);
	}

	if ( strcmp(temp,"SCHEDULING") == 0)
	{
		temp = strtok(NULL,"\n");
		strcpy(scheduling,temp);
	}
	if ( strcmp(temp,"THREADPOOL") == 0)
	{
		temp = strtok(NULL,"\n");
		max_threads = (int) atoi(temp);
	}
	if ( strcmp(temp,"ALLOWED") == 0)
	{
		temp = strtok(NULL,",\n");
		strcpy(allowed,temp);
	}
}