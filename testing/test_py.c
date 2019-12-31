#include <stdio.h>

int fib(int n){
	int a = 0, b= 1;
	for(int i = 0; i < n; i++){
		b = a+b;
		a = b-a;
	}
	return a;
}