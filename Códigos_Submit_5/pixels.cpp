#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <unordered_map>
#include <omp.h>

using namespace std;

int idx(int r, int c, int cols) { return r * cols + c; }

void printBinary(const vector<int>& A, int rows, int cols, const string& name) {
    cout << "\n" << name << " (" << rows << "x" << cols << "):\n";
    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j)
            cout << A[idx(i,j,cols)] << "\t";
        cout << "\n";
    }
}

void printLabels(const vector<int>& L, int rows, int cols, const string& name) {
    cout << "\n" << name << " (" << rows << "x" << cols << "):\n";
    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j)
            cout << L[idx(i,j,cols)] << "\t";
        cout << "\n";
    }
}

int main(int argc, char* argv[]) {
    srand((unsigned)time(nullptr));

    int rows = 10, cols = 12;          
    double p = 0.4;                    

    if (argc >= 3) {
        rows = atoi(argv[1]);
        cols = atoi(argv[2]);
        if (rows <= 0 || cols <= 0) {
            cerr << "Uso: " << argv[0] << " [filas cols] [prob_1]\n";
            return 1;
        }
        if (argc >= 4) p = atof(argv[3]);
    } else {
        cout << "No se pasaron filas/cols -> usando por defecto 10x12. Para personalizar: "
             << argv[0] << " filas cols [prob_1]\n";
    }

    int n = rows * cols;
    vector<int> A(n,0);
    for (int i = 0; i < n; ++i) {
        double r = (double)rand() / RAND_MAX;
        A[i] = (r < p) ? 1 : 0;
    }

    
    vector<int> L(n,0), Lnew(n,0);
    for (int i = 0; i < n; ++i) L[i] = (A[i] == 1) ? (i+1) : 0;

    printBinary(A, rows, cols, "Imagen binaria (A)");

    bool changed = true;
    long iter = 0;

    while (changed) {
        changed = false;
        ++iter;

        
        #pragma omp parallel for schedule(static) reduction(|:changed)
        for (int r = 0; r < rows; ++r) {
            for (int c = 0; c < cols; ++c) {
                int id = idx(r,c,cols);
                if (L[id] == 0) {
                    Lnew[id] = 0;
                    continue;
                }
                int m = L[id];
        
                if (r-1 >= 0) {
                    int t = L[idx(r-1,c,cols)];
                    if (t != 0 && t < m) m = t;
                }
        
                if (r+1 < rows) {
                    int t = L[idx(r+1,c,cols)];
                    if (t != 0 && t < m) m = t;
                }
                if (c-1 >= 0) {
                    int t = L[idx(r,c-1,cols)];
                    if (t != 0 && t < m) m = t;
                }
                if (c+1 < cols) {
                    int t = L[idx(r,c+1,cols)];
                    if (t != 0 && t < m) m = t;
                }
                Lnew[id] = m;
                if (m != L[id]) changed = true;
            }
        }

        L.swap(Lnew);
    }

    unordered_map<int,int> remap;
    int nextLabel = 1;
    for (int i = 0; i < n; ++i) {
        int v = L[i];
        if (v == 0) continue;
        auto it = remap.find(v);
        if (it == remap.end()) {
            remap[v] = nextLabel++;
            L[i] = remap[v];
        } else {
            L[i] = it->second;
        }
    }

    cout << "\nIteraciones de relajación realizadas: " << iter << "\n";
    cout << "Componentes encontradas: " << (nextLabel - 1) << "\n";

    printLabels(L, rows, cols, "Etiquetas finales (component labeling)");

    return 0;
}
