# functions to extract the IPC value from grep logs
import os

def averageIPC():
    vals = []
    curr_dir = os.path.abspath(os.getcwd())
    print(curr_dir)
    with open(os.path.join(curr_dir,"grepIPC.txt")) as grepfile:
        for line in grepfile:
            vals.append(float(line.split("= ")[1]))
            #print(vals)
            #print(type(vals[0])
    sumlist = sum(vals)
    ave = sumlist/10
    return ave

#sum_list = sum(vals)
#ave = sum_list/10

#print(ave)

val = averageIPC()
print(val)
