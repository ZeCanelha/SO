#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <string.h>
#include "config.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>


FILE *fp;

int read_configs();
void parse(char *);

int main(int argc, char const *argv[])
{
	
	pipe_comunication();
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
		fflush(stdin);
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
				
				if ( (strcmp(allowed,"gz\n")==0) || (strcmp(allowed,"zip\n")==0) || (strcmp(allowed,"default\n")==0) )
				{
					printf("Alterado!\n");
					flag = 1;
				}
			}while(flag == 0);
			flag = 0;

			printf("\nNumero de threads ( 0 para manter ): ");
			scanf("%d",&threadpool);

			update_values(scheduling,allowed,threadpool);
		}
	}
}


void update_values(char * scheduling , char * allowed , int threadpool )
{

	int named_pipe;
	char * new_schedul = ( char *) malloc (sizeof(scheduling) * sizeof(char));
	char * new_allowed = ( char *) malloc (sizeof(allowed) * sizeof(char));

	configuracoes = ( config_ptr  ) malloc (sizeof(config_node));
	
	/* Criar a comunicaçao com o processo principal */ 
	/* Escrever no pipe */
	/* Ter em atenção as threads em execução */
	/* Criar um semafore q abre qd as execuções terminam */


	strcpy(new_schedul,scheduling);
	new_schedul[strlen(scheduling) - 1] = 0;
	strcpy(new_allowed,allowed);
	new_allowed[strlen(new_allowed) - 1] = 0;


	if ( strcmp(new_schedul,"defaul") < 0 || strcmp(new_schedul,"default") > 0)
	{
		strcpy(configuracoes->scheduling,new_schedul);
		printf("%s\n",configuracoes->scheduling);
	}

	if ( strcmp(new_allowed,"default") < 0 || strcmp(new_allowed,"defaul") > 0)
	{
		strcpy(configuracoes->allowed, new_allowed);
		printf("%s\n", configuracoes->allowed);
	}

	if ( configuracoes->max_threads != threadpool )
	{
		/*TODO: mutex */
	}

	/* Open named_pipe for comunications */

	if ( (named_pipe = open("configs_pipe",O_WRONLY)) != -1 )
	{
		printf("Named Pipe opened for comunications.\n");
	}
	else
	{
		printf("Error opening named pipe.\n");
	}

	if ( write(named_pipe,configuracoes,sizeof(config_node)) != -1 )
	{
		printf("Writed to pipe..\n");
	}
	else
	{
		printf("Error writting pipe--\n");
	}
}