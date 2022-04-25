from mpl_toolkits import mplot3d
import numpy as np
import matplotlib.pyplot as plt

axes = np.loadtxt("../lab1/axes.txt", delimiter='\n', dtype=float)
x_max = axes[0]
t_max = axes[1]
M = int(axes[2])
K = int(axes[3])


tau = t_max / (K-1)
h = x_max / (M-1)

X = np.arange(0, x_max + h / 2, h)
Y = np.arange(0, t_max + tau / 2, tau)
X, Y = np.meshgrid(X, Y)

# matrix = np.loadtxt("../lab1/solution.txt", delimiter='\n', dtype=float)

matrix = []
with open('../lab1/solution.txt', 'r') as f:
    for line in f:
        matrix.append(line.split())

# turn string into float
for i in range(M-1):
    for j in range(K-1):
        matrix[i][j] = float(matrix[i][j])
print(matrix)
u = np.array(matrix, float)
print(u)
print(X)
print(Y)


# create 3d axes
fig = plt.figure()
ax = plt.axes(projection='3d')
ax.plot_surface(X, Y, u, cmap='viridis', linewidths=0.2)
ax.view_init(10, -100)
ax.set_xlabel('x', fontsize=15)
ax.set_ylabel('t', fontsize=15)
ax.set_zlabel('u', fontsize=15)
plt.show()
