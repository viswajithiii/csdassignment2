mkdir outputs
for (( i=8; i<= 1024; i*=2));
do
    cp config_LRU.txt config.txt
    ./pin -injection child -t obj-intel64/cachetrace.so -- ./matmult $i
    mv cachetrace.out outputs/cachetrace_LRU_$i.out

    cp config_LFU.txt config.txt
    ./pin -injection child -t obj-intel64/cachetrace.so -- ./matmult $i
    mv cachetrace.out outputs/cachetrace_LFU_$i.out

    cp config_RR.txt config.txt
    ./pin -injection child -t obj-intel64/cachetrace.so -- ./matmult $i
    mv cachetrace.out outputs/cachetrace_RR_$i.out
done
rm config.txt
python tables.py > tables.tex
pdflatex tables.tex
rm tables.aux tables.log
