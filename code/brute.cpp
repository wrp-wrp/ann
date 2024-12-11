#include <bits/stdc++.h>
#include "brute.h"

using namespace std;

using i64 = long long;
#define debug(...) fprintf(stderr, __VA_ARGS__), fflush(stderr)

int main() {
    std :: ios :: sync_with_stdio(0);
    std :: cin.tie(0); std :: cout.tie(0);
    brute gragh;
    int n, dim;
    std :: cin >> n >> dim;
    for (int i = 0; i < n; i ++) {
        std :: vector<int> a(dim);
        for (int j = 0; j < dim; j ++) std :: cin >> a[j];
        gragh.insert(a); 
    }
    debug("Finished Building!\n");
    int q; 
    std :: cin >> q; 
    for (int i = 0; i < q; i ++) {
        int k;
        std :: vector<int> a(dim);
        std :: cin >> k;
        for (int j = 0; j < dim; j ++) std :: cin >> a[j];
        
        auto res = gragh.get_min_kth_dist(a, k);
        for (auto x : res) {
        std ::     cout << x << " ";
        }
        std :: cout << "\n";
    }
    return 0;
}