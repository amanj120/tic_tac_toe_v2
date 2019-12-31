#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <time.h>
#include <string.h>
#include <omp.h>

#define bool short
#define true 1
#define false 0

#define cm(b,i) (!(~((b)&m[(i)]|~m[(i)])))
#define c3x3(b) (cm((b),0)||cm((b),1)||cm((b),2)||cm((b),3)||cm((b),4)||cm((b),5)||cm((b),6)||cm((b),7))

//d[2][9] holds important data: _ _ _ _ _ _ _ _  _ _ _ _ _ _ _ _
//								a b c c          d d d d d d d d
//a = 0 if the cpu is the first player, 1 if the cpu is the second player
//b = 0 is the cpu is "O", 1 if the cpu is "X"
//c c -> level (00, 01, 10, 11)
//rightmost eight digits are the last move (values range from 0 to 80, or 0000 0000 -> 0101 0000)

// 0000 0001 0010 0011 0100 0101 0110 0111 1000 1001
// 0	 1	  2	   3	4	 5	  6	   7	8

#define last d[2][9]
#define cpu_player_number ((last & 0x8000) >> 15)
#define cpu_is_X ((last & 0x4000) >> 14)
#define cpu_difficulty ((last & 0x3000)>>12)
#define last_move (last & 0xFF)
#define set_last_move(move) ((last & ~0xFFF) | (move))
//cm - check mask

static const short m[] = {0700, 070, 07, 0444, 0222, 0111, 0421, 0124}; //masks
//{111 000 000, 000 111 000, 000 000 111, 100 100 100, 010 010 010, 001 001 001, 100 010 001, 001 010 100};
static const char board[] = "A0 A1 A2 | B0 B1 B2 | C0 C1 C2\nA3 A4 A5 | B3 B4 B5 | C3 C4 C5\nA6 A7 A8 | B6 B7 B8 | C6 C7 C8\n---------+----------+---------\nD0 D1 D2 | E0 E1 E2 | F0 F1 F2\t\t- - -\nD3 D4 D5 | E3 E4 E5 | F3 F4 F5\t\t- - -\nD6 D7 D8 | E6 E7 E8 | F6 F7 F8\t\t- - -\n---------+----------+---------\nG0 G1 G2 | H0 H1 H2 | I0 I1 I2\nG3 G4 G5 | H3 H4 H5 | I3 I4 I5\nG6 G7 G8 | H6 H7 H8 | I6 I7 I8\n";
static const short board_pos[] = {0,3,6,31,34,37,62,65,68,11,14,17,42,45,48,73,76,79,22,25,28,53,56,59,84,87,90,124,127,130,162,165,168,200,203,206,135,138,141,173,176,179,211,214,217,146,149,152,184,187,190,222,225,228,269,272,275,300,303,306,331,334,337,280,283,286,311,314,317,342,345,348,291,294,297,322,325,328,353,356,359,156,158,160,194,196,198,232,234,236};
static const int size_of_data = (30*sizeof(short));

static int num_times;

void printData(short d[3][10]);
void register_move(short data[3][10], int p, int move);
int select_rand_move(short d[3][10]);
void seed();
long printTimeForSingleRandomTrial(bool print);
void further_testing();
// all data will be passed via a multidimensional short array: data[3][10]
// p0 9x9 board, p0 bigger 3x3 board
// p1 9x9 board, p1 bigger 3x3 board
// state dependen/t valid moves tracker, (first 9 entries), last entry (data[2][9]) is the last move played

void set_metadata(short d[3][10], int cpu_p_no, int cpu_x){
	int n = d[2][9];
	n = n & 0x0FFF;
	n = n|(cpu_p_no << 15);
	n = n|(cpu_x << 14);
	d[2][9] = n;
	return;
}

void printData(short d[3][10]){
	char out[363];
	//char out[] = "A0 A1 A2 | B0 B1 B2 | C0 C1 C2\nA3 A4 A5 | B3 B4 B5 | C3 C4 C5\nA6 A7 A8 | B6 B7 B8 | C6 C7 C8\n---------+----------+---------\nD0 D1 D2 | E0 E1 E2 | F0 F1 F2\nD3 D4 D5 | E3 E4 E5 | F3 F4 F5\nD6 D7 D8 | E6 E7 E8 | F6 F7 F8\n---------+----------+---------\nG0 G1 G2 | H0 H1 H2 | I0 I1 I2\nG3 G4 G5 | H3 H4 H5 | I3 I4 I5\nG6 G7 G8 | H6 H7 H8 | I6 I7 I8\n";
	strcpy(out, board);
	for(int j = 0; j < 3; j++){
		for(int i = 0; i < 10; i++){
			//printf("%d ", d[j][i]);
		}
		//printf("\n");
	}
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

//returns the number of valid moves
void setValid(short dd[3][10]){
	//printf("value of d in set_valid: %p\n", dd);
	//printf("value of d[2][0] in set_valid: %p\n", &dd[2][0]);
	//printf("value of d[2][1] in set_valid: %p\n", &dd[2][1]);
	int lastMove = (dd[2][9]&0xFF);
	int lastBoard = lastMove/9;
	int lastSquare = lastMove%9;
	//printf("lm: %d, lb: %d, ls: %d\n", lastMove, lastBoard, lastSquare);
	//int lastPlayer = (d[1][lastBoard] & 1<<lastSquare);
	//if player 1 played on the lastMove, then the value of this will be 1, otherwise it's 0
	//int p = !lastPlayer;//current player
	for(short i = 0; i < 9; i++){
		dd[2][i] = 0;
	}
	if((dd[0][9] & 1<<lastSquare) || (dd[1][9] & 1<<lastSquare)){
		//the last move sent someone to a square that's already filled
		for(short i = 0; i < 9; i++){

			if(!((dd[0][9] & 1<<i) || (dd[1][9] & 1<<i))){
				//the square isn't already filled
				//printf("address being edited: %p\n", &dd[2][i]);
				short filled = dd[1][i]|dd[0][i];
				dd[2][i] = ~((~0777)|filled);
			}
		}
	} else {
		//the last move sent someone to a square that is not filled
		short filled = dd[1][lastSquare]|dd[0][lastSquare];
		short val = ~((~0777)|filled);
		//printf("address being edited: %p\n", &dd[2][lastSquare]);
		dd[2][lastSquare] = val;
	}
	return;
}

void register_move(short d[3][10], int p, int move){
	//this just assumes it's valid
	d[p][move/9] = (d[p][move/9]|1<<(move%9)); //move a piece
	if(c3x3(d[p][move/9])){ //if it completed a mini board
		d[p][9] = (d[p][9]|1<<(move/9));
	}
	d[2][9] = set_last_move(move);
	return;
}

//0x7ffe33e95310
//0x7ffe33e94fd0

int min(int ls[], int len){
	int min = 0x0FFF;
	for(int i = 0; i < len; i++){
		if(ls[i] < min){
			min = ls[i];
		}
	}
	return min;
}

int max(int ls[], int len){
	int max = 0;
	for(int i = 0; i < len; i++){
		if(ls[i] > max){
			max = ls[i];
		}
	}
	return max;
}

void clear(int ls[], int len){
	for(int i = 0; i < len; i++){
		ls[i] = 0;
	}
}

int randomTrial(const short data[3][10], bool print){
	//printf("inside random trial\n");
	short d[3][10];
	memcpy(d, data, size_of_data);
	int lastMove = last_move;
	short p = (d[1][lastMove/9] & 1<<(lastMove%9));
	while(1){
		if(print){
			printData(d);
		}
		if(c3x3(d[0][9])){
			return 0;
		} else if(c3x3(d[1][9])){
			return 1;
		} else {
			p = !p;
			int move = select_rand_move(d);
			if(move == -1){
				return -1;
			}
			register_move(d, p, move);
			set_last_move(move);
		}
	}
}

/*
conducts n random trials
returns how many times player p wins
p is either 1 or 0
if ties == true, then returns how many times p wins or the game is a tie
*/
static int nrand(const short data[3][10], short p, bool ties, int n){
	//printf("inside nrand\n");
	//printData(data);
	int ret = 0;
	int nums[3];
	for(int i = 0; i < n; i++){
		int val = randomTrial(data, false) + 1;
		nums[val]++;
	}
	return nums[p + 1] + (ties * nums[0]);
}

static int l3(short data[3][10], int move){
	//has the last move the user played, along with the move the spu should play, before running the random trial
	//printf("value of d(in) in l3: %p\n", data);
	//printf("inside l3\n");
	short d[3][10];
	memcpy(d,data,size_of_data);

	//printf("value of d(copy) in l3: %p\n", data);
	register_move(d, cpu_player_number, move);

	//printData(d);
	int ret = nrand(d,cpu_player_number,true,1000);//return the number of times the cpu wins
	// printf("rand trial results: %d\n", ret);
	num_times += 1000;
	return ret;
}

static int l2(short data[3][10], int move){
	//the board will have the last move the cpu played, along with the move the user should play
	//printf("value of d(in) in l2: %p\n", data);
	short d[3][10];
	memcpy(d,data,size_of_data);
	register_move(d, !cpu_player_number, move);
	//change d to include the move
	//printf("value of d(copy) in l2: %p\n", d);
	setValid(d);
	//printData(d);
	int l3s[81];
	clear(l3s, 81);
	for(int i = 0; i < 9; i++){
		int a = d[2][i];
		for(int j = 0; j < 9; j++){
			if(a & 1<<j){
				int idx = i*9 + j;
				l3s[idx] = l3(d, idx);
			}
		}
	}

	return max(l3s, 81);
}

static int l1(short data[3][10], int move){
	//the board will have the last move the user played, along with the move the cpu should play
	// printf("value of d(in) in l1: %p\n", data);
	short d[3][10];
	memcpy(d,data,size_of_data);
	register_move(d, cpu_player_number, move);
	printf("value of d(copy) in l1: %p\n", d);
	setValid(d);
	//printData(d);.
	// int min = 100000;
	int valids[81];
	int count = 0;
	for(int i = 0; i < 9; i++){
		int a = d[2][i];
		for(int j = 0; j < 9; j++){
			if(a & 1<<j){
				int idx = i*9 + j;
				valids[count] = idx;
				count++;
			}
		}
	}
	int r[count];
	//#pragma omp parallel for
	for(int i = 0; i < count; i++){
		int idx = valids[i];
		int res = l2(d, idx);
		r[i] = res;
	}

	//l2s has the ones the user chose, so we want to minimize this number
	return min(r, count);
}

int cpuMove(short d[3][10]){ //same as l0
	//input: a board with the last move the user played
	//note: user will be (!cpu_player_number)
	//printf("value of d inside cpuMove: %p\n", d);
	setValid(d);
	////printData(d);
	int valids[81];
	int count = 0;
	for(int i = 0; i < 9; i++){
		int a = d[2][i];
		for(int j = 0; j < 9; j++){
			if(a & 1<<j){	
				int idx = i*9 + j;
				valids[count] = idx;
				count++;
			} 
		}
	}
	int r[count];

	#pragma omp parallel for default(none) shared(d, count, valids, r)
	for(int i = 0; i < count; i++){
		//long elapsed = 0;
	 //    struct timeval t0, t1;
	 //    gettimeofd		//long elapsed = 0;
	 //    struct timeval t0, t1;
	 //    gettimeofday(&t0, 0);
	    // short dcopy[3][10];
	    // memcpy(dcopy, d, size_of_data);ay(&t0, 0);
	    // short dcopy[3][10];
	    // memcpy(dcopy, d, size_of_data);
		//printf("value of d %p in thread %d\n", d, omp_get_thread_num());
		printf("value of d %p in cpuMove\n", d);
		r[i] = l1(d, valids[i]);
		//doing: c = l1(...) doesn't cause any speedup, so the issue isn't in there
		//printf("value of c: %d in thread %d\n", c, omp_get_thread_num());
	    //gettimeofday(&t1, 0);
	    // elapsed = ((t1.tv_usec - t0.tv_usec)/1000) + ((t1.tv_sec - t0.tv_sec)*1000);
		//printf("checking move %d with thread %d took %ld ms\n", valids[i], omp_get_thread_num(), elapsed);
		 
	}
	//at this point l1s contains the minimax value 
	//for every single possible move that the 
	//cpu could make, so we want the maximum of those values
	int target = max(r, count);
	for(int i = 0; i < count; i++){
		if(r[i] == target){
			return valids[i];
		}
	}
}






int select_rand_move(short d[3][10]){
	setValid(d);
	int count = 0;
	int moves[81];
	for(int i = 0; i < 9; i++){
		int a = d[2][i];
		for(int j = 0; j < 9; j++){
			if(a & 1<<j){
				moves[count] = i*9 + j;//check if this works, it probably does
				count++;
			}
		}
	}
	if(count == 0){
		return -1;
	}
	////printData(d);
	int move = moves[rand()%count];
	return move;
}

void seed(){ //can be called multiple times
	struct timeval time;
	gettimeofday(&time, NULL);
	int seed = (time.tv_sec * 1000) + (time.tv_usec/1000);
	srand(seed);
}

void further_testing(){
	long times[1000];
	long total = 0;
	for(int i = 0; i < 1000; i++){
		long t = //printTimeForSingleRandomTrial(false);
		times[i] = t;
		if(i%100 == 0)
			//printf("%d\n", i);
		total += t;
	}
	//printf("Average time to run 4,000 trials (1000 trials) is %f milliseconds.\n", (double)(total/1000.0));
}




long printTimeForSingleRandomTrial(bool print){
	short data[3][10];
	for(int i = 0; i < 3; i++)
		for(int j = 0; j < 10; j++)
			data[i][j] = 0;
    register_move(data, 0, 0);

    long elapsed;
    struct timeval t0, t1;
    gettimeofday(&t0, 0);
    for(int i = 0; i < 4000; i++)
	    randomTrial(data, false);
    gettimeofday(&t1, 0);
    elapsed = ((t1.tv_usec - t0.tv_usec)/1000) + ((t1.tv_sec - t0.tv_sec)*1000);
    if(print)
    	printf("4,000 random trials took %ld milliseconds to complete.\n", elapsed);
    // between 90-110 ms
    return elapsed;
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
	num_times = 0;
	short data[3][10];
	for(int i = 0; i < 3; i++)
		for(int j = 0; j < 10; j++)
			data[i][j] = 0;
	set_metadata(data, 1, 1);
	//printf("%d\n", data[2][9]);
    register_move(data, 0, 40);
    // register_move(data, 1, 37);
    // register_move(data, 0, 12);
    ////printf("sizeof(data): %ld\n", sizeof(data));
    // printData(data);
    // int n = nrand(data, 1, false, 10);
    // printf("\n%d\n", n);
    //randomTrial(data, true);
	//printf("value of d in main: %p\n", data);
    long elapsed;
    struct timeval t0, t1;
    gettimeofday(&t0, 0);
    int r = cpuMove(data);
    gettimeofday(&t1, 0);
    elapsed = ((t1.tv_usec - t0.tv_usec)/1000) + ((t1.tv_sec - t0.tv_sec)*1000);
    printf("\n%d, took %ld ms\n", r, elapsed);

    printf("num times: %d\n", num_times);
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
/*TODO
 - further verify that c3x3 works
 - actually do the whole monte carlo tree search thing
 - create a more user friendly terminal output
*/