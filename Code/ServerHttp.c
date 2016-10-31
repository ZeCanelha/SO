#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ServerHttp.h"


int main(int argc, char const *argv[])
{
	
	int ret_val;

	ret_val = read_configs();
	if ( ret_val == 1 )
	{
		printf("Error reading config file...\nClosing task.");
		return 1;
	}

	printf("SERVER PORT: %d\n",server_port);


	return 0;
}