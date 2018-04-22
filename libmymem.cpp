#include <bits/stdc++.h>
#include <sys/mman.h>
#include "libmymem.hpp"

//1 means occupied

typedef struct bucket{

	//Per bucket lock

	unsigned int bucket_size;
	//Pointer to slab
	struct slab_header * slab = NULL;

}bucket_struct;


typedef struct slab_header{

	unsigned int totobj;
	unsigned int freeobj;
	int bitmap[171];
	bucket_struct * bucket = NULL;
	slab_header * nxt_slab = NULL;
	
}slab_header;



void *mymalloc(const unsigned size){


	bucket_struct bucket_array[12];
// 
	for ( int i = 0 ; i < 12 ; i++) {

		bucket_array[i].bucket_size = 1 << (i + 2);

	}

	// int bucket_obj;
	void *address;

	for (int i = 0 ; i < 12 ; i++ ){

		if (bucket_array[i].bucket_size >= size) {
			
			// bucket_obj = i;
			bool flag = true;

			slab_header *q;

			q = bucket_array[i].slab;
			slab_header * prev = NULL;
			
			for( ;q != NULL && flag ; prev = q , q=q->nxt_slab){

				for (unsigned int j = 0 ; j <= (q->totobj/32) + 1 && flag ; ){
					
					if(q -> bitmap[j] == 4294967295) j++; //(2 power 32) - 1			
					
					else{

						for(int k = 0; k < 32; k++){
 
							int check = q->bitmap[j] & (1 << (31 - k)) >> (31 - k);  //k th bit of jth 
							if( check == 1){}
							else{

								q->bitmap[j*32+k] = 1; //free object

								q->freeobj--;

								slab_header **obj_slab_pointer;

								obj_slab_pointer = (slab_header **)((void *)q + sizeof(slab_header));

								*(obj_slab_pointer) = q;

								address = (void *)q + sizeof(slab_header) + (j*32 + k)*(sizeof(slab_header *) + bucket_array[i].bucket_size) + sizeof(slab_header *);
								flag = false;
								break;
							}
						}

					}
				}

			}

			if(flag){

				void *y;

				y = mmap(NULL, 64*1024, PROT_WRITE | PROT_READ, MAP_PRIVATE|MAP_ANONYMOUS, -1 , 0 );
				
				slab_header *p;
				p = (slab_header *)y;

				// bucket_array[i].slab = ;

				if (prev == NULL) bucket_array[i].slab = p;
				else prev->nxt_slab = p;
				
				p->totobj = (64*1024 - sizeof(slab_header))/(bucket_array[i].bucket_size + sizeof(slab_header *));
				p->freeobj = p->totobj - 1;
				p->bucket = &bucket_array[i];

				for ( int m = 0 ; m < 171 ; m++) p->bitmap[m] = 0;

				p->nxt_slab = NULL;

				slab_header **obj_slab_pointer;

				obj_slab_pointer = (slab_header **)((void *)p + sizeof(slab_header *));

				*(obj_slab_pointer) = p;

				address = y + sizeof(slab_header) + sizeof(slab_header); 
			}	
		}

	}

	return address;
	// return x;
}