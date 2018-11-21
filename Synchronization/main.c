#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#include <pthread.h>
#include <time.h>

#define MAXLENSIZE 256
#define No_of_crossings 4
#define EAST 0
#define NORTH 1
#define WEST 2
#define SOUTH 3
#define TRUE 1
#define FALSE 0

char dir_arr [No_of_crossings][MAXLENSIZE]={"EAST","NORTH","WEST","SOUTH"};

pthread_mutex_t check_mutex = PTHREAD_MUTEX_INITIALIZER;
/// four mutexes each for a crossing resource (N,W,E,S)
pthread_mutex_t resource_crossing[No_of_crossings]={PTHREAD_MUTEX_INITIALIZER,PTHREAD_MUTEX_INITIALIZER,PTHREAD_MUTEX_INITIALIZER,PTHREAD_MUTEX_INITIALIZER};
/// four condition variables for each direction to signal bats on the left
pthread_cond_t cond_next_signal[No_of_crossings]={PTHREAD_COND_INITIALIZER,PTHREAD_COND_INITIALIZER,PTHREAD_COND_INITIALIZER,PTHREAD_COND_INITIALIZER};
/// condition variables for one queue (same direction many bats)
pthread_cond_t direction_queue[No_of_crossings]={PTHREAD_COND_INITIALIZER,PTHREAD_COND_INITIALIZER,PTHREAD_COND_INITIALIZER,PTHREAD_COND_INITIALIZER};
///four mutexes for each direction queue
pthread_mutex_t queue_lock[No_of_crossings]={PTHREAD_MUTEX_INITIALIZER,PTHREAD_MUTEX_INITIALIZER,PTHREAD_MUTEX_INITIALIZER,PTHREAD_MUTEX_INITIALIZER};

int wait[No_of_crossings]={FALSE ,FALSE,FALSE,FALSE};

int direction_int(char ch)
{
	switch(ch)
	{
		case 'e':	return EAST;
		case 'n':	return NORTH;
		case 'w':	return WEST;
		case 's':	return SOUTH;
		default :	return 0;
	}
}

typedef struct data{
	int direction;
	int thread_number;
}data;

void *BAT(void *arg)
{
	data dt = *((data *)arg); free(arg);
	int direction = dt.direction;


	pthread_mutex_lock(&queue_lock[direction]);
	while(wait[direction])
	{
		pthread_cond_wait( &direction_queue[direction], &queue_lock[direction]);
	}
	wait[direction]=TRUE;
	pthread_mutex_unlock(&queue_lock[direction]);
	// The thread just started
	///arrive(dt);

	printf("Bat %d from %s arrived at crossing\n" , dt.thread_number , dir_arr[dt.direction]);
	check();

	/// >>>>>>>>>>>>>>>>>> GETING FIRST RESOURCE LOCK <<<<<<<<<<<<<<<<<<<<<<<<<< //

	pthread_mutex_lock(&resource_crossing[direction]);
	/// >>>>>>>>>>>>>>>>>>> BEFORE LEAVING GET ANOTHER LOCK <<<<<<<<<<<<<<<<<<<<//
	pthread_mutex_lock(&resource_crossing[(direction+1)%No_of_crossings]);

	pthread_mutex_lock(&queue_lock[direction]);

	wait[direction]=FALSE;
	pthread_cond_signal( &direction_queue[direction]);
	pthread_mutex_unlock(&queue_lock[direction]);

	///signalling queues threads to come in

	// TASK DELAY of 1s
	sleep(1);
	//cross (dt);
	// >>>>>>>>>>>>>>>>>>>>>>>>>> THE MID GROUND STATE <<<<<<<<<<<<<<<<<<<<<<<///

	pthread_mutex_unlock(&resource_crossing[direction]);
	pthread_mutex_unlock(&resource_crossing[(direction+1)%No_of_crossings]);


	printf("Bat %d from %s leaving crossing  \n" , dt.thread_number , dir_arr[dt.direction]);


}

int check()
{
	pthread_mutex_lock(&check_mutex);
	int t= (wait[0] && wait[1] && wait[2] && wait[3]);
	pthread_mutex_unlock(&check_mutex);
//	if (t)
//	{
//	"DEADLOCK DETECTED Signaling north to go \n"
//	} ;
	return t;
}

void *MAN(void *arg)
{
	int wait_flag = 1;
	while(1)
	{
		if(check() && wait_flag)
		{
			printf("DEADLOCK Jam DETECTED\n");
			wait_flag=0;
		}
		if(!check())
		{
			wait_flag=1;
		}

	}
}

void *BATMAN (void *arg)
{
    char* sequence = (char *)arg;   // sequence of the bats arrivals
    int seqlen =strlen(sequence); // lenght of the sequence of bats

    pthread_t managerthread ;

    pthread_create(&managerthread,NULL,MAN,NULL);

    // each thread in the directions will be created, thus we will create threads equal to the size of the sequence length
    pthread_t *bat_threads = (pthread_t *)malloc(sizeof(pthread_t) * seqlen);
    data *dt;

    for(int i=0; i < seqlen ; i++)
	{
		dt=(data *)malloc(sizeof(data));
		dt->direction = direction_int(sequence[i]);
		dt->thread_number=i+1;
		pthread_create(&bat_threads[i], NULL , BAT ,(void *)dt);
		//check();
	}



	/// wait for the threads to finish up the job
        for(int i=0; i < seqlen ; i++)
        pthread_join(bat_threads[i],NULL);


}

void init_shared_resource()
{
    pthread_mutex_init(&check_mutex,NULL);

    for(int i =0; i< No_of_crossings;i++)
    {
        pthread_mutex_init(&resource_crossing[i],NULL);
        pthread_mutex_init(&queue_lock[i],NULL);
        pthread_cond_init(&cond_next_signal[i],NULL);
        pthread_cond_init(&direction_queue[i],NULL);
    }

}


void delete_shared_resource()
{
	pthread_mutex_destroy(&check_mutex);
	for(int i=0 ; i < No_of_crossings ; i++)
	{
		pthread_mutex_destroy(&resource_crossing[i]);
		pthread_cond_destroy(&cond_next_signal[i]);
		pthread_cond_destroy(&direction_queue[i]);
		pthread_mutex_destroy(&queue_lock[i]);
	}
}

int main(int argc, char *argv[])
{
    char inputstring[MAXLENSIZE] ;
    char *str;
    init_shared_resource();
    pthread_t bat_threadID;
    if (argv <2)
    {
        printf("Enter the directions:\n");
        scanf("%s",inputstring);
        str = inputstring;

        pthread_create(&bat_threadID,NULL,BATMAN,str);
        pthread_join(bat_threadID,NULL);
    }
    else{

        pthread_create(&bat_threadID,NULL,BATMAN,argv[1]);
        pthread_join(bat_threadID,NULL);

        }

        delete_shared_resource();
        exit(0);




    return 0;
}
