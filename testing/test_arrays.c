#include <stdlib.h>
#include <stdio.h>

void abss(int f[2][4]){
	printf("value of f: %p\n", f);
	printf("sizeof f: %ld\n", sizeof(f));
	for(int i = 0; i < 2; i++){
		for(int j = 0; j < 4; j++){
			if(f[i][j] < 0){
				f[i][j] = (-1) * f[i][j];
			}
		}
	}
}

int sum(int e[2][4]){
	printf("value of e: %p\n", e);
	printf("sizeof e: %ld\n\n", sizeof(e));
	abss(e);
	int sum = 0;
	for(int i = 0; i < 2; i++){
		for(int j = 0; j < 4; j++){
			sum += e[i][j];
		}
	}
	return sum;
}

int main(){
	int d[2][4];
	for(int i = 0; i < 2; i++){
		for(int j = 0; j < 4; j++){
			d[i][j] = -1*(i*4 + j + 1);
		}
	}

	for(int i = 0; i < 2; i++){
		for(int j = 0; j < 4; j++){
			printf("%d ", d[i][j]);
			//d[i][j] = i*4 + j + 1;
		}
		printf("\n");
	}
	printf("value of d: %p\n", d);
	printf("value of d[0]: %p\n", d[0]);
	printf("address of d[0][0]: %p\n", &d[0][0]);
	printf("value of d[0][0]: %d\n", d[0][0]);
	printf("value of d[1]: %p\n", d[1]);
	printf("sizeof d: %ld\n\n", sizeof(d));
	printf("sum=%d should be 36\n", sum(d));
	for(int i = 0; i < 2; i++){
		for(int j = 0; j < 4; j++){
			printf("%d ", d[i][j]);
			//d[i][j] = i*4 + j + 1;
		}
		printf("\n");
	}

	//printf("value of d %ld\n", d);
}