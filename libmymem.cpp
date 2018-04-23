#include <bits/stdc++.h>
#include <sys/mman.h>
#include <mutex>
#include "libmymem.hpp"

typedef struct bucket{

	unsigned int bucket_size;
	//Per bucket lock

	std::mutex bucket_mutex;

	//Pointer to slab
	struct slab_header * slab = NULL;

}bucket_struct;


typedef struct slab_header{

	unsigned int totobj;
	unsigned int freeobj;
	unsigned int bitmap[171]; // In the bitmap - 1 mean occupied and 0 is unoccupied
	bucket_struct * bucket = NULL;
	slab_header * nxt_slab = NULL;
	slab_header * prev_slab = NULL;

	
}slab_header;

bucket_struct bucket_array[12];

void *mymalloc(const unsigned size){

	void *address;

	address = NULL;

	for (int i = 0 ; i < 12 ; i++ ){

		if ( (unsigned int) (1 << (i + 2)) >= size) {

			bool flag = true;

			slab_header *q;
			slab_header p;

			//Critical Section
			// std::lock_guard <std::mutex> lock (bucket_array[i].bucket_mutex);

			bucket_array[i].bucket_mutex.lock();
			// printf("CS\n");

			q = bucket_array[i].slab;
			slab_header * prev = NULL;
			
			for( ;q != NULL && flag ; prev = q , q=q->nxt_slab){

				for (unsigned int j = 0 ; j <= (q->totobj/32) + 1 && flag && q->freeobj > 0; ){
					
					if( q->bitmap[j] == 4294967295) j++; //(2 power 32) - 1			
					
					else{

						for(int k = 0; k < 32; k++){
 
							int check = (q->bitmap[j] & (1 << (31 - k))) >> (31 - k);  //k th bit of jth 
							
							if (check == 0){

								//Mark object as occupied						
							    q->bitmap[j] += 1 << (31 - k);  

							    q->freeobj--;


								slab_header **obj_slab_pointer;

								obj_slab_pointer = (slab_header **)((char *)q + sizeof(slab_header) + (j*32 + k)*(sizeof(slab_header *) + bucket_array[i].bucket_size));

								*(obj_slab_pointer) = q;

								address = (void *)((char *)q + sizeof(slab_header) + (j*32 + k)*(sizeof(slab_header *) + bucket_array[i].bucket_size) + sizeof(slab_header *));
								
								//End critical section. 
								bucket_array[i].bucket_mutex.unlock();
								
								return address;
							}

						}

					}
				}

			}

			if(flag){

				void *y;

				y = mmap(NULL, 64*1024, PROT_WRITE | PROT_READ, MAP_PRIVATE|MAP_ANONYMOUS, -1 , 0 );
				
				if (y == MAP_FAILED) {
					return y;
				}

				// printf("First\n");

				slab_header *p;
				p = (slab_header *)y;

				bucket_array[i].bucket_size = (1 << (i + 2));
				
				
				p->totobj = (64*1024 - sizeof(slab_header))/(bucket_array[i].bucket_size + sizeof(slab_header *));
				p->freeobj = p->totobj - 1;
				p->bucket = &bucket_array[i];
				p->prev_slab = prev;


				for ( int m = 0 ; m < 171 ; m++) p->bitmap[m] = 0;
				
				p->bitmap[0] = 1 << 31;

				p->nxt_slab = NULL;

				slab_header **obj_slab_pointer;

				obj_slab_pointer = (slab_header **)((char *)y + sizeof(slab_header));

				*(obj_slab_pointer) = p;

				address = (void *)((char *)y + sizeof(slab_header) + sizeof(slab_header *)); 
				

				if (prev == NULL) bucket_array[i].slab = p;
				else prev->nxt_slab = p;
				
				//End critical section. 
				bucket_array[i].bucket_mutex.unlock();
				return address;
			}	
			

			break;
		}

	}

	// printf("Done\n");

	return address;
}

void myfree(const void *ptr){
// 
	slab_header **x;

	x = (slab_header **)( (char *)ptr - sizeof(slab_header*));

	slab_header *q;

	//Critical Section

	(*x)->bucket->bucket_mutex.lock();

	q = *x;

	// printf("%d\n", q->freeobj );
	

	unsigned int free_block = q->freeobj;
	unsigned int bucket_size = q->bucket->bucket_size;

	if (free_block == q->totobj) {

		//Delete the entire slab
		slab_header *prev;
		prev = q->prev_slab;

		if (prev == NULL && q->nxt_slab == NULL){

			q->bucket->slab = NULL;
		}
		else if(prev == NULL) {
			q->nxt_slab->prev_slab = NULL;
			q->bucket->slab = q->nxt_slab;
		}
		else prev->nxt_slab = q->nxt_slab;

		int unmap = munmap((void *)q, 64*1024);

		if(unmap == -1) fprintf(stderr,"Memory De-allocation failed.\n");

		// printf("Slab deleted\n");


	}
	else {

		//Modify bitmap to reflect that this index is free

		long int index = ((long int)x - (long int)q - sizeof(slab_header))/(sizeof(slab_header *) + bucket_size );

		int j = index/32;
		int i = index%32;

		if ((q->bitmap[j] & (1 << (31 - i))) >> (31 - i) == 1){

			q->bitmap[j] -= 1 << (31 - i);

		}
		else {

			fprintf(stderr,"Can't de allocate Memory that hasn't been allocated\n");
			q->freeobj--;
		}

	}

	q->freeobj++;

	(*x)->bucket->bucket_mutex.unlock();
	//End critical section

}