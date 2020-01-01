rm flask_app/tic.so
gcc -shared -o tic.so -fPIC *.c
mv tic.so flask_app