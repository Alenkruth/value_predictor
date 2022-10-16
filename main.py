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
    return_list = []
    # calculating average IPC across workloads
    ipc_list = []
    curr_dir = os.path.abspath(os.getcwd())
    grep_ipc_path = os.path.join(curr_dir, "logs/{0}/grepIPC.txt".format(dirname))
    with open(grep_ipc_path) as grepfile:
        for line in grepfile:
            ipc_list.append(float(line.split("= ")[1]))
    average_IPC = sum(ipc_list)/len(ipc_list)
    # calculating size of the predictor in KB
    grep_size_path = os.path.join(curr_dir, "logs/{0}/grepSize.txt".format(dirname))
    with open(grep_size_path) as grepfile:
        for line in grepfile:
            predictor_size_string = (line.split(" ||| ")[1]).split(": ")[1]
            predictor_size_bits = int(predictor_size_string.split(" bits")[0])
    predictor_size_bytes = (predictor_size_bits/8)
    predictor_size_kbytes = (predictor_size_bytes/1024)
    
    # return values
    return_list.append(average_IPC)
    return_list.append(predictor_size_kbytes)
    return return_list

run_dirname = sys.argv[1]
returns = averageIPC(run_dirname)
print(f"The average IPC is {returns[0]}")
print(f"The predictor size is {returns[1]} kB")
