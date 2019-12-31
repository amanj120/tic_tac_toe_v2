#include "terminal_io.h"

void printData(short d[3][10]){
	char out[strlen(board)];
	//char out[] = "A0 A1 A2 | B0 B1 B2 | C0 C1 C2\nA3 A4 A5 | B3 B4 B5 | C3 C4 C5\nA6 A7 A8 | B6 B7 B8 | C6 C7 C8\n---------+----------+---------\nD0 D1 D2 | E0 E1 E2 | F0 F1 F2\nD3 D4 D5 | E3 E4 E5 | F3 F4 F5\nD6 D7 D8 | E6 E7 E8 | F6 F7 F8\n---------+----------+---------\nG0 G1 G2 | H0 H1 H2 | I0 I1 I2\nG3 G4 G5 | H3 H4 H5 | I3 I4 I5\nG6 G7 G8 | H6 H7 H8 | I6 I7 I8\n";
	strcpy(out, board);
	for(int i = 0; i < 9; i++){
		for(int j = 0; j < 9; j++){
			int p = i*9 + j;
			int pos = board_pos[p];
			if(cpu_is_X && cpu_player_number==0){
				if(d[0][i] & 1<<j){
					out[pos] = '>';
					out[pos+1] = '<';
				} else if (d[1][i] & 1<<j){
					out[pos] = '<';
					out[pos+1] = '>';
				} else if (!(d[2][i] & 1<<j)){
					out[pos] = ' ';
					out[pos+1] = ' ';
				} else {
					continue;
				}
			} else {
				if(d[0][i] & 1<<j){
					out[pos] = '<';
					out[pos+1] = '>';
				} else if (d[1][i] & 1<<j){
					out[pos] = '>';
					out[pos+1] = '<';
				} else if (!(d[2][i] & 1<<j)){
					out[pos] = ' ';
					out[pos+1] = ' ';
				} else {
					continue;
				}
			}
		}
	}
	for(int j = 0; j < 9; j++){
		int pos = board_pos[81 + j];
		if(cpu_is_X && cpu_player_number==0){
			if(d[0][9] & 1<<j){
				out[pos] = 'X';
			} else if (d[1][9] & 1<<j){
				out[pos] = 'O';
			} else {
				continue;
			}
		} else {
			if(d[0][9] & 1<<j){
				out[pos] = 'O';
			} else if (d[1][9] & 1<<j){
				out[pos] = 'X';
			} else {
				continue;
			}
		}
	}
	printf("%s\n", out);
}

int main(){
	seed();
	
	
	// OpenMP Test
	// #pragma omp parallel for
	// for(int i = 0; i < 16; i++){
	// 	printf("%d from thread %d\n", i, omp_get_thread_num());
	// }
	// #pragma omp parallel
 //    {
	//     printf("hello from thread %d\n", omp_get_thread_num());
	// }
	
	
	////printTimeForSingleRandomTrial(true);
	short data[3][10];
	for(int i = 0; i < 3; i++)
		for(int j = 0; j < 10; j++)
			data[i][j] = 0;
	set_metadata(data, 1, 1);
	//printf("%d\n", data[2][9]);
    register_move(data, 0, 40);
    printData(data);
    // register_move(data, 1, 37);
    // register_move(data, 0, 12);
    ////printf("sizeof(data): %ld\n", sizeof(data));
    // printData(data);
    // int n = nrand(data, 1, false, 10);
    // printf("\n%d\n", n);
    //randomTrial(data, true);
	//printf("value of d in main: %p\n", data);
    // long elapsed;
    // struct timeval t0, t1;
    // gettimeofday(&t0, 0);
    // int r = cpuMove(data);
    // gettimeofday(&t1, 0);
    // elapsed = ((t1.tv_usec - t0.tv_usec)/1000) + ((t1.tv_sec - t0.tv_sec)*1000);
    // printf("\n%d, took %ld ms\n", r, elapsed);

    // printf("num times: %d\n", num_times);
    /*
    //test to make sure each random trial is different
    int sum = 0;
    for(int i = 0; i < 20; i++){
    	int n = randomTrial(data, false);
    	sum += n;
		//printf("%d, ", n);
	}
	int n = randomTrial(data, false);
	sum += n;
	//printf("%d\n", n);
	//printf("%d\n", sum);
	*/
}