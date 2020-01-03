#include "game_engine.h"
#include <stdio.h>

#define debug //does nothing, but easy to do a CTRL+F to find everything that needs to be commented out
#define min_num_for_trials (150)
#define counting_ties (1)
// static void printData(short d[3][10]){
// 	char c[430];
// 	to_string(d,c);
// 	printf("%s\n", c);
// 	return;
// }

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

static int random_trial(const short data[3][10]){
	short d[3][10];
	memcpy(d, data, size_of_data);
	int lastMove = last_move;
	short p = (d[1][lastMove/9] & 1<<(lastMove%9));
	while(1){			
		// //debug printData(d);
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
static long nrand(const short data[3][10], short p, long n){
	long ret = 0;
	long nums[3];
	nums[0] = 0;
	nums[1] = 0;
	nums[2] = 0;
	for(long i = 0; i < n; i++){
		int val = random_trial(data) + 1;
		nums[val]++;
	}
	return nums[p + 1] + (counting_ties * nums[0]);
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
				short filled = dd[1][i]|dd[0][i];
				dd[2][i] = ~((~0777)|filled);
			}
		}
	} else {
		//the last move sent someone to a square that is not filled
		short filled = dd[1][lastSquare]|dd[0][lastSquare];
		short val = ~((~0777)|filled);
		dd[2][lastSquare] = val;
	}
	return;
}

int game_over(short d[3][10]){
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

void set_metadata(short d[3][10], int cpu_p_no, int cpu_x){
	int n = d[2][9];
	d[2][9] = (((n & 0x0FFF)|(cpu_p_no << 15))|(cpu_x << 14));
	return;
}

void register_move(short d[3][10], int p, int move){
	d[p][move/9] = (d[p][move/9]|1<<(move%9)); //move a piece
	if(c3x3(d[p][move/9]))//if it completed a mini board
		d[p][9] = (d[p][9]|1<<(move/9));
	d[2][9] = set_last_move(move);
	setValid(d);
	return;
}

static double traverse_game_tree(short data[3][10], int move, int player, long n, int lev){
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

	long next_n = (long)(n/count);
	//EXPERIMENT WITH THIS
	if(next_n < 150){//we have reached the end of our traversal 
		long num_wins = nrand(d,cpu_player_number,n);//return the number of times the cpu wins
		double ret = (double)(((double)num_wins)/n);
		//debug printf("in the end, nrand gave us %ld when running %ld trials, with a %f percentage\n", num_wins, n, ret);
		return ret;
	} else if(player == cpu_player_number){
		double ret = 0.0;
		for(int i = 0; i < count; i++){
			double v = traverse_game_tree(d, valids[i], !player, next_n, lev+1);
			//debug printf("move: %d gave us a value of %f in level %d\n", valids[i], v,lev);
			if(v > ret)
				ret = v;
		}
		return ret;
	} else {
		double ret = 1.0;
		for(int i = 0; i < count; i++){
			double v = traverse_game_tree(d, valids[i], !player, next_n, lev+1);
			//debug printf("move: %d gave us a value of %f in level %d\n", valids[i], v,lev);
			if(v < ret)
				ret = v;
		}
		return ret;
	}
}

int cpu_move(short d[3][10], long n){ //same as l0
	setValid(d);
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
	double max = -1;
	int move;
	long l1_nt = (long)(n/count);//num trials for each of l1
	if(l1_nt < 1000)
		l1_nt = 1000;
	for(int i = 0; i < count; i++){
		double v = traverse_game_tree(d, valids[i], !cpu_player_number ,l1_nt, 1);
		//debug printf("move: %d gave us a value of %f in level %d\n", valids[i], v,0);
		if(v > max){
			move = valids[i];
			max = v;
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

// int main(){
// 	seed();
// 	short data[3][10];
// 	for(int i = 0; i < 3; i++)
// 		for(int j = 0; j < 10; j++)
// 			data[i][j] = 0;
// 	printData(data);

// 	set_metadata(data, 0, 1);//cpu plays first, as X
// 	register_move(data, 0, 40);//cpu's first move is in the center
// 	printData(data);
// 	register_move(data, 1, 39);//cpu's first move is in the center
// 	printData(data);

// 	int m = cpu_move(data,1000000);
// 	printf("cpu_move chose: %d\n", m);
// }

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