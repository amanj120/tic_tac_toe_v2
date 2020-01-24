#include <stdlib.h>
#include <sys/time.h>
#include <string.h>

#define bool short
#define true 1
#define false 0

#define debug //does nothing, but easy to do a CTRL+F to find everything that needs to be commented out
#define counting_ties (0)

//cm -> check mask, c3x3 -> check 3x3 board
// #define cm(b,i) (!(~(((b)&i)|~i)))
#define cm(b,i) (!(((b)&(i)) - (i)))
#define c3x3(b) (cm((b),0700)||cm((b),070)||cm((b),07)||cm((b),0444)||cm((b),0222)||cm((b),0111)||cm((b),0421)||cm((b),0124))
// static const short m[] = {0700, 070, 07, 0444, 0222, 0111, 0421, 0124}; //we don't need this array anymore
#define b_len (361) //length of board[]

///d[2][9] holds important metadata --bit 0: cpu plays first?, bit 1: cpu is x?, bit 2&3: difficulty?, bits8-15:last move?
#define last d[2][9]
#define cpu_player_number ((last & 0x8000) >> 15)
#define cpu_is_x ((last & 0x4000) >> 14)
#define cpu_difficulty ((last&0x3000)>>12)
#define last_move (last & 0xFF)
#define set_last_move(move) ((last & ~0xFFF) | (move))
#define size_of_data  (30*sizeof(short))

//masks to check if a 3x3 board is complete
//{111 000 000, 000 111 000, 000 000 111, 100 100 100, 010 010 010, 001 001 001, 100 010 001, 001 010 100};
// short m[] = {0700, 070, 07, 0444, 0222, 0111, 0421, 0124}; 
char board[] = "A0 A1 A2 | B0 B1 B2 | C0 C1 C2\nA3 A4 A5 | B3 B4 B5 | C3 C4 C5\nA6 A7 A8 | B6 B7 B8 | C6 C7 C8\n---------|--------------------\nD0 D1 D2 | E0 E1 E2 | F0 F1 F2\nD3 D4 D5 | E3 E4 E5 | F3 F4 F5\nD6 D7 D8 | E6 E7 E8 | F6 F7 F8\n--------------------|---------\nG0 G1 G2 | H0 H1 H2 | I0 I1 I2\nG3 G4 G5 | H3 H4 H5 | I3 I4 I5\nG6 G7 G8 | H6 H7 H8 | I6 I7 I8\n\n~ ~ ~\n~ ~ ~\n~ ~ ~\n";
short board_pos[] = {0,3,6,31,34,37,62,65,68,11,14,17,42,45,48,73,76,79,22,25,28,53,56,59,84,87,90,124,127,130,155,158,161,186,189,192,135,138,141,166,169,172,197,200,203,146,149,152,177,180,183,208,211,214,248,251,254,279,282,285,310,313,316,259,262,265,290,293,296,321,324,327,270,273,276,301,304,307,332,335,338,342,344,346,348,350,352,354,356,358};

//arbitrarily chosen for now, fix later (every 100,000 is about 1 second on my machine)
static const int n_difficulty[] = {50000,100000,200000,400000}; //ideally there's more but 
static const int min_leaf_difficulty[] = {150,150,150,150};

static double traverse_game_tree(short data[3][10], int move, int player, int n, int lev);
static void set_valid(short dd[3][10]);
static int nrand(const short data[3][10], short p, long n);
static int random_trial(const short data[3][10]);
static int select_rand_move(short d[3][10]);

void set_metadata(short d[3][10], int cpu_p_no, int cpu_x, int difficulty);
void register_move(short d[3][10], int p, int move);
void to_string(short d[3][10], char out[]);
void seed();
int cpu_move(short d[3][10]);
int check_valid(short d[3][10], int move);
int game_over(short d[3][10]);
