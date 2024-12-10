#include <bits/stdc++.h>

using i64 = long long;
using std :: vector;
using std :: cin;
using std :: cout;
using std :: pair;

#define debug(...) fprintf(stderr, __VA_ARGS__), fflush(stderr)

class hnsw {
public :

    const int M = 20;
    const double Ml = 1 / log(M);
    std :: mt19937 rng;
    const int Mmax = M;
    const int efConstruction = 100;
    const int Mmax0 = M * 2;

    int enter_point;

    

    inline int GetLayer() {
        double r = -log((double) rng() / rng.max());
        assert (Ml * r >= 0);
        return (int) (Ml * r);
    }

    struct Layer {
        vector<vector<int> > e;
        
        void test(int x) {
            if (e.size() <= x) e.resize(x + 1);
        }
    } ;
    vector<Layer> layer;
    vector<vector<int> > A;

    hnsw() {
        //rng.seed(std :: chrono :: steady_clock :: now().time_since_epoch().count());
        rng.seed(0);
        enter_point = 0;
        layer.resize(1);
    }

    inline i64 dist(const vector<int> &a, const vector<int> &b) {
        i64 res = 0;
        for (int i = 0; i < (int) a.size(); i ++) {
            res += 1ll * (a[i] - b[i]) * (a[i] - b[i]);
        }
        return res;
    }

    vector<int> search_layer(vector<int> q, int ep, int ef, int lc) {
        std :: unordered_map<int, int> v; // visited
        vector<pair<i64, int> > C; // candidate
        vector<pair<i64, int> > W; // working set
        v[ep] = 1;
        i64 init_len = dist(q, A[ep]);
        W.push_back ({init_len, ep});
        C.push_back ({init_len, ep});

        //debug("search_layer %d\n", lc);

        while (C.size() > 0) {
            auto cit = std :: min_element(C.begin(), C.end());
            int c = cit -> second;
            C.erase(cit);
            //debug("c = %d\n", c);
            int f = std :: max_element(W.begin(), W.end()) -> second;
            if (dist (q, A[c]) > dist(q, A[f])) {
                break;
            }
            //debug("dist = %lld\n", dist(q, A[c]));
            //debug("FUCK");

            layer[lc].test(c);
            //debug("layer[%d].e[%d].size() = %d\n", lc, c, (int) layer[lc].e[c].size());
            for (int e : layer[lc].e[c]) {
                if (v[e] == 0) {
                    v[e] = 1;
                    //debug ("e = %d\n", e);
                    f = std :: max_element(W.begin(), W.end()) -> second;
                    if (dist(q, A[e]) < dist(q, A[f]) || W.size() < ef) {
                        C.push_back ({dist(q, A[e]), e});
                        W.push_back ({dist(q, A[e]), e});
                        if (W.size() > ef) {
                            auto it = std :: max_element(W.begin(), W.end());
                            W.erase(it);
                        }
                    }
                }
            }

            //debug("C.size() = %d\n", (int) C.size());
        }

        //debug("W.size() = %d\n", (int) W.size());

        std :: sort(W.begin(), W.end());

        vector<int> res;
        for (auto [dist, x] : W) {
            res.push_back(x);
        }
        return res;
    }

    vector<int> select_neighbors_simple(vector<int> q, vector<int> C, int M) {
        std :: priority_queue<pair<i64, int> > pq;
        for (int x : C) {
            pq.push({- dist(q, A[x]), x});
        }
        vector<int> res;
        while (pq.size() > 0 && res.size() < M) {
            res.push_back(pq.top().second);
            pq.pop();
        }
        return res; // 返回可能小于 M 个数 ！！！
    }

    vector<int> select_neighbors_heuristic(vector<int> q, vector<int> C, int M, int lc, int extendCandidates, int keepPrunedConnections) {
        vector<int> R;
        std :: set<int> W;
        if (extendCandidates) {
            for (int x : C) {
                layer[lc].test(x);
                for (int y : layer[lc].e[x]) {
                    if (W.find(y) == W.end()) {
                        W.insert(y);
                    }
                }
            }
        }
        std :: set<int> W0;
        while (W.size() > 0 && R.size() < M) {
            int e = *W.begin();
            for (int x : C) if (x != e) {
                i64 d = dist(A[x], q);
                if (d < dist(A[e], q)) {
                    e = x;
                }
            }
            W.erase(e);

            bool small_all = 1;
            for (int x : R) {
                if (dist(A[x], q) > dist(A[e], q)) {
                    small_all = 0;
                    break;
                }
            }

            if (small_all) {
                R.push_back(e);
            }
            else {
                W0.insert(e);
            }
        }

        if (keepPrunedConnections) {
            while (W0.size() > 0 && R.size() < M) {
                int e = *W0.begin();
                for (int x : W0) if (x != e) {
                    i64 d = dist(A[x], q);
                    if (d < dist(A[e], q)) {
                        e = x;   
                    }
                }
                W0.erase(e);
                R.push_back(e);
            }
        }

        return R;
    }

    vector<vector<int> > k_nn_search(vector<int> q, int K, int ef) {
        vector<int> W;
        int ep = 0;
        for (int i = layer.size() - 1; i >= 1; i --) {
            W = search_layer(q, ep, ef, i);
            ep = W[0];
        }
        W = search_layer(q, ep, ef, 0);
        W.resize(K);
        vector<vector<int> > res;
        for (int x : W) {
            res.push_back(A[x]);
        }
        return res;
    }

    vector<i64> get_min_kth_dist(vector<int> q, int K) {
        auto res = k_nn_search(q, K, K * 3);
        vector<i64> res2;
        for (auto x : res) {
            res2.push_back(dist(q, x));
        }
        return res2;
    }

    void insert (vector<int> q) {
        A.push_back(q);

        if (A.size() == 1) {
            return ;
        }

        vector<int> W;
        int ep = enter_point;
        int L = layer.size() - 1;
        int l = GetLayer();

        //debug("insert %d\n", (int) A.size() - 1);
        //debug("layer %d\n", l);

        if (l > L) {
            layer.resize(l + 1);
        }

        //debug("L = %d\n", L);
        
        for (int lc = L; lc >= l + 1; lc --) {
            W = search_layer(q, ep, efConstruction, lc);
            ep = W[0];
            //debug("layer %d\n", lc);
        }
        
        for (int lc = std :: min(L, l); lc >= 0; lc --) {
            //debug("layer %d\n", lc);
            W = search_layer(q, ep, efConstruction, lc);
            //debug("W.size() = %d\n", (int) W.size());

            vector<int> neighbors = select_neighbors_simple(q, W, Mmax);
            for (int x : neighbors) {
                layer[lc].test(x);
                layer[lc].test(A.size() - 1);
                layer[lc].e[x].push_back(A.size() - 1);
                layer[lc].e[A.size() - 1].push_back(x);
            }
            for (int e : neighbors) {
                layer[lc].test(e);
                auto &eConn = layer[lc].e[e];
                if (eConn.size() > Mmax) {
                    eConn = select_neighbors_simple(A[e], eConn, Mmax);
                }
            }
            ep = W[0];
        }
        if (l > L) {
            enter_point = A.size() - 1;
        }
    }
} ;

int main() {
    std :: ios :: sync_with_stdio(0);
    cin.tie(0); cout.tie(0);
    hnsw gragh;
    int n, dim;
    cin >> n >> dim;
    //gragh.dim = dim;
    
    for (int i = 0; i < n; i ++) {
        vector<int> a(dim);
        for (int j = 0; j < dim; j ++) cin >> a[j];
        //std :: cerr << i << std :: endl;
        //debug("global insert %d\n", i);
        gragh.insert(a); //return 0;
    }
    debug("Finished Building!\n");
    int q; //return 0;
    cin >> q; //q = 1;
    for (int i = 0; i < q; i ++) {
        int k;
        vector<int> a(dim);
        cin >> k;
        for (int j = 0; j < dim; j ++) cin >> a[j];
        
        auto res = gragh.get_min_kth_dist(a, k);
        for (auto x : res) {
            cout << x << " ";
        }
        cout << "\n";
    }
    return 0;
}