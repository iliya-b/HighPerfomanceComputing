import numpy as np

n = 3
with open('B.txt', 'r') as f:
    B = eval(f.read())
with open('C.txt', 'r') as f:
    C = eval(f.read())
with open('x.txt', 'r') as f:
    x = eval(f.read())
with open('y.txt', 'r') as f:
    y = eval(f.read())
B = np.array(B)
C = np.array(C)
x = np.array(x)
y = np.array(y)
E = np.ones((n,n))
I = np.identity(n)
tr = np.trace(B@C@E)
A = C * tr + I + np.dot(B@E@x, y) / np.dot(x, y)
np.set_printoptions(precision=10)
print(A)