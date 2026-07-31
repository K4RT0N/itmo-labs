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

def draw_trajectory(main_dot: Dot, dots):
    x, y = main_dot.x, main_dot.y
    vx = 0
    vy = 0
    
    t_max = 100
    dt = 0.001
    steps = int(t_max / dt)
    
    xs = [x]
    ys = [y]
    
    for _ in range(steps):
        fx, fy = 0, 0
        
        for dot in dots:
            dx = dot.x - x
            dy = dot.y - y
            r_sq = dx*dx + dy*dy
            
            if r_sq < 1e-8:
                continue
            
            r = sqrt(r_sq)
            force = -main_dot.charge * dot.charge / r_sq
            
            fx += force * dx / r
            fy += force * dy / r
        
        ax = fx
        ay = fy
        
        vx += ax * dt
        vy += ay * dt
        
        x += vx * dt
        y += vy * dt
        
        xs.append(x)
        ys.append(y)
        
        if x*x + y*y > 10000:
            break
        ret = False
        for dot in dots:
            if sqrt((x - dot.x)**2 + (y - dot.y)**2) < 0.2:
                ret = True
        if ret:
            break
    
    plt.plot(xs, ys, color='black')