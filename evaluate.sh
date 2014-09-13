for (( i=8; i<= 1024; i*=2));
do
    cp config_LRU.txt config.txt
    ./pin -injection child -t obj-intel64/cachetrace.so -- ./matmult $i
    mv cachetrace.out cachetrace_LRU_$i.out

    cp config_LFU.txt config.txt
    ./pin -injection child -t obj-intel64/cachetrace.so -- ./matmult $i
    mv cachetrace.out cachetrace_LFU_$i.out

    cp config_RR.txt config.txt
    ./pin -injection child -t obj-intel64/cachetrace.so -- ./matmult $i
    mv cachetrace.out cachetrace_RR_$i.out
done
