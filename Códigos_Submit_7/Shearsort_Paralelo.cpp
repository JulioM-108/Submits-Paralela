#include <iostream>
#include <vector>
#include <algorithm>
#include <ctime>
#include <cstdlib>
#include <iomanip>
#include <cmath>
#include <omp.h>

using namespace std;

void sort_row(vector<vector<int>>& M, int row, bool ascending) {
    if (ascending) {
        sort(M[row].begin(), M[row].end());
    } else {
        sort(M[row].begin(), M[row].end(), greater<int>());
    }
}

void sort_column(vector<vector<int>>& M, int column) {
    int n = M.size();
    vector<int> temp(n);
    
    for (int i = 0; i < n; i++) {
        temp[i] = M[i][column];
    }
    
    sort(temp.begin(), temp.end());
    
    for (int i = 0; i < n; i++) {
        M[i][column] = temp[i];
    }
}

void shearsort_parallel(int n, vector<vector<int>>& M) {
    int iterations = static_cast<int>(log2(n)) + 1;
    
    for (int iter = 0; iter < iterations; iter++) {
        #pragma omp parallel for
        for (int tid = 0; tid < n; tid++) {
            sort_row(M, tid, tid % 2 == 0);
        }
        
        #pragma omp parallel for
        for (int tid = 0; tid < n; tid++) {
            sort_column(M, tid);
        }
    }
}

void generate_random_matrix(vector<vector<int>>& M, int n, int max_value = 1000) {
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            M[i][j] = rand() % max_value;
        }
    }
}

void print_matrix(const vector<vector<int>>& M) {
    int n = M.size();
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            cout << setw(6) << M[i][j] << " ";
        }
        cout << "\n";
    }
    cout << "\n";
}

int main() {
    srand(time(0));
    
    int n;
    cout << "Introduce el tamaño de la matriz (n): ";
    cin >> n;
    
    vector<vector<int>> M(n, vector<int>(n));
    
    generate_random_matrix(M, n);
    
    cout << "\nMatriz Original:\n";
    print_matrix(M);
    
    double start = omp_get_wtime();
    shearsort_parallel(n, M);
    double end = omp_get_wtime();
    
    cout << "Matriz Ordenada:\n";
    print_matrix(M);
    
    double walltime_ms = (end - start) * 1000.0;
    
    cout << "Walltime: " << fixed << setprecision(4) << walltime_ms << " ms\n";
    cout << "Threads usados: " << omp_get_max_threads() << "\n";
    
    return 0;
}