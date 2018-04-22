#include <bits/stdc++.h>
#include "libmymem.hpp"

using namespace std;

typedef struct longdata{

	long int array[1000];
	struct longdata *ptr=NULL;

} bigData;

int main(){
	
	bigData * x[1000];

	cout << sizeof(bigData) << endl;

	x[0] = (bigData *) mymalloc (sizeof(bigData));
	x[0]->array[0] = 0;

	for ( int i = 1 ; i < 100 ; i++){


		x[i] = (bigData *) mymalloc (sizeof(bigData));

		x[i]->array[i] = i;
		x[i]->ptr = x[i-1];
		
		// if (i == 123) cout << x[i]->array[0] << endl;

		// myfree((void *) x[i]);
	}

	cout << x[6]->array[0] << endl;

	for (int i = 5 ; i < 13 ; i++ ) {
		printf("%d\n", i );
		myfree((void *) x[i]);
		// printf("Freed %d\n",i);
	}

	for ( int i = 6 ; i < 10 ; i++){


		x[i] = (bigData *) mymalloc (sizeof(bigData));

		x[i]->array[i] = i;
		x[i]->ptr = x[i-1];
		
		// if (i == 123) cout << x[i]->array[0] << endl;

		// myfree((void *) x[i]);
	}

	// for (int i = 5 ; i < 13 ; i++ ) {
	// 	printf("%d\n", i );
	// 	myfree((void *) x[i]);
	// }

	// printf("%ld %ld\n", x[5], x[6]);


	// myfree((void *)x[5]);

	// int *y;
	// y = (int *) mymalloc(sizeof(int));

	// *y = 28;

	// int *x;
	// x = (int *) mymalloc(sizeof(int));

	// *x = 38;

	// myfree(y);
	// myfree(x);

	// printf("%d\n", *y);

	// printf("%d\n",sizeof(int));

	return 0;
}