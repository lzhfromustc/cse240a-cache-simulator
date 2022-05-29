cd ./src
make all

TRACE1=../traces/insertsort.bz2
TRACE2=../traces/mat_20M.bz2

# # mips -> insertsort
# #in the following setting, L1: offset has 7 bits, index has 7 bits, tag has 18 bits
# echo "mips -> insertsort"
# bunzip2 -kc $TRACE1 | ./cache --icache=128:2:2 --dcache=64:4:2 --l2cache=128:8:50 --blocksize=128 --memspeed=100

# # mips -> mat_20M
# echo "mips -> mat_20M"
# bunzip2 -kc $TRACE2 | ./cache --icache=128:2:2 --dcache=64:4:2 --l2cache=128:8:50 --blocksize=128 --memspeed=100

# # alpha -> mat_20M
# echo "alpha -> mat_20M"
# bunzip2 -kc $TRACE2 | ./cache --icache=512:2:2 --dcache=256:4:2 --l2cache=16384:8:50 --blocksize=64 --memspeed=100

# alpha -> insertsort
echo "alpha -> insertsort"
bunzip2 -kc $TRACE1 | ./cache --icache=512:2:2 --dcache=256:4:2 --l2cache=16384:8:50 --blocksize=64 --memspeed=100