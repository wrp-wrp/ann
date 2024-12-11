#include <bits/stdc++.h>

using i64 = long long;
using std :: vector;
using std :: cin;
using std :: cout;

#define debug(...) fprintf(stderr, __VA_ARGS__), fflush(stderr)

class nsw {
public :
    int dim;
    const int Max_Friend = 5;
    const int Max_C = 10;
    vector<vector<int> > A;
    vector<vector<int> > e;
    std :: unordered_map<int, int> g; // 废弃
    std :: mt19937 rng;

    nsw() {
        rng.seed(std :: chrono :: steady_clock :: now().time_since_epoch().count());
    }

    i64 dist(const vector<int> &a, const vector<int> &b) {
        i64 res = 0;
        for (int i = 0; i < dim; i ++) {
            res += 1ll * (a[i] - b[i]) * (a[i] - b[i]);
        }
        return res;
    }

    vector<std :: pair<i64, int> > find(vector<int> a, int cnt) {
        // cnt <= 10
        vector<std :: pair<i64, int> > c(Max_C);
        std :: fill(c.begin(), c.end(), std :: make_pair(1e18, -1));
        g.clear();
        
        int u = rng() % A.size();
        c[0] = {dist(a, A[u]), u};

        while (1) {
            auto tc = c;
            vector<int> c_friends;
            for (auto [dist, x] : c) {
                if (x == -1) {
               //     c_friends.push_back(x);
                    continue;
                }
  
                for (int y : e[x]) 
                    if (g[y] == 0) {
                        c_friends.push_back(y);
                        g[y] = 1;
                    }
              //  c_friends.push_back(x);
            }

            sort(c_friends.begin(), c_friends.end());
            c_friends.erase(std :: unique(c_friends.begin(), c_friends.end()), c_friends.end());
            
            
            //c.clear();
            for (int x : c_friends) {
                c.push_back({x != -1 ? dist(a, A[x]) : 1e18, x});
            }

            std :: sort(c.begin(), c.end());
            c.erase(std :: unique(c.begin(), c.end()), c.end());
            while (c.size() < Max_C) c.push_back ({1e18, -1});
            c.resize(Max_C);
            if (c == tc) break;
        }

        c.resize(cnt);

        return c;
    }

    void insert(vector<int> a) {
        if (A.size() == 0) {
            A.push_back(a);
            e.push_back(vector<int> ());
            return;
        }

        if (A.size() < Max_Friend) {
            A.push_back(a);
            e.push_back (vector<int> ());
            for (int i = 0; i < (int) A.size() - 1; i ++) {
                e[i].push_back (A.size() - 1), e[A.size() - 1].push_back (i);
            }
            return;
        }
        
        auto c = find(a, Max_Friend);
        e.push_back(vector<int> ());

        for (auto [dist, x] : c) {
            e[x].push_back(A.size());    
            e[A.size()].push_back(x);
        }

        A.push_back (a);
    }

    vector<vector<int> > get_min_kth_vec(vector<int> a, int k) {
        auto c = find(a, k);
        vector<vector<int> > res;
        for (auto [dist, x] : c) {
            res.push_back(A[x]);
        }
        return res;
    }

    vector<i64> get_min_kth_dist(vector<int> a, int k) {
        auto c = find(a, k);
        vector<i64> res;
        for (auto [dist, x] : c) {
            res.push_back(dist);
        }
        return res;
    }
} ;

int main() {
    std :: ios :: sync_with_stdio(0);
    cin.tie(0); cout.tie(0);
    nsw gragh;
    int n, dim;
    cin >> n >> dim;
    gragh.dim = dim;
    
    for (int i = 0; i < n; i ++) {
        vector<int> a(dim);
        for (int j = 0; j < dim; j ++) cin >> a[j];
        //std :: cerr << i << std :: endl;
        gragh.insert(a);
    }
    int q; 
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