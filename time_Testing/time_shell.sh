#!/bin/bash
echo "Running start"
gcc time_testing.c -pthread -o runnable

for parallelized in ../Test_cases/Test_case_*
do
    make
    for n in {1..4..1}
    do
        echo "Running ${parallelized} with ${n}"
        for i in {1..30}
        do
            echo "start ${i}"
            ./runnable ${n} ${parallelized}/m1.txt ${parallelized}/m2.txt >> ${parallelized}.out
        done
        cat ${parallelized}.out | python3 $1
        rm ${parallelized}.out
        echo ""
    done

    for n in {8..32..8}
    do
        echo "Running ${parallelized} with ${n}"
        for i in {1..30}
        do
            ./runnable ${n} ${parallelized}/m1.txt ${parallelized}/m2.txt >> ${parallelized}.out
        done
        cat ${parallelized}.out | python3 $1
        rm ${parallelized}.out
        echo ""
    done
    make clean
done


