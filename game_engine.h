#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <string.h>

#define bool short
#define true 1
#define false 0

//cm -> check mask, c3x3 -> check 3x3 board
#define cm(b,i) (!(~((b)&m[(i)]|~m[(i)])))
#define c3x3(b) (cm((b),0)||cm((b),1)||cm((b),2)||cm((b),3)||cm((b),4)||cm((b),5)||cm((b),6)||cm((b),7))

//d[2][9] holds important data: _ _ _ _ _ _ _ _  _ _ _ _ _ _ _ _
//								a b c c          d d d d d d d d
//a = 0 if the cpu is the first player, 1 if the cpu is the second player
//b = 0 is the cpu is "O", 1 if the cpu is "X"
//c c -> level of difficulty (00, 01, 10, 11) 
//rightmost eight digits are the last move (values range from 0 to 80, or 0000 0000 -> 0101 0000)

// 0000 0001 0010 0011 0100 0101 0110 0111 1000 1001
// 0	 1	  2	   3	4	 5	  6	   7	8

#define last d[2][9]
#define cpu_player_number ((last & 0x8000) >> 15)
#define cpu_is_X ((last & 0x4000) >> 14)
#define cpu_difficulty ((last & 0x3000)>>12)
#define last_move (last & 0xFF)
#define set_last_move(move) ((last & ~0xFFF) | (move))

static const short m[] = {0700, 070, 07, 0444, 0222, 0111, 0421, 0124}; 
//masks to check if a 3x3 board is complete
//{111 000 000, 000 111 000, 000 000 111, 100 100 100, 010 010 010, 001 001 001, 100 010 001, 001 010 100};
static const int size_of_data = (30*sizeof(short));

void set_metadata(short d[3][10], int cpu_p_no, int cpu_x);
void register_move(short d[3][10], int p, int move);
void seed();
int cpuMove(short d[3][10]);
int check_valid(short d[3][10], int move);

static void setValid(short dd[3][10]);
static int randomTrial(const short data[3][10], bool print);
static int select_rand_move(short d[3][10]);
static void clear(int ls[], int len);
static int max(int ls[], int len);
static int min(int ls[], int len);
static int nrand(const short data[3][10], short p, bool ties, int n);
static int l1(short data[3][10], int move);
static int l2(short data[3][10], int move);
static int l3(short data[3][10], int move);