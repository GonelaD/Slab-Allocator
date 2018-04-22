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
	unsigned int bitmap[171];
	bucket_struct * bucket = NULL;
	slab_header * nxt_slab = NULL;
	slab_header * prev_slab = NULL;

	
}slab_header;

bucket_struct bucket_array[12];

void *mymalloc(const unsigned size){


	for ( int i = 0 ; i < 12 ; i++) {

		bucket_array[i].bucket_size = 1 << (i + 2);

	}

	// int bucket_obj;
	void *address;

	for (int i = 0 ; i < 12 ; i++ ){

		if (bucket_array[i].bucket_size >= size) {
			
			// printf("bucket size %d\n", bucket_array[i].bucket_size );

			// bucket_obj = i;
			bool flag = true;

			slab_header *q;

			q = bucket_array[i].slab;
			slab_header * prev = NULL;
			
			for( ;q != NULL && flag ; prev = q , q=q->nxt_slab){

				for (unsigned int j = 0 ; j <= (q->totobj/32) + 1 && flag && q->freeobj > 0; ){
					
					if(q -> bitmap[j] == 4294967295) j++; //(2 power 32) - 1			
					
					else{

						for(int k = 0; k < 32; k++){
 
							int check = (q->bitmap[j] & (1 << (31 - k))) >> (31 - k);  //k th bit of jth 
							
							if (check == 0){
								// printf("abefore %u\n", (unsigned int)q->bitmap[j] );
								q->bitmap[j] += 1 << (31 - k);  //free object
								// printf("aafter %u\n", (unsigned int) q->bitmap[j] );

								// printf("bitmap %d\n", q->bitmap[j]);

								q->freeobj--;

								slab_header **obj_slab_pointer;

								obj_slab_pointer = (slab_header **)((char *)q + sizeof(slab_header) + (j*32 + k)*(sizeof(slab_header *) + bucket_array[i].bucket_size));

								*(obj_slab_pointer) = q;

								address = (void *)((char *)q + sizeof(slab_header) + (j*32 + k)*(sizeof(slab_header *) + bucket_array[i].bucket_size) + sizeof(slab_header *));
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
				
				if (y == MAP_FAILED) {
					return y;
				}

				slab_header *p;
				p = (slab_header *)y;

				// printf("LOL\n");

				// bucket_array[i].slab = ;

				if (prev == NULL) bucket_array[i].slab = p;
				else prev->nxt_slab = p;
				
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
			}	

			break;
		}

	}

	// printf("%ld\n", address );
	return address;
	// return x;
}

void myfree(const void *ptr){
// 
	slab_header **x;

	x = (slab_header **)( (char *)ptr - sizeof(slab_header*));

	slab_header *q;

	q = *x;

	// printf("%d\n", q->freeobj );
	
	q->freeobj++;

	unsigned int free_block = q->freeobj;
	unsigned int bucket_size = q->bucket->bucket_size;

	// printf("%d\n", q->freeobj );

	// printf("The bucket size : %ld\n", (*x)->bucket->bucket_size);

	if (free_block == q->totobj) {

		//Delete the entire slab
		slab_header *prev;
		prev = q->prev_slab;
		// printf("%d\n", q->totobj );
		// printf("LOL\n");

		if (prev == NULL && q->nxt_slab == NULL){

			q->bucket->slab = NULL;
		}
		else if(prev == NULL) {
			// q->bucket->slab->nxt_slab->prev_slab = NULL;
			q->nxt_slab->prev_slab = NULL;
			q->bucket->slab = q->nxt_slab;
		}
		else prev->nxt_slab = q->nxt_slab;

		int unmap = munmap((void *)q, 64*1024);

		if(unmap == -1) fprintf(stderr,"Memory De-allocation failed.\n");

		printf("Slab deleted\n");


	}
	else {

		//Modify bitmap to reflect that this index is free

		long int index = ((long int)x - (long int)q - sizeof(slab_header))/(sizeof(slab_header *) + bucket_size );

		int j = index/32;
		int i = index%32;

		// printf("%d %d \n", i, j);
		// printf("bbefore %u\n", q->bitmap[j] );

		if ((q->bitmap[j] & (1 << (31 - i))) >> (31 - i) == 1){

			printf("LOL\n");
			q->bitmap[j] -= 1 << (31 - i);
			printf("%d\n",(q->bitmap[j] & (1 << (31 - i))) >> (31 - i));
		}
		else {

			printf("Can't de allocate Memory that hasn't been allocatd\n");
		}

		// printf("bafter %u\n", q->bitmap[j] );

	}

}