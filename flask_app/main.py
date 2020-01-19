from flask import Flask, render_template, request, render_template_string
from ctypes import *
from os import path

so = path.abspath("game_lib/tic.so")
frontend = path.abspath("frontend")
func = CDLL(so)
app = Flask(__name__)#, template_folder=frontend, static_folder=frontend)

data = (c_short*10)*3 	#this is a type, all instances of data will be of this type
base = c_char*361 	#the type of the returned string
min_num_leaf = 150 #vary this between 150 and 500, increase as num_trials increases
num_trials = 200000 #directly proportional to how well the CPU plays, and inversely proportional to the time it takes the CPU to play
#observations: anything more than 1,600,000 ish is overkill, less than 200,000 is also not great
def data_to_str(d):
	s = ""
	for i in range(30):
		s += (str(d[i//10][i%10]) + ".")
	return s[:-1]

def str_to_data(s):
	d = data()
	n = s.split(".")
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

@app.errorhandler(404)
def page_not_found(e):
	return render_template("404.html")

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
	func.set_metadata(byref(d),p,x,int(diff)) 
	initialize(d)
	board = to_string(d)
	if(((d[2][9] & 0x8000) >> 15) == 0):
		#return render_template("game.html", last="CPU Played E4", input=board, data=data_to_str(d))
		return get_mobile_html(board, "CPU Played E4", data_to_str(d))
	else:
		#return render_template("game.html", last="It is your turn to play", input=board, data=data_to_str(d))
		return get_mobile_html(board, "It is your turn to play", data_to_str(d))

@app.route('/turn', methods=['POST'])
def turn():
	p = request.form['pass']
	arr = p.split('/')
	g = arr[1]
	o = arr[0]
	print("p: {}, arr {}".format(p, arr))
	# g = request.form['move']
	# o = request.form['passed']
	d = str_to_data(o)
	last_string = "'{}' is not a valid move".format((g[0].upper()) + g[1])
	m = (((ord(g[0])&95)-65)*9)+(int(g[1]))
	if check_valid(d, m):
		usr_no = 1 - cpu_no(d)
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
			l = chr(65+(last_move//9)) + str(last_move%9)
			s = "The CPU won the game by playing {}".format(l)
			for i in range(9):
				d[2][i] = 0
			return render_template("end.html", board=to_string(d), input=s)
		elif over == 2:
			for i in range(9):
				d[2][i] = 0
			return render_template("end.html", board=to_string(d), input="The game ended in a tie")
		last_move = d[2][9]&0xFF
		l = chr(65+(last_move//9)) + str(last_move%9)
		last_string = "CPU played {}".format(l)
	board = to_string(d)
	return render_template_string(get_mobile_html(board, last_string, data_to_str(d)))
	#render_template("game.html", last=last_string, input=board, data=data_to_str(d))
	# return render_template("game.html", something=dddd) #last=last_string, input=board, data=data_to_str(d))


@app.route('/peepee/<a>/<b>')
def lmao(a,b):
	return a + "lll" + b

def get_mobile_html(board_str, last, data):
	thing = '''<!DOCTYPE html>
		<html>
		<head>
			<title>Ultimate Tic Tac Toe</title>
			<link href="https://fonts.googleapis.com/css?family=Source+Code+Pro:300&display=swap" rel="stylesheet">
			<link rel="stylesheet" href="/static/style.css">
			<script src="https://ajax.googleapis.com/ajax/libs/jquery/3.4.1/jquery.min.js"></script>
			<script>
	            $(document).ready(function() {{
	                $('#form').submit(function() {{
	                    $('#progress').show();
	                }});
	            }});
	        </script>
		</head>
		<body class="common">
		<a href="/">Home</a>	
		<br>
		<h1 class="common">{}</h1>
		<form id="form" action="/turn" method="post">
		{}
		</form>
		<div id="progress">
			CPU moving...
			<br>
			<div class="loader x"></div>
		</div>
		</body>
		</html>'''
	# new_data = 
	ddd = board_str.replace(' ', '&nbsp').replace('\n', '<br>')
	for i in range(9):
		for j in range(9):
			s = chr(i+65) + str(j)
			s = s.strip()
			ddd = ddd.replace(s, '<button type="submit" style="padding:0px;" class="common" name="pass" value="{}/{}">{}</button>'.format(data,s,s))
	return thing.format(last, ddd)

if __name__ == '__main__':
	app.run(debug=True)
