from flask import Flask, render_template, request
from ctypes import *
from os import path

so = path.abspath("tic.so")
frontend = path.abspath("frontend")
func = CDLL(so)
app = Flask(__name__, template_folder=frontend, static_folder=frontend)

data = (c_short*10)*3 	#this is a type, all instances of data will be of this type
base = c_char*361 	#the type of the returned string
min_num_leaf = 150 #vary this between 150 and 500, increase as num_trials increases
num_trials = 200000 #directly proportional to how well the CPU plays, and inversely proportional to the time it takes the CPU to play
#observations: anything more than 1,600,000 ish is overkill, less than 200,000 is also not great
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
	if g == 2:
		return 2
	elif g == cpu_no(d):
		return 1
	elif g == -1:
		return -1
	else:
		return 0

@app.route('/')
def home():
	return render_template("home.html")

@app.route('/rule')
def rules():
	return render_template("rules.html")

@app.route('/init', methods=['POST'])
def metadata():
	func.seed() #seeds the rng
	d = data()
	diff = request.form['difficulty']
	play = request.form['player']
	p = 0
	x = 0
	if(play[8] == 'O'):
		x = 1
	if(play[14] == 'f'):
		p = 1
	print('{}, {}, {}'.format(p,x,diff))
	func.set_metadata(byref(d),p,x,int(diff)) 
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
	d = str_to_data(o)
	last_string = "'{}' is not a valid move".format((g[0].upper()) + g[1])
	m = (((ord(g[0])&95)-65)*9)+(int(g[1]))
	if check_valid(d, m):
		usr_no = 1 -cpu_no(d)
		register_usr_move(d,m)
		over = cpu_won_game(d)
		if over == 0:
			for i in range(9):
				d[2][i] = 0
			return render_template("end.html", board=to_string(d), input="You won the game")
		elif over == 2:
			for i in range(9):
				d[2][i] = 0
			return render_template("end.html", board=to_string(d), input="The game ended in a tie")
		register_cpu_move(d)
		over = cpu_won_game(d)
		if over == 1:
			last_move = d[2][9]&0xFF
			l = chr(65+(last_move/9)) + str(last_move%9)
			s = "The CPU won the game by playing {}".format(l)
			for i in range(9):
				d[2][i] = 0
			return render_template("end.html", board=to_string(d), input=s)
		elif over == 2:
			for i in range(9):
				d[2][i] = 0
			return render_template("end.html", board=to_string(d), input="The game ended in a tie")
		last_move = d[2][9]&0xFF
		l = chr(65+(last_move/9)) + str(last_move%9)
		last_string = "CPU played {}".format(l)
	board = to_string(d)
	return render_template("game.html", last=last_string, input=board, data=data_to_str(d))

if __name__ == 'main':
	app.run()
