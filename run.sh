#!/bin/sh
SO_F=tic.so
GL_F=game_lib/game_engine.c
PYC_F=app.pyc
export FLASK_APP=app.py
if [ $# -gt 0 ]; then
	export FLASK_ENV=development
else
	export FLASK_ENV=production
fi
test -f "$PYC_F" && rm "$PYC_F"
test -f "$SO_F" && rm "$SO_F"
gcc -shared -o "$SO_F" -fPIC "$GL_F"
flask run
