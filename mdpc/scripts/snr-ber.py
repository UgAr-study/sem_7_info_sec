import os
from concurrent.futures import process
import subprocess
import sys
import tempfile
from termios import CWERASE

# N0 = range(2, 6)
N0 = [2,3,4,5]
P = [128, 256, 512, 1024]
samples = 1000
path = "../../results/"
matPath = "../../matrix/"


for n0 in N0:
    for p in P:
        processes = []
        # w = int(32 / 640 * n0 * p)
        w = int((n0 * p)**0.49)
        for snr10 in range(50, 81):
            snr = snr10 / 10
            f = tempfile.NamedTemporaryFile()
            proc = subprocess.Popen(
                ["./helper-snr-ber.sh", 
                matPath + "private_"+str(n0)+str(p)+str(w)+str(".txt"),
                 str(samples), str(snr), os.path.abspath(f.name)])
            processes.append((proc, f))

        resFile = open(path + "res_" + str(n0)+str(p)+str(w)+str(".txt"), 'w')
        resFile.write("SNR\tErrors\tTotal\n")
        for proc, f in processes:
            proc.wait()
            f.seek(0)
            bytes = f.read()
            resFile.write(bytes.decode('utf-8'))
            resFile.flush()
            f.close()
        resFile.close()
        print(path + "res_" + str(n0)+str(p)+str(w)+str(".txt") + " Ready")
