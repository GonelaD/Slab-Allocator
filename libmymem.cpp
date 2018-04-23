#include <bits/stdc++.h>
#include <sys/mman.h>
#include <mutex>
#include "libmymem.hpp"

typedef struct bucket{


	unsigned int bucket_size;

	//Per bucket lock
	std::mutex bucket_mutex;

	//Pointer to first slab
	struct slab_header * slab = NULL;

}bucket_struct;


typedef struct slab_header{

	//total number of objects in the slab
	unsigned int totobj;

	// number of unoccupied objects in the slab
	unsigned int freeobj;

	//For the smallest bucket, the max number of ints required to store the bitmap is 171
	unsigned int bitmap[171]; // In the binary representation - 1 mean occupied and 0 is unoccupied

	//pointer to the bucket
	bucket_struct * bucket = NULL;

	//to traverse forward in the list
	slab_header * nxt_slab = NULL;

	//to traverse backward in the list
	slab_header * prev_slab = NULL;

	
}slab_header;

//array of struct bucket
bucket_struct bucket_array[12];

void *mymalloc(const unsigned size){

	//pointer to the address of an unoccupied object
	void *address;

	address = NULL;

	for (int i = 0 ; i < 12 ; i++ ){//to search for a suitable bucket, having size greater than or equal to required size.

		if ( (unsigned int) (1 << (i + 2)) >= size) {//after finding the required bucket

			//flag to check if a slab has empty object 
			bool flag = true;

			slab_header *q;
			slab_header p;

			//Critical Section			
			bucket_array[i].bucket_mutex.lock();
			

			q = bucket_array[i].slab;
			slab_header * prev = NULL;
			
			for( ;q != NULL && flag ; prev = q , q=q->nxt_slab){//to check if there is a free object in already existing slabs

				for (unsigned int j = 0 ; j <= (q->totobj/32) + 1 && flag && q->freeobj > 0; ){								
					//slab is full
					if( q->bitmap[j] == 4294967295) j++; //(2 power 32) - 1			
					
					else{//to find the address of free object in the slab

						for(int k = 0; k < 32; k++){

 							//to get kth bit in 32-bits integer

							int check = (q->bitmap[j] & (1 << (31 - k))) >> (31 - k);  							

							if (check == 0){//object is empty

								//Mark object as occupied						
							    q->bitmap[j] += 1 << (31 - k);  

							    //decrease the number of free objects in the slab
							    q->freeobj--;

							    //pointer thats points to the pointer of the object, which in turn points to the slab header
								slab_header **obj_slab_pointer;


								obj_slab_pointer = (slab_header **)((char *)q + sizeof(slab_header) + (j*32 + k)*(sizeof(slab_header *) + bucket_array[i].bucket_size));

								*(obj_slab_pointer) = q;

								//address of the free object

								address = (void *)((char *)q + sizeof(slab_header) + (j*32 + k)*(sizeof(slab_header *) + bucket_array[i].bucket_size) + sizeof(slab_header *));
								
								//End critical section. 

								bucket_array[i].bucket_mutex.unlock();
								
								return address; 
							}

						}

					}
				}

			}

			if(flag){//if there is no free object in any of the existing slabs

				void *y;  

				//Bring 64KB of memory into the virtual address space
				y = mmap(NULL, 64*1024, PROT_WRITE | PROT_READ, MAP_PRIVATE|MAP_ANONYMOUS, -1 , 0 );
				
				if (y == MAP_FAILED) {
					return y;
				}

				//creation of slab
				slab_header *p;

				//pointer to point to 64KB memory
				p = (slab_header *)y;

				bucket_array[i].bucket_size = (1 << (i + 2));
								
				p->totobj = (64*1024 - sizeof(slab_header))/(bucket_array[i].bucket_size + sizeof(slab_header *));
				p->freeobj = p->totobj - 1;
				p->bucket = &bucket_array[i];
				p->prev_slab = prev;

				//initially all objects are unoccupied, so bitmap is set to 0 for all objects
				for ( int m = 0 ; m < 171 ; m++) p->bitmap[m] = 0;
				
				//The first bit is set to 1
				p->bitmap[0] = 1 << 31;

				p->nxt_slab = NULL;

				//Pointer back to the slab header
				slab_header **obj_slab_pointer;

				obj_slab_pointer = (slab_header **)((char *)y + sizeof(slab_header));

				//Store the slab header 
				*(obj_slab_pointer) = p;

				//Address of the first object of the newly created slab
				address = (void *)((char *)y + sizeof(slab_header) + sizeof(slab_header *)); 
				
			 	//Add new slab to the list	
				if (prev == NULL) bucket_array[i].slab = p;
				else prev->nxt_slab = p;
				
				//End critical section. 
				bucket_array[i].bucket_mutex.unlock();
				return address;
			}	
			

			break;
		}

	}

	//Return NULL if size exceeds the largest bucket size
	return address;
}

void myfree(const void *ptr){

	//to find the corresponding slab header of this object
	slab_header **x;

	x = (slab_header **)( (char *)ptr - sizeof(slab_header*));


	slab_header *q;

	if(*x == NULL){
		fprintf(stderr, "Invalid address");
		return;
	}

	bucket_struct *bucket = (*x)->bucket;
	
	//Critical Section
	bucket->bucket_mutex.lock();

	q = *x;

	//after freeing the object 
	q->freeobj++;
	
	unsigned int free_block = q->freeobj;
	unsigned int bucket_size = q->bucket->bucket_size;

	if (free_block == q->totobj) { //if all objects are empty then delete the slab

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

	}
	else {

		//Modify bitmap to reflect that this index is free
		long int index = ((long int)x - (long int)q - sizeof(slab_header))/(sizeof(slab_header *) + bucket_size );

		int j = index/32; 
		int i = index%32;

		if ((q->bitmap[j] & (1 << (31 - i))) >> (31 - i) == 1){

			q->bitmap[j] -= 1 << (31 - i);

		}
		else {//if the memory not allocated

			fprintf(stderr,"Can't de allocate Memory that hasn't been allocated\n");

			//To negate the initial increment
			q->freeobj--;
		}

	}

	bucket->bucket_mutex.unlock();
	//End critical section

}