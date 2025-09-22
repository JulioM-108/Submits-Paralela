#include <iostream>
#include <vector>
#include <mpi.h>
#include <iomanip>

constexpr int ARRAY_SIZE = 10000;

int main(int argc, char** argv) {
    MPI_Init(&argc, &argv);

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (size < 2) {
        if (rank == 0) std::cout << "Este ejercicio requiere al menos 2 procesos.\n";
        MPI_Finalize();
        return 1;
    }

    std::vector<float> A(ARRAY_SIZE), B(ARRAY_SIZE);

    for (int i = 0; i < ARRAY_SIZE; ++i) A[i] = static_cast<float>(rank);

    int dest = (rank + 1) % size;               
    int source = (rank - 1 + size) % size;        

    MPI_Request req;
    MPI_Irecv(B.data(), ARRAY_SIZE, MPI_FLOAT, source, 0, MPI_COMM_WORLD, &req);

    MPI_Send(A.data(), ARRAY_SIZE, MPI_FLOAT, dest, 0, MPI_COMM_WORLD);

    MPI_Wait(&req, MPI_STATUS_IGNORE);

    std::cout << std::fixed << std::setprecision(2)
              << "Soy el proceso con rango " << rank
              << " y he recibido B[0] = " << B[0] << std::endl;

    MPI_Finalize();
    return 0;
}
