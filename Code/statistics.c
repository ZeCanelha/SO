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

// TODO: Handling with the siguser;


void stats()
{
	signal(SIGUSR1,write_screen);
	signal(SIGUSR2,reset_info);
	sleep(1);

	/* Print proccess pid */

	statistics_pid = getpid();
	printf("Statiscs PID: %ld\n", (long)statistics_pid);

	/* File descriptor and pointer to mmap */

	int fd;
	void *pmap;

	

	/* Open/Create log file */

	fd = open(LOGFILE , O_CREAT | O_RDWR );
	if ( fd == -1 )
	{
		perror("Error creating the file.");
	}

	/* Obtain file size */

	if ( fstat(fd,display_stats) < 0)
	{
		perror("fstat");
		close(fd);
		exit(0);
	}

	/* Map the file in memory */
	// Write data to file
	// TODO: Update while running
	// Mutex ( Critical )
	pmap = mmap(NULL, display_stats->st_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);


	sleep(360);

	// Todo: Write data to file while runnig;
	// unmap when finish;
}

void write_screen()
{
	printf("\n\n\t\t\ttGESTOR DE ESTATISTICAS\n\n");
	printf("\t - INFORMACAO DE ESTATICAS AGREGADS: \n");
	printf("\t\tNúmero total de pedidos estaticos: %d\nNumero total de pedidos comprimidos: %d\n",display_stats->static_total_requests,display_stats->cp_totalrequests);
	printf("\t\tTempo medio para servir um pedido estatico nao comprimido: %.2f\nTempo medio para servir um pedido estatico comprimido: %.2f\n",display_stats->static_request_medtime, display_stats->cp_request_medtime);

}
void reset_info()
{
	printf("\n\n\t\t\ttGESTOR DE ESTATISTICAS\n\n");
	printf("\t - INFORMACAO DE ESTATICAS AGREGADS: \n");
	printf("\t\t A fazer reset á informação estatisca agregada...\n");

	/* Mutex ? */
	display_stats->static_total_requests = 0;
	display_stats->cp_totalrequests = 0;
	display_stats->static_request_medtime = 0;
	display_stats->cp_request_medtime = 0;

	printf("\t\tReset concluído...\n\n");

}
