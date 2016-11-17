#include "ServerHttp.h"


int create_shared_memory()
{
	if((shmid = shmget(IPC_PRIVATE, sizeof(stat_node), IPC_CREAT | 0700)) == -1 )
	{
		perror("Error creating shared memory\n");
		return 0;
	}

	if((statistics = (stats_ptr) shmat(shmid, NULL, 0)) == ((stats_ptr) -1)) 
	{
		perror("Error attaching shared memory\n");
		return 0;
	}
	return 1;

}

void * process_request()
{

}



int read_configs()
{
	FILE *fp;
	
	configuracoes =(config_node *) malloc(sizeof(config_node));	

	char line[LINE_SIZE];

	fp = fopen(FILENAME, "r");

	if ( fp == NULL )
	{
		fprintf(stderr, "Error: could not open config file\n");
		return 0;
	}

	while(fgets(line,LINE_SIZE,fp))
	{
		parse(line);

	}
	fclose(fp);
	return 1;
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
		configuracoes->server_port = (int)atoi(temp);
	}

	if ( strcmp(temp,"SCHEDULING") == 0)
	{
		temp = strtok(NULL,"\n");
		strcpy(configuracoes->scheduling,temp);
	}
	if ( strcmp(temp,"THREADPOOL") == 0)
	{
		temp = strtok(NULL,"\n");
		configuracoes->max_threads = (int)atoi(temp);
	}
	if ( strcmp(temp,"ALLOWED") == 0)
	{
		temp = strtok(NULL,",\n");
		strcpy(configuracoes->allowed,temp);
	}
}

