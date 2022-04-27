import matplotlib.pyplot as plt
time = []
cwnd = []
pckt_drops = []
with open("output.txt", "r") as f:
    for line in f.readlines():
        if (line.startswith('RxDrop')):
            _, t = line.strip().split('RxDrop at ')
            pckt_drops.append(float(t))
        else:
            t, c = line.strip().split('\t')
            time.append(float(t))
            cwnd.append(int(c))
plt.plot(time, cwnd)
plt.xlabel('Time')
plt.ylabel('cwnd')
plt.show()