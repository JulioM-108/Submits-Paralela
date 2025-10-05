#include <iostream>
#include <iomanip>
#include <cstdlib>
#include <omp.h>
#include <random>
#include <ctime>
#include <cmath>   // M_PI

int main(int argc, char* argv[]) {
    if (argc != 3) {
        std::cerr << "Uso: " << argv[0] << " <num_threads> <num_tosses>\n";
        return 1;
    }

    int num_threads = std::atoi(argv[1]);
    long long num_tosses = std::atoll(argv[2]);
    if (num_threads <= 0 || num_tosses <= 0) {
        std::cerr << "Error: argumentos deben ser positivos.\n";
        return 1;
    }

    // Iniciar tiempos
    clock_t cpu_start = clock();
    time_t wall_start = time(NULL);

    long long in_circle = 0;

    // Bucle paralelo con reducción
    #pragma omp parallel num_threads(num_threads) reduction(+:in_circle)
    {
        std::mt19937_64 rng(omp_get_thread_num() + time(NULL)); // semilla distinta por hilo
        std::uniform_real_distribution<double> dist(-1.0, 1.0);

        long long local_tosses = num_tosses / num_threads;
        if (omp_get_thread_num() < num_tosses % num_threads) {
            local_tosses++; // repartir sobras
        }

        for (long long i = 0; i < local_tosses; i++) {
            double x = dist(rng);
            double y = dist(rng);
            if (x*x + y*y <= 1.0) {
                in_circle++;
            }
        }
    }

    double pi = 4.0 * (double) in_circle / (double) num_tosses;

    // Terminar tiempos
    clock_t cpu_end = clock();
    time_t wall_end = time(NULL);

    // Calcular métricas
    double cpu_time = double(cpu_end - cpu_start) / CLOCKS_PER_SEC;
    double wall_time = wall_end - wall_start; 
    double error = fabs(pi - M_PI);

    // Mostrar resultados
    std::cout << std::setprecision(17);
    std::cout << "PI estimate = " << pi << "\n";
    std::cout << "Error       = " << error << "\n";
    std::cout << "CPU time    = " << cpu_time << " s\n";
    std::cout << "Wall time   = " << wall_time << " s\n";

    return 0;
}

