#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <pthread.h>

using namespace std;

struct ThreadArgs {
    const vector<vector<int>>* A;
    const vector<vector<int>>* B;
    vector<vector<int>>* C; 
    int filasA, colsA, filasB, colsB;
    int task; 
    bool possible;
    pthread_barrier_t* barrier;
};

void printMatrix(const vector<vector<int>>& M, const string& name) {
    cout << "\nMatriz " << name << " (" << M.size() << "x" << M[0].size() << "):\n";
    for (const auto& fila : M) {
        for (int val : fila) cout << val << "\t";
        cout << "\n";
    }
}

vector<vector<int>> randomMatrix(int filas, int cols, int minV = 0, int maxV = 9) {
    vector<vector<int>> M(filas, vector<int>(cols));
    for (int i = 0; i < filas; ++i)
        for (int j = 0; j < cols; ++j)
            M[i][j] = rand() % (maxV - minV + 1) + minV;
    return M;
}

void* worker(void* arg) {
    ThreadArgs* t = (ThreadArgs*)arg;
    t->possible = false;
    if (t->task == 1) { 
        if (t->filasA != t->filasB || t->colsA != t->colsB) {
         
            pthread_barrier_wait(t->barrier);
            return nullptr;
        }
        t->C = new vector<vector<int>>(t->filasA, vector<int>(t->colsA));
        for (int i = 0; i < t->filasA; ++i)
            for (int j = 0; j < t->colsA; ++j)
                (*t->C)[i][j] = (*t->A)[i][j] + (*t->B)[i][j];
        t->possible = true;
        pthread_barrier_wait(t->barrier);
        return nullptr;
    } else if (t->task == 2) { 
        if (t->colsA != t->filasB) {
            pthread_barrier_wait(t->barrier);
            return nullptr;
        }
        int filasR = t->filasA;
        int colsR = t->colsB;
        t->C = new vector<vector<int>>(filasR, vector<int>(colsR, 0));
        for (int i = 0; i < filasR; ++i) {
            for (int j = 0; j < colsR; ++j) {
                int sum = 0;
                for (int k = 0; k < t->colsA; ++k)
                    sum += (*t->A)[i][k] * (*t->B)[k][j];
                (*t->C)[i][j] = sum;
            }
        }
        t->possible = true;
        pthread_barrier_wait(t->barrier);
        return nullptr;
    } else if (t->task == 3) { 
        int filasT = t->colsA;
        int colsT = t->filasA;
        t->C = new vector<vector<int>>(filasT, vector<int>(colsT));
        for (int i = 0; i < t->filasA; ++i)
            for (int j = 0; j < t->colsA; ++j)
                (*t->C)[j][i] = (*t->A)[i][j];
        t->possible = true;
        pthread_barrier_wait(t->barrier);
        return nullptr;
    }
    pthread_barrier_wait(t->barrier);
    return nullptr;
}

int main(int argc, char* argv[]) {
    srand(time(nullptr));

    int filasA, colsA, filasB, colsB;
    if (argc == 5) {
        filasA = atoi(argv[1]);
        colsA  = atoi(argv[2]);
        filasB = atoi(argv[3]);
        colsB  = atoi(argv[4]);
        if (filasA <= 0 || colsA <= 0 || filasB <= 0 || colsB <= 0) {
            cout << "Argumentos invalidos. Deben ser enteros positivos.\n";
            return 1;
        }
    } else {
        filasA = rand() % 4 + 2;
        colsA  = rand() % 4 + 2;
        filasB = rand() % 4 + 2;
        colsB  = rand() % 4 + 2;
        cout << "No se recibieron 4 argumentos; usando tamaños aleatorios.\n";
    }

    vector<vector<int>> A = randomMatrix(filasA, colsA);
    vector<vector<int>> B = randomMatrix(filasB, colsB);

    printMatrix(A, "A");
    printMatrix(B, "B");

    pthread_t th1, th2, th3;
    pthread_barrier_t barrier;
    pthread_barrier_init(&barrier, nullptr, 3);

    ThreadArgs arg1 { &A, &B, nullptr, filasA, colsA, filasB, colsB, 1, false, &barrier };
    ThreadArgs arg2 { &A, &B, nullptr, filasA, colsA, filasB, colsB, 2, false, &barrier };
    ThreadArgs arg3 { &A, &B, nullptr, filasA, colsA, filasB, colsB, 3, false, &barrier };

    pthread_create(&th1, nullptr, worker, &arg1);
    pthread_create(&th2, nullptr, worker, &arg2);
    pthread_create(&th3, nullptr, worker, &arg3);

    pthread_join(th1, nullptr);
    pthread_join(th2, nullptr);
    pthread_join(th3, nullptr);

    pthread_barrier_destroy(&barrier);

    if (arg1.possible) {
        printMatrix(*arg1.C, "A + B");
        delete arg1.C;
    } else {
        cout << "\nNo se puede sumar A y B (diferentes dimensiones).\n";
    }

    if (arg2.possible) {
        printMatrix(*arg2.C, "A x B");
        delete arg2.C;
    } else {
        cout << "\nNo se puede multiplicar A y B (dimensiones incompatibles).\n";
    }

    if (arg3.possible) {
        printMatrix(*arg3.C, "Transpuesta de A");
        delete arg3.C;
    }

    return 0;
}
