##Note for myself
L1 or L2 (or both?) cache may not be initialized and the parameters are all 0

L2 cache is now non-inclusive non-exclusive. Don't care if part of L1 is inside L2 or not

Replacement policy is LRU

(???) If L1 misses and L2 hits, the total time is L1 hit time + L2 hit time

Remember to compile with -O0 and -O3 to see if you have a bug hidden by -O0

Your output result would have only the statistics for the levels of cache that exist

Make sure that your code runs correctly on the docker container