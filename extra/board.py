b = "A0 A1 A2 | B0 B1 B2 | C0 C1 C2\nA3 A4 A5 | B3 B4 B5 | C3 C4 C5\nA6 A7 A8 | B6 B7 B8 | C6 C7 C8\n---------|--------------------\nD0 D1 D2 | E0 E1 E2 | F0 F1 F2\nD3 D4 D5 | E3 E4 E5 | F3 F4 F5\nD6 D7 D8 | E6 E7 E8 | F6 F7 F8\n--------------------|---------\nG0 G1 G2 | H0 H1 H2 | I0 I1 I2\nG3 G4 G5 | H3 H4 H5 | I3 I4 I5\nG6 G7 G8 | H6 H7 H8 | I6 I7 I8\n\nA B C\nD E F\nG H I\n"
c = '* * * | * * * | * * *\n* * * | * * * | * * *\n* * * | * * * | * * *\n------|--------------\n* * * | * * * | * * *\n* * * | * * * | * * *\n* * * | * * * | * * *\n--------------|------\n* * * | * * * | * * *\n* * * | * * * | * * *\n* * * | * * * | * * *\n* * *\n* * *\n* * *'
first = ['A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I']
last = ['0', '1', '2', '3', '4', '5', '6', '7', '8']

print(c)
print(b)

c_idx = []
for i in range(len(c)):
	if(c[i] == '*'):
		c_idx.append(i)

print(c_idx)

print('{', end='');

for i in range(81):
	s = first[i//9] + last[i%9]
	print(b.find(s), end = ',')

print(b.find('A '), end = ',')
print(b.find('B '), end = ',')
print(b.find('B ')+2, end = ',')
print(b.find('D '), end = ',')
print(b.find('E '), end = ',')
print(b.find('E ')+2, end = ',')
print(b.find('G '), end = ',')
print(b.find('H '), end = ',')
print(b.find('H ')+2, end = '}')
print();
print(len(b)) #407
#"{0:b}".format(330).rjust(9,'0')

'''
XOO
-XX
XXO
'''