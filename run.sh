cd ./src
make all

TRACE1=./traces/insertsort.bz2
TRACE2=./traces/mat_20M.bz2

bunzip2 -kc $TRACE1 | ./cache --icache=128:2:2 --dcache=64:4:2 --l2cache=128:8:50 --blocksize=128 --memspeed=100