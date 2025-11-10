#include <iostream>
#include <vector>
#include <algorithm>
#include <ctime>
#include <cstdlib>
#include <iomanip>
#include <cmath>

using namespace std;

void sort_row(vector<vector<int>>& M, int row, bool ascending) {
    if (ascending) {
        sort(M[row].begin(), M[row].end());
    } else {
        sort(M[row].begin(), M[row].end(), greater<int>());
    }
}

void transpose(vector<vector<int>>& M) {
    int n = M.size();
    for (int i = 0; i < n; i++) {
        for (int j = i + 1; j < n; j++) {
            swap(M[i][j], M[j][i]);
        }
    }
}

void alternative_shearsort(int n, vector<vector<int>>& M) {
    int iterations = static_cast<int>(log2(n)) + 1;
    
    for (int iter = 0; iter < iterations; iter++) {
        for (int tid = 0; tid < n; tid++) {
            sort_row(M, tid, tid % 2 == 0);
        }
        
        transpose(M);
        
        for (int tid = 0; tid < n; tid++) {
            sort_row(M, tid, true);
        }
        
        transpose(M);
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
    
    clock_t start = clock();
    alternative_shearsort(n, M);
    clock_t end = clock();
    
    cout << "Matriz Ordenada:\n";
    print_matrix(M);
    
    double walltime_ms = (double)(end - start) / CLOCKS_PER_SEC * 1000.0;
    
    cout << "Walltime: " << fixed << setprecision(4) << walltime_ms << " ms\n";
    
    return 0;
}