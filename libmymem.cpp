#include <bits/stdc++.h>
#include <sys/mman.h>
#include <sys/stat.h>        
#include <fcntl.h>
#include "libmymem.hpp"

void *mymalloc(const unsigned size){

	void *x;

	x = mmap(NULL, size, PROT_WRITE | PROT_READ, MAP_PRIVATE|MAP_ANONYMOUS, -1 , 0 );

	return x;
	
}