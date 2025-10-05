#include <iostream>
#include <vector>
#include <mpi.h>

constexpr int ARRAY_SIZE = 10000;

int main(int argc, char** argv) {
    int rank, size;
    std::vector<float> A(ARRAY_SIZE), B(ARRAY_SIZE);

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (size != 2) {
        if (rank == 0) {
            std::cout << "Este ejercicio requiere exactamente 2 procesos.\n";
        }
        MPI_Finalize();
        return 1;
    }

    // Inicializar arreglo A con el rank.
    for (int i = 0; i < ARRAY_SIZE; i++) {
        A[i] = static_cast<float>(rank);
    }

    int dest = (rank == 0) ? 1 : 0;

    if (rank == 0) {
        MPI_Send(A.data(), ARRAY_SIZE, MPI_FLOAT, dest, 0, MPI_COMM_WORLD);
        MPI_Recv(B.data(), ARRAY_SIZE, MPI_FLOAT, dest, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    } else {
        MPI_Recv(B.data(), ARRAY_SIZE, MPI_FLOAT, dest, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        MPI_Send(A.data(), ARRAY_SIZE, MPI_FLOAT, dest, 0, MPI_COMM_WORLD);
    }

    std::cout << "Soy el proceso con rango " << rank
              << " y he recibido B[0] = " << B[0] << "\n";

    MPI_Finalize();
    return 0;
}
