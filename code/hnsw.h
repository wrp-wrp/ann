#include <bits/stdc++.h>

namespace Hnsw {

#define debug(...) fprintf(stderr, __VA_ARGS__), fflush(stderr)

using i64 = long long;
using std ::cin;
using std ::cout;
using std ::pair;
using std ::vector;

std ::mt19937 rng;
using std ::string;

const string HERE_FILE_NAME = "data.txt";

const int efConstruction = 100;

template <typename ValueType = int>
class hnsw {
  private:
    const int M = 50;
    const int Mmax = M;
    
    const int Mmax0 = M * 2;
    
    using T = ValueType;
    int enter_point;
    const double Ml = 1 / log(M);
    struct Layer {
        vector<vector<int>> e;
        void test(int x) {
            if (e.size() <= x)
                e.resize((x + 1));
        }
    };
    vector<Layer> layer;
    vector<vector<T>> A;
    std :: set<void*> erased;

    inline int GetLayer() {
        double r = -log((double)rng() / rng.max());
        assert(Ml * r >= 0);
        return (int)(Ml * r);
    }
    inline long long dist(const vector<T> &a, const vector<T> &b) {
        auto Ap = a.data(), Bp = b.data(); int len = a.size();
        double res = 0;

        //#pragma GCC unroll 16
        for (int i = 0; i < len; i ++)
            res += 1LL * (Ap[i] - Bp[i]) * (Ap[i] - Bp[i]);
        return res;
    }

  public:
    // hnsw();
    // hnsw(int len);
    // ~hnsw();
    // bool erase(vector<T *> q);
    // vector<int> search_layer(vector<T> &q, int ep, int ef, int lc);
    // vector<int> select_neighbors_simple(vector<T> q, vector<int> &C, int m);
    // vector<int> select_neighbors_heuristic(vector<T> q, vector<int> &C, int m, int lc, int extendCandidates, int keepPrunedConnections);
    // vector<vector<T>> k_nn_search_no_erase(vector<T> &q, int K, int ef = efConstruction);
    // vector<vector<T>> k_nn_search(vector<T> &q, int K, int ef = efConstruction);
    // vector<vector<T> *> k_nn_search_pointer_no_erase(vector<T> &q, int K, int ef = efConstruction);
    // vector<vector<T> *> k_nn_search_pointer(vector<T> &q, int K, int ef = efConstruction);
    // vector<double> get_min_kth_dist_no_erase(vector<T> &q, int K);
    // vector<double> get_min_kth_dist(vector<T> &q, int K);
    // string save_data();
    // void read_data(string FILE_NAME);
    // vector<T> * insert(vector<T> q);

    //--------------------------------------------------------------------------------

    hnsw() {
        rng.seed(
            std ::chrono ::steady_clock ::now().time_since_epoch().count());
        enter_point = 0;
        layer.resize(1);
    }

    hnsw(int len) {
        rng.seed(
            std ::chrono ::steady_clock ::now().time_since_epoch().count());
        enter_point = 0;
        layer.resize(1);
        A.reserve(len);
    }

    ~hnsw() {
        save_data();
    }

    bool erase(vector<T *> q) {
        if (erased.find(q) == erased.end()) {
            return false;
        }
        erased.insert(q);
        return true;
    }

    vector<int> search_layer(vector<T> &q, int ep, int ef, int lc) {
        std ::unordered_map<int, int> v;
        vector<pair<double, int>> C;
        std ::priority_queue<pair<double, int>> W;
        v[ep] = 1;
        double init_len = dist(q, A[ep]);
        W.push({init_len, ep});
        C.push_back({init_len, ep});

        while (C.size() > 0) {
            auto cit = std ::min_element(C.begin(), C.end());
            int c = cit->second;
            C.erase(cit);
            int f = W.top().second;
            if (dist(q, A[c]) > dist(q, A[f])) {
                break;
            }

            layer[lc].test(c);
            for (int e : layer[lc].e[c]) {
                if (v[e] == 0) {
                    v[e] = 1;
                    f = W.top().second;
                    double d = dist(q, A[e]);
                    if (d < dist(q, A[f]) || W.size() < ef) {
                        C.push_back({d, e});
                        W.push({d, e});
                        if (W.size() > ef) {
                            W.pop();
                        }
                    }
                }
            }
        }
        vector<int> res;
        while (W.size()) {
            res.push_back(W.top().second);
            W.pop();
        }
        reverse(res.begin(), res.end());
        return res;
    }

    vector<int> select_neighbors_simple(vector<T> &q, vector<int> &C, int m) {
        if (C.size() <= m) {
            return C;
        }
        vector<pair<double, int>> D;

        for (int x : C) {
            D.push_back({dist(q, A[x]), x});
        }

        std ::nth_element(D.begin(), D.begin() + m, D.end());
        vector<int> res;

        for (int i = 0; i < m; i++) {
            res.push_back(D[i].second);
        }

        return res; // 返回可能小于 m 个数 ！！！
    }

    vector<int> select_neighbors_heuristic(vector<T> q, vector<int> &C, int m,
                                           int lc, int extendCandidates,
                                           int keepPrunedConnections) {
        vector<int> R;
        std ::set<int> W;
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
        std ::set<int> W0;
        while (W.size() > 0 && R.size() < m) {
            int e = *W.begin();
            for (int x : W)
                if (x != e) {
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
            } else {
                W0.insert(e);
            }
        }

        if (keepPrunedConnections) {
            while (W0.size() > 0 && R.size() < m) {
                int e = *W0.begin();
                for (int x : W0)
                    if (x != e) {
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

    vector<vector<T>> k_nn_search_no_erase(vector<T> &q, int K, int ef = efConstruction) {
        vector<int> W;
        int ep = 0;
        for (int i = layer.size() - 1; i >= 1; i--) {
            W = search_layer(q, ep, ef, i);
            ep = W[0];
        }
        W = search_layer(q, ep, ef, 0);
        W.resize(K);
        vector<vector<T>> res;
        for (int x : W) {
            res.push_back(A[x]);
        }
        return res;
    }

    vector<vector<T>> k_nn_search(vector<T> &q, int K, int ef = efConstruction) {
        vector<vector<T>> res;
        int nowcnt = K;
        while (1) {
            auto res2 = k_nn_search_no_erase(q, nowcnt, ef);

            int cnt = 0;
            for (auto x : res2) {
                if (erased.find(&x) == erased.end()) {
                    res.push_back(x);
                    cnt++;
                }
                if (cnt == K) {
                    break;
                }
            }

            if (cnt == K) {
                break;
            }

            nowcnt += K;
        }
        return res;
    }

    vector<vector<T> *> k_nn_search_pointer_no_erase(vector<T> &q, int K, int ef = efConstruction) {
        vector<int> W;
        int ep = 0;
        for (int i = layer.size() - 1; i >= 1; i--) {
            W = search_layer(q, ep, ef, i);
            ep = W[0];
        }
        W = search_layer(q, ep, ef, 0);
        W.resize(K);
        vector<vector<T> *> res;
        for (int x : W) {
            res.push_back(&A[x]);
        }
        return res;
    }

    vector<vector<T> *> k_nn_search_pointer(vector<T> &q, int K, int ef = efConstruction) {
        vector<vector<T> *> res;
        int nowcnt = K;
        while (1) {
            auto res2 = k_nn_search_pointer_no_erase(q, nowcnt, ef);

            int cnt = 0;
            for (auto x : res2) {
                if (erased.find(x) == erased.end()) {
                    res.push_back(x);
                    cnt++;
                }
                if (cnt == K) {
                    break;
                }
            }

            if (cnt == K) {
                break;
            }

            nowcnt += K;
        }
        return res;
    }

    vector<double> get_min_kth_dist_no_erase(vector<T> &q, int K) {
        auto res = k_nn_search(q, K, efConstruction);
        vector<double> res2;
        for (auto x : res) {
            res2.push_back(dist(q, x));
        }
        return res2;
    }

    vector<double> get_min_kth_dist(vector<T> &q, int K) {
        auto res = k_nn_search_pointer(q, K, efConstruction);
        vector<double> res2;
        for (auto x : res) {
            res2.push_back(dist(q, *x));
        }
        return res2;
    }

    string save_data() {
        debug("Saving Data...\n");
        const std :: string FILE_NAME = HERE_FILE_NAME;
        std ::ofstream out(FILE_NAME);
        out << A.size() << " " << A[0].size() << "\n"; // n dim
        for (auto &x : A) {
            for (auto y : x) {
                out << y << " ";
            }
            out << "\n";
        } // data
        out << layer.size() << "\n"; // layer.size()
        for (auto &x : layer) {
            out << x.e.size() << "\n"; // layer.e.size()
            for (auto &y : x.e) {
                out << y.size() << "\n"; // layer.e[i].size()
                for (auto z : y) {
                    out << z << " ";
                } // layer.e[i]
                out << "\n";
            }
        }
        out << enter_point << "\n"; // enter_point
        out << erased.size() << "\n"; // erased.size()
        for (auto x : erased) {
            out << (unsigned long long) x << " ";
        }
        out << "\n"; // erased

        out.close();

        debug("Data Saved!\n");
        return FILE_NAME;
    }

    void read_data(string FILE_NAME) {
        debug("Reading Data...\n");
        std ::ifstream in(FILE_NAME);
        int n, dim;
        in >> n >> dim;
        A.resize(n);
        for (auto &x : A) {
            x.resize(dim);
            for (auto &y : x) {
                in >> y;
            }
        }
        int layer_size;
        in >> layer_size;
        layer.resize(layer_size);
        for (auto &x : layer) {
            int e_size;
            in >> e_size;
            x.e.resize(e_size);
            for (auto &y : x.e) {
                int y_size;
                in >> y_size;
                y.resize(y_size);
                for (auto &z : y) {
                    in >> z;
                }
            }
        }
        in >> enter_point;
        int erased_size;
        in >> erased_size;
        erased.clear();
        for (int i = 0; i < erased_size; i++) {
            unsigned long long x;
            in >> x;
            erased.insert((void *) x);
        }
        in.close();
        debug("Data Read!\n");
    }

    vector<T> * insert(vector<T> q) {
        A.push_back(q);
        auto ret = &A[A.size() - 1];
        if (A.size() == 1) {
            return ret;
        }

        vector<int> W;
        int ep = enter_point;
        int L = layer.size() - 1;
        int l = GetLayer();
        if (l > L) {
            layer.resize(l + 1);
        }

        for (int lc = L; lc >= l + 1; lc--) {
            W = search_layer(q, ep, efConstruction, lc);
            ep = W[0];
        }

        for (int lc = std ::min(L, l); lc >= 0; lc--) {
            W = search_layer(q, ep, efConstruction, lc);

            vector<int> neighbors =
                std ::move(select_neighbors_simple(q, W, Mmax));

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
                    eConn =
                        std ::move(select_neighbors_simple(A[e], eConn, Mmax));
                }
            }
            ep = W[0];
        }
        if (l > L) {
            enter_point = A.size() - 1;
        }
        return ret;
    }
};

} // namespace Hnsw

using Hnsw ::hnsw;