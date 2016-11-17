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
#include "statistics.h"

// TODO: Handling with the siguser;


void stats()
{	
	signal(SIGUSR1,write_screen);
	signal(SIGUSR2,reset_info);
	sleep(1);
	statistics_pid = getpid();
	printf("Statiscs PID: %ld\n", (long)statistics_pid);
	// File descriptor; To be verified;
	int fd;	
	void *pmap;
	statistics = ( stats_ptr ) malloc (sizeof(stat_node));
	// Open file 
	fd = open(LOGFILE , O_CREAT | O_RDWR );

	if ( fd == -1 )
	{
		perror("Error creating the file.");
	}
	printf("Gestor de Estatisticas iniciado.\n");
	printf("Não faço nada...\n");
	
	/*
	if ( fstat(fd,statistics) < 0)
	{
		perror("fstat");
		close(fd);
		exit(0);
	} 
	// Write data to file
	// TODO: Update while running
	// Mutex ( Critical )
	pmap = mmap(0, statistics.st_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
	*/
	
	sleep(360);

	// Todo: Write data to file while runnig;
	// unmap when finish;
}

void write_screen(){}
void reset_info(){}