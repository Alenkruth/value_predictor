#!/usr/bin/env/ python
"""
    Python snippet to calculate the mean IPC obtained
    on workloads (traces) run with the value predictor
    Part of the scripts built for the CVP in CS6345 - UVA CS
    
    author(s) = Khyati Kiyawat, Alenkruth Krishnan Murali
    email = 
"""

# for directory and file path resolution 
import os

# for getting an argument from the command line
import sys

def averageIPC(dirname):
    ipc_list = []
    curr_dir = os.path.abspath(os.getcwd())
    grep_path = os.path.join(curr_dir, "logs/{0}/grepIPC.txt".format(dirname))
    with open(grep_path) as grepfile:
        for line in grepfile:
            ipc_list.append(float(line.split("= ")[1]))
    average_IPC = sum(ipc_list)/len(ipc_list)
    return average_IPC

run_dirname = sys.argv[1]
mean_IPC = averageIPC(run_dirname)
print(f"The average IPC is {mean_IPC}")
