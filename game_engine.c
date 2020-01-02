#include "game_engine.h"

static int min(int ls[], int len){
	int min = 0x0FFF;
	for(int i = 0; i < len; i++)
		if(ls[i] < min)
			min = ls[i];
	return min;
}

static int max(int ls[], int len){
	int max = 0;
	for(int i = 0; i < len; i++)
		if(ls[i] > max)
			max = ls[i];
	return max;
}

static void clear(int ls[], int len){
	for(int i = 0; i < len; i++)
		ls[i] = 0;
}

static int select_rand_move(short d[3][10]){
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
	int move = moves[rand()%count];
	return move;
}

static int random_trial(const short data[3][10], bool print){
	//printf("inside random trial\n");
	short d[3][10];
	memcpy(d, data, size_of_data);
	int lastMove = last_move;
	short p = (d[1][lastMove/9] & 1<<(lastMove%9));
	while(1){
		if(print){
			//printData(d);
			continue;
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
		int val = random_trial(data, false) + 1;
		nums[val]++;
	}
	return nums[p + 1] + (ties * nums[0]);
}

static int l3(short data[3][10], int move, int n){
	short d[3][10];
	memcpy(d,data,size_of_data);
	register_move(d, cpu_player_number, move);
	int ret = nrand(d,cpu_player_number,true,n);//return the number of times the cpu wins
	return ret;
}

static int l2(short data[3][10], int move, int n){
	//the board will have the last move the cpu played, along with the move the user should play
	//printf("value of d(in) in l2: %p\n", , int ndata);
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
				l3s[idx] = l3(d, idx, n);
			}
		}
	}

	return max(l3s, 81);
}

static int l1(short data[3][10], int move, int n){
	//the board will have the last move the user played, along with the move the cpu should play
	// printf("value of d(in) in l1: %p\n", data);
	short d[3][10];
	memcpy(d,data,size_of_data);
	register_move(d, cpu_player_number, move);
	//printf("value of d(copy) in l1: %p\n", d);
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
		int res = l2(d, idx, n);
		r[i] = res;
	}

	//l2s has the ones the user chose, so we want to minimize this number
	return min(r, count);
}

static void setValid(short dd[3][10]){
	int lastMove = (dd[2][9]&0xFF);
	int lastBoard = lastMove/9;
	int lastSquare = lastMove%9;
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

int game_over(short d[3][10]){
	if(c3x3(d[0][9])){
		return 0;
	} else if (c3x3(d[1][9])){
		return 1;
	} else {
		return -1;
	}
}

int check_valid(short d[3][10], int move){
	return (d[2][move/9] & (1<<(move%9)));
}

void set_metadata(short d[3][10], int cpu_p_no, int cpu_x){
	int n = d[2][9];
	n = n & 0x0FFF;
	n = n|(cpu_p_no << 15);
	n = n|(cpu_x << 14);
	d[2][9] = n;
	return;
}

void register_move(short d[3][10], int p, int move){
	//this just assumes it's valid
	d[p][move/9] = (d[p][move/9]|1<<(move%9)); //move a piece
	if(c3x3(d[p][move/9])){ //if it completed a mini board
		d[p][9] = (d[p][9]|1<<(move/9));
	}
	d[2][9] = set_last_move(move);
	setValid(d);
	return;
}

int cpu_move(short d[3][10], int n){ //same as l0
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

	//#pragma omp parallel for default(none) shared(d, count, valids, r)
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
		//printf("value of d %p in cpuMove\n", d);
		r[i] = l1(d, valids[i], n);
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

void seed(){ //can be called multiple times
	struct timeval time;
	gettimeofday(&time, NULL);
	int seed = (time.tv_sec * 1000) + (time.tv_usec/1000);
	srand(seed);
}

void to_string(short d[3][10], char out[]){
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
}

/*
static void further_testing(){
	long times[1000];
	long total = 0;
	for(int i = 0; i < 1000; i++){
		long t = //printTimeForSingleRandomTrial(false);
		times[i] = t;
		if(i%100 == 0)
			//printf("%d\n", i);
		total += t;
	}
}

static long printTimeForSingleRandomTrial(bool print){
	short data[3][10];
	for(int i = 0; i < 3; i++)
		for(int j = 0; j < 10; j++)
			data[i][j] = 0;
    register_move(data, 0, 0);

    long elapsed;
    struct timeval t0, t1;
    gettimeofday(&t0, 0);
    for(int i = 0; i < 4000; i++)
	    random_trial(data, false);
    gettimeofday(&t1, 0);
    elapsed = ((t1.tv_usec - t0.tv_usec)/1000) + ((t1.tv_sec - t0.tv_sec)*1000);
    if(print)
    	printf("4,000 random trials took %ld milliseconds to complete.\n", elapsed);
    // between 90-110 ms
    return elapsed;
}
*/