#include <bits/stdc++.h>

namespace Hnsw {

#define debug(...) fprintf(stderr, __VA_ARGS__), fflush(stderr)

using i64 = long long;
using std ::cin;
using std ::cout;
using std ::pair;
using std ::vector;
using std ::mt19937;
using std ::string;

const int efConstruction = 30;
const double alpha = 0.3;

template <typename ValueType = int>
class hnsw {
  private:
    using T = ValueType;
    using ull = unsigned long long;
    using IT = typename std ::vector<T> ::iterator;
    const int M = 200;
    const int Mmax = M;
    const int Mmax0 = M * 2;
    const double Ml = 1 / log(M);
    int enter_point;
    int tot;
    std ::mt19937 rng;
    
    struct Layer {
        vector<vector<int>> e;
        void test(int x) {
            if (e.size() <= x)
                e.resize((x + 1));
        }
    };
    vector<Layer> layer;
    vector<pair<vector<T>, ull> > VectorPool;
    std :: unordered_map<ull, int> id2pos;
    std :: set<ull> ErasedCookie;

    inline int GetLayer() {
        double r = -log((double)rng() / rng.max());
        assert(Ml * r >= 0);
        return (int)(Ml * r);
    }
    inline long long dist(const vector<T> &a, const vector<T> &b) {
        auto Ap = a.data(), Bp = b.data(); int len = a.size();
        long long res = 0;
        //#pragma GCC unroll 16
        for (int i = 0; i < len; i ++)
            res += 1LL * (Ap[i] - Bp[i]) * (Ap[i] - Bp[i]);
        return res;
    }

    inline bool IsErased(ull cookie) {
        return ErasedCookie.find(cookie) != ErasedCookie.end();
    }

    inline void EraseIt(ull cookie) {
        ErasedCookie.insert(cookie);
    }

    vector<int> search_layer(const vector<T> &QueryVector, int EnterPoint, int ef, int LayerIndex) {
        std ::unordered_map<int, int> Visited;
        vector<pair<double, int>> Candidator;
        std ::priority_queue<pair<double, int>> NearestNeighbors;
        Visited[EnterPoint] = 1;
        double init_len = dist(QueryVector, VectorPool[EnterPoint].first);
        NearestNeighbors.push({init_len, EnterPoint});
        Candidator.push_back({init_len, EnterPoint});

        while (Candidator.size() > 0) {
            auto cit = std ::min_element(Candidator.begin(), Candidator.end());
            int c = cit->second;
            Candidator.erase(cit);
            int f = NearestNeighbors.top().second;
            if (dist(QueryVector, VectorPool[c].first) > dist(QueryVector, VectorPool[f].first)) {
                break;
            }

            layer[LayerIndex].test(c);
            for (int e : layer[LayerIndex].e[c]) {
                if (Visited[e] == 0) {
                    Visited[e] = 1;
                    f = NearestNeighbors.top().second;
                    double d = dist(QueryVector, VectorPool[e].first);
                    if (d < dist(QueryVector, VectorPool[f].first) || NearestNeighbors.size() < ef) {
                        Candidator.push_back({d, e});
                        NearestNeighbors.push({d, e});
                        if (NearestNeighbors.size() > ef) {
                            NearestNeighbors.pop();
                        }
                    }
                }
            }
        }
        vector<int> res;
        while (NearestNeighbors.size()) {
            res.push_back(NearestNeighbors.top().second);
            NearestNeighbors.pop();
        }
        reverse(res.begin(), res.end());
        return res;
    }

    vector<int> select_neighbors_simple(const vector<T> &QueryVector, vector<int> &Candidator, int m) {
        if (Candidator.size() <= m) {
            return Candidator;
        }
        vector<pair<double, int>> D;

        for (int x : Candidator) {
            D.push_back({dist(QueryVector, VectorPool[x].first), x});
        }

        std ::nth_element(D.begin(), D.begin() + m, D.end());
        vector<int> res;

        for (int i = 0; i < m; i++) {
            res.push_back(D[i].second);
        }

        return res; // 返回可能小于 m 个数 ！！！
    }

    vector<int> select_neighbors_heuristic(vector<T> QueryVector, vector<int> &Candidator, int m,
                                           int LayerIndex, int extendCandidates,
                                           int keepPrunedConnections) {
        vector<int> R;
        std ::set<int> NearestNeighbors;
        for (int x : Candidator) {
            NearestNeighbors.insert(x);
        }
        if (extendCandidates) {
            for (int x : Candidator) {
                layer[LayerIndex].test(x);
                for (int y : layer[LayerIndex].e[x]) {
                    if (NearestNeighbors.find(y) == NearestNeighbors.end()) {
                        NearestNeighbors.insert(y);
                    }
                }
            }
        }
        std ::set<int> W0;
        while (NearestNeighbors.size() > 0 && R.size() < m) {
            int e = *NearestNeighbors.begin();
            for (int x : NearestNeighbors)
                if (x != e) {
                    double d = dist(VectorPool[x].first, QueryVector);
                    if (d < dist(VectorPool[e].first, QueryVector)) {
                        e = x;
                    }
                }
            NearestNeighbors.erase(e);

            bool small_all = 0;
            for (int x : R) {
                if (dist(VectorPool[x].first, QueryVector) > dist(VectorPool[e].first, QueryVector)) {
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
                        double d = dist(VectorPool[x].first, QueryVector);
                        if (d < dist(VectorPool[e].first, QueryVector)) {
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
        if (ErasedCookie.size() > alpha * VectorPool.size()) {
            return 1;
        }
        return 0;
    }
    
    void rebuild() {
        debug("rebuilding !\n");
        layer.clear();
        id2pos.clear();
        auto rec = VectorPool;
        VectorPool.clear();
        enter_point = 0;
        layer.resize(1);

        for (auto &x : rec) {
            if (! IsErased(x.second)) {
                insert(x.first, x.second);
            }
        }

        ErasedCookie.clear();
        debug("rebuild finished !\n");
    }

    vector<vector<T>> k_nn_search(const vector<T> &QueryVector, int K, int ef = efConstruction) {
        vector<vector<T>> res;
        int nowcnt = K;
        if (VectorPool.size() < K) {
            for (auto &x : VectorPool) {
                if (! IsErased(x.second)) res.push_back(x.first);
            }
            return res;
        }
        while (1) {
            auto res2 = k_nn_search_no_erase(QueryVector, nowcnt, ef);

            int cnt = 0;
            for (auto x : res2) {
                if (! IsErased(VectorPool[x].second)) {
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

    vector<vector<T>> k_nn_search_no_erase(const vector<T> &QueryVector, int K, int ef = efConstruction) {
        vector<int> NearestNeighbors;
        int EnterPoint = 0;
        for (int i = layer.size() - 1; i >= 1; i--) {
            NearestNeighbors = search_layer(QueryVector, EnterPoint, ef, i);
            EnterPoint = NearestNeighbors[0];
        }
        NearestNeighbors = search_layer(QueryVector, EnterPoint, ef, 0);
        NearestNeighbors.resize(K);
        vector<vector<T>> res;
        for (int x : NearestNeighbors) {
            res.push_back(VectorPool[x].first);
        }
        return res;
    }

    vector<ull> k_nn_search_cookie_no_erase(const vector<T> &QueryVector, int K, int ef = efConstruction) {
        vector<int> NearestNeighbors;
        int EnterPoint = 0;
        for (int i = layer.size() - 1; i >= 1; i--) {
            NearestNeighbors = search_layer(QueryVector, EnterPoint, ef, i);
            EnterPoint = NearestNeighbors[0];
        }
        NearestNeighbors = search_layer(QueryVector, EnterPoint, ef, 0);
        NearestNeighbors.resize(K);
        vector<ull> res;
        for (int x : NearestNeighbors) {
            res.push_back(VectorPool[x].second);
        }
        return res;
    }

    vector<double> get_min_kth_dist_no_erase(const vector<T> &QueryVector, int K) {
        auto res = k_nn_search_cookie(QueryVector, K, efConstruction);
        vector<double> res2;
        for (auto x : res) {
            res2.push_back(dist(QueryVector, VectorPool[id2pos[x]].first));
        }
        return res2;
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
        VectorPool.reserve(len);
    }

    ~hnsw() {
        //save_data();
    }

    bool erase(const ull id) {

        if (IsErased(id)) {
            return 0;
        }
        ErasedCookie.insert(id);
        if (CheckRebuild()) {
            rebuild();
        }
        return true;
    }

    

    vector<ull> k_nn_search_cookie(const vector<T> &QueryVector, int K, int ef = efConstruction) {
        vector<ull> res;
        if (VectorPool.size() < K) {
            for (auto &x : VectorPool) {
                if (! IsErased(x.second)) res.push_back(x.second);
            }
            return res;
        }
        int nowcnt = K;
        while (1) {
            auto res2 = k_nn_search_cookie_no_erase(QueryVector, nowcnt, ef);

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

    

    vector<double> get_min_kth_dist(const vector<T> &QueryVector, int K) {
        int nowcnt = K;
        while (1) {
            auto res = k_nn_search_cookie(QueryVector, K, efConstruction);
            int cnt = 0;
            vector<double> res2;
            for (auto x : res) {
                if (! IsErased(x)) {
                    res2.push_back(dist(QueryVector, VectorPool[id2pos[x]].first));
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
        std ::ofstream out(FILE_NAME);
        out << VectorPool.size() << " " << VectorPool[0].first.size() << "\n";
        for (auto &x : VectorPool) {
            for (auto y : x.first) {
                out << y << " ";
            }
            out << x.second << "\n";
        }
        out << ErasedCookie.size() << "\n";
        for (auto x : ErasedCookie) {
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
        VectorPool.resize(n);
        for (int i = 0; i < n; i++) {
            VectorPool[i].first.resize(dim);
            for (int j = 0; j < dim; j++) {
                in >> VectorPool[i].first[j];
            }
            in >> VectorPool[i].second;
            id2pos[VectorPool[i].second] = i;
        }
        int m;
        in >> m;
        for (int i = 0; i < m; i++) {
            ull x;
            in >> x;
            ErasedCookie.insert(x);
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

    ull insert(const vector<T> &QueryVector, ull Cookie = 0) {  // cookie = 0 表示 cookie 未知
        ull cookie;
        if (! Cookie)
            cookie = ++ tot;
        else
            cookie = Cookie;
        VectorPool.push_back( {QueryVector, cookie} );

        id2pos[cookie] = VectorPool.size() - 1;
        
        if (VectorPool.size() == 1) {
            
            return cookie;
        }

        vector<int> NearestNeighbors;
        int EnterPoint = enter_point;
        int L = layer.size() - 1;
        int l = GetLayer();
        if (l > L) {
            layer.resize(l + 1);
        }

        for (int LayerIndex = L; LayerIndex >= l + 1; LayerIndex--) {
            NearestNeighbors = search_layer(QueryVector, EnterPoint, efConstruction, LayerIndex);
            EnterPoint = NearestNeighbors[0];
        }

        for (int LayerIndex = std ::min(L, l); LayerIndex >= 0; LayerIndex--) {
            NearestNeighbors = search_layer(QueryVector, EnterPoint, efConstruction, LayerIndex);

            vector<int> neighbors =
                std ::move(select_neighbors_simple(QueryVector, NearestNeighbors, Mmax));

            for (int x : neighbors) {
                layer[LayerIndex].test(x);
                layer[LayerIndex].test(VectorPool.size() - 1);
                layer[LayerIndex].e[x].push_back(VectorPool.size() - 1);
                layer[LayerIndex].e[VectorPool.size() - 1].push_back(x);
            }

            for (int e : neighbors) {

                layer[LayerIndex].test(e);
                auto &eConn = layer[LayerIndex].e[e];
                if (eConn.size() > Mmax) {
                    eConn =
                        std ::move(select_neighbors_simple(VectorPool[e].first, eConn, Mmax));
                }
            }
            EnterPoint = NearestNeighbors[0];
        }
        if (l > L) {
            enter_point = VectorPool.size() - 1;
        }

        return cookie;
    }
};

} // namespace Hnsw

using Hnsw :: hnsw;