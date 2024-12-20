#include <bits/stdc++.h>

using namespace std;

mt19937 myrand(114514);
#define endl '\n'

const int dim = 5; // 维度
const int N = 5000; // 向量数
const int MaxVal = 1e6;
const int Max_Testcase = 500;
const int Max_K = 20;

int main() {
    ios :: sync_with_stdio(0);
    cin.tie(0); cout.tie(0);
    //freopen("input.txt", "w", stdout);
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
    return 0;
}