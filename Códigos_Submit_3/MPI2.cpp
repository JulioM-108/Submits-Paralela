#include <iostream>
#include <mpi.h>

int main(int argc, char** argv) {
    int rank, size;
    int value = 0;  // Variable a difundir

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (rank == 0) {
        value = 47;  // Inicializar
        value = value * value;  // Modificar (cuadrado: 2209)
    }
    MPI_Bcast(&value, 1, MPI_INT, 0, MPI_COMM_WORLD);

    std::cout << "Soy el proceso con rango " << rank
              << " y el valor recibido es " << value << std::endl;

    MPI_Finalize();
    return 0;
}

