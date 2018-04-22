#include <bits/stdc++.h>
#include "libmymem.hpp"

using namespace std;

typedef struct longdata{

	long int array[1000];

} bigData;

int main(){

	cout << sizeof(bigData) << endl;

	for ( int i = 0 ; i < 1000 ; i++){

		bigData *x;

		x = (bigData *) mymalloc (sizeof(bigData));
		
		x->array[0] = i + 25;
		
		cout << x->array[0] << endl;
	}


	// void *y;
	// y = mymalloc(1024);

	// printf("%s\n," );

	// printf("%d\n",sizeof(int));

	return 0;
}