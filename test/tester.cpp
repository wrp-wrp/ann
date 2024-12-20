#include <bits/stdc++.h>
#include "../hnsw/hnsw.h"
#include "brute.h"

#define debug(...) fprintf(stderr, __VA_ARGS__), fflush(stderr)

const double divclk = CLOCKS_PER_SEC;

int main() {
    std :: ios :: sync_with_stdio(0);
    std :: cin.tie(0); std :: cout.tie(0);

    hnsw<int> hnsw_sol;
    brute brute_sol;

    int n, dim;
    std :: cin >> n >> dim;
    //std :: cout << n << " " << dim << std :: endl;

    const std :: string FILE_NAME = "data.txt";
    //hnsw_sol.read_data(FILE_NAME);

    clock_t start, end;
    clock_t bf_building_time = 0, hnsw_building_time = 0;
    
    for (int i = 0; i < n; i ++) {
        std :: vector<int> a(dim);
        for (int j = 0; j < dim; j ++) std :: cin >> a[j];

        start = clock(); hnsw_sol.insert(a); end = clock();
        hnsw_building_time += end - start;

        start = clock(); brute_sol.insert(a); end = clock();
        bf_building_time += end - start;
    
    }

    debug ("Finished Building! \n HNSW Time: %lf\n Brute Time: %lf\n", 1.0 * hnsw_building_time / CLOCKS_PER_SEC, 1.0 * bf_building_time / CLOCKS_PER_SEC);
    std :: cout << n <<  ' '; 
    std :: cout << hnsw_building_time / divclk << ' ' << bf_building_time / divclk << ' ';
    int q; 
    std :: cin >> q; 
    double score = 0, per = 100.0 / q;
    double query_time = 0;

    debug("Start Query!\n");

    clock_t hnsw_query_time = 0, bf_query_time = 0;

    for (int i = 0; i < q; i ++) {
        int k;
        std :: vector<int> a(dim);
        std :: cin >> k;
        for (int j = 0; j < dim; j ++) std :: cin >> a[j];

        
        
        start = clock();
        auto res = hnsw_sol.get_min_kth_dist(a, k);
        end = clock();
        hnsw_query_time += end - start;

        start = clock();
        auto res2 = brute_sol.get_min_kth_dist(a, k);
        end = clock();
        bf_query_time += end - start;

        // differ
        assert(res.size() == res2.size());
        assert(res.size() == k);

        double pnt = per / k;
        for (auto x : res2) {
            bool ok = 0;
            //std :: cerr << x << " ";
            for (auto y : res) {
                //std :: cerr << x << " " << (long long) y << "\n";
                if (x == (long long) y) {
                    ok = 1;
                    break;
                }
            }
            if (ok) {
                score += pnt;
            }
        }

        if (i % 1000 == 0) {
            debug("Progress: %.2lf%%\n", i * per);
            debug ("HNSW Time: %lf\n Brute Time: %lf\n", 1.0 * hnsw_query_time / CLOCKS_PER_SEC, 1.0 * bf_query_time / CLOCKS_PER_SEC);
        }

    }

    debug ("Finished Query! \n HNSW Time: %lf\n Brute Time: %lf\n", 1.0 * hnsw_query_time / CLOCKS_PER_SEC, 1.0 * bf_query_time / CLOCKS_PER_SEC);

    std :: cout << hnsw_query_time / divclk<< ' ' << bf_query_time/ divclk << std :: endl;

    //std :: cout << "Score: " << score << "\n";
    //std :: cout << n << " " << score << "\n";
    
    return 0;
}