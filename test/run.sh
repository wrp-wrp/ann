g++ generator.cpp -o generator -O2 -ftrapv
g++ tester.cpp -o tester -O2 -ftrapv
./generator > input.txt
./tester < input.txt