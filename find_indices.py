b = "A0 A1 A2 | B0 B1 B2 | C0 C1 C2\nA3 A4 A5 | B3 B4 B5 | C3 C4 C5\nA6 A7 A8 | B6 B7 B8 | C6 C7 C8\n---------+----------+---------\nD0 D1 D2 | E0 E1 E2 | F0 F1 F2\nD3 D4 D5 | E3 E4 E5 | F3 F4 F5\nD6 D7 D8 | E6 E7 E8 | F6 F7 F8\n---------+----------+---------\nG0 G1 G2 | H0 H1 H2 | I0 I1 I2\nG3 G4 G5 | H3 H4 H5 | I3 I4 I5\nG6 G7 G8 | H6 H7 H8 | I6 I7 I8\n"
c = "A0 A1 A2 | B0 B1 B2 | C0 C1 C2\nA3 A4 A5 | B3 B4 B5 | C3 C4 C5\nA6 A7 A8 | B6 B7 B8 | C6 C7 C8\n---------+----------+---------\nD0 D1 D2 | E0 E1 E2 | F0 F1 F2\t\tA B C\nD3 D4 D5 | E3 E4 E5 | F3 F4 F5\t\tD E F\nD6 D7 D8 | E6 E7 E8 | F6 F7 F8\t\tG H I\n---------+----------+---------\nG0 G1 G2 | H0 H1 H2 | I0 I1 I2\nG3 G4 G5 | H3 H4 H5 | I3 I4 I5\nG6 G7 G8 | H6 H7 H8 | I6 I7 I8\n";

first = ['A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I']
last = ['0', '1', '2', '3', '4', '5', '6', '7', '8']


for i in range(81):
	s = first[i//9] + last[i%9]
	print(c.find(s), end = ',')

print(c.find('A '))
print(c.find('B '))
print(c.find('D '))
print(c.find('E '))
print(c.find('G '))
print(c.find('H '))

'''
{0,3,6,31,34,37,62,65,68,11,14,17,42,45,48,73,76,79,22,25,28,53,56,59,84,87,90,124,127,130,162,165,168,200,203,206,135,138,141,173,176,179,211,214,217,146,149,152,184,187,190,222,225,228,269,272,275,300,303,306,331,334,337,280,283,286,311,314,317,342,345,348,291,294,297,322,325,328,353,356,359}
{0,3,6,31,34,37,62,65,68,11,14,17,42,45,48,73,76,79,22,25,28,53,56,59,84,87,90,124,127,130,155,158,161,186,189,192,135,138,141,166,169,172,197,200,203,146,149,152,177,180,183,208,211,214,248,251,254,279,282,285,310,313,316,259,262,265,290,293,296,321,324,327,270,273,276,301,304,307,332,335,338}
'''
#"{0:b}".format(330).rjust(9,'0')

'''
XOO
-XX
XXO
'''