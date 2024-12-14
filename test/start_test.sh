g++ generator.cpp -o generator -O2
g++ tester.cpp -o tester -O2
./generator > input.txt
./tester < input.txt