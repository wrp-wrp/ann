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
const double alpha = 0.3;

template <typename ValueType = int>
class hnsw {
  private:
    using T = ValueType;
    using ull = unsigned long long;
    using IT = typename std ::vector<T> ::iterator;
    const int M = 50;
    const int Mmax = M;
    const int Mmax0 = M * 2;
    const double Ml = 1 / log(M);
    int enter_point;
    int tot;
    
    struct Layer {
        vector<vector<int>> e;
        void test(int x) {
            if (e.size() <= x)
                e.resize((x + 1));
        }
    };
    vector<Layer> layer;
    vector<pair<vector<T>, ull> > A;
    std :: unordered_map<ull, int> id2pos;
    std :: set<ull> erased;

    inline int GetLayer() {
        double r = -log((double)rng() / rng.max());
        assert(Ml * r >= 0);
        return (int)(Ml * r);
    }
    inline double dist(const vector<T> &a, const vector<T> &b) {
        auto Ap = a.data(), Bp = b.data(); int len = a.size();
        double res = 0;
        //#pragma GCC unroll 16
        for (int i = 0; i < len; i ++)
            res += 1LL * (Ap[i] - Bp[i]) * (Ap[i] - Bp[i]);
        return res;
    }

    inline bool IsErased(ull id) {
        return erased.find(id) != erased.end();
    }

    inline void EraseIt(ull id) {
        erased.insert(id);
    }

    vector<int> search_layer(const vector<T> &q, int ep, int ef, int lc) {
        std ::unordered_map<int, int> v;
        vector<pair<double, int>> C;
        std ::priority_queue<pair<double, int>> W;
        v[ep] = 1;
        double init_len = dist(q, A[ep].first);
        W.push({init_len, ep});
        C.push_back({init_len, ep});

        while (C.size() > 0) {
            auto cit = std ::min_element(C.begin(), C.end());
            int c = cit->second;
            C.erase(cit);
            int f = W.top().second;
            if (dist(q, A[c].first) > dist(q, A[f].first)) {
                break;
            }

            layer[lc].test(c);
            for (int e : layer[lc].e[c]) {
                if (v[e] == 0) {
                    v[e] = 1;
                    f = W.top().second;
                    double d = dist(q, A[e].first);
                    if (d < dist(q, A[f].first) || W.size() < ef) {
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

    vector<int> select_neighbors_simple(const vector<T> &q, vector<int> &C, int m) {
        if (C.size() <= m) {
            return C;
        }
        vector<pair<double, int>> D;

        for (int x : C) {
            D.push_back({dist(q, A[x].first), x});
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
                    double d = dist(A[x].first, q);
                    if (d < dist(A[e].first, q)) {
                        e = x;
                    }
                }
            W.erase(e);

            bool small_all = 0;
            for (int x : R) {
                if (dist(A[x].first, q) > dist(A[e].first, q)) {
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
                        double d = dist(A[x].first, q);
                        if (d < dist(A[e].first, q)) {
                            e = x;
                        }
                    }
                W0.erase(e);
                R.push_back(e);
            }
        }

        return R;
    }

    bool CheckRebuild() {
        if (erased.size() > alpha * A.size()) {
            return 1;
        }
        return 0;
    }
    
    void rebuild() {
        debug("rebuilding !\n");
        layer.clear();
        id2pos.clear();
        auto rec = A;
        A.clear();
        enter_point = 0;
        layer.resize(1);

        for (auto &x : rec) {
            if (! IsErased(x.second)) {
                insert(x.first, x.second);
            }
        }

        erased.clear();
        debug("rebuild finished !\n");
    }

  public:

    //--------------------------------------------------------------------------------
    

    hnsw() {
        tot = 0;
        rng.seed(
            std ::chrono ::steady_clock ::now().time_since_epoch().count());
        enter_point = 0;
        layer.resize(1);
    }

    hnsw(int len) {
        tot = 0;
        rng.seed(
            std ::chrono ::steady_clock ::now().time_since_epoch().count());
        enter_point = 0;
        layer.resize(1);
        A.reserve(len);
    }

    ~hnsw() {
        //save_data();
    }

    bool erase(const ull id) {

        if (IsErased(id)) {
            return 0;
        }
        erased.insert(id);
        if (CheckRebuild()) {
            rebuild();
        }
        return true;
    }
    
    vector<vector<T>> k_nn_search_no_erase(const vector<T> &q, int K, int ef = efConstruction) {
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
            res.push_back(A[x].first);
        }
        return res;
    }

    vector<vector<T>> k_nn_search(const vector<T> &q, int K, int ef = efConstruction) {
        vector<vector<T>> res;
        int nowcnt = K;
        if (A.size() < K) {
            for (auto &x : A) {
                if (! IsErased(x.second)) res.push_back(x.first);
            }
            return res;
        }
        while (1) {
            auto res2 = k_nn_search_no_erase(q, nowcnt, ef);

            int cnt = 0;
            for (auto x : res2) {
                if (! IsErased(A[x].second)) {
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

    vector<ull> k_nn_search_cookie_no_erase(const vector<T> &q, int K, int ef = efConstruction) {
        vector<int> W;
        int ep = 0;
        for (int i = layer.size() - 1; i >= 1; i--) {
            W = search_layer(q, ep, ef, i);
            ep = W[0];
        }
        W = search_layer(q, ep, ef, 0);
        W.resize(K);
        vector<ull> res;
        for (int x : W) {
            res.push_back(A[x].second);
        }
        return res;
    }

    vector<ull> k_nn_search_cookie(const vector<T> &q, int K, int ef = efConstruction) {
        vector<ull> res;
        if (A.size() < K) {
            for (auto &x : A) {
                if (! IsErased(x.second)) res.push_back(x.second);
            }
            return res;
        }
        int nowcnt = K;
        while (1) {
            auto res2 = k_nn_search_cookie_no_erase(q, nowcnt, ef);

            int cnt = 0;
            for (auto x : res2) {
                if (! IsErased(x)) {
                    res.push_back(x);
                    cnt++;
                }
                if (cnt == K) {
                    break;
                }
            }

            if (cnt == K) {
                res = std ::move(res2);
                break;
            }

            nowcnt += K;
        }
        return res;
    }

    vector<double> get_min_kth_dist_no_erase(const vector<T> &q, int K) {
        auto res = k_nn_search_cookie(q, K, efConstruction);
        vector<double> res2;
        for (auto x : res) {
            res2.push_back(dist(q, A[id2pos[x]].first));
        }
        return res2;
    }

    vector<double> get_min_kth_dist(const vector<T> &q, int K) {
        //debug("get_min_kth_dist\n");
        //debug("A.size = %d\n", A.size());
        int nowcnt = K;
        while (1) {
            auto res = k_nn_search_cookie(q, K, efConstruction);
            int cnt = 0;
            vector<double> res2;
            for (auto x : res) {
                if (! IsErased(x)) {
                    res2.push_back(dist(q, A[id2pos[x]].first));
                    cnt++;
                }
                if (cnt == K) {
                    break;
                }
            }
            if (cnt == K) {
                return res2;
            }
            nowcnt += K;

        }
        assert(0);
        return {};
    }

    void save_data(string FILE_NAME) {
        debug("Saving Data...\n");
        //const std :: string FILE_NAME = HERE_FILE_NAME;
        std ::ofstream out(FILE_NAME);
        out << A.size() << " " << A[0].first.size() << "\n";
        for (auto &x : A) {
            for (auto y : x.first) {
                out << y << " ";
            }
            out << x.second << "\n";
        }
        out << erased.size() << "\n";
        for (auto x : erased) {
            out << x << "\n";
        }
        out << layer.size() << "\n";
        for (auto &x : layer) {
            out << x.e.size() << "\n";
            for (auto &y : x.e) {
                out << y.size() << "\n";
                for (auto z : y) {
                    out << z << " ";
                }
                out << "\n";
            }
        }
        out << enter_point << " " << tot << "\n";
        out << id2pos.size() << "\n";
        for (auto &x : id2pos) {
            out << x.first << " " << x.second << "\n";
        }
        
        out.close();

        debug("Data Saved!\n");
        return void();
    }

    void read_data(string FILE_NAME) {
        debug("Reading Data...\n");
        std ::ifstream in(FILE_NAME);
        int n, dim;
        in >> n >> dim;
        A.resize(n);
        for (int i = 0; i < n; i++) {
            A[i].first.resize(dim);
            for (int j = 0; j < dim; j++) {
                in >> A[i].first[j];
            }
            in >> A[i].second;
            id2pos[A[i].second] = i;
        }
        int m;
        in >> m;
        for (int i = 0; i < m; i++) {
            ull x;
            in >> x;
            erased.insert(x);
        }
        int l;
        in >> l;
        layer.resize(l);
        for (int i = 0; i < l; i++) {
            int s;
            in >> s;
            layer[i].e.resize(s);
            for (int j = 0; j < s; j++) {
                int t;
                in >> t;
                layer[i].e[j].resize(t);
                for (int k = 0; k < t; k++) {
                    in >> layer[i].e[j][k];
                }
            }
        }
        in >> enter_point >> tot;
        int sz;
        in >> sz;
        for (int i = 0; i < sz; i++) {
            ull x;
            int y;
            in >> x >> y;
            id2pos[x] = y;
        }
        
        in.close();
        debug("Data Read!\n");
    }

    ull insert(const vector<T> &q, ull Cookie = 0) {  // cookie = 0 表示 cookie 未知
        ull cookie;
        if (! Cookie)
            cookie = ++ tot;
        else
            cookie = Cookie;
        A.push_back( {q, cookie} );

        id2pos[cookie] = A.size() - 1;
        
        if (A.size() == 1) {
            
            return cookie;
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
                        std ::move(select_neighbors_simple(A[e].first, eConn, Mmax));
                }
            }
            ep = W[0];
        }
        if (l > L) {
            enter_point = A.size() - 1;
        }

        return cookie;
    }
};

} // namespace Hnsw

using Hnsw ::hnsw;