from flask import Flask, render_template, request
from ctypes import *
from os import path

so = path.abspath("tic.so")
frontend = path.abspath("frontend")
func = CDLL(so)
app = Flask(__name__, template_folder=frontend, static_folder=frontend)

data = (c_short*10)*3 	#this is a type, all instances of data will be of this type
base = c_char*430 	#the type of the returned string
num_trials = 500;

def data_to_str(d):
	s = ""
	for i in range(30):
		s += (str(d[i//10][i%10]) + ",")
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
	return b.value.decode('ascii')

def cpu_no(d):
	return ((d[2][9] & 0x8000) >> 15)

def set_metadata(d, p, x):
	#p = cpu player number, x = cpu is x
	func.set_metadata(byref(d),p,x)

def check_valid(d, m):
	return func.check_valid(d,m)

def register_usr_move(d, m):
	usr_no = (1 - cpu_no(d))
	func.register_move(byref(d),usr_no,m)

def register_cpu_move(d):
	m = func.cpu_move(byref(d), num_trials)
	func.register_move(byref(d), cpu_no(d), m)

def initialize(d):
	if cpu_no(d) == 0:
		func.register_move(byref(d), 0, 40) #sets the middle square
	else:
		for i in range(9):
			d[2][i] = 511 #sets everything as valid

def cpu_won_game(d):
	g = func.game_over(byref(d))
	if g == -1:
		return None
	elif g == cpu_no(d):
		return 1
	else:
		return 0

@app.route('/')
def home():
	return render_template("home.html")

@app.route('/rule')
def rules():
	return render_template("rules.html")

@app.route('/init/<input>', methods=['GET'])
def metadata(input):
	func.seed() #seeds the rng
	i = int(input)
	d = data()
	set_metadata(d,(i//2),(i%2)) 
	initialize(d)
	board = to_string(d)
	if(((d[2][9] & 0x8000) >> 15) == 0):
		return render_template("game.html", last="CPU Played E4", input=board, data=data_to_str(d))
	else:
		return render_template("game.html", last="It is your turn to play", input=board, data=data_to_str(d))

@app.route('/turn', methods=['POST'])
def turn():
	g = request.form['move']
	o = request.form['passed']
	print("data passed in {} {}".format(g, o))
	d = str_to_data(o)
	last_string = "That is not a valid move"
	if(len(g) == 2):
		m = (((ord(g[0])&95)-65)*9)+(int(g[1]))
		if check_valid(d, m):
			usr_no = 1 -cpu_no(d)
			register_usr_move(d,m)
			over = cpu_won_game(d)
			if over == 0:
				for i in range(9):
					d[2][i] = 0
				return render_template("end.html", board=to_string(d), input="You won the game")
			register_cpu_move(d)
			over = cpu_won_game(d)
			if over == 1:
				last_move = d[2][9]&0xFF
				l = chr(65+(last_move/9)) + str(last_move%9)
				s = "The CPU won the game by playing {}".format(l)
				for i in range(9):
					d[2][i] = 0
				return render_template("end.html", board=to_string(d), input=s)
			last_move = d[2][9]&0xFF
			l = chr(65+(last_move/9)) + str(last_move%9)
			last_string = "CPU played {}".format(l)
	board = to_string(d)
	return render_template("game.html", last=last_string, input=board, data=data_to_str(d))

if __name__ == 'main':
	app.run()
