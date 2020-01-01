# tic_tac_toe_v2
A better version of an earlier project I worked on

Stuff that still needs fixing (in order of most to least important):

- cpu needs to play to WIN (rn playing to lose)
- some way to handle the end of a game
- make the game state info better passed to and from the frontend
- need a way to let the user know what move the cpu played (should be pretty easy, just replace what the current "Rules" says by using the last_move in data)
- make the game page not be able to redirect to rules (will be solved by doing the above task)
- automatically submit on pressing enter
- automatically make the cursor go to the input box (both of the above things are HTML/CSS specific, will need to be stack overflowed)
- some way to add a level of difficulty to the game (most likely just the number of trials, will need some fine tuning)
- general speedups for the monte carlo tree search
- make a legitimate rules/info page