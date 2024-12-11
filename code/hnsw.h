#include <bits/stdc++.h>

namespace Hnsw {

using i64 = long long;
using std :: vector;
using std :: cin;
using std :: cout;
using std :: pair;

std :: mt19937 rng;

template <typename ValueType = int, typename ReturnType = double, int M = 20, int Mmax = M, int efConstruction = 100, int Mmax0 = M * 2> 
class hnsw {
private :
    using T = ValueType;
    int enter_point;
    const double Ml = 1 / log(M);
    struct Layer {
        vector<vector<int> > e;
        void test(int x) { if (e.size() <= x) e.resize(x + 1); }
    } ;
    vector<Layer> layer;
    vector<vector<T> > A;
    inline int GetLayer() {
        double r = -log((double) rng() / rng.max());
        assert (Ml * r >= 0); return (int) (Ml * r);
    }
    inline ReturnType dist(const vector<T> &a, const vector<T> &b) {
        ReturnType res = 0;
        for (int i = 0; i < (int) a.size(); i ++) {
            res += (ReturnType) (a[i] - b[i]) * (a[i] - b[i]);
        }
        return res;
    }
public :
    // hnsw();
    // vector<int> search_layer(vector<T> q, int ep, int ef, int lc);
    // vector<int> select_neighbors_simple(vector<T> q, vector<int> C, int m);
    // vector<int> select_neighbors_heuristic(vector<T> q, vector<int> C, int m, int lc, int extendCandidates, int keepPrunedConnections);
    // vector<vector<T> > k_nn_search(vector<T> q, int K, int ef);
    // vector<ReturnType> get_min_kth_dist(vector<T> q, int K);
    // void insert(vector<T> q);

    //--------------------------------------------------------------------------------

    hnsw() {
        rng.seed(std :: chrono :: steady_clock :: now().time_since_epoch().count());
        enter_point = 0;
        layer.resize(1);
    }

    vector<int> search_layer(vector<T> q, int ep, int ef, int lc) {
        std :: unordered_map<int, int> v; 
        vector<pair<ReturnType, int> > C; 
        vector<pair<ReturnType, int> > W; 
        v[ep] = 1;
        ReturnType init_len = dist(q, A[ep]);
        W.push_back ({init_len, ep});
        C.push_back ({init_len, ep});

        while (C.size() > 0) {
            auto cit = std :: min_element(C.begin(), C.end());
            int c = cit -> second;
            C.erase(cit);
            int f = std :: max_element(W.begin(), W.end()) -> second;
            if (dist (q, A[c]) > dist(q, A[f])) {
                break;
            }

            layer[lc].test(c);
            for (int e : layer[lc].e[c]) {
                if (v[e] == 0) {
                    v[e] = 1;
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
        }

        std :: sort(W.begin(), W.end());

        vector<int> res;
        for (auto [dist, x] : W) {
            res.push_back(x);
        }
        return res;
    }

    vector<int> select_neighbors_simple(vector<T> q, vector<int> C, int m) {
        std :: priority_queue<pair<ReturnType, int> > pq;
        for (int x : C) {
            pq.push({- dist(q, A[x]), x});
        }
        vector<int> res;
        while (pq.size() > 0 && res.size() < m) {
            res.push_back(pq.top().second);
            pq.pop();
        }
        return res; // 返回可能小于 m 个数 ！！！
    }

    vector<int> select_neighbors_heuristic(vector<T> q, vector<int> C, int m, int lc, int extendCandidates, int keepPrunedConnections) {
        vector<int> R;
        std :: set<int> W;
        for (int x : C) {
            W.insert(x);
        }
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
        while (W.size() > 0 && R.size() < m) {
            int e = *W.begin();
            for (int x : W) if (x != e) {
                double d = dist(A[x], q);
                if (d < dist(A[e], q)) {
                    e = x;
                }
            }
            W.erase(e);

            bool small_all = 0;
            for (int x : R) {
                if (dist(A[x], q) > dist(A[e], q)) {
                    small_all = 1;
                    break;
                }
            }

            if (small_all) {
                R.push_back(e);
            }
            else {
                W0.insert(e);
            }
            //debug("e = %d\n", e);
            //debug("W0.size() = %d\n", W0.size());
            //debug("R.size() = %d\n", R.size());
            //debug
        }

        if (keepPrunedConnections) {
            while (W0.size() > 0 && R.size() < m) {
                int e = *W0.begin();
                for (int x : W0) if (x != e) {
                    double d = dist(A[x], q);
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

    vector<vector<T> > k_nn_search(vector<T> q, int K, int ef) {
        vector<int> W;
        int ep = 0;
        for (int i = layer.size() - 1; i >= 1; i --) {
            W = search_layer(q, ep, ef, i);
            ep = W[0];
        }
        W = search_layer(q, ep, ef, 0);
        W.resize(K);
        vector<vector<T> > res;
        for (int x : W) {
            res.push_back(A[x]);
        }
        return res;
    }

    vector<ReturnType> get_min_kth_dist(vector<T> q, int K) {
        auto res = k_nn_search(q, K, K * 4);
        vector<ReturnType> res2;
        for (auto x : res) {
            res2.push_back(dist(q, x));
        }
        return res2;
    }

    void insert (vector<T> q) {
        A.push_back(q);

        if (A.size() == 1) {
            return ;
        }

        vector<int> W;
        int ep = enter_point;
        int L = layer.size() - 1;
        int l = GetLayer();
        if (l > L) {
            layer.resize(l + 1);
        }
        
        for (int lc = L; lc >= l + 1; lc --) {
            W = search_layer(q, ep, efConstruction, lc);
            ep = W[0];
        }
        
        for (int lc = std :: min(L, l); lc >= 0; lc --) {
            W = search_layer(q, ep, efConstruction, lc);

            vector<int> neighbors = select_neighbors_simple(q, W, Mmax);
            //neighbors = select_neighbors_heuristic(q, W, Mmax0, lc, 0, 1);
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

} // namespace Hnsw

using Hnsw :: hnsw;