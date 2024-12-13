g++ -o generator generator.cpp -O2
g++ -o brute brute.cpp -O2
./generator > input.txt
./brute < input.txt > correct.txt