import sys
import time


def readline(f):
    s = f.readline()
    while s == "":
        time.sleep(0.0001)
        s = f.readline()
    return s


while True:
    f = open("input.txt", "r")
    x = float(readline(f))
    g = open("output.txt", "w")
    f.close()
    g.write(str(x ** 2) + "\n")
    g.close()
    sys.stdout.write("Processed " + repr(x) + ".\n")
