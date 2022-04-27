import matplotlib.pyplot as plt
time = []
cwnd = []
with open("output.txt", "r") as f:
    for line in f.readlines():
        t, c = line.strip().split('\t')
        time.append(float(t))
        cwnd.append(int(c))
plt.plot(time, cwnd)
plt.xlabel('Time')
plt.ylabel('cwnd')
plt.show()