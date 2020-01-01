from flask import Flask
from flask import render_template, request
import os
from ctypes import *

so = os.path.abspath("tic.so")
func = CDLL(so)
app = Flask(__name__)

data = (c_short*10)*3 	#this is a type, all instances of data will be of this type
base = c_char*430		#the type of the board string

def data_to_str(d):
	s = ""
	for i in range(3):
		for j in range(10):
			s += str(d[i][j])
			s += ","
	return s[:-1]

def str_to_data(s):
	d = data()
	n = s.split(",")
	for i in range(30):
		d[i//10][i%10] = int(n[i])
	return d;

def to_string(d):
	b = base()
	func.to_string(byref(d),byref(b))
	s = b.value.decode('ascii')
	return s

def set_metadata(d, cpu_p_no=0, cpu_is_x=0):
	func.set_metadata(byref(d),cpu_p_no,cpu_is_x)

def check_valid(d, move):
	return func.check_valid(d,move)

def seed():
	func.seed()

def register_usr_move(d, m):
	usr_no = (1 - ((d[2][9] & 0x8000) >> 15))
	func.register_move(byref(d),usr_no,m)

def register_cpu_move(d):
	m = cpu_move(d)
	cpu_no = ((d[2][9] & 0x8000) >> 15)
	func.register_move(byref(d), cpu_no, m)

def cpu_move(d):
	return func.cpuMove(byref(d))

def initialize(d):
	cpu_no = ((d[2][9] & 0x8000) >> 15)
	if cpu_no == 0:
		func.register_move(byref(d), 0, 40) #sets the middle square
	else:
		for i in range(9):
			d[2][i] = 511 #sets everything as valid
#needs to have the user choose the cpu player and x/o
@app.route('/')
def home():
	return render_template("home.html")
	# a = data()
	# for i in range(3):
	# 	for j in range(10):
	# 		a[i][j] = (i*10 + j) + 1
	# #need to convert the array to a string, and convert it back as well
	# return render_template("game.html", input="hello", data=data_to_str(a))

'''                                       		  cpu player | cpu is X
<a href="/metadata/2">Play as X, go first</a><br> 			1 0 -> 2
<a href="/metadata/0">Play as X, go second</a><br> 			0 0 -> 0
<a href="/metadata/3">Play as O, go first</a><br> 			1 1 -> 3
<a href="/metadata/1">Play as O, go second</a><br> 			0 1 -> 1
'''

@app.route('/metadata/<input>', methods=['GET'])
def metadata(input):
	i = int(input)
	p = i//2
	x = i%2
	d = data()
	set_metadata(d,p,x) 
	initialize(d)
	board = to_string(d)
	return render_template("game.html", input=board, data=data_to_str(d))
	# return "option selected was {}".format(str(opt))

@app.route('/turn', methods=['POST'])
def turn():
	g = request.form['move']
	o = request.form['passed']
	m = (((ord(g[0])&95)-65)*9)+(int(g[1]))
	d = str_to_data(o)
	if check_valid(d, m):
		register_usr_move(d,m)
		register_cpu_move(d)
	board = to_string(d)
	return render_template("game.html", input=board, data=data_to_str(d))
	'''
	if m is a valid move then -> register the user move, register the cpu move, return the new data and board
	otherwise, return the old data and board
	'''
	#return("the form input was {} and the board input was {}".format(g,o))

@app.route('/rule', methods=['GET'])
def rules():
	return render_template("rules.html")

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

'''
data = (c_short*10)*3
d = data() #will create a short array [3][10]
board_base = c_char*430
b = board_base()
func.to_string(d,b)
s = b.value.decode('ascii')
'''