import matplotlib.pyplot as plt
from math import pi, cos, sin
from dots import *

def get_color(x, y, levels):
    return (0, 0, min(max(0, 0.1*levels[x+100][y+100]), 1) * 0.75 + 0.25)

dots = get_dots()

plt.figure(figsize=(10, 10))
plt.xlim(-10, 10)
plt.ylim(-10, 10)

potentials = []
for x in range(-20, 21):
    for y in range(-20, 21):
        if x/2 not in [x.x for x in dots] and y/2 not in [x.y for x in dots]:
            potentials.append(get_potential(x/2, y/2, dots))
dif = abs(max(potentials) - min(potentials))
doze = dif / 10

levels = []
for x in range(-100, 101):
    levels.append([])
    for y in range(-100, 101):
        levels[-1].append((get_potential(x/10, y/10, dots) - min(potentials)) // doze)

for x in range(-100, 101):
    for y in range(-100, 101):
        plt.fill_between((x/10, x/10+0.1), (y/10, y/10), (y/10+0.1, y/10+0.1),
                         color=get_color(x, y, levels))

for dot in dots:
    for i in range(1, 17):
        draw_line(dot.x + 0.1*cos(pi/6*i), dot.y + 0.1*sin(pi/6*i), dots)

#for i in range(-10, 11, 2):
#    for j in range(-10, 11, 2):
#        draw_line(i, j, dots)

for dot in dots:
    draw_dot(dot)

plt.show()