#!/bin/bash
#file run.sh

# script to automate the running of all the traces and obtain the mean IPC.
# built as a part of the CVP in CS6354 - UVA CS
# author(s) - Khyati Kiyawat, Alenkruth Krishnan Murali
# email - 

# create a variable to keep track of the time stamp
timestamp=$(date "+%Y-%m-%d_%H-%M-%S")

if [ ! -d ./logs];
then 
    mkdir logs
    echo "created logs directory"
fi

cd ./logs/
mkdir $timestamp
cd ../

echo "Cleaning the existing binaries and rebuilding"

# run make
make clean &> ./logs/log_make_clean.txt
make 2>&1 | tee ./logs/log_make_build.txt

echo "done making"

echo "running the traces with the updated predictor"

logdir="./logs/$timestamp"

# echo $logdir

# running the traces with the updated predictor
echo "running compute int 18" && ./cvp -v -t 1 ./traces/compute_int_18.gz &> $logdir/compute_int_18.txt &
echo "running compute int 25" && ./cvp -v -t 1 ./traces/compute_int_25.gz &> $logdir/compute_int_25.txt &
echo "running compute int 26" && ./cvp -v -t 1 ./traces/compute_int_26.gz &> $logdir/compute_int_26.txt &
echo "running compute int 27" && ./cvp -v -t 1 ./traces/compute_int_27.gz &> $logdir/compute_int_27.txt &
echo "running compute int 9" && ./cvp -v -t 1 ./traces/compute_int_9.gz &> $logdir/compute_int_9.txt &
echo "running srv 0" && ./cvp -v -t 1 ./traces/srv_0.gz &> $logdir/srv_0.txt &
echo "running srv 10" && ./cvp -v -t 1 ./traces/srv_10.gz &> $logdir/srv_10.txt &
echo "running srv 20" && ./cvp -v -t 1 ./traces/srv_20.gz &> $logdir/srv_20.txt &
echo "running srv 30" && ./cvp -v -t 1 ./traces/srv_30.gz &> $logdir/srv_30.txt &
echo "running srv 40" && ./cvp -v -t 1 ./traces/srv_30.gz &> $logdir/srv_40.txt

wait

echo "finished running the traces with the updated value predictor"
# grepping for the results

echo "grepping the results"
grep -nH -e "IPC" -r ./logs/$timestamp/*.txt 2>&1 | tee $logdir/grepIPC.txt

python3 main.py $timestamp

echo "you can find your logs in the logs directory"
echo "done"	
