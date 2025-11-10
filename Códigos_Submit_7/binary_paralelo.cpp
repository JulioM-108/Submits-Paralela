#include <iostream>
#include <vector>
#include <algorithm>
#include <cstdlib>
#include <ctime>
#include <iomanip>
#include <atomic>
#include <omp.h>
using namespace std;

int p_bsa_ordered(const vector<int>& arr, int target, int threads, double &time_sec) {
    int n = arr.size();
    atomic<int> found_pos(-1);
    atomic<int> found_flag(0);
    clock_t start = clock();
    #pragma omp parallel num_threads(threads)
    {
        int tid = omp_get_thread_num();
        int tcnt = omp_get_num_threads();
        int chunk = n / tcnt;
        if (chunk == 0) chunk = 1;
        int l = tid * chunk;
        int r = (tid == tcnt - 1) ? n - 1 : (l + chunk - 1);
        int low = l, high = r;
        while (low <= high && !found_flag.load()) {
            int mid = low + (high - low) / 2;
            int val = arr[mid];
            if (val == target) { found_pos.store(mid); found_flag.store(1); break; }
            else if (val < target) low = mid + 1;
            else high = mid - 1;
        }
    }
    clock_t end = clock();
    time_sec = double(end - start) / CLOCKS_PER_SEC;
    return found_flag.load() ? found_pos.load() : -1;
}

int p_bsa_unordered(const vector<int>& arr, int target, int threads, double &time_sec) {
    int n = arr.size();
    atomic<int> found_pos(-1);
    atomic<int> found_flag(0);
    clock_t start = clock();
    #pragma omp parallel num_threads(threads)
    {
        int tid = omp_get_thread_num();
        int tcnt = omp_get_num_threads();
        int chunk = n / tcnt;
        if (chunk == 0) chunk = 1;
        int l = tid * chunk;
        int r = (tid == tcnt - 1) ? n - 1 : (l + chunk - 1);
        int low = l, high = r;
        while (low <= high && !found_flag.load()) {
            int mid = low + (high - low) / 2;
            int val = arr[mid];
            if (val == target) { found_pos.store(mid); found_flag.store(1); break; }
            else if (val < target) low = mid + 1;
            else high = mid - 1;
        }
    }
    clock_t end = clock();
    time_sec = double(end - start) / CLOCKS_PER_SEC;
    return found_flag.load() ? found_pos.load() : -1;
}

int main(int argc, char* argv[]) {
    int n = 1000000;
    int threads = 4;
    if (argc > 1) n = atoi(argv[1]);
    if (argc > 2) threads = atoi(argv[2]);

    vector<int> data(n);
    srand(time(nullptr));
    for (int i = 0; i < n; ++i) data[i] = rand() % (n * 10);
    int target = data[rand() % n];

    vector<int> sorted_data = data;
    sort(sorted_data.begin(), sorted_data.end());

    double t_ord = 0.0, t_unord = 0.0;
    int pos_ord = p_bsa_ordered(sorted_data, target, threads, t_ord);
    cout << fixed << setprecision(6);
    if (pos_ord != -1) cout << "Ordenado: encontrado en " << pos_ord << "\n";
    else cout << "Ordenado: no encontrado\n";
    cout << "Tiempo (P-BSA ordenado): " << t_ord << " segundos\n\n";

    int pos_unord = p_bsa_unordered(data, target, threads, t_unord);
    if (pos_unord != -1) cout << "No ordenado: encontrado (posible error) en " << pos_unord << "\n";
    else cout << "No ordenado: no encontrado (búsqueda binaria inválida)\n";
    cout << "Tiempo (P-BSA no ordenado): " << t_unord << " segundos\n";

    return 0;
}
