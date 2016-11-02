#ifndef STATISTICS_H
#define STATISTICS_H

#define FILENAME "server.log"

// Statistics will be on shared memory

// File pointer
FILE *fp;


typedef struct stats
{
	int static_total_requests;
	int cp_totalrequests;
	float static_request_medtime;
	float cp_request_medtime;

	int request_type;
	char * html_file;
	char * request_time;
	char * request_end_time; 
} stat_node;
typedef stat_node * stats_ptr;

int shmid;

void stats();
void update_stats();
#endif 