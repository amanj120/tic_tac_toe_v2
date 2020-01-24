from flask import Flask, render_template, request
from ctypes import *
from os import path

so = path.abspath("game_lib/tic.so")
func = CDLL(so)
app = Flask(__name__)#, template_folder=frontend, static_folder=frontend)

data = (c_short*10)*3 	#this is a type, all instances of data will be of this type
base_32 = ['z','y','x','w','v','u','t','s','r','q','p','o','n','m','l','k','j','i','h','g','f','e','d','c','b','a','8','7','5','3','2','1']
base_str = '*,*,*,|,*,*,*,|,*,*,*\n*,*,*,|,*,*,*,|,*,*,*\n*,*,*,|,*,*,*,|,*,*,*\n------|--------------\n*,*,*,|,*,*,*,|,*,*,*\n*,*,*,|,*,*,*,|,*,*,*\n*,*,*,|,*,*,*,|,*,*,*\n--------------|------\n*,*,*,|,*,*,*,|,*,*,*\n*,*,*,|,*,*,*,|,*,*,*\n*,*,*,|,*,*,*,|,*,*,*\n\n*,*,*\n*,*,*\n*,*,*\n'
base_pos = [0, 2, 4, 22, 24, 26, 44, 46, 48, 8, 10, 12, 30, 32, 34, 52, 54, 56, 16, 18, 20, 38, 40, 42, 60, 62, 64, 88, 90, 92, 110, 112, 114, 132, 134, 136, 96, 98, 100, 118, 120, 122, 140, 142, 144, 104, 106, 108, 126, 128, 130, 148, 150, 152, 176, 178, 180, 198, 200, 202, 220, 222, 224, 184, 186, 188, 206, 208, 210, 228, 230, 232, 192, 194, 196, 214, 216, 218, 236, 238, 240, 243, 245, 247, 249, 251, 253, 255, 257, 259]
order = [0,1,2,9,10,11,18,19,20,3,4,5,12,13,14,21,22,23,6,7,8,15,16,17,24,25,26,27,28,29,36,37,38,45,46,47,30,31,32,39,40,41,48,49,50,33,34,35,42,43,44,51,52,53,54,55,56,63,64,65,72,73,74,57,58,59,66,67,68,75,76,77,60,61,62,69,70,71,78,79,80,81,82,83,84,85,86,87,88,89]

def other_str(d):
	temp = base_str
	data = data_to_str(d)
	last_move = d[2][9] & 0xFF;
	# print("last move: {}".format(last_move))

	for i in range(8,-1,-1):
		idx = 81 + i;
		pos = base_pos[idx]
		token_type = 2 #CPU, User, Possible move, nothing
		tokens = ['X', 'O', '*']
		for k in range(2):
			if(d[k][9] & 1 << i != 0):
				token_type = k
		temp = temp[:pos] + tokens[token_type] + temp[pos+1:]

	for o_idx in range(80,-1,-1):
		idx = order[o_idx]
		i = idx//9
		j = idx%9
		pos = base_pos[idx]
		type_arr = [0,0,0,1] #CPU, User, Possible move, nothing
		for k in range(3):
			if(d[k][i] & 1 << j != 0):
				type_arr[k] = 1;
		if(type_arr[0] == 1):
			if(idx == last_move):
				temp = temp[:pos] + '<font color="red">X</font>' + temp[pos+1:]
			else:
				temp = temp[:pos] + 'X' + temp[pos+1:]
		elif(type_arr[1] == 1):
			if(idx == last_move):
				temp = temp[:pos] + '<font color="red">O</font>' + temp[pos+1:]
			else:
				temp = temp[:pos] + 'O' + temp[pos+1:]
		elif(type_arr[2] == 1):
			temp = temp[:pos] + '<a class="tempclass" href="/turn/{}/{}">{}</a>'.format(data, idx, '*') + temp[pos+1:]
		else:
			temp = temp[:pos] + '&nbsp' + temp[pos+1:]
		# print(temp)
	temp = temp.replace(',', '&nbsp').replace('\n', '<br>')
	return temp

def to_32(i):
	return base_32[(i & 0x3e0)>>5] + base_32[i & 0x1f]

def to_int(b_32):
	return 32*(base_32.index(b_32[0])) + (base_32.index(b_32[1]))

def data_to_str(d):
	s = ""
	for i in range(29):
		s += to_32(d[i//10][i%10])
	s += str(d[2][9])
	return s

def str_to_data(s):
	d = data()
	for i in range(29):
		d[i//10][i%10] = to_int(s[(i*2):((i+1)*2)])
	d[2][9] = int(s[58:])
	return d;

def cpu_no(d):
	return ((d[2][9] & 0x8000) >> 15)

def register_usr_move(d, m):
	usr_no = (1 - cpu_no(d))
	func.register_move(byref(d),usr_no,m)

def register_cpu_move(d):
	m = func.cpu_move(byref(d))
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

def clear(d):
	for i in range(9):
		d[2][i] = 0

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
	if(play[3] == 'f'):
		p = 1
	func.set_metadata(byref(d),p,0,int(diff)) 
	initialize(d)
	return render_template("game.html", board=other_str(d)) 

@app.route('/turn/<o>/<p>')
def turn(o,p):
	d = str_to_data(o)
	m = int(p)
	
	register_usr_move(d,m)
	over = cpu_won_game(d)
	if over == 0:
		clear(d)
		return render_template("end.html", board=other_str(d), input="You won the game")
	elif over == 2:
		clear(d)
		return render_template("end.html", board=other_str(d), input="The game ended in a tie")
	
	register_cpu_move(d)
	over = cpu_won_game(d)
	if over == 1:
		clear(d)
		return render_template("end.html", board=other_str(d), input="The CPU won")
	elif over == 2:
		clear(d)
		return render_template("end.html", board=other_str(d), input="The game ended in a tie")
	
	return render_template("game.html", board=other_str(d))

if __name__ == '__main__':
	app.run(debug=True)
