#include <bits/stdc++.h>
#include "libmymem.hpp"

using namespace std;

int main(){

	char *x;

	x = (char *)mymalloc(sizeof(char));

	*x = 'A';

	// printf("%s\n," );

	cout << *x << endl;

	return 0;
}