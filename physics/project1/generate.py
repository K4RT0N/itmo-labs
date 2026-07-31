from random import randint
from sys import argv

f = open("dots.txt", "w")

for i in range(int(argv[1])):
    f.write(str(randint(-10, 10)) + " " + str(randint(-10, 10)) + " " + str(randint(-100, 100)) + "\n")