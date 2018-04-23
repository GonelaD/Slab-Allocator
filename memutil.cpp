#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>
#include <unistd.h>
#include "libmymem.hpp"

#define N 10

using namespace std;

// The thread iterates for niterations and allocates objects of various sizes.
void *threadmain(void * niterations){
	//pthread_mutex_t lock;

	int objsizelist[12] = {4,8,16,32,64,128,256,512,1024,2048,4096,8192};
	long int n_iterations = (long int) niterations;
	// printf("itera:%ld\n",n_iterations);

	for (int i = 0; i < n_iterations; i++) {
		
		int objsize = rand()%12;
		// printf("%d th iteration %d\n", i , objsizelist[objsize]);

		int *x[5];

		for(int j = 0 ; j < 5 ;j++){

			x[j] = (int *)mymalloc (objsizelist[objsize]);
			// printf("%ld\n", x[j] );
			(*x[j]) = j;
		}

		sleep(1); //sleep for random time

		for(int k = 0; k < 5; k++){

			// printf("%ld\n", x[k]);
			myfree((void *)x[k]);
		}
		// for(int j = 0 ; j < 5 ; j++){
			
		// 	printf("%dth iteration : %d\n",i,*x[j]);
		// }
	
	}
	pthread_exit(0);
}

int main() {
 
	int nthreads = 2;
	int niterations = 1;
	pthread_t arr[N];
	pthread_attr_t attr;

	srand(time(NULL));
	// Create nthreads to do allocations/free concurrently
	for (long int t = 0; t < nthreads; t++) {
	// create a list of object sizes for this iththread
		
		pthread_attr_init(&attr);
		int ret = pthread_create(&arr[t], NULL, &threadmain, (void*) niterations);
		if(ret != 0){

			printf("pthread_create failed\n");
			break;
		}
	// Create a thread
	}

	for(long int t = 0 ; t < nthreads ; t++){

		pthread_join(arr[t], NULL);
	}
	// wait for all threads to join
}