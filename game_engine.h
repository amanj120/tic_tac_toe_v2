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
static const char board[] = "A0 A1 A2 | B0 B1 B2 | C0 C1 C2\t       \nA3 A4 A5 | B3 B4 B5 | C3 C4 C5\t       \nA6 A7 A8 | B6 B7 B8 | C6 C7 C8\t       \n---------|--------------------\t       \nD0 D1 D2 | E0 E1 E2 | F0 F1 F2\t  ~ ~ ~\nD3 D4 D5 | E3 E4 E5 | F3 F4 F5\t  ~ ~ ~\nD6 D7 D8 | E6 E7 E8 | F6 F7 F8\t  ~ ~ ~\n--------------------|---------\t       \nG0 G1 G2 | H0 H1 H2 | I0 I1 I2\t       \nG3 G4 G5 | H3 H4 H5 | I3 I4 I5\t       \nG6 G7 G8 | H6 H7 H8 | I6 I7 I8\t       \n";
static const short board_pos[] = {0,3,6,39,42,45,78,81,84,11,14,17,50,53,56,89,92,95,22,25,28,61,64,67,100,103,106,156,159,162,195,198,201,234,237,240,167,170,173,206,209,212,245,248,251,178,181,184,217,220,223,256,259,262,312,315,318,351,354,357,390,393,396,323,326,329,362,365,368,401,404,407,334,337,340,373,376,379,412,415,418,189,191,193,228,230,232,267,269,271};

void to_string(short d[3][10], char out[]);
void set_metadata(short d[3][10], int cpu_p_no, int cpu_x);
void register_move(short d[3][10], int p, int move);
void seed();
int cpu_move(short d[3][10], int n);
int check_valid(short d[3][10], int move);
int game_over(short d[3][10]);

static void setValid(short dd[3][10]);
static void clear(int ls[], int len);
static int nrand(const short data[3][10], short p, bool ties, int n);
static int random_trial(const short data[3][10], bool print);
static int select_rand_move(short d[3][10]);
static int max(int ls[], int len);
static int min(int ls[], int len);
static int l1(short data[3][10], int move, int n);
static int l2(short data[3][10], int move, int n);
static int l3(short data[3][10], int move, int n);