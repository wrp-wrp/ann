#include <bits/stdc++.h>
#include <omp.h>

namespace BruteForce {
    #define debug(...) fprintf(stderr, __VA_ARGS__), fflush(stderr)

    using i64 = long long;
    using namespace std;

    class brute {
    public:
        vector<vector<int> > A;
        void insert(vector<int> q) {
            A.push_back(q);
        }
        vector<i64> get_min_kth_dist(vector<int> q, int K) {
            vector<pair<i64, int> > res;
            
            for (int i = 0; i < (int) A.size(); i ++) {
                i64 dist = 0;
                for (int j = 0; j < (int) q.size(); j ++) {
                    dist += 1LL * (q[j] - A[i][j]) * (q[j] - A[i][j]);
                }
                res.push_back({dist, i});
            }
            sort(res.begin(), res.end());
            vector<i64> res2;
            for (int i = 0; i < K; i ++) {
                res2.push_back(res[i].first);
            }
            return res2;
        }
    } ;
}

using BruteForce :: brute;