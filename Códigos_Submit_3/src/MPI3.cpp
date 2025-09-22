#include <iostream>
#include <mpi.h>

constexpr int ARRAY_SIZE = 10000;

int main(int argc, char** argv) {
    int rank, size;
    float A[ARRAY_SIZE], B[ARRAY_SIZE];
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    for (int i = 0; i < ARRAY_SIZE; i++) {
        A[i] = static_cast<float>(rank);
    }
    int dest = (rank + 1) % size;
    int source = (rank - 1 + size) % size;
    if (rank % 2 == 0) {
        MPI_Send(A, ARRAY_SIZE, MPI_FLOAT, dest, 0, MPI_COMM_WORLD);
        MPI_Recv(B, ARRAY_SIZE, MPI_FLOAT, source, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    } else {
        MPI_Recv(B, ARRAY_SIZE, MPI_FLOAT, source, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        MPI_Send(A, ARRAY_SIZE, MPI_FLOAT, dest, 0, MPI_COMM_WORLD);
    }
    std::cout << "Soy el proceso con rango " << rank
              << " y he recibido B[0] = " << B[0] << std::endl;

    MPI_Finalize();
    return 0;
}
