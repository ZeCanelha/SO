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
	int scheduling;
	int allowed;
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
			printf("\nAlterar definições..\n");

			do
			{
				printf("Tipo de escalonamento:\n1.Static.\n2.Compressed\n3.FIFO\n4.Default\n");
				scanf("%d",&scheduling);

				if ( scheduling > 0 && scheduling < 5 )
					flag = 1;
			}while(flag == 0);
			flag = 0;
			do
			{
				printf("Permissoes:\n1.gz\n2.zip\n3.default): ");
				scanf("%d",&allowed);
				if ( allowed > 0 && allowed < 4 )
					flag = 1;
			}while(flag == 0);
			flag = 0;

			printf("\nNumero de threads(0 para manter): ");
			scanf("%d",&threadpool);

			update_values(scheduling,allowed,threadpool);
		}
	}

	/* unlink named_pipe */
	unlink(NAMED_PIPE);

}


void update_values(int  scheduling , int  allowed , int threadpool )
{

	int named_pipe;

	config new_configs;

	new_configs.schedulling = scheduling;
	new_configs.allowed = allowed;
	new_configs.max_threads = threadpool;
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

	if ( write(named_pipe,&new_configs,sizeof(config)) < 0 )
	{
		perror("Cannot write to pipe");
	}
	else
	{
		printf("Written to pipe.\n");
		/* Abrir o semaforo para o processo principal poder ler do pipe */
		//sem_post(pipe_controller);
	}
	/*
	if ( close(named_pipe) < 0 )
	{
		perror("Error closing pipe.");
	}*/

}