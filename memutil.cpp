#include <bits/stdc++.h>
#include "libmymem.hpp"

using namespace std;

int main(){

	int *x;

	x = (int *)mymalloc(sizeof(int));

	*x = 38;

	// void *y;
	// y = mymalloc(1024);

	// printf("%s\n," );

	cout << *x << endl;

	return 0;
}