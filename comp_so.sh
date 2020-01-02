FILE=flask_app/tic.so
test -f "$FILE" && rm "$FILE"
gcc -shared -o "$FILE" -fPIC game_engine.c