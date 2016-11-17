#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <string.h>
#include "config.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <semaphore.h>
#include <unistd.h>
#include <errno.h>
#include <sys/wait.h>

FILE *fp;

sem_t * pipe_controller;

int main(int argc, char const *argv[])
{
	//sem_unlink("PIPE_C");

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

	/* unlink named_pipe */
	unlink(NAMED_PIPE);

}


void update_values(char * scheduling , char * allowed , int threadpool )
{

	int named_pipe;
	char * new_schedul = ( char *) malloc (MAX_BUFF * sizeof(char));
	char * new_allowed = ( char *) malloc (MAX_BUFF * sizeof(char));

	//_configuracoes = ( config_ptr  ) malloc (sizeof(config_node));
	
	/* Criar a comunicaçao com o processo principal */ 
	/* Escrever no pipe */
	/* Ter em atenção as threads em execução */
	/* Criar um semafore q abre qd as execuções terminam */

	configuracoes = malloc(sizeof(config_node));

	strcpy(new_schedul,scheduling);
	new_schedul[strlen(scheduling) - 1] = 0;
	strcpy(new_allowed,allowed);
	new_allowed[strlen(new_allowed) - 1] = 0;

	configuracoes->server_port = 7504;
	strcpy(configuracoes->allowed, new_allowed);
	strcpy(configuracoes->scheduling,new_schedul);
	configuracoes->max_threads = threadpool;


	/* Open named_pipe and semaphore for comunications */

	pipe_controller = sem_open("PIPE_C",0);

	if ( (named_pipe = open(NAMED_PIPE,O_WRONLY)) < 0 )
	{
		perror("Cannot open  pipe.");
	}
	else
	{
		printf("Pipe opened with success!\n");
	}

	if ( write(named_pipe,configuracoes,sizeof(*configuracoes)) < 0 )
	{
		perror("Cannot write to pipe");
	}
	else
	{
		printf("Written to pipe.\n");
		/* Abrir o semaforo para o processo principal poder ler do pipe */
		sem_post(pipe_controller);
	}
	
	if ( close(named_pipe) < 0 )
	{
		perror("Error closing pipe.");
	}

}