# tic_tac_toe_v2
A better version of an earlier project I worked on

Stuff that still needs fixing (in order of most to least important):

- automatically make the cursor go to the input box (both of the above things are HTML/CSS specific, will need to be stack overflowed)
- some way to add a level of difficulty to the game (most likely just the number of trials, will need some fine tuning)
- general speedups for the monte carlo tree search -> adaptive number of moves depending on how many next options you have, returning floats from l1-l3, optimizing near end game by doing a check to see if there is only a winning or losing situation, etc.
- make a legitimate rules/info page