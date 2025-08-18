
#include <iostream>
#include <iomanip>
#include <cstdlib>
#include <omp.h>
#include <ctime>   
#include <cmath>   

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Uso: " << argv[0] << " <upper_bound>\n";
        return 1;
    }

    long long N = std::atoll(argv[1]);
    if (N <= 0) {
        std::cerr << "Error: <upper_bound> debe ser un entero positivo.\n";
        return 1;
    }

    // Iniciar tiempos
    clock_t cpu_start = clock();
    double wall_start = omp_get_wtime();

    long double sum = 0.0L;

    // Paralelizamos el bucle con reducción para acumular en "sum".
    #pragma omp parallel for reduction(+:sum)
    for (long long k = 0; k < N; ++k) {
        long double term = ((k % 2 == 0) ? 1.0L : -1.0L) / (2.0L * k + 1.0L);
        sum += term;
    }

    long double pi = 4.0L * sum;

    // Terminar tiempos
    clock_t cpu_end = clock();
    double wall_end = omp_get_wtime();

    // Calcular métricas
    double cpu_time = double(cpu_end - cpu_start) / CLOCKS_PER_SEC;
    double wall_time = wall_end - wall_start;
    long double error = fabs(pi - M_PI);

    // Mostrar resultados
    std::cout << std::setprecision(17);
    std::cout << "PI estimate = " << pi << "\n";
    std::cout << "Error       = " << error << "\n";
    std::cout << "CPU time    = " << cpu_time << " s\n";
    std::cout << "Wall time   = " << wall_time << " s\n";

    return 0;
}

