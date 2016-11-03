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

void create_pool_threads(int num_threads)
{
    int i, thrd;
    pthread_t threads [num_threads];
    
    for(i =0 , i<num_threads;i++)
    {
        printf("Thread %ld\n", t);
        thrd = pthread_create(&threads[i], NULL, DoSomeStuff, (void *)t);
        if (rc){
            printf("ERROR; return code from pthread_create() is %d\n", rc);
            exit(-1);
        }
    }
    printf("PoolThread Created \n");
}
