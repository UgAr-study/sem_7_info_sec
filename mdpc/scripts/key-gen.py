import os
from concurrent.futures import process
import subprocess
import sys
import tempfile
from termios import CWERASE
import numpy as np

N0 = [3, 4]
P = [128, 256, 512, 1024]
path = "../../matrix/"

for n0 in N0:
    processes = []
    for p in P:
        w = int((n0 * p)**0.49)
        proc = subprocess.Popen(
            ["./helper-key-gen.sh",
             str(n0), str(p), str(w), str(n0*p+w), 
             path + "private_"+str(n0)+str(p)+str(w)+str(".txt"),
             path + "public_"+str(n0)+str(p)+str(w)+str(".txt")])
        processes.append(proc)
    for proc in processes:
        proc.wait()
