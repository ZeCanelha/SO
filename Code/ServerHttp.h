#ifndef SERVERHTTP_H
#define SERVERHTTP_H

#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
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
#include <pthread.h>
#include <semaphore.h>
#include <sys/shm.h>


// Produce debug information
#define DEBUG	  	1
#define FILENAME "..//configs/configs.txt"
#define LOGFILE "server.log"
#define LINE_SIZE 50
#define MAX_BUFF 15

// Header of HTTP reply to client
#define	SERVER_STRING 	"Server: simpleserver/0.1.0\r\n"
#define HEADER_1	"HTTP/1.0 200 OK\r\n"
#define HEADER_2	"Content-Type: text/html\r\n\r\n"

#define GET_EXPR	"GET /"
#define CGI_EXPR	"cgi-bin/"
#define SIZE_BUF	1024


/* Operations for pipe comunication */

sem_t * pipe_controller1;
#define SEM_NAME "pipe_control"
#define NAMED_PIPE "configspipe"

/* Struct Info for cleanig */
typedef struct clean_p
{
	int shm;
	int mmap;
	int thread;
	int socket;
	int pipe;
}clean_no;
typedef clean_no * clean_ptr;

/* Configurations struct */
typedef struct
{
	int server_port;
	int max_threads;
	char scheduling[MAX_BUFF];
	char allowed[MAX_BUFF];
}config_node;

typedef struct
{
	int schedulling;
	int allowed;
	int max_threads;

}config;

/* Statistics structs */

typedef struct
{
	int static_total_requests;
	int cp_totalrequests;
	float static_request_medtime;
	float cp_request_medtime;

	int request_type;
	char * html_file;
	char * request_time;
	char * request_end_time;

}display_stat_node;


// Pointer to struct

display_stat_node  * display_stats;

/* Proccesses pid */
pid_t statistics_pid;
pid_t config_pid;
pid_t ppid;
pid_t pipe_pid;

/* Pointers */

clean_ptr clean;
config_node  * configuracoes;



// Threads

pthread_mutex_t mutex;


// Global Variables Declaration

char buf[SIZE_BUF];
char req_buf[SIZE_BUF];
char buf_tmp[SIZE_BUF];
int socket_conn,new_conn;
int named_pipe;
int shmid;


/* ServerHttp functions */

int read_configs();
void parse( char * );
void http_main_listener();
void init();
void pipe_listener();
int  fireup(int port);
void identify(int socket);
int get_request(int socket);
int  read_line(int socket, int n);
void send_header(int socket);
void send_page(int socket);
void execute_script(int socket);
void not_found(int socket);
void cannot_execute(int socket);

int create_shared_memory();
void * process_request();
void clean_up();

/* Statistics functions */

void stats();
void update_stats();
void write_screen();
void reset_info();


#endif
