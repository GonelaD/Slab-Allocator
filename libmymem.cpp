#include <bits/stdc++.h>
#include <sys/mman.h>
#include "libmymem.hpp"

typedef struct bucket{

	//Per bucket lock

	unsigned int bucket_size;
	//Pointer to slab
	struct slab_header * slab;

}bucket_struct;


typedef struct slab_header{

	unsigned int totobj;
	unsigned int freeobj;
	void *slab_start;
	//bitmap
	bucket_struct * bucket;
	slab_header * nxt_slab;
	
}slab_header;



void *mymalloc(const unsigned size){


	bucket_struct bucket_array[12];

	void *x;

	for ( int i = 0 ; i < 12 ; i++) {

		bucket_array[i].bucket_size = 1 << (i + 2);


		x = mmap(NULL, 64*1024, PROT_WRITE | PROT_READ, MAP_PRIVATE|MAP_ANONYMOUS, -1 , 0 );
		
		slab_header *q;
		q = (slab_header *)x;

		bucket_array[i].slab = q;
		
		q->totobj = (64*1024 - sizeof(slab_header))/(bucket_array[i].bucket_size + sizeof(slab_header *));
		q->freeobj = q->totobj;
		q->bucket = &bucket_array[i];
		q->nxt_slab = NULL;


		x = (void *)((char *)x + sizeof(slab_header));
		q->slab_start = x;

		// return y;

	}

	int bucket_obj;

	for (int i = 0 ; i < 12 ; i++ ){

		if (bucket_array[i].bucket_size > size) {
			bucket_obj = i;
			break;
		}

	}

	printf("%d\n", bucket_obj);

	return bucket_array[bucket_obj].slab->slab_start;


	// return x;
}