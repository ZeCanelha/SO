#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <ctype.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <sys/shm.h>
#include <sys/mman.h>
#include <pthread.h>
#include <semaphore.h>
#include <fcntl.h>
#include "ServerHttp.h"

#define FILE_MODE (S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH)
#define FILE_SIZE (1000 * sizeof(char)) // To be checked;

// TODO: Handling with the siguser;
display_stat_node * stats_shared;

void stats()
{

	sleep(1);

	signal(SIGUSR1,write_screen);
	signal(SIGUSR2,reset_info);

	/* Print proccess pid */

	statistics_pid = getpid();
	printf("Statiscs PID: %ld\n", (long)statistics_pid);

	if ( (stats_shared = (display_stat_node *)shmat(shmid,NULL,0)) == (display_stat_node *) -1)
	{
		perror("Attaching memory ( statistics.c )");
		exit(EXIT_FAILURE);
	}

	/* Open/Create log file */

	if ( (log_fd = open(LOGFILE, O_RDWR | O_CREAT | O_TRUNC , FILE_MODE )) != -1 )
	{
		clean->log_fd = 1;
	}
	else
	{
		printf("Error opennig log file.\nTerminating...\n");
		clean_up();
	}

	/* Obtain file size */

	size_t size;
	size = get_stat(log_fd);

	/* Stretch the file size to the size of the log file */

	if (lseek(log_fd, FILE_SIZE - 1, SEEK_SET) == -1)
	{
		fprintf(stderr,"lseek error\n");
		exit(1);
	}

	/* Something needs to be written to the file
		* in order to MMAP successfully
	*/
	int result;
	if ( (result = write(log_fd, "", 1)) == -1 )
	{
 		perror("Error writing last byte of the file");
 		clean_up();
	}
	else
	{
		printf("Write with success.\n");
	}

	/* Map the file in memory */

	if ( (pmap = mmap(NULL, FILE_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, log_fd, 0)) == MAP_FAILED)
	{
		printf("Error: Mapping file in memory.\n");
	}
	//sleep(360);

	while(running)
	{
		// TODO: Write information to file anytime a request is completed
		// TODO: Synchronized with a read/write semaphore

	}
	// Unmap memory file
	if ( munmap(pmap,FILE_SIZE) == -1 )
	{
		perror("Unmapping the memory");
	}
	else
	{
		printf("Memory unmapped.\n");
	}
}

void write_screen()
{
	printf("\n\n\t\t\ttGESTOR DE ESTATISTICAS\n\n");
	printf("\t - INFORMACAO DE ESTATICAS AGREGADS: \n");
	printf("\t\tNúmero total de pedidos estaticos: %d\n\t\tNumero total de pedidos comprimidos: %d\n",stats_shared->static_total_requests,stats_shared->cp_totalrequests);
	printf("\t\tTempo medio para servir um pedido estatico nao comprimido: %.2f\n\t\tTempo medio para servir um pedido estatico comprimido: %.2f\n",stats_shared->static_request_medtime, stats_shared->cp_request_medtime);

}
void reset_info()
{
	printf("\n\n\t\t\tGESTOR DE ESTATISTICAS\n\n");
	printf("\t - INFORMACAO DE ESTATICAS AGREGADS: \n");
	printf("\t\tA fazer reset á informação estatisca agregada...\n");

	/* Mutex ? */
	/* Reset on the shared memory */
	stats_shared->static_total_requests = 0;
	stats_shared->cp_totalrequests = 0;
	stats_shared->static_request_medtime = 0;
	stats_shared->cp_request_medtime = 0;

	printf("\t\tReset concluído...\n\n");
}

int get_stat(int fdin)
{
	struct stat pstatbuf;
	if (fstat(fdin, &pstatbuf) < 0)
	{
		fprintf(stderr,"fstat error\n");
		exit(1);
	}
	return pstatbuf.st_size;
}
