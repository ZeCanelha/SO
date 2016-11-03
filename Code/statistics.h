#ifndef STATISTICS_H
#define STATISTICS_H

#define LOGFILE "server.log"

// Statistics will be on shared memory

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

// Pointer to struct

stats_ptr statistics;

int shmid;

void stats();
void update_stats();
void write_screen();
void reset_info();
#endif 