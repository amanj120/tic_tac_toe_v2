#include <stdio.h>

int fib(int n){
	int a = 0, b= 1;
	for(int i = 0; i < n; i++){
		b = a+b;
		a = b-a;
	}
	return a;
}

int sum(short d[3][10]){
	int s = 0;
	for(int i = 0; i < 3; i++){
		for(int j = 0; j < 10; j++){
			s += d[i][j];
		}
	}
	return s;
}

void set(short d[3][10], short val){
	for(int i = 0; i < 3; i++){
		for(int j = 0; j < 10; j++){
			d[i][j] = val;;
		}
	}
	return;
}