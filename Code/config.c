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

//sem_t * pipe_controller;
int named_pipe;


int main(int argc, char const *argv[])
{
	pipe_comunication();
	signal(SIGPIPE,catch_pipe);
}
void pipe_comunication()
{
	char c;
	char aux[FILES_ALLOWED_SIZE];
	int flag = 0;
	int sched;
	int thread;
	if ( (named_pipe = open(NAMED_PIPE,O_WRONLY)) < 0 )
	{
		perror("Cannot open  pipe.");
	}
	else
	{
		printf("Pipe opened with success!\n");
	}

	printf("Gestor de configurações.. \n");
	while(1)
	{
		int i = 0;
		fflush(stdin);
		printf("Press enter to continue\n");
		c = getchar();
		if ( c == '\n')
		{
			printf("\nAlterar definições..\n");

			do
			{
				printf("Tipo de escalonamento:\n1.Static.\n2.Compressed\n3.FIFO\n4.Default\n");
				scanf("%d",&sched);
				switch (sched) {
					case 1:
						strcpy(new_configs.schedulling,"Static");
						flag = 1;
						break;
					case 2:
						strcpy(new_configs.schedulling,"Compressed");
						flag = 1;
						break;
					case 3:
						strcpy(new_configs.schedulling,"FIFO");
						flag = 1;
						break;
					case 4:
						strcpy(new_configs.schedulling,"Default");
						flag = 1;
						break;
					default:
						printf("ValueError.\n");
						break;
				}
			}while(flag == 0);
			getchar();

			printf("\nNumero de threads(0 para manter): ");
			scanf("%d",&thread);
			new_configs.max_threads = thread;

			getchar();
			flag = 0;
			printf("(type EXIT to end.Files allowed: \n");
			while( i < MAX_FILES_ALLOWED)
			{
				printf("-> ");
				fgets(aux,FILES_ALLOWED_SIZE,stdin);
				if ( strcmp(aux,"EXIT\n") == 0)
					break;
				aux[strlen(aux)-1] = '\0';
				strcpy(new_configs.allowed[i],aux);
				i++;
			}
			new_configs.string_counter = i;


			update_values();
		}
	}

	if ( close(named_pipe) < 0 )
	{
		perror("Error closing pipe.");
	}

}
void update_values()
{
	if ( write(named_pipe, &new_configs, sizeof(config)) < 0 )
	{
		perror("Cannot write to pipe");
	}
	else
	{
		printf("Written to pipe.\n");
	}
}

void catch_pipe()
{
	printf("Error: Pipe error.\nTerminating..");
	//exit(0);
}
