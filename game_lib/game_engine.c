#include "game_engine.h"
#include <stdio.h>
#include <omp.h>


static int select_rand_move(short d[3][10]){
	short count = 0;
	short dt[9];
	for(int i = 0; i < 9; i++){
		short v = d[2][i] & 0x1FF;
		short a = (v&0x155) + ((v>>1)&0x155);// v&101010101 + (v&010101010>>1);
		short b = (a&0x133) + ((a>>2)&0x133);// a&100110011 + (a&011001100>>2);
		short c = (b&0x10F) + ((b>>4)&0x10F);// b&100001111 + b&011110000>>4;
		short e = (c>>8) + (c&0xFF);//&01111111 & c>>8;
		dt[i] = e;
		count += e;
	}

	if(count == 0)
		return -1;

	int m = rand()%count;
	for(short i = 0; i < 9; i++){
		short v = d[2][i];
		if(v && m < dt[i]){
			short a = (v&0x155) + ((v>>1)&0x155);
			short b = (a&0x133) + ((a>>2)&0x133);
			short j,t,n;
			if(v>>8 && (m==(dt[i]-1)))
				return (i*9) + 8 ;//if it's the leftmost bit

			j = 0;//check if its in bits 0-3 or 4-7
			t = b & 0xF;
			n = (m>=t);
			m -= (n*t);

			j = (n*4);//check if it's in bits 0-1/4-5 or 2-3/6-7
			t = (a>>j)&3;
			n = (m>=t);
			m -= (n*t);

			j += (n*2);//figure out which bit its in
			t = (v>>j) & 1;
			return (i*9) + (j + (m>=t));
		}
		m -= dt[i];
	}
}

static int random_trial(const short data[3][10]){
	short d[3][10];
	set_valid(d);
	memcpy(d, data, size_of_data);
	int lastMove = last_move;
	short p = (d[1][lastMove/9] & 1<<(lastMove%9));
	short m[] = {0700, 070, 07, 0444, 0222, 0111, 0421, 0124}; 
	while(1){			
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
		}
	}
}

static void set_valid(short dd[3][10]){
	int lastSquare = (dd[2][9]&0xFF)%9;
	for(short i = 0; i < 9; i++){
		dd[2][i] = 0;
	}
	if((dd[0][9] & 1<<lastSquare) || (dd[1][9] & 1<<lastSquare)){
		//the last move sent someone to a square that's already filled
		for(short i = 0; i < 9; i++){
			short a = (dd[0][9]|dd[1][9]);//big squares already filled
			if((a&(1<<i)))
				dd[2][i] = (~((~0777)|dd[1][i]|dd[0][i]));
		}
	} else {
		dd[2][lastSquare] = ~((~0777)|dd[1][lastSquare]|dd[0][lastSquare]);
	}
	return;
}

void register_move(short d[3][10], int p, int move){
	short m[] = {0700, 070, 07, 0444, 0222, 0111, 0421, 0124}; 
	d[p][move/9] = (d[p][move/9]|1<<(move%9)); //move a piece
	if(c3x3(d[p][move/9]))//if it completed a mini board
		d[p][9] = (d[p][9]|1<<(move/9));
	d[2][9] = set_last_move(move);
	set_valid(d);
	return;
}


/*
conducts n random trials
returns how many times player p wins
p is either 1 or 0
if ties == true, then returns how many times p wins or the game is a tie
*/
static int nrand(const short data[3][10], short p, long n){
	int ret = 0;
	int nums[3] = {0,0,0};
	for(int i = 0; i < n; i++){
		int val = random_trial(data) + 1;
		nums[val]++;
	}
	return nums[p + 1] + (counting_ties * nums[0]);
}

int game_over(short d[3][10]){
	short m[] = {0700, 070, 07, 0444, 0222, 0111, 0421, 0124}; 
	if(c3x3(d[0][9]))
		return 0;
	else if (c3x3(d[1][9]))
		return 1;
	else {
		for(int i = 0; i < 9; i++)
			if(d[2][i] & 0x1FF) //there is a valid move
				return -1; //game was a tie
		return 2; //game not over yet
	}
}

int check_valid(short d[3][10], int move){
	return (d[2][move/9] & (1<<(move%9)));
}

void set_metadata(short d[3][10], int cpu_p_no, int cpu_x, int difficulty){
	int n = d[2][9];
	d[2][9] = ((((n & 0x0FFF)|(cpu_p_no << 15))|(cpu_x << 14))|(difficulty<<12));
	return;
}

/*
data = board passed in
move = player's move
player = which player is making the move
n = number of trials to run
lev = level we are on (unecessary except for debugging)
min_leaf = the minimum number of trials necessary for each leaf node
*/
static double traverse_game_tree(short data[3][10], int move, int player, int n, int lev){
	short d[3][10];
	memcpy(d,data,size_of_data);
	register_move(d, player, move); //note, registering a move sets valid already

	int g = game_over(d);
	if(g == cpu_player_number)
		return 1;
	else if(g == (1-cpu_player_number))
		return 0;
	else if(g == 2)
		return counting_ties; //game was a tie, if we are counting ties then return a 1, or else a 0

	int valids[81];
	int count = 0;
	for(int i = 0; i < 9; i++){
		int a = d[2][i];
		for(int j = 0; j < 9; j++)
			if(a & 1<<j)
				valids[count++] = i*9 + j;
	}

	if(count == 0){
		return -1;
	}
	int min_leaf = min_leaf_difficulty[cpu_difficulty];
	int next_n = (int)(n/count);
	if(next_n < min_leaf){//we have reached the end of our traversal 
		int num_wins = nrand(d,cpu_player_number,n);//return the number of times the cpu wins
		double ret = (double)(((double)num_wins)/n);
		// debug printf("in the end, nrand gave us %ld when running %ld trials, with a %f percentage\n", num_wins, n, ret);
		return ret;
	} else if(player == cpu_player_number){ //now its the users turn to play, and we want to minimize
		double ret = 1.0;
		for(int i = 0; i < count; i++){
			double v = traverse_game_tree(d, valids[i], !player, next_n, lev+1);
			//debug printf("move: %d gave us a value of %f in level %d\n", valids[i], v,lev);
			if(v < ret)
				ret = v;
			if(v < 0.0001)
				return 0;
		}
		return ret;
	} else {
		double ret = 0.0;
		for(int i = 0; i < count; i++){
			double v = traverse_game_tree(d, valids[i], !player, next_n, lev+1);
			//debug printf("move: %d gave us a value of %f in level %d\n", valids[i], v,lev);
			if(v > ret)
				ret = v;
			if(v > 0.9999)
				return 1;
		}
		return ret;
	}
}

int cpu_move(short d[3][10]){ //same as l0
	set_valid(d);
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
	
	int n = n_difficulty[cpu_difficulty];
	int l1_nt = (int)(n/count);	// print_data(data);

	// int counts[81];
	// for(int i = 0; i < 81; i++){
	// 	counts[i] = 0;
	// }
	// for(int i = 0; i < 6900000; i++){
	// 	int m = select_rand_move(data);
	// 	counts[m]++;
	// }
	// for(int i = 0; i < 81; i++){
	// 	printf("counts[%d]: %f\n",i,(counts[i]/100000.0));
	// };//num trials for each of l1
	if(l1_nt < 1000)
		l1_nt = 1000;

	/* //old code prior to performance tuning
	double max = -1;
	int move;
	for(int i = 0; i < count; i++){
		double v = traverse_game_tree(d, valids[i], cpu_player_number ,l1_nt, 2);
		//debug printf("move: %d gave us a value of %f in level %d\n", valids[i], v,1);
		if(v > max){
			move = valids[i];
			max = v;
		}
		if(max >= 0.9999){
			return move;
		}
	}
	debug printf("move: %d gave us a value of %f in level %d with %d moves\n", move,max,0,n);
	*/
	double vals[count];
	int c = cpu_player_number;

//	export OMP_NUM_THREADS=whatever
//	this computer: 4 cores, 2 threads per core
// 	the problem with running in parallel : about 100x more context switching (given by vmstat, ~5,000/s in serial, ~600,000/s in parallel)


	//ideal parallelization
	// const int num_threads = 8;
	// printf("%d = numthreads, %d = count, %d = max_i\n", num_threads, count, (count/num_threads));
	// #pragma omp parallel
	// {
	// 	for(int i = 0; i <=(count/num_threads); i++){
	// 		int l = (i*num_threads) + (omp_get_thread_num());
	// 		if(l < count){
	// 			printf("checking move %d with thread %d\n", valids[l], omp_get_thread_num());
	// 			vals[l] = traverse_game_tree(d, valids[l], c ,l1_nt, 2);
	// 		}
	// 	}
	// }

	//ideally parallelizing this loop just has a 10x speedup lol
	#pragma omp parallel for
	for(int i = 0; i < count; i++){
		printf("Checking move %d\n", valids[i]);
		double v = traverse_game_tree(d, valids[i], c ,l1_nt, 2);
		vals[i] = v;
		//printf("checking move %d with thread %d with result %f\n", valids[i], omp_get_thread_num(), v);
	}

	double max = -1;
	int move = -1;
	for(int i = 0; i < count; i++){
		printf("%d: %f\n", valids[i], vals[i]);
		if(vals[i] > max){
			max = vals[i];
			move = valids[i];
		}
	}
	return move;
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
			if(cpu_is_x && cpu_player_number==0){
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
		if(cpu_is_x && cpu_player_number==0){
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

static void print_data(short d[3][10]){char c[b_len]; to_string(d,c);printf("%s\n", c);}

int main(){
	seed();

	// #pragma omp parallel
	// {
	// 	#pragma omp single
	// 	for(int i = 0; i < 10; i++)
	// 	{
	// 		printf("hello from thread %d\n", omp_get_thread_num());
	// 	}
	// }

	short data[3][10];
	for(int i = 0; i < 3; i++)
		for(int j = 0; j < 10; j++)
			data[i][j] = 0;
	// print_data(data);

	set_metadata(data, 0, 1,3);//cpu plays first, as X, max difficulty
	register_move(data, 0, 40);//cpu's first move is in the center
	register_move(data, 1, 39);//center left
	register_move(data, 0, 31);//center
	register_move(data, 1, 36);//top left
	register_move(data, 0, 4);//center
	// register_move(data, 1, 42);//complete center square
	// register_move(data,0,58);//send back to center square for 81 moves

	// print_data(data);

	// int counts[81];
	// for(int i = 0; i < 81; i++){
	// 	counts[i] = 0;
	// }
	// for(int i = 0; i < 6900000; i++){
	// 	int m = select_rand_move(data);
	// 	counts[m]++;
	// }
	// for(int i = 0; i < 81; i++){
	// 	printf("counts[%d]: %f\n",i,(counts[i]/100000.0));
	// }
	int m = cpu_move(data);
	printf("cpu moved: %d\n", m);
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