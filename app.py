from flask import Flask
from flask import render_template, request
import os
from ctypes import *

so = os.path.abspath("fib.so")
func = CDLL(so)
temp = os.path.abspath('game.html')
app = Flask(__name__, template_folder='/')


@app.route('/')
def home():
	#return "hello"
	return render_template(temp, input='hello<><>\nworld')

@app.route('/fib', methods=['POST'])
def fib():
	ii = request.form['integer']
	i = int(ii)
	s = "the {}th fibonacci number is \n{}".format(i, func.fib(i))
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