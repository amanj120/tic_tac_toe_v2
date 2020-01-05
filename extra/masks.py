def bin(i):
	print("{0:08b}".format(i))

def bin2(i):
	return("{0:09b}".format(i))

def find_set_bits(v):
	a = (v&0x55) + ((v>>1)&0x55) #;// v&101010101 + (v&010101010>>1); contains how many bits in each pair
	b = (a&0x33) + ((a>>2)&0x33)#;// a&100110011 + (a&011001100>>2); contains hor many bits in each four
	c = (b&0xF) + ((b>>4)&0xF)#kinda pointless here
	d = (c + (v>>8))
	return d

def find_set_bits_easy(v):
	return bin2(v).count('1')

def find_nth_bit_easy(v,m):
	s = bin2(v)[::-1] #we have to scan from right to left
	m+=1
	# print(s)
	for i in range(9):
		if(s[i] == '1'):
			m -= 1
			if m==0:
				return i

def find_nth_bit(v,m):
	#v = 0b110100110			# contains the bits where the are
	# bin(v&0xFF)
	# bin(v&0x55)
	# bin((v>>1)&0x55)
	# bin((v&0x55) + ((v>>1)&0x55))
	a = (v&0x55) + ((v>>1)&0x55) #;// v&101010101 + (v&010101010>>1); contains how many bits in each pair
	b = (a&0x33) + ((a>>2)&0x33)#;// a&100110011 + (a&011001100>>2); contains hor many bits in each four
	c = (b&0xF) + ((b>>4)&0xF)#kinda pointless here
	d = (c + (v>>8))
	# print(d)
	# bin(a)
	# bin(b)
	# bin(c)
	j = 8;
	t = 0;
	i = 0;
	
	#//c: n_b in left half concat n_b in right half
	if(v>>8 and (m==(d-1))): #//if the 9th bit (index 8) is set and m == 0 then return index 8
		return (i*9) + j#;//(i*9) + 8
	else:
		j = 0;
		t = b & 0xF
		# print("t1 =", t)
		if(m < t):
			m -= (0*t);
			j = (0*4);
			t = (a>>j)&3;
			# print("t2 =", t)
			if(m < t):
				m -= (0*t)
				j += (0*2);
				t = (v>>j) & 1;
				# print("t3 =", t)
				return (j + (0 if(m<t) else 1));#//0,1

			else:
				m -= (1*t)
				j += (1*2);
				t = (v>>j) & 1;
				# print("t3 =", t)
				return (j + (0 if(m<t) else 1));#//2,3
		else:
			m -= (1*t);
			j = (1*4);
			t = (a>>j)&3;
			# print("t2 =", t)
			if(m < t):
				m -= (0*t)
				j += (0*2);
				t = (v>>j) & 1;
				# print("t3 =", t)
				return (j + (0 if(m<t) else 1));#//4,5
			else:
				m -= (1*t)
				j += (1*2);
				t = (v>>j) & 1;
				# print("t3 =", t)
				return (j + (0 if(m<t) else 1));#//6,7

# f = find_nth_bit(0b110100110,1)
# print(f)
# for i in range(5):
# 	print("the {} bit in {} is {}\n".format(i, bin2(0b110100110),find_nth_bit(0b110100110,i)));

# print(find_nth_bit_easy(256,0))

for i in range(512):
	a = find_set_bits_easy(i)
	b = find_set_bits(i)
	if not(a==b):
		print("set bits not working for {}, easy gives us {}, hard gives us{}".format(i,a,b))
	else:
		for j in range(a):
			c = find_nth_bit_easy(i,j)
			d = find_nth_bit(i,j)
			if not(c==d):
				print("find nth bit not working for {}, {}: easy gives us {}, hard gives us {}".format(i,j,c,d))
