SO_F=tic.so
GL_F=game_lib/game_engine.c
test -f "$SO_F" && rm "$SO_F"
gcc -shared -o "$SO_F" -fPIC "$GL_F"