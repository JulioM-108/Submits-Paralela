#include <iostream>
#include <vector>
#include <ctime>
#include <cstdlib>
#include <omp.h>

using namespace std;

vector<double> parallel_prefix_sum_openmp(const vector<double>& A) {
    size_t N = A.size();
    if (N == 0) return {};
    vector<double> P = A;
    vector<double> tmp(N);

    for (size_t step = 1; step < N; step <<= 1) {
        #pragma omp parallel for schedule(static)
        for (size_t i = 0; i < N; ++i) {
            if (i >= step) tmp[i] = P[i] + P[i - step];
            else tmp[i] = P[i];
        }
        P.swap(tmp);
    }
    return P;
}

int main(int argc, char** argv) {
    size_t N = 8;
    if (argc > 1) N = strtoull(argv[1], NULL, 10);

    vector<double> data(N);
    for (size_t i = 0; i < N; ++i) data[i] = i + 1;  

    clock_t t0 = clock();
    vector<double> P = parallel_prefix_sum_openmp(data);
    clock_t t1 = clock();

    double elapsed = double(t1 - t0) / CLOCKS_PER_SEC;

    cout << "N=" << N << ", time = " << elapsed << " s\n";
    cout << "A: ";
    for (size_t i = 0; i < N; ++i) cout << data[i] << (i + 1 == N ? '\n' : ' ');
    cout << "P: ";
    for (size_t i = 0; i < N; ++i) cout << P[i] << (i + 1 == N ? '\n' : ' ');
    return 0;
}
