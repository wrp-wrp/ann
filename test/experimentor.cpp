#include <bits/stdc++.h>

using namespace std;

mt19937 myrand(114514);
#define endl '\n'

namespace Generator {

const int dim = 10; // 维度
int N = 10000; // 向量数
const int MaxVal = 1e6;
const int Max_Testcase = 1000;
const int Max_K = 10;

int main() {
    freopen("input.txt", "w", stdout);
    cout << N << " " << dim << endl;
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < dim; j++) {
            cout << myrand() % MaxVal << " ";
        }
        cout << endl;
    }
    cout << Max_Testcase << endl;
    for (int i = 0; i < Max_Testcase; i++) {
        int k = myrand() % Max_K + 1;
        cout << k << endl;
        for (int j = 0; j < dim; j++) {
            cout << myrand() % MaxVal << " ";
        }
        cout << endl;
    }
    fclose(stdout);
    return 0;
}

}

const int para[] = {100, 200, 500, 1000, 1500, 2000, 2500, 3000, 3500, 4000, 4500, 5000, 5500, 6000, 6500, 7000, 7500, 8000, 8500, 9000, 9500, 10000, 12000, 15000, 20000, 25000};
const int testcase = sizeof(para) / sizeof(para[0]);

int main() {
    system("g++ -o tester tester.cpp -Ofast");
    for (int i = 0; i < 10; i++) {
        Generator :: N = para[i];
        Generator :: main();
        cerr << "Testcase " << i << " finished!" << endl;
        system("./tester < input.txt >> output.txt");
    }
    return 0;
}