#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#include "ServerHttp.h"


/* 
 * -- simplehttpd.c --
 * A (very) simple HTTP server
 *
 * Sistemas Operativos 2016/2017
 * 
 * Configs into shared mem ?
 */



int main(int argc, char ** argv)
{
	 
	/* Creating process' and running initial configuration */
	init();
	// main thread
	http_main_listener();
}



void init()
{

	clean = (clean_ptr) malloc(sizeof(clean_no));

	clean->shm = 0;
	clean->thread = 0;
	clean->socket = 0;

	/* Reading default configs */ 

	if (read_configs())
	{
		printf("Current configurations: \n");
		printf("SERVERPORT: %d\n", configuracoes->server_port);
		printf("THREADPOOL: %d\n", configuracoes->max_threads);
		printf("SCHEDULING: %s\n", configuracoes->scheduling);
		printf("ALLOWED: %s\n", configuracoes->allowed);
	}
	else exit(0);

	/* Creating child process' */

	/*
	if ( fork() == 0 )
	{
		stats();
	}
	
	else if ( statistics_pid == -1)
	{
		printf("Erro: Could not create Statistics Process!\n");
		clean_up();
	}

	/* Creating shared memory */

	if (create_shared_memory())
	{
		printf("Shared memory created!\n");
	}
	else clean_up();
	clean->shm = 1;

	if ((mkfifo(NAMED_PIPE, O_CREAT|O_EXCL|0600)<0) && (errno!= EEXIST))
  	{
    	perror("Cannot create pipe: ");
    	exit(0);
  	}
  	// TODO: Remove pipe 
  	printf("Piped created for comunications.\n");
	

    /* Creating threadpool */
    
    pthread_t threads [configuracoes->max_threads];
    for ( int i = 0; i < configuracoes->max_threads; i++ )
    {
    	if (pthread_create(&threads[i], NULL, process_request, NULL)) 
    	{
			perror("Error creating thread pool");
			clean_up();
		}
    }
 	
	

	// Create configuration process to comunicate
	

	/*
	config_pid = fork();
	if ( config_pid == 0)
	{
		printf("Config PID: %ld\n", (long) config_pid);
	}
	else if ( config_pid == -1 )
	{
		printf("Erro: Could not create Config process!\n");
		clean_up();
	}*/
	//return;
}

void http_main_listener()
{	
	struct sockaddr_in client_name;
	socklen_t client_name_len = sizeof(client_name);

	signal(SIGINT,clean_up);

	printf("Listening for HTTP requests on server_port %d\n",configuracoes->server_port);

	// Configure listening server_port
	if ((socket_conn=fireup(configuracoes->server_port))==-1)
		exit(1);
	clean ->socket = 1;

	// Serve requests 
	while (1)
	{
		// Abrir o pipe para comunicações

		// Accept connection on socket
		if ( (new_conn = accept(socket_conn,(struct sockaddr *)&client_name,&client_name_len)) == -1 ) {
			printf("Error accepting connection\n");
			exit(1);
		}

		// Identify new client
		identify(new_conn);

		// Process request
		if (get_request(new_conn) == -1)
		{
			close(new_conn);
			continue;
		}

		// Verify if request is for a page or script
		if(!strncmp(req_buf,CGI_EXPR,strlen(CGI_EXPR)))
			execute_script(new_conn);	
		else
			// Search file with html page and send to client
			send_page(new_conn);
	
		// Terminate connection with client 
		close(new_conn);

	}

}

// Processes request from client
int get_request(int socket)
{
	int i,j;
	int found_get;

	found_get=0;
	while ( read_line(socket,SIZE_BUF) > 0 ) {
		if(!strncmp(buf,GET_EXPR,strlen(GET_EXPR))) {
			// GET received, extract the requested page/script
			found_get=1;
			i=strlen(GET_EXPR);
			j=0;
			while( (buf[i]!=' ') && (buf[i]!='\0') )
				req_buf[j++]=buf[i++];
			req_buf[j]='\0';
		}
	}	

	// Currently only suports GET 
	if(!found_get) {
		printf("Request from client without a GET\n");
		exit(1);
	}
	// If no particular page is requested then we consider htdocs/index.html
	if(!strlen(req_buf))
		sprintf(req_buf,"index.html");

	if(!strcmp(req_buf, "favicon.ico")){
		printf("FAVICON REQUEST.\n");
		return -1;
	}

	#if DEBUG
	printf("get_request: client requested the following page: %s\n",req_buf);
	#endif

	return 1;
}


// Send message header (before html page) to client
void send_header(int socket)
{
	#if DEBUG
	printf("send_header: sending HTTP header to client\n");
	#endif
	sprintf(buf,HEADER_1);
	send(socket,buf,strlen(HEADER_1),0);
	sprintf(buf,SERVER_STRING);
	send(socket,buf,strlen(SERVER_STRING),0);
	sprintf(buf,HEADER_2);
	send(socket,buf,strlen(HEADER_2),0);

	return;
}


// Execute script in /cgi-bin
void execute_script(int socket)
{
	// Currently unsupserver_ported, return error code to client
	cannot_execute(socket);
	
	return;
}


// Send html page to client
void send_page(int socket)
{
	FILE * fp;

	// Searchs for page in directory htdocs
	sprintf(buf_tmp,"htdocs/%s",req_buf);

	#if DEBUG
	printf("send_page: searching for %s\n",buf_tmp);
	#endif

	// Verifies if file exists
	if((fp=fopen(buf_tmp,"rt"))==NULL) {
		// Page not found, send error to client
		printf("send_page: page %s not found, alerting client\n",buf_tmp);
		not_found(socket);
	}
	else {
		// Page found, send to client 
	
		// First send HTTP header back to client
		send_header(socket);

		printf("send_page: sending page %s to client\n",buf_tmp);
		while(fgets(buf_tmp,SIZE_BUF,fp))
			send(socket,buf_tmp,strlen(buf_tmp),0);
		
		// Close file
		fclose(fp);
	}

	return; 

}


// Identifies client (address and server_port) from socket
void identify(int socket)
{
	char ipstr[INET6_ADDRSTRLEN];
	socklen_t len;
	struct sockaddr_in *s;
	//int server_port;
	struct sockaddr_storage addr;

	len = sizeof addr;
	getpeername(socket, (struct sockaddr*)&addr, &len);

	// Assuming only IPv4
	s = (struct sockaddr_in *)&addr;
	configuracoes->server_port = ntohs(s->sin_port);
	inet_ntop(AF_INET, &s->sin_addr, ipstr, sizeof ipstr);

	printf("identify: received new request from %s server_port %d\n",ipstr,configuracoes->server_port);

	return;
}


// Reads a line (of at most 'n' bytes) from socket
int read_line(int socket,int n) 
{ 
	int n_read;
	int not_eol; 
	int ret;
	char new_char;

	n_read=0;
	not_eol=1;

	while (n_read<n && not_eol) {
		ret = read(socket,&new_char,sizeof(char));
		if (ret == -1) {
			printf("Error reading from socket (read_line)");
			return -1;
		}
		else if (ret == 0) {
			return 0;
		}
		else if (new_char=='\r') {
			not_eol = 0;
			// consumes next byte on buffer (LF)
			read(socket,&new_char,sizeof(char));
			continue;
		}		
		else {
			buf[n_read]=new_char;
			n_read++;
		}
	}

	buf[n_read]='\0';
	#if DEBUG
	printf("read_line: new line read from client socket: %s\n",buf);
	#endif
	
	return n_read;
}


// Creates, prepares and returns new socket
int fireup(int port)
{
	int new_sock;
	struct sockaddr_in name;

	// Creates socket
	if ((new_sock = socket(PF_INET, SOCK_STREAM, 0))==-1) {
		printf("Error creating socket\n");
		return -1;
	}

	// Binds new socket to listening port 
 	name.sin_family = AF_INET;
 	name.sin_port = htons(configuracoes->server_port);
 	name.sin_addr.s_addr = htonl(INADDR_ANY);
	if (bind(new_sock, (struct sockaddr *)&name, sizeof(name)) < 0) {
		printf("Error binding to socket\n");
		return -1;
	}

	// Starts listening on socket
 	if (listen(new_sock, 5) < 0) {
		printf("Error listening to socket\n");
		return -1;
	}
 
	return(new_sock);
}



// Sends a 404 not found status message to client (page not found)
void not_found(int socket)
{
 	sprintf(buf,"HTTP/1.0 404 NOT FOUND\r\n");
	send(socket,buf, strlen(buf), 0);
	sprintf(buf,SERVER_STRING);
	send(socket,buf, strlen(buf), 0);
	sprintf(buf,"Content-Type: text/html\r\n");
	send(socket,buf, strlen(buf), 0);
	sprintf(buf,"\r\n");
	send(socket,buf, strlen(buf), 0);
	sprintf(buf,"<HTML><TITLE>Not Found</TITLE>\r\n");
	send(socket,buf, strlen(buf), 0);
	sprintf(buf,"<BODY><P>Resource unavailable or nonexistent.\r\n");
	send(socket,buf, strlen(buf), 0);
	sprintf(buf,"</BODY></HTML>\r\n");
	send(socket,buf, strlen(buf), 0);

	return;
}


// Send a 5000 internal server error (script not configured for execution)
void cannot_execute(int socket)
{
	sprintf(buf,"HTTP/1.0 500 Internal Server Error\r\n");
	send(socket,buf, strlen(buf), 0);
	sprintf(buf,"Content-type: text/html\r\n");
	send(socket,buf, strlen(buf), 0);
	sprintf(buf,"\r\n");
	send(socket,buf, strlen(buf), 0);
	sprintf(buf,"<P>Error prohibited CGI execution.\r\n");
	send(socket,buf, strlen(buf), 0);

	return;
}


// Closes socket before closing
void clean_up(int sig)
{
	printf("Server terminating..\n");
	printf("Removing shared memory object.\n");

	if ( clean->shm == 1 )
	{
		if ( shmctl(shmid,IPC_RMID, NULL) == -1)
		{
			perror("Error on removing shm.\n");
		}
		else
		{
			printf("Shared memory freed.\n");
		}
	}
	printf("Closing socket..\n");
	
	printf("Socket closed...");
	printf("Statistics process terminated.");
	kill(statistics_pid,SIGKILL);
	
	if ( clean->socket == 1)
	{
		close(socket_conn);
	}

	exit(0);

}
