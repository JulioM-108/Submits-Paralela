#include <iostream>
#include <vector>
#include <cstdlib>   
#include <ctime>     
#include <sstream>
#include <iomanip>
#include <stdexcept>

using namespace std;

class Vector {
private:
    vector<double> data;
public:
    Vector() = default;
    Vector(int n) : data(n) {}

    void fromArgs(int n, char* argv[], int startIndex) {
        data.resize(n);
        for (int i = 0; i < n; ++i) {
            try {
                data[i] = stod(argv[startIndex + i]);
            } catch (...) {
                throw runtime_error("Valor de vector inválido en los argumentos.");
            }
        }
    }

    int size() const { return (int)data.size(); }
    double operator[](int i) const { return data[i]; }
    double& operator[](int i) { return data[i]; }

    void print() const {
        for (double v : data) cout << v << " ";
        cout << "\n";
    }
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

    vector<double> multiply(const Vector &vec) const {
        if (cols != vec.size()) throw runtime_error("Dimensiones incompatibles para multiplicación.");
        vector<double> res(rows, 0.0);
        for (int i = 0; i < rows; ++i) {
            double sum = 0.0;
            for (int j = 0; j < cols; ++j) sum += data[i][j] * vec[j];
            res[i] = sum;
        }
        return res;
    }

    void print() const {
        for (int i = 0; i < rows; ++i) {
            for (int j = 0; j < cols; ++j)
                cout << setw(6) << data[i][j] << " ";
            cout << "\n";
        }
    }
};



int main(int argc, char* argv[]) {
    if (argc < 4) {
        return 1;
    }

    int m, n;
    try {
        m = stoi(argv[1]);
        n = stoi(argv[2]);
    } catch (...) {
        cerr << "m y n deben ser enteros.\n";
        return 1;
    }

    if (m <= 0 || n <= 0) {
        cerr << "m y n deben ser positivos.\n";
        return 1;
    }

    if (argc != 3 + n) {
        cerr << "Número de argumentos inválido: se esperaban " << n << " valores para el vector.\n";
        return 1;
    }

    Vector x;
    try {
        x.fromArgs(n, argv, 3);
    } catch (const exception &e) {
        cerr << "Error leyendo vector: " << e.what() << "\n";
        return 1;
    }

    srand((unsigned)time(nullptr));
    Matrix A(m, n);
    A.fillRandomInts();

    cout << "Matriz A (" << m << "x" << n << "):\n";
    A.print();

    cout << "\nVector x (" << n << "):\n";
    x.print();

    try {
        vector<double> y = A.multiply(x);
        cout << "\nEl resultado es el siguiente con tamaño de " << m << ":\n";
        for (double val : y) cout << val << " ";
        cout << "\n";
    } catch (const exception &e) {
        cerr << "Error multiplicación: " << e.what() << "\n";
        return 1;
    }

    return 0;
}

