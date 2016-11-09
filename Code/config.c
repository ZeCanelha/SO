#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <string.h>
#include "config.h"


FILE *fp;

int read_configs();
void parse(char *);

int main(int argc, char const *argv[])
{
	
	pipe_comunication();
}

int read_configs()
{
	
	configuracoes = ( config_ptr  ) malloc (sizeof(config_node));

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


/* Update configurations through named pipe */
/* Wait for threads to finish if we need to change the threadpool */

void pipe_comunication()
{
	char c;
	char scheduling[15];
	char allowed[15];
	int threadpool;
	int flag = 0;

	printf("Gestor de configurações.. \n");
	while(1)
	{
		printf("Press enter to continue\n");
		c = getchar();
		if ( c == '\n')
		{
			printf("\nAlterar definições..");

			do
			{
				printf("\n - Tipo de escalonamento (static/compressed/fifo/default ): ");
				fgets(scheduling,15,stdin);
				
				if ( (strcmp(scheduling,"static\n")==0) || (strcmp(scheduling,"fifo\n")==0)  || ( strcmp(scheduling,"compressed\n") == 0) || (strcmp(scheduling,"default\n")==0))
				{
					printf("\nAlterado!\n");
					flag = 1;
				}
			}while(flag == 0);
			flag = 0;
			do
			{
				printf("\n - Permissoes: (gz/zip/default): ");
				fgets(allowed,15,stdin);
				
				if ( (strcmp(scheduling,"gz\n")==0) || (strcmp(scheduling,"zip\n")==0) || (strcmp(scheduling,"default\n")==0) )
				{
					printf("Alterado!\n");
					flag = 1;
				}
			}while(flag == 0);
			flag = 0;

			printf("\nNumero de threads: ");
			scanf("%d",&threadpool);

			update_values(scheduling,allowed,threadpool);
		}
	}
}


void update_values(char * scheduling , char * allowed , int threadpool )
{
	
}