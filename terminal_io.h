#include "game_engine.h"

const char board[] = "A0 A1 A2 | B0 B1 B2 | C0 C1 C2\t       \nA3 A4 A5 | B3 B4 B5 | C3 C4 C5\t       \nA6 A7 A8 | B6 B7 B8 | C6 C7 C8\t       \n---------+----------+---------\t       \nD0 D1 D2 | E0 E1 E2 | F0 F1 F2\t  ~ ~ ~\nD3 D4 D5 | E3 E4 E5 | F3 F4 F5\t  ~ ~ ~\nD6 D7 D8 | E6 E7 E8 | F6 F7 F8\t  ~ ~ ~\n---------+----------+---------\t       \nG0 G1 G2 | H0 H1 H2 | I0 I1 I2\t       \nG3 G4 G5 | H3 H4 H5 | I3 I4 I5\t       \nG6 G7 G8 | H6 H7 H8 | I6 I7 I8\t       \n";
const short board_pos[] = {0,3,6,39,42,45,78,81,84,11,14,17,50,53,56,89,92,95,22,25,28,61,64,67,100,103,106,156,159,162,195,198,201,234,237,240,167,170,173,206,209,212,245,248,251,178,181,184,217,220,223,256,259,262,312,315,318,351,354,357,390,393,396,323,326,329,362,365,368,401,404,407,334,337,340,373,376,379,412,415,418,189,191,193,228,230,232,267,269,271};

void printData(short d[3][10]);
void to_string(short d[3][10], char out[]);