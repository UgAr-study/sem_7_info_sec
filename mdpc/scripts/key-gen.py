import os
from concurrent.futures import process
import subprocess
import sys
import tempfile
from termios import CWERASE

N0 = range(2, 6)
P = [128, 256, 512, 1024]
path = "../../matrix/"

for n0 in N0:
    processes = []
    for p in P:
        w = int(32 / 640 * n0 * p)
        proc = subprocess.Popen(
            ["./helper-key-gen.sh",
             str(n0), str(p), str(p), str(n0*p+w), 
             path + "private_"+str(n0)+str(p)+str(w)+str(".txt"),
             path + "public_"+str(n0)+str(p)+str(w)+str(".txt")])
        processes.append(proc)
    for proc in processes:
        proc.wait()
