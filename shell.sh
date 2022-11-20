#!/bin/bash
echo "start to do"

cd module
make
sudo insmod test.ko
cd ..
gcc main.c -pthread
echo "start to count test case 1"
./a.out 4 ./Test_cases/Test_cases_1/m1.txt ./Test_cases/Test_cases_1/m2.txt
echo "start to diff test case 1"
diff ./answer.txt ./Test_cases/Test_cases_1/result.txt 
echo "end diff test case 1"
rm ./answer.txt
sudo rmmod test

# cd module
# sudo insmod test.ko
# cd ..
# echo "start to count test case 2"
# ./a.out 16 ./Test_cases/Test_cases_2/m1.txt ./Test_cases/Test_cases_2/m2.txt
# echo "start to diff test case 2"
# diff ./answer.txt ./Test_cases/Test_cases_2/result.txt 
# echo "end diff test case 2"
# rm ./answer.txt
# sudo rmmod test

cd module
sudo insmod test.ko
cd ..
echo "start to count test case 3"
./a.out 4 ./Test_cases/Test_cases_3/m1.txt ./Test_cases/Test_cases_3/m2.txt
echo "start to diff test case 3"
diff ./answer.txt ./Test_cases/Test_cases_3/result.txt 
echo "end diff test case 3"
rm ./answer.txt
sudo rmmod test

cd module
sudo insmod test.ko
cd ..
echo "start to count test case 4"
./a.out 4 ./Test_cases/Test_cases_4/m1.txt ./Test_cases/Test_cases_4/m2.txt
echo "start to diff test case 4"
diff ./answer.txt ./Test_cases/Test_cases_4/result.txt 
echo "end diff test case 4"
rm ./answer.txt
rm ./a.out
cd module
make clean
cd ..
sudo rmmod test