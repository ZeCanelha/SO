#include "ServerHttp.h"


int create_shared_memory()
{
	if((shmid = shmget(IPC_PRIVATE, sizeof(stat_node), IPC_CREAT | 0700)) == -1 )
	{
		perror("Error creating shared memory\n");
		return 0;
	}

	if((statistics = (stats_ptr) shmat(shmid, NULL, 0)) == ((stats_ptr) -1)) 
	{
		perror("Error attaching shared memory\n");
		return 0;
	}
	return 1;

}

