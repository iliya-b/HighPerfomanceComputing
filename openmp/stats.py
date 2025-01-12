import pandas as pd
import matplotlib.pyplot as plt


def get_speedup(x):
    orig = df[(df.threads == 1) & (df.m_size == x.m_size)] #.groupby('size')
    return orig.iloc[0].time / x.time

df = pd.read_csv('./m1.csv', header=0)
df2 = pd.read_csv('./m2.csv', header=0)
df3 = pd.read_csv('./m3.csv', header=0)

df['time1'] = df.time
df['time2'] = df2.time
df['time3'] = df3.time
df['time'] = (df.time1 + df.time2 + df.time3) / 3

df['speedup'] = None

df['speedup'] = df.apply(lambda x: get_speedup(x), axis=1)

for i in (3000, 6000, 8000, 10000):
    plt.plot(df[df.m_size == i]['threads'], df[df.m_size == i]['speedup'], label = "Размер матрицы %d" % i)
plt.legend(loc="lower right")
df = df[df.m_size == 10000]

df.to_csv('result.csv')

plt.xlabel("Число потоков")
plt.ylabel("Ускорение")
plt.show()