import matplotlib.pyplot as plt
from math import sqrt

class Dot:
    def __init__(self, x, y, charge):
        self.x = x
        self.y = y
        self.charge = charge

def get_dots():
    f = open("dots.txt", "r")
    dots = []
    for i in f.readlines():
        data = [float(x) for x in i.split()]
        dots.append(Dot(data[0], data[1], data[2]))
    return dots

def draw_dot(dot):
    if dot.charge > 0:
        plt.plot(dot.x, dot.y, marker='o', color='red', markersize=15, mec='black')
        plt.plot(dot.x, dot.y, marker='+', color='black', markersize=8)
    elif dot.charge < 0:
        plt.plot(dot.x, dot.y, marker='o', color='blue', markersize=15, mec='black')
        plt.plot(dot.x, dot.y, marker='_', color='black', markersize=8)

def draw_line(x, y, dots):
    def draw_onedir_line(x, y, dots, direction):
        counter = 0
        while x >= -10 and x <= 10 and y >= -10 and y <= 10 and counter < 1000:
            fx = 0
            fy = 0
            for dot in dots:
                if (x - dot.x)**2 + (y - dot.y)**2 == 0:
                    continue
                f = direction * dot.charge / ((x - dot.x)**2 + (y - dot.y)**2)
                fx += f * (dot.x - x) / sqrt((dot.x - x)**2 + (dot.y - y)**2)
                fy += f * (dot.y - y) / sqrt((dot.x - x)**2 + (dot.y - y)**2)
            f = sqrt(fx**2 + fy**2)
            if f == 0:
                break
            dx = fx * 0.1 / f
            dy = fy * 0.1 / f
            plt.plot([x, x+dx], [y, y+dy], color='black')
            x += dx
            y += dy
            counter += 1
    draw_onedir_line(x, y, dots, 1)
    draw_onedir_line(x, y, dots, -1)

def get_potential(x, y, dots):
    potential = 0
    for dot in dots:
        potential += dot.charge / sqrt((x - dot.x)**2 + (y - dot.y)**2 + 1e-10)
    return potential