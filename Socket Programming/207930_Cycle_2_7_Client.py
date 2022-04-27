import sys
import time


def readline(f):
    s = f.readline()
    while s == "":
        time.sleep(0.0001)
        s = f.readline()
    return s


def req(x):
    f = open("input.txt", "w")
    f.write(str(x) + "\n")
    f.flush()
    g = open("output.txt", "r")
    result = float(readline(g))
    g.close()
    f.close()
    return result


for i in range(100000):
    sys.stdout.write("%i, %s\n" % (i, i * i == req(i)))
