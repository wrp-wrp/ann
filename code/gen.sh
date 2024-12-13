g++ -o generator generator.cpp -O2
 ./generator > input.txt
g++ -o tester tester.cpp -O2
./tester < input.txt 