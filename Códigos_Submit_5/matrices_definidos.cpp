#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>
using namespace std;

void printMatrix(const vector<vector<int>>& M, const string& name) {
    cout << "\nMatriz " << name << " (" << M.size() << "x" << M[0].size() << "):\n";
    for (const auto& fila : M) {
        for (int val : fila) cout << val << "\t";
        cout << "\n";
    }
}

vector<vector<int>> randomMatrix(int filas, int columnas, int minVal = 0, int maxVal = 9) {
    vector<vector<int>> M(filas, vector<int>(columnas));
    for (int i = 0; i < filas; ++i)
        for (int j = 0; j < columnas; ++j)
            M[i][j] = rand() % (maxVal - minVal + 1) + minVal;
    return M;
}

vector<vector<int>> addMatrices(const vector<vector<int>>& A, const vector<vector<int>>& B) {
    int filas = A.size(), columnas = A[0].size();
    vector<vector<int>> C(filas, vector<int>(columnas));
    for (int i = 0; i < filas; ++i)
        for (int j = 0; j < columnas; ++j)
            C[i][j] = A[i][j] + B[i][j];
    return C;
}

vector<vector<int>> multiplyMatrices(const vector<vector<int>>& A, const vector<vector<int>>& B) {
    int filasA = A.size(), colsA = A[0].size(), colsB = B[0].size();
    vector<vector<int>> C(filasA, vector<int>(colsB, 0));
    for (int i = 0; i < filasA; ++i)
        for (int j = 0; j < colsB; ++j)
            for (int k = 0; k < colsA; ++k)
                C[i][j] += A[i][k] * B[k][j];
    return C;
}

vector<vector<int>> transposeMatrix(const vector<vector<int>>& A) {
    int filas = A.size(), columnas = A[0].size();
    vector<vector<int>> T(columnas, vector<int>(filas));
    for (int i = 0; i < filas; ++i)
        for (int j = 0; j < columnas; ++j)
            T[j][i] = A[i][j];
    return T;
}

int main(int argc, char* argv[]) {
    srand(time(nullptr));

    if (argc != 5) {
        cout << "Uso: " << argv[0] << " filasA columnasA filasB columnasB\n";
        cout << "Ejemplo: " << argv[0] << " 3 2 2 4\n";
        return 1;
    }

    int filasA = atoi(argv[1]);
    int colsA = atoi(argv[2]);
    int filasB = atoi(argv[3]);
    int colsB = atoi(argv[4]);

    vector<vector<int>> A = randomMatrix(filasA, colsA);
    vector<vector<int>> B = randomMatrix(filasB, colsB);

    printMatrix(A, "A");
    printMatrix(B, "B");

    if (filasA == filasB && colsA == colsB) {
        auto S = addMatrices(A, B);
        printMatrix(S, "A + B");
    } else cout << "\nNo se puede sumar A y B (diferentes dimensiones)." << endl;

    if (colsA == filasB) {
        auto M = multiplyMatrices(A, B);
        printMatrix(M, "A x B");
    } else cout << "\nNo se puede multiplicar A y B (dimensiones incompatibles)." << endl;

    auto T = transposeMatrix(A);
    printMatrix(T, "Transpuesta de A");

    return 0;
}
