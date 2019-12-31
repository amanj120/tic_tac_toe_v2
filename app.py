from flask import Flask
from flask import render_template, request
import os
from ctypes import *

#so = os.path.abspath("fib.so")
#func = CDLL(so)
temp = os.path.abspath('game.html')
board = "A0 A1 A2 | B0 B1 B2 | C1 C2 C3\t       \nA3 A4 A5 | B3 B4 B5 | C3 C4 C5\t       \nA6 A7 A8 | B6 B7 B8 | C6 C7 C8\t       \n---------|--------------------\t       \nD0 D1 D2 | E0 E1 E2 | F0 F1 F2\t  ~ ~ ~\nD3 D4 D5 | E3 E4 E5 | F3 F4 F5\t  ~ ~ ~\nD6 D7 D8 | E6 E7 E8 | F6 F7 F8\t  ~ ~ ~\n--------------------|---------\t       \nG0 G1 G2 | H0 H1 H2 | I0 I1 I2\t       \nG3 G4 G5 | H3 H4 H5 | I3 I4 I5\t       \nG6 G7 G8 | H6 H7 H8 | I6 I7 I8\t       \n";
app = Flask(__name__, template_folder='/')

#A B C D E F G H I 0 1 2 3 4 5 6 7 8

@app.route('/')
def home():
	#return "hello"
	return render_template(temp, input=board)

@app.route('/fib', methods=['POST'])
def fib():
	ii = request.form['integer']
	i = int(ii)
	s = "the {}th fibonacci number is \n{}".format(i, i)
	return render_template(temp, input=s)

if __name__ == 'main':
	app.run()

'''
import os
from ctypes import *
so = os.path.abspath("fib.so")
func = CDLL(so)
print(func.fib(1))
'''
#gcc -shared -o libhello.so -fPIC hello.c