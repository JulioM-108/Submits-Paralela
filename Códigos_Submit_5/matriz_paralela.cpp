#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <iomanip>
#include <stdexcept>
#include <omp.h>

using namespace std;

class Vector {
private:
    vector<double> data;
public:
    Vector() = default;
    Vector(int n) : data(n) {}
    void fromArgs(int n, char* argv[], int startIndex) {
        data.resize(n);
        for (int i = 0; i < n; ++i) data[i] = stod(argv[startIndex + i]);
    }
    int size() const { return (int)data.size(); }
    double operator[](int i) const { return data[i]; }
    double& operator[](int i) { return data[i]; }
    void print() const { for (double v : data) cout << v << " "; cout << "\n"; }
};

class Matrix {
private:
    int rows, cols;
    vector<vector<double>> data;
public:
    Matrix(int m, int n) : rows(m), cols(n), data(m, vector<double>(n, 0.0)) {}
    void fillRandomInts(int minVal = 0, int maxVal = 9) {
        for (int i = 0; i < rows; ++i)
            for (int j = 0; j < cols; ++j)
                data[i][j] = rand() % (maxVal - minVal + 1) + minVal;
    }
    int numRows() const { return rows; }
    int numCols() const { return cols; }

    vector<double> multiply_parallel(const Vector &vec) const {
        if (cols != vec.size()) throw runtime_error("Dimensiones incompatibles para multiplicación.");
        vector<double> res(rows, 0.0);

        #pragma omp parallel for schedule(static)
        for (int i = 0; i < rows; ++i) {
            double sum = 0.0;
            for (int j = 0; j < cols; ++j) sum += data[i][j] * vec[j];
            res[i] = sum;
        }
        return res;
    }

    void print() const {
        for (int i = 0; i < rows; ++i) {
            for (int j = 0; j < cols; ++j) cout << setw(6) << data[i][j] << " ";
            cout << "\n";
        }
    }
};

void usage(const char* prog) {
    cerr << "Uso: " << prog << " m n v1 v2 ... vn [num_threads]\n";
    cerr << "Ejemplo: " << prog << " 3 4 1.0 2.5 0 7 4\n";
    cerr << "Si no pasas num_threads, usa OMP_NUM_THREADS o la decisión por defecto de OpenMP.\n";
}

int main(int argc, char* argv[]) {
    if (argc < 4) { usage(argv[0]); return 1; }

    int m, n;
    try { m = stoi(argv[1]); n = stoi(argv[2]); }
    catch (...) { cerr << "m y n deben ser enteros.\n"; return 1; }
    if (m <= 0 || n <= 0) { cerr << "m y n deben ser positivos.\n"; return 1; }

    int expectedMinArgs = 3 + n;       
    int expectedMaxArgs = 4 + n;      
    if (argc != expectedMinArgs && argc != expectedMaxArgs) {
        cerr << "Número de argumentos inválido: se esperaban " << n << " valores para el vector.\n";
        usage(argv[0]);
        return 1;
    }

    int argIndexThreads = -1;
    if (argc == expectedMaxArgs) argIndexThreads = argc - 1;

    Vector x;
    try {
        x.fromArgs(n, argv, 3);
    } catch (...) {
        cerr << "Error leyendo vector (asegúrate de pasar " << n << " números).\n";
        return 1;
    }

    if (argIndexThreads != -1) {
        int t = stoi(argv[argIndexThreads]);
        if (t > 0) omp_set_num_threads(t);
    }

    srand((unsigned)time(nullptr));
    Matrix A(m, n);
    A.fillRandomInts();

    cout << "Matriz A (" << m << "x" << n << "):\n";
    A.print();
    cout << "\nVector x (" << n << "):\n";
    x.print();

    #pragma omp parallel
    {
        #pragma omp master
        cout << "\nEjecutando en " << omp_get_num_threads() << " hilos (OpenMP)\n";
    }

    vector<double> y;
    try {
        y = A.multiply_parallel(x);
    } catch (const exception &e) {
        cerr << "Error multiplicación: " << e.what() << "\n";
        return 1;
    }

    cout << "\nEl resultado es el siguiente con tamaño de " << m << ":\n";
    for (double v : y) cout << v << " ";
    cout << "\n";
    return 0;
}
