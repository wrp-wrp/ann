#include <bits/stdc++.h>

using i64 = long long;
using std :: vector;
using std :: cin;
using std :: cout;
using std :: pair;

#define debug(...) fprintf(stderr, __VA_ARGS__), fflush(stderr)

class hnsw {
public :

    struct Layer {
        vector<int> e;
    } layer;

    vector<vector<int> > A;

    inline i64 dist(const vector<int> &a, const vector<int> &b) {
        i64 res = 0;
        for (int i = 0; i < (int) a.size(); i ++) {
            res += 1ll * (a[i] - b[i]) * (a[i] - b[i]);
        }
        return res;
    }

    vector<int> search_layer(vector<int> q, int ep, int ef, int lc) {
        vector<int> v; // visited
        vector<pair<i64, int> > C; // candidate
        vector<pair<i64, int> > W; // working set
        v.push_back (ep);
        i64 init_len = dist(q, A[ep]);
        W.push_back ({init_len, ep});
        C.push_back ({init_len, ep});

        while (C.size() > 0) {
            int c = std :: min_element(C.begin(), C.end()) -> second;
            int f = std :: max_element(W.begin(), W.end()) -> second;
            if (dist (q, A[c]) > dist(q, A[f])) {
                break;
            }
            
        }
    }
} ;

int main() {
    std :: ios :: sync_with_stdio(0);
    cin.tie(0); cout.tie(0);
    // nsw gragh;
    // int n, dim;
    // cin >> n >> dim;
    // gragh.dim = dim;
    
    // for (int i = 0; i < n; i ++) {
    //     vector<int> a(dim);
    //     for (int j = 0; j < dim; j ++) cin >> a[j];
    //     //std :: cerr << i << std :: endl;
    //     gragh.insert(a);
    // }
    // int q; 
    // cin >> q; //q = 1;
    // for (int i = 0; i < q; i ++) {
    //     int k;
    //     vector<int> a(dim);
    //     cin >> k;
    //     for (int j = 0; j < dim; j ++) cin >> a[j];
        
    //     auto res = gragh.get_min_kth_dist(a, k);
    //     for (auto x : res) {
    //         cout << x << " ";
    //     }
    //     cout << "\n";
    // }
    return 0;
}