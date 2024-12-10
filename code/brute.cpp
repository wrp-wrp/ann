#include <bits/stdc++.h>

using namespace std;

using i64 = long long;

class BruteForce {
public:
    int N, dim, Max_Testcase;
    vector<vector<int>> A, B;
    void init() {
        cin >> N >> dim;
        for (int i = 0; i < N; i++) {
            vector<int> a(dim);
            for (int j = 0; j < dim; j++) {
                cin >> a[j];
            }
            A.push_back(a);
        }

    }

    void solve() {
        cin >> Max_Testcase;
        for (int i = 0; i < Max_Testcase; i++) {
            int K;
            cin >> K;
            vector<int> a(dim);
            for (int j = 0; j < dim; j ++) cin >> a[j];
            B.push_back(a);
            vector<i64> res;
            
            for (int j = 0; j < N; j++) {
                i64 cur = 0;
                for (int k = 0; k < dim; k++) {
                    cur += 1ll * (A[j][k] - B[i][k]) * (A[j][k] - B[i][k]);
                }
                res.push_back (cur);
            }
            sort(res.begin(), res.end());
            for (int j = 0; j < K; j++) {
                cout << res[j] << " ";
            }
            cout << endl;
        }
    }
} ;

int main() {
    ios :: sync_with_stdio(0);
    cin.tie(0); cout.tie(0);
    BruteForce bf;
    bf.init();
    bf.solve();
    return 0;
}