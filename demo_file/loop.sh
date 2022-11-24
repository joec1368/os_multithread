#!/bin/bash
echo "Running start"
make
make load
for parallelized in ../Test_cases/Test_case_*
do
    for n in {1..4..1}
    do
        echo "Running ${parallelized} with ${n}"
        ./MT_matrix ${n} ${parallelized}/m1.txt ${parallelized}/m2.txt >> /dev/null
        diff ./result.txt ${parallelized}/result.txt
        echo ""
    done

    for n in {8..32..8}
    do
        echo "Running ${parallelized} with ${n}"
        ./MT_matrix ${n} ${parallelized}/m1.txt ${parallelized}/m2.txt >> /dev/null
        diff result.txt ${parallelized}/result.txt
        echo ""
    done
    rm ./result.txt
done
make unload 
make clean

