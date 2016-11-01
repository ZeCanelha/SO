#ifndef SERVERHTTP_H
#define SERVERHTTP_H


#include "config.h"

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


// Produce debug information
#define DEBUG	  	1	

// Header of HTTP reply to client 
#define	SERVER_STRING 	"Server: simpleserver/0.1.0\r\n"
#define HEADER_1	"HTTP/1.0 200 OK\r\n"
#define HEADER_2	"Content-Type: text/html\r\n\r\n"

#define GET_EXPR	"GET /"
#define CGI_EXPR	"cgi-bin/"
#define SIZE_BUF	1024


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

typedef struct clean_p
{
	int shm;
	int thread;
	int socket;
}clean_no;
typedef clean_no * clean_ptr;

// pid

pid_t statistics_pid;
pid_t ppid;


// Pointers

stats_ptr statistics;
clean_ptr clean;

// Global Variables Declaration

int shmid;

char buf[SIZE_BUF];
char req_buf[SIZE_BUF];
char buf_tmp[SIZE_BUF];
int socket_conn,new_conn;



void http_main_listener();
void init();
int  fireup(int port);
void identify(int socket);
void get_request(int socket);
int  read_line(int socket, int n);
void send_header(int socket);
void send_page(int socket);
void execute_script(int socket);
void not_found(int socket);
void catch_ctrlc(int);
void cannot_execute(int socket);



int create_shared_memory();
void clean_up();


#endif