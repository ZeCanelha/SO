#include "ServerHttp.h"


int create_shared_memory()
{
	if((shmid = shmget(IPC_PRIVATE, sizeof(display_stat_node), IPC_CREAT | 0700)) == -1 )
	{
		perror("Error creating shared memory\n");
		return 0;
	}

	if((display_stats = (display_stat_node *)shmat(shmid, NULL, 0)) == (display_stat_node *) -1)
	{
		perror("Error attaching shared memory\n");
		return 0;
	}
	return 1;

}

int read_configs()
{
	FILE *fp;

	configuracoes =(config_node *) malloc(sizeof(config_node));

	char line[LINE_SIZE];

	fp = fopen(FILENAME, "r");

	if ( fp == NULL )
	{
		fprintf(stderr, "Error: could not open config file\n");
		return 0;
	}

	while(fgets(line,LINE_SIZE,fp))
	{
		parse(line);

	}
	fclose(fp);
	return 1;
}

void parse( char * line)
{
	char *temp =(char*)malloc(LINE_SIZE * sizeof(char));
	char *line_temp = (char*)malloc(LINE_SIZE * sizeof(char));
	strcpy(line_temp,line);

	int i = 0;

	temp = strtok(line_temp, "=");

	if ( strcmp(temp,"SERVERPORT") == 0)
	{
		temp = strtok(NULL,"\n");
		configuracoes->server_port = (int)atoi(temp);
	}

	if ( strcmp(temp,"SCHEDULING") == 0)
	{
		temp = strtok(NULL,"\n");
		strcpy(configuracoes->scheduling,temp);
	}
	if ( strcmp(temp,"THREADPOOL") == 0)
	{
		temp = strtok(NULL,"\n");
		configuracoes->max_threads = (int)atoi(temp);
	}
	if ( strcmp(temp,"ALLOWED") == 0)
	{
		while ( ( temp = strtok(NULL,";")) != NULL )
		{
			strcpy(configuracoes->allowed[i],temp);
			i++;
		}
		configuracoes->allowed[i-1][strlen(configuracoes->allowed[i-1]) -1] = '\0';
		current_files_allowed_count = i;

	}
}


void to_upper_case(char * string )
{
    while( (*string = toupper(*string)))
			string++;
}

int decompress( char * file_name )
{
	int ret_val;
	char buffer[LINE_SIZE];

	 if ( sprintf(buffer,"gunzip -c ./compressed %s > output.html",file_name) == -1 )
	{
		printf("Error reading compressed file.\n");
	}
	else
	{
		ret_val = system(buffer);
		if ( (ret_val == -1 ) || (ret_val == 256) || (ret_val == 127) )
		{
			return -1;
		}
		else
		{
			printf("File decompressd with success\n");
		}
	}
	return 1;
}

void print_configs()
{
	printf("Current configurations: \n");
	printf("SERVERPORT: %d\n", configuracoes->server_port);
	printf("THREADPOOL: %d\n", configuracoes->max_threads);
	printf("SCHEDULING: %s\n", configuracoes->scheduling);
	printf("ALLOWED:");
	for ( int i = 0; i < current_files_allowed_count; i++)
	{
		printf(" %s ", configuracoes->allowed[i]);
	}
	printf("\n");
}

int is_permited( char * filename )
{
	int i;
	for ( i = 0; i < current_files_allowed_count; i++)
	{
		if ( strcmp(filename,configuracoes->allowed[i]) == 0 )
		{
			return 1;
		}
	}
	return -1;
}


int check_existent_file( char * filename, int type )
{
	FILE * fp;
	char temp[MAX_FILES_ALLOWED+MAX_BUFF];

	if ( type == 1 )
	{
		sprintf(temp,"htdocs/%s",filename);
		if ( (fp = fopen(temp,"rt")) == NULL )
		{
			fclose(fp);
			return -1;
		}
	}
	if ( type == 2 )
	{
		sprintf(temp,"compressed/%s", filename);

		if ( is_permited(filename) == -1 )
		{
			printf("File not allowed.\n");
			return -1;
		}
		if ( (fp = fopen(temp,"rt")) == NULL )
		{
			fclose(fp);
			return -1;
		}
	}
	else
	{
		return -1;
	}
	fclose(fp);
	return 1;
}

void * scheduler ( )
{

	int ret_val;
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

		if ( (ret_val = check_existent_file(request.html_file,request.request_type)) == -1 )
		{
			pthread_mutex_lock(&mutex_buffer);
			buffer_count--;
			pthread_mutex_unlock(&mutex_buffer);
			not_found(request.socket_id);
			//continue;
		}
		else
		{
			if ( request.request_type == 1 )
			{
				send_page(request);
			}
			else if ( request.request_type == 2)
			{
				execute_script(request);
			}
			pthread_mutex_lock(&mutex_buffer);
			buffer_count--;
			pthread_mutex_unlock(&mutex_buffer);

		}



	}

	return NULL;
}

void catch_pipe()
{
	printf("Error on reading/write file..\n");
	clean_up();
}

void sig_stop()
{
	printf("Control+Z pressed.\n");
	clean_up();
}
