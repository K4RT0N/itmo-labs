import matplotlib.pyplot as plt
from math import pi, cos, sin
from dots import *

def get_color(x, y, levels):
    return (0, 0, min(max(0, 0.1*levels[x+100][y+100]), 1) * 0.75 + 0.25)

dots = get_dots()

plt.figure(figsize=(20, 20))
plt.xlim(-10, 10)
plt.ylim(-10, 10)

potentials = []
for x in range(-20, 21):
    for y in range(-20, 21):
        if x/2 not in [x.x for x in dots[1:]] and y/2 not in [x.y for x in dots[1:]]:
            potentials.append(get_potential(x/2, y/2, dots[1:]))
dif = abs(max(potentials) - min(potentials))
doze = dif / 10

levels = []
for x in range(-100, 101):
    levels.append([])
    for y in range(-100, 101):
        levels[-1].append((get_potential(x/10, y/10, dots[1:]) - min(potentials)) // doze)

for x in range(-100, 101):
    for y in range(-100, 101):
        plt.fill_between((x/10, x/10+0.1), (y/10, y/10), (y/10+0.1, y/10+0.1),
                         color=get_color(x, y, levels))

draw_trajectory(dots[0], dots[1:])

for dot in dots[1:]:
    draw_dot(dot)

plt.plot(dots[0].x, dots[0].y, marker='o', color='white', markersize=15, mec='black')
if dots[0].charge > 0:
    plt.plot(dots[0].x, dots[0].y, marker='+', color='black', markersize=8)
elif dots[0].charge < 0:
    plt.plot(dots[0].x, dots[0].y, marker='_', color='black', markersize=8)

plt.show()