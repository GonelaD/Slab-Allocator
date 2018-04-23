#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>
#include <unistd.h>
#include "libmymem.hpp"

#define N 10

using namespace std;

void *threadmain(void *niterations);
int main() {
 
	int nthreads=1;
	int niterations=5;
	pthread_t arr[N];
	pthread_attr_t attr;
	// Create nthreads to do allocations/free concurrently
	for (long int t=0; t<nthreads; t++) {
	// create a list of object sizes for this iththread
		
		pthread_attr_init(&attr);
		int ret=pthread_create(&arr[t], NULL, &threadmain, (void*) niterations);
		if(ret!=0)
		{
			printf("pthread_create failed\n");
			break;
		}
	// Create a thread
	}

	for(long int t=0;t<nthreads;t++){
		pthread_join(arr[t], NULL);
	}
	// wait for all threads to join
}
// The thread iterates for niterations and allocates objects of various sizes.
void *threadmain(void *niterations)
{
	//pthread_mutex_t lock;

	int objsizelist[12]={4,8,16,32,64,128,256,512,1024,2048,4096,8192};
	long int n_iterations =(long int) niterations;
	srand(time(NULL));
	printf("itera:%ld\n",n_iterations);

	for (int i=0; i<n_iterations; i++) {
		


		
		int objsize = rand()%12;
		printf("%d th iteration %d\n",i,objsizelist [ objsize ]);

		int *x[5];

		for(int j=0 ; j < 5 ;j++){

			x[j] = (int *)mymalloc (objsizelist [ objsize ] ) ;
			*x[j] = j;
		}
		for(int j=0 ; j < 5 ; j++)
		{
			printf("%dth iteration : %d\n",i,*x[j]);
		}


		sleep(5); //sleep for random time

		for(int k=0; k<5; k++){

			myfree((void *)x[i]);
		}
		for(int j=0 ; j < 5 ; j++)
		{
			printf("%dth iteration : %d\n",i,*x[j]);
		}
	//randomly pickup a size from objsizelist;
	//invoke mymalloc() bail out if error;
	// write to the allocated memory
	// sleep for small random time (no sleep causes more contention)
	// call myfree()
	}
	pthread_exit(0);
}