# Slab-Allocator

A simplified version of Linux Slab Allocator (in user space). 

The library exports 2 APIs :

~~~
void* mymalloc(unsigned int size);
void myfree(void *ptr);
~~~

Refer to Read_me.txt file for further information.
