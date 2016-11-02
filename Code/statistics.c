#include <stdio.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <ctype.h>
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

	size_t sz;
	// Open file 
	fp = fopen(FILENAME,"a");
	if ( fp == NULL )
	{
		// Create file if not created
		fp = fopen(FILENAME,"a+");
	}

	// Write data to file
	// TODO: Update while running
	fprintf(fp,"Tipo de pedido: %d, %s,Hora Recepcao: %s,Hora de termino: %s", stats_ptr->request_type, stats_ptr->html_file,stats_ptr->request_time,stats_ptr->request_end_time);
	
	//TODO: mmap ??;
	fseek(fp, 0L, SEEK_END);
	sz = ftell(fp);

	void *addr = mmap(0, sz, PROT_READ | PROT_WRITE, MAP_SHARED, fp, 0);

	fp(rewind);
	fclose(fp);
	// Unmap when closing;			

}

void write_screen()
{
	
}