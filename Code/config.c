#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "config.h"


FILE *fp;

int read_configs();
void parse(char *);


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