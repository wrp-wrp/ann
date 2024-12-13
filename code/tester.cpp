#include <bits/stdc++.h>
#include "hnsw.h"
#include "brute.h"

#define debug(...) fprintf(stderr, __VA_ARGS__), fflush(stderr)

int main() {
    std :: ios :: sync_with_stdio(0);
    std :: cin.tie(0); std :: cout.tie(0);

    hnsw<int> graph;
    brute bf;

    clock_t start = clock();

    int n, dim;
    std :: cin >> n >> dim;

    const std :: string FILE_NAME = "data.txt";
    graph.read_data(FILE_NAME);
    
    for (int i = 0; i < n; i ++) {
        std :: vector<int> a(dim);
        for (int j = 0; j < dim; j ++) std :: cin >> a[j];
     //   graph.insert(a); 
        bf.insert(a);
     //   debug("Insert %d\n", i);
    }

    clock_t end = clock();

    debug("Finished Building! Time: %lf\n", 1.0 * (end - start) / CLOCKS_PER_SEC);
    
    int q; 
    std :: cin >> q; 
    double score = 0, per = 100.0 / q;
    double query_time = 0;

    for (int i = 0; i < q; i ++) {
        int k;
        std :: vector<int> a(dim);
        std :: cin >> k;
        for (int j = 0; j < dim; j ++) std :: cin >> a[j];
        
        start = clock();
        auto res = graph.get_min_kth_dist(a, k);
        end = clock();
        query_time += 1.0 * (end - start) / CLOCKS_PER_SEC;

        auto res2 = bf.get_min_kth_dist(a, k);

        double pnt = per / k;
        for (auto x : res2) {
            bool ok = 0;
            for (auto y : res) {
                if (x == (long long) y) {
                    ok = 1;
                    break;
                }
            }
            if (ok) {
                score += pnt;
            }
        }
    }

    debug("Finished Query! Time: %lf\n", query_time);

    std :: cout << "Score: " << score << "\n";

    return 0;
}