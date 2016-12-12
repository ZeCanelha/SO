/* SISTEMAS OPERATIVOS 2016/2017
 *
 * JOSÉ CANELHA 2012169312 125H
 * RÚBEN TOMÁS 2012172347 100h
 *
*/

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

 pthread_mutex_t mutex_buffer = PTHREAD_MUTEX_INITIALIZER;
 pthread_mutex_t config_mutex = PTHREAD_MUTEX_INITIALIZER;
 pthread_cond_t buffer_cond = PTHREAD_COND_INITIALIZER;



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

  counter_static = 0;
  counter_script = 0;
  wait_thread = 0;
  old_thread = 0;
	running = 1;
	clean->shm = 0;
	clean->thread = 0;
	clean->socket = 0;
	clean->mmap = 0;
	clean->pipe = 0;
	clean->log_fd = 0;

	/* Reading default configs */

	if (read_configs())
	{
		print_configs();
	}
	else exit(0);

	/* Creating shared memory */

	if (create_shared_memory())
	{
		printf("Shared memory created!\n");
	}
	else clean_up();
	clean->shm = 1;


  /* Inicializar o valor do semaforo a 0 */
	sem_unlink("STATS_SEM");
	stats_semaphore = sem_open("STATS_SEM",O_CREAT|O_EXCL, 0777,0);

	/* Creating statistics process */

	if ( fork() == 0 )
	{
		stats();
		exit(0);
	}
	else if ( statistics_pid == -1)
	{
		printf("Erro: Could not create Statistics Process!\n");
		clean_up();
	}

	/* Creating listener for pipe */

	if ( fork() == 0 )
	{
		pipe_listener();
		exit(0);
	}

	/* Create request buffer */

    create_queue(&buffer);
		buffer_count = 0;


}

void http_main_listener()
{
	struct sockaddr_in client_name;
	socklen_t client_name_len = sizeof(client_name);

	signal(SIGINT,clean_up);
	//signal(SIGPIPE,catch_pipe);
	//signal(SIGTSTP,sig_stop);

  /* Create thread_pool */
  pthread_t scheduler_thread[configuracoes->max_threads];
  for ( int i = 0; i < configuracoes->max_threads; i++)
  {
    if ( pthread_create(&scheduler_thread[i],NULL,scheduler,NULL))
    {
      printf("Error creating threadpool.\n");
      clean_up();
    }
  }

  printf("Threadpoll created\n" );


	printf("Listening for HTTP requests on server_port %d\n",configuracoes->server_port);

	// Configure listening server_port
	if ((socket_conn=fireup(configuracoes->server_port))==-1)
		exit(1);
	clean ->socket = 1;

	// Serve requests
	while (1)
	{
    if ( wait_thread == 1 )
    {
      // W8 all threads to finisih
      for ( int k = 0; k < old_thread; k++ )
      {
        pthread_join(scheduler_thread[k],NULL);
      }
      for ( int j = 0; j < configuracoes->max_threads; j++ )
      {
        if ( pthread_create(&scheduler_thread[j],NULL,scheduler,NULL))
        {
          printf("Error creating threadpool.\n");
          clean_up();
        }
      }
      wait_thread = 0;
    }
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
		// Create struct to store the request
		new_request request;
		// Verify if request is for a page or script
		if(!strncmp(req_buf,CGI_EXPR,strlen(CGI_EXPR)))
		{}
		else if(strstr(req_buf,".gz"))
    {
			request.socket_id = new_conn;
			strcpy(request.html_file, req_buf);
			request.request_type = 2;
		}
		else
		{
			// Search file with html page and send to client
			request.socket_id = new_conn;
			strcpy(request.html_file, req_buf);
			request.request_type = 1;
		}

		pthread_mutex_lock(&config_mutex);
		pthread_mutex_lock(&mutex_buffer);

		/*
				* If server is full:
						* Send unavailable service to client
		*/
		if ( buffer_count == SERVER_CAPACITY )
		{
			server_unavailable(new_conn);
			close(new_conn);
			pthread_mutex_unlock(&mutex_buffer);
			continue;
		}
		enqueue(&buffer,request,configuracoes->scheduling);
		pthread_mutex_unlock(&config_mutex);

		buffer_count++;

		pthread_cond_signal(&buffer_cond);

		pthread_mutex_unlock(&mutex_buffer);
		// Terminate connection with client
		//close(new_conn);

	}
}

void * scheduler ( )
{

	int ret_val;
  char hora_rec[15];
  char hora_term[15];
	while(1)
	{
		/* Waits on a condition variable, in this case, buffer count */
		pthread_mutex_lock(&mutex_buffer);
		/*  If the condition is false:
					* buffer_cond*
					* Mutex is released
					* Waits until gets notified
		*/
		while(buffer_count == 0 )
		{
			pthread_cond_wait(&buffer_cond,&mutex_buffer);
		}

		/* Mutual Exclusion
				* Get request from buffer
				* Process request
				* Update buffer count
		*/
		new_request request;
		request = dequeue(&buffer);
		pthread_mutex_unlock(&mutex_buffer);
		//printf("<%d>\n", request.request_type);
		/* Verify:
				* Request Type:
		 			* 1 - Static
					* 2 - Compressed
				* Verify if file is allowed;
		*/

    printf("REQUEST TYPE: %d\n", request.request_type);
    ret_val = check_existent_file(request.html_file,request.request_type);
    printf("DEBUG: EXISTENT FILE: %d\n",ret_val);

    if ( ret_val == -1 )
    {
      not_found(request.socket_id);
      pthread_mutex_lock(&mutex_buffer);
  		buffer_count--;
  		pthread_mutex_unlock(&mutex_buffer);
      continue;

    }
		if ( request.request_type == 1 )
		{
      /* Reception time + Ending time */

      time_t now;
      time(&now);

      struct tm * ct = localtime(&now);
      display_stats->static_total_requests++;
      display_stats->request_type = 1;
      strcpy(display_stats->html_file,request.html_file);
      sprintf(hora_rec,"%d:%d:%d",ct->tm_hour,ct->tm_min,ct->tm_sec);
      strcpy(display_stats->request_time,hora_rec);

      clock_t begin = clock();

			send_page(request);

      clock_t end = clock();

      float time_spent = (float) (end - begin) / CLOCKS_PER_SEC;

      update_time(time_spent,1);


      sprintf(hora_term,"%d:%d:%d",ct->tm_hour,ct->tm_min,ct->tm_sec);
      strcpy(display_stats->request_end_time,hora_term);

      sem_post(stats_semaphore);


		}
		if ( request.request_type == 2 )
		{
      time_t now;
      time(&now);



      struct tm * ct = localtime(&now);
      display_stats->cp_totalrequests++;
      display_stats->request_type = 1;
      strcpy(display_stats->html_file,request.html_file);
      sprintf(hora_rec,"%d:%d:%d",ct->tm_hour,ct->tm_min,ct->tm_sec);
      strcpy(display_stats->request_time,hora_rec);



      clock_t begin = clock();

			execute_script(request);

      clock_t end = clock();

      float time_spent = (float) (end - begin) / CLOCKS_PER_SEC;

      update_time(time_spent,2);
      sprintf(hora_term,"%d:%d:%d",ct->tm_hour,ct->tm_min,ct->tm_sec);
      strcpy(display_stats->request_end_time,hora_term);



      sem_post(stats_semaphore);
		}

		pthread_mutex_lock(&mutex_buffer);
		buffer_count--;
		pthread_mutex_unlock(&mutex_buffer);
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
		return -1;
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
void execute_script(new_request new)
{
	int ret_val;
	char buff_temp[SIZE_BUF];
	FILE * fp = NULL;

	if ( (ret_val = decompress(new.html_file)) == -1 )
	{
		cannot_execute(new.socket_id);
		close(new.socket_id);
		return;
	}

	send_header(new.socket_id);
	printf("send_page: sending page output.hmtl to client\n");

  fp = fopen("./htdocs/output.html","rt");

	while(fgets(buff_temp,SIZE_BUF,fp))
		send(new.socket_id,buff_temp,strlen(buff_temp),0);
	close(new.socket_id);
	return;
}


// Send html page to client
void send_page(new_request new)
{
	FILE * fp = NULL;
	char buff_temp[SIZE_BUF];
	// Searchs for page in directory htdocs
	sprintf(buff_temp,"htdocs/%s",new.html_file);

	#if DEBUG
	printf("send_page: searching for %s\n",buf_tmp);
	#endif

	// Page found, send to client
	fp = fopen(buff_temp,"rt");
	// First send HTTP header back to client
	send_header(new.socket_id);
	printf("send_page: sending page %s to client\n",buff_temp);
	while(fgets(buff_temp,SIZE_BUF,fp))
		send(new.socket_id,buff_temp,strlen(buff_temp),0);

	// Close file
	fclose(fp);
	close(new.socket_id);
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

void server_unavailable(int socket)
{
	sprintf(buf,"HTTP/1.0 503 Service Unavailable\r\n");
	send(socket,buf, strlen(buf), 0);
	sprintf(buf,"Content-type: text/html\r\n");
	send(socket,buf, strlen(buf), 0);
	sprintf(buf,"\r\n");
	send(socket,buf, strlen(buf), 0);
	sprintf(buf,"<P>The server is currently unable to handle the request due to a temporary overloading or maintenance of the server.\r\n");
	send(socket,buf, strlen(buf), 0);

	return;
}

// Closes socket before closing
void clean_up(int sig)
{
	printf("Server terminating..\n");


	printf("Removing shared memory object.\n");
	if ( clean->log_fd == 1 )
	{
		close(log_fd);
	}
	running = 0;
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
  if ( munmap(pmap,FILE_SIZE) == -1 )
	{
		perror("Unmapping the memory");
	}
	else
	{
		printf("Memory unmapped.\n");
	}
	printf("Closing socket..\n");
	if ( clean->socket == 1)
	{
		close(socket_conn);
	}
	printf("Socket closed...");

  pthread_mutex_destroy(&mutex_buffer);
  pthread_mutex_destroy(&config_mutex);

	printf("Statistics process terminated.");

	kill(statistics_pid,SIGKILL);
	kill(pipe_pid,SIGKILL);

	exit(0);

}

void pipe_listener()
{
	/* Read configs from the named pipe */

	int num_bytes;
	pipe_pid = getpid();

	unlink(NAMED_PIPE);
																				//ugo
	if ((mkfifo(NAMED_PIPE, O_CREAT|O_EXCL|0600)<0) && (errno!= EEXIST))
  	{
    	perror("Cannot create pipe: ");
    	exit(0);
  	}
  	printf("Piped created for comunications.\n");

  	/* Start reading */
	if ( (named_pipe = open(NAMED_PIPE,O_RDWR)) < 0 )
	{
		printf("Error openig pipe for reading.\n");
	}

	config new_configs;

  	while(1)
  	{
			printf("Waiting for pipe to comunicate %d..\n", getpid());

			// sem_wait(pipe_controller1);
			if ( (num_bytes = read(named_pipe, &new_configs, sizeof(config))) < 0)
				printf("Error on reading from named pipe.");
			//printf("Received: <%lu> <%d>, Schedulling: %s,String Counter: %d, Max Threads: %d\n", sizeof(config), num_bytes, new_configs.schedulling, new_configs.string_counter , new_configs.max_threads);

			// When we accept a request, buffer needs to check what schedulling is running.
			// TO prevent memory conflits , use mutex

			pthread_mutex_lock(&config_mutex);

			if ( strcmp(new_configs.schedulling,"Default") == 0 || strcmp(new_configs.schedulling,configuracoes->scheduling) == 0)
			{
				printf("Schedul set as default: %s\n",configuracoes->scheduling);
			}
			else
			{
				strcpy(configuracoes->scheduling,new_configs.schedulling);
				printf("New schedul: %s\n",configuracoes->scheduling);
			}
			printf("Allowed files: \n");
			for ( int i = 0; i < new_configs.string_counter; i++ )
			{
        printf("DEBUG_FILES\n");
				strcpy(configuracoes->allowed[i],new_configs.allowed[i]);
				printf("- %s\n",configuracoes->allowed[i]);
			}

			// Update number o files allowed
			current_files_allowed_count = new_configs.string_counter;

			if ( new_configs.max_threads == configuracoes->max_threads || new_configs.max_threads == 0)
			{
				printf("Number of thread set as default: %d\n",configuracoes->max_threads);
			}
			else
			{
	       wait_thread = 1;
         old_thread = configuracoes->max_threads;
        /* Recreate thread poll */
        configuracoes->max_threads = new_configs.max_threads;
			}

			pthread_mutex_unlock(&config_mutex);

  	}
}
