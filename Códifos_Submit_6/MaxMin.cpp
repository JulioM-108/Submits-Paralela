#include <iostream>
#include <vector>
#include <limits>
#include <random>
#include <ctime>
#include <cstdlib>
#include <omp.h>

using namespace std;

pair<double, double> parallel_min_max_openmp(const vector<double>& a) {
    if (a.empty()) throw runtime_error("vector vacío");
    double gmin = numeric_limits<double>::infinity();
    double gmax = -numeric_limits<double>::infinity();

    #pragma omp parallel for reduction(min:gmin) reduction(max:gmax) schedule(static)
    for (size_t i = 0; i < a.size(); ++i) {
        double v = a[i];
        if (v < gmin) gmin = v;
        if (v > gmax) gmax = v;
    }

    return {gmin, gmax};
}

int main(int argc, char** argv) {
    size_t N = 10000000;
    if (argc > 1) N = strtoull(argv[1], NULL, 10);

    vector<double> data(N);
    mt19937_64 rng(static_cast<unsigned long>(time(NULL)));
    uniform_real_distribution<double> dist(-1e6, 1e6);

    for (size_t i = 0; i < N; ++i) data[i] = dist(rng);

    clock_t t0 = clock();
    auto [minv, maxv] = parallel_min_max_openmp(data);
    clock_t t1 = clock();

    double elapsed = double(t1 - t0) / CLOCKS_PER_SEC;
    cout << "N=" << N << ", min = " << minv << ", max = " << maxv
         << ", time = " << elapsed << " s\n";

    return 0;
}
