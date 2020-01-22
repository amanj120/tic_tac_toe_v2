from flask import Flask, render_template, request, render_template_string
from ctypes import *
from os import path

so = path.abspath("game_lib/tic.so")
frontend = path.abspath("frontend")
func = CDLL(so)
app = Flask(__name__)#, template_folder=frontend, static_folder=frontend)

data = (c_short*10)*3 	#this is a type, all instances of data will be of this type
base = c_char*361 	#the type of the returned string
base_32 = ['z','y','x','w','v','u','t','s','r','q','p','o','n','m','l','k','j','i','h','g','f','e','d','c','b','a','8','7','5','3','2','1']
base_str = '*,*,*,|,*,*,*,|,*,*,*\n*,*,*,|,*,*,*,|,*,*,*\n*,*,*,|,*,*,*,|,*,*,*\n------|--------------\n*,*,*,|,*,*,*,|,*,*,*\n*,*,*,|,*,*,*,|,*,*,*\n*,*,*,|,*,*,*,|,*,*,*\n--------------|------\n*,*,*,|,*,*,*,|,*,*,*\n*,*,*,|,*,*,*,|,*,*,*\n*,*,*,|,*,*,*,|,*,*,*\n\n*,*,*\n*,*,*\n*,*,*\n'
# base_pos = [
# 0, 2, 4, 
# 22, 24, 26, 
# 44, 46, 48, 

# 8, 10, 12, 
# 30, 32, 34, 
# 52, 54, 56, 

# 16, 18, 20, 
# 38, 40, 42, 
# 60, 62, 64, 

# 88, 90, 92, 
# 110, 112, 114, 
# 132, 134, 136, 

# 96, 98, 100, 
# 118, 120, 122, 
# 140, 142, 144, 

# 104, 106, 108, 
# 126, 128, 130, 
# 148, 150, 152, 

# 176, 178, 180, 
# 198, 200, 202, 
# 220, 222, 224, 

# 184, 186, 188, 
# 206, 208, 210, 
# 228, 230, 232, 

# 192, 194, 196, 
# 214, 216, 218, 
# 236, 238, 240, 

# 242, 244, 246, 
# 248, 250, 252, 
# 254, 256, 258]

# order = [
# 0,1,2,
# 9,10,11,
# 18,19,20,

# 3,4,5,
# 12,13,14,
# 21,22,23,

# 6,7,8,
# 15,16,17,
# 24,25,26,

# 27,28,29,
# 36,37,38,
# 45,46,47,

# 30,31,32,
# 39,40,41,
# 48,49,50,

# 33,34,35,
# 42,43,44,
# 51,52,53,

# 54,55,56,
# 63,64,65,
# 72,73,74,

# 57,58,59,
# 66,67,68,
# 75,76,77,

# 60,61,62,
# 69,70,71,
# 78,79,80
# ]

base_pos = [0, 2, 4, 22, 24, 26, 44, 46, 48, 8, 10, 12, 30, 32, 34, 52, 54, 56, 16, 18, 20, 38, 40, 42, 60, 62, 64, 88, 90, 92, 110, 112, 114, 132, 134, 136, 96, 98, 100, 118, 120, 122, 140, 142, 144, 104, 106, 108, 126, 128, 130, 148, 150, 152, 176, 178, 180, 198, 200, 202, 220, 222, 224, 184, 186, 188, 206, 208, 210, 228, 230, 232, 192, 194, 196, 214, 216, 218, 236, 238, 240, 243, 245, 247, 249, 251, 253, 255, 257, 259]
order = [0,1,2,9,10,11,18,19,20,3,4,5,12,13,14,21,22,23,6,7,8,15,16,17,24,25,26,27,28,29,36,37,38,45,46,47,30,31,32,39,40,41,48,49,50,33,34,35,42,43,44,51,52,53,54,55,56,63,64,65,72,73,74,57,58,59,66,67,68,75,76,77,60,61,62,69,70,71,78,79,80]

def other_str_end(d):
	temp = base_str
	data = data_to_str(d)
	last_move = d[2][9] & 0xFF;
	print("last move: {}".format(last_move))

	for i in range(8,-1,-1):
		idx = 81 + i;
		pos = base_pos[idx]
		type_arr = [0,0,1] #CPU, User, Possible move, nothing
		for k in range(2):
			if(d[k][9] & 1 << i != 0):
				type_arr[k] = 1;
		if(type_arr[0] == 1):
			temp = temp[:pos] + 'X' + temp[pos+1:]
		elif(type_arr[1] == 1):
			temp = temp[:pos] + 'O' + temp[pos+1:]
		else:
			temp = temp[:pos] + '*' + temp[pos+1:]
		# print(temp)

	for o_idx in range(80,-1,-1):
		idx = order[o_idx]
		i = idx/9
		j = idx%9
		pos = base_pos[idx]
		type_arr = [0,0,0,1] #CPU, User, Possible move, nothing
		for k in range(3):
			if(d[k][i] & 1 << j != 0):
				type_arr[k] = 1;
		if(type_arr[0] == 1):
			temp = temp[:pos] + 'X' + temp[pos+1:]
		elif(type_arr[1] == 1):
			temp = temp[:pos] + 'O' + temp[pos+1:]
		else:
			temp = temp[:pos] + ' '+ temp[pos+1:]
		# print(temp)
	temp = temp.replace(',', ' ')
	return temp

def other_str(d):
	temp = base_str
	data = data_to_str(d)
	last_move = d[2][9] & 0xFF;
	print("last move: {}".format(last_move))

	for i in range(8,-1,-1):
		idx = 81 + i;
		pos = base_pos[idx]
		type_arr = [0,0,1] #CPU, User, Possible move, nothing
		for k in range(2):
			if(d[k][9] & 1 << i != 0):
				type_arr[k] = 1;
		if(type_arr[0] == 1):
			temp = temp[:pos] + 'X' + temp[pos+1:]
		elif(type_arr[1] == 1):
			temp = temp[:pos] + 'O' + temp[pos+1:]
		else:
			temp = temp[:pos] + '*' + temp[pos+1:]
		# print(temp)

	for o_idx in range(80,-1,-1):
		idx = order[o_idx]
		i = idx/9
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
	print(i)
	return base_32[(i & 0x3e0)>>5] + base_32[i & 0x1f]

def to_int(b_32):
	return 32*(base_32.index(b_32[0])) + (base_32.index(b_32[1]))

def data_to_str(d):
	print(d)
	s = ""
	for i in range(29):
		# print(ord(d[i//10][i%10]))
		s += to_32(d[i//10][i%10])
	s += str(d[2][9])
	return s

def str_to_data(s):
	d = data()
	for i in range(29):
		d[i//10][i%10] = to_int(s[(i*2):((i+1)*2)])
	d[2][9] = int(s[58:])
	return d;

def to_string(d):
	b = base() 
	func.to_string(byref(d),byref(b))
	return b.value.decode('ascii')

#def to_dif_string(d):


def cpu_no(d):
	return ((d[2][9] & 0x8000) >> 15)

def check_valid(d, m):
	return func.check_valid(d,m)

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
	board = other_str(d)
	if(((d[2][9] & 0x8000) >> 15) == 0):
		#return render_template("game.html", last="CPU Played E4", input=board, data=data_to_str(d))
		return get_mobile_html(d)
	else:
		#return render_template("game.html", last="It is your turn to play", input=board, data=data_to_str(d))
		return get_mobile_html(d)

@app.route('/turn/<o>/<p>')
def turn(o,p):
	# p = request.form['pass']
	# arr = p.split('/')
	# g = arr[1]
	# o = arr[0]
	# print("p: {}, arr {}".format(p, arr))
	# g = request.form['move']
	# o = request.form['passed']
	d = str_to_data(o)
	last_string = "" #"'{}' is not a valid move".format((g[0].upper()) + g[1])
	# m = (((ord(g[0])&95)-65)*9)+(int(g[1]))
	m = int(p)
	if check_valid(d, m):
		usr_no = 1 - cpu_no(d)
		register_usr_move(d,m)
		over = cpu_won_game(d)
		if over == 0:
			for i in range(9):
				d[2][i] = 0
			return render_template("end.html", board=other_str_end(d), input="You won the game")
		elif over == 2:
			for i in range(9):
				d[2][i] = 0
			return render_template("end.html", board=other_str_end(d), input="The game ended in a tie")
		register_cpu_move(d)
		over = cpu_won_game(d)
		if over == 1:
			last_move = d[2][9]&0xFF
			l = chr(65+(last_move//9)) + str(last_move%9)
			s = "The CPU won"
			for i in range(9):
				d[2][i] = 0
			return render_template("end.html", board=other_str_end(d), input=s)
		elif over == 2:
			for i in range(9):
				d[2][i] = 0
			return render_template("end.html", board=other_str_end(d), input="The game ended in a tie")
		last_move = d[2][9]&0xFF
		l = chr(65+(last_move//9)) + str(last_move%9)
		last_string = "CPU played {}".format(l)
	board = to_string(d)
	return render_template_string(get_mobile_html(d))
	#render_template("game.html", last=last_string, input=board, data=data_to_str(d))
	# return render_template("game.html", something=dddd) #last=last_string, input=board, data=data_to_str(d))

def get_mobile_html(d):
	thing = '''<!DOCTYPE html>
		<html>
		<head>
			<title>Ultimate Tic Tac Toe</title>
			<link rel="stylesheet" href="/static/style.css">
			<script src="https://ajax.googleapis.com/ajax/libs/jquery/3.4.1/jquery.min.js"></script>
			<script>
	            $(document).ready(function() {{
	                $('a[class="tempclass"]').click(function() {{
	                    $('#progress').show();
	                }});
	            }});
	        </script>
		</head>
		<body class="common">
		<a href="/">Home</a>	
		<br>
		<br>
		{}
		<div id="progress">
			CPU moving
			<br>
			<div class="loader a"></div>
			<div class="loader b"></div>
			<div class="loader c"></div>
		</div>
		</body>
		</html>'''
	# new_data = 

	ddd = other_str(d) #.replace('<>', 'O').replace('><','X')
	# for i in range(9):
	# 	for j in range(9):
	# 		s = chr(i+65) + str(j)
	# 		s = s.strip()
	# 		num = str(i*9 + j);
	# 		#do this if you want to do a form (with post) instead of a hyperlink (with get)
	# 		#ddd = ddd.replace(s, '<button type="submit" style="padding:0px;" class="common" name="pass" value="{}/{}">{}</button>'.format(data,s,s))
	# 		ddd = ddd.replace(s, '<a class="temp_class" href="/turn/{}/{}">{}</a>'.format(data, num, s))
	return thing.format(ddd)

if __name__ == '__main__':
	app.run(debug=True)
